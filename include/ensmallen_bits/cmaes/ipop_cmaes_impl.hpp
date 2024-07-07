/**
 * @file ipop_cmaes_impl.hpp
 * @author Marcus Edel
 * @author Suvarsha Chennareddy
 * @author Benjami Parellada
 *
 * Implementation of the IPOP Covariance Matrix Adaptation Evolution Strategy
 * as proposed by A. Auger and N. Hansen in "A Restart CMA Evolution
 * Strategy With Increasing Population Size".
 *
 * ensmallen is free software; you may redistribute it and/or modify it under
 * the terms of the 3-clause BSD license.  You should have received a copy of
 * the 3-clause BSD license along with ensmallen.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef ENSMALLEN_CMAES_IPOP_CMAES_IMPL_HPP
#define ENSMALLEN_CMAES_IPOP_CMAES_IMPL_HPP

#include "ipop_cmaes.hpp"
#include <ensmallen_bits/function.hpp>

namespace ens {

template<typename CMAESType>
IPOPCMAES<CMAESType>::IPOPCMAES(const CMAESType& CMAES,
                                const double populationFactor,
                                const size_t maxRestarts) :
    cmaes(CMAES),
    populationFactor(populationFactor),
    maxRestarts(maxRestarts)
{ /* Nothing to do. */ }

template<typename CMAESType>
IPOPCMAES<CMAESType>::IPOPCMAES(const size_t lambda,
                                const typename CMAESType::transformationPolicyType& transformationPolicy,
                                const size_t batchSize,
                                const size_t maxIterations,
                                const double tolerance,
                                const typename CMAESType::selectionPolicyType& selectionPolicy,
                                double stepSize,
                                const double populationFactor,
                                const size_t maxRestarts) :
    cmaes(lambda, transformationPolicy, batchSize, maxIterations, tolerance, selectionPolicy, stepSize),
    populationFactor(populationFactor),
    maxRestarts(maxRestarts)
{ /* Nothing to do. */  }

template<typename CMAESType>
template<typename SeparableFunctionType, typename MatType, typename... CallbackTypes>
typename MatType::elem_type IPOPCMAES<CMAESType>::Optimize(
    SeparableFunctionType& function,
    MatType& iterateIn,
    CallbackTypes&&... callbacks)
{
    StoreBestCoordinates<MatType> sbc;
    size_t totalFunctionEvaluations = 0;
    for (size_t i = 0; i < maxRestarts; ++i)
    {
        // Use the starting point.
        MatType iterate = iterateIn;

        // Optimize using the CMAES object.
        cmaes.Optimize(function, iterate, sbc, callbacks...);

        // Retrieve and accumulate the number of function evaluations used by CMAES.
        totalFunctionEvaluations += cmaes.FunctionEvaluations();

        // Check if the total number of evaluations has exceeded the limit
        if (totalFunctionEvaluations >= 1e9) {
            break;
        }

        // Increase the population size by the population factor for next restart.
        cmaes.PopulationSize() *= populationFactor;
    }

    // Store the best coordinates.
    iterateIn = sbc.BestCoordinates();
    // Return the best objective.
    return sbc.BestObjective();
}

} // namespace ens

#endif
