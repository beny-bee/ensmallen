/**
 * @file snapshots_sgdr_impl.hpp
 * @author Marcus Edel
 *
 * Implementation of SGDR method using snapshots ensembles.
 *
 * ensmallen is free software; you may redistribute it and/or modify it under
 * the terms of the 3-clause BSD license.  You should have received a copy of
 * the 3-clause BSD license along with ensmallen.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef ENSMALLEN_SGDR_SNAPSHOT_SGDR_IMPL_HPP
#define ENSMALLEN_SGDR_SNAPSHOT_SGDR_IMPL_HPP

// In case it hasn't been included yet.
#include "snapshot_sgdr.hpp"

namespace ens {

template<typename UpdatePolicyType>
SnapshotSGDR<UpdatePolicyType>::SnapshotSGDR(
    const size_t epochRestart,
    const double multFactor,
    const size_t batchSize,
    const double stepSize,
    const size_t maxIterations,
    const double tolerance,
    const bool shuffle,
    const size_t snapshots,
    const bool accumulate,
    const UpdatePolicyType& updatePolicy,
    const bool resetPolicy) :
    batchSize(batchSize),
    accumulate(accumulate),
    optimizer(OptimizerType(stepSize,
                            batchSize,
                            maxIterations,
                            tolerance,
                            shuffle,
                            updatePolicy,
                            SnapshotEnsembles(
                                epochRestart,
                                multFactor,
                                stepSize,
                                maxIterations,
                                snapshots),
                            resetPolicy))
{
  /* Nothing to do here */
}

template<typename UpdatePolicyType>
template<typename DecomposableFunctionType, typename MatType, typename GradType>
typename MatType::elem_type SnapshotSGDR<UpdatePolicyType>::Optimize(
    DecomposableFunctionType& function,
    MatType& iterate)
{
  // If a user changed the step size he hasn't update the step size of the
  // cyclical decay instantiation, so we have to do here.
  if (optimizer.StepSize() != optimizer.DecayPolicy().StepSize())
  {
    optimizer.DecayPolicy().StepSize() = optimizer.StepSize();
  }

  optimizer.DecayPolicy().EpochBatches() = function.NumFunctions() /
      double(batchSize);

  // If a user changed the batch size we have to update the restart fraction
  // of the cyclical decay instantiation.
  if (optimizer.BatchSize() != batchSize)
  {
    batchSize = optimizer.BatchSize();
  }

  typename MatType::elem_type overallObjective = optimizer.Optimize(function,
      iterate);

  typedef typename MatTypeTraits<MatType>::BaseMatType BaseMatType;
  typedef typename MatTypeTraits<GradType>::BaseMatType BaseGradType;

  typedef SnapshotEnsembles::Policy<BaseMatType, BaseGradType>
      InstDecayPolicyType;

  // Accumulate snapshots.
  if (accumulate)
  {
    Any& instDecayPolicy = optimizer.InstDecayPolicy();
    size_t numSnapshots =
        instDecayPolicy.As<InstDecayPolicyType>().Snapshots().size();

    for (size_t i = 0; i < numSnapshots; ++i)
    {
      iterate += instDecayPolicy.As<InstDecayPolicyType>().Snapshots()[i];
    }
    iterate /= (numSnapshots + 1);

    // Calculate final objective.
    overallObjective = 0;
    for (size_t i = 0; i < function.NumFunctions(); ++i)
      overallObjective += function.Evaluate(iterate, i, 1);
  }

  return overallObjective;
}

} // namespace ens

#endif
