#include <ensmallen.hpp>
#include "catch.hpp"
#include "test_function_tools.hpp"
#include <ensmallen_bits/cmaes/pop_cmaes.hpp>

using namespace ens;
using namespace ens::test;

TEST_CASE("IPOPCMAESRastriginFunctionTest", "[IPOPCMAESTest]")
{
  RastriginFunction f(2);
  BoundaryBoxConstraint<> b(-10, 10);

  IPOP_CMAES<FullSelection, BoundaryBoxConstraint<>> ipopcmaes(
    15, // lambda
    b, // transformationPolicy
    32, // batchSize
    1000, // maxIterations
    1e-8, // tolerance
    FullSelection(), // selectionPolicy
    3.72, // stepSize
    2.0, // populationFactor
    5, // maxRestarts
    1e4 // maxFunctionEvaluations
  );


  arma::mat initialPoint = f.GetInitialPoint();
  arma::mat expectedResult = f.GetFinalPoint();

  MultipleTrialOptimizerTest(f, ipopcmaes, initialPoint, expectedResult, 0.01, f.GetFinalObjective(), 0.1, 5);
}

TEST_CASE("BIPOPCMAESRastriginFunctionTest", "[BIPOPCMAESTest]")
{
  RastriginFunction f(2);
  BoundaryBoxConstraint<> b(-10, 10);

  IPOP_CMAES<FullSelection, BoundaryBoxConstraint<>> ipopcmaes(
    15, // lambda
    b, // transformationPolicy
    32, // batchSize
    1000, // maxIterations
    1e-8, // tolerance
    FullSelection(), // selectionPolicy
    3.72, // stepSize
    2.0, // populationFactor
    5, // maxRestarts
    1e4 // maxFunctionEvaluations
  );

  arma::mat initialPoint = f.GetInitialPoint();
  arma::mat expectedResult = f.GetFinalPoint();

  MultipleTrialOptimizerTest(f, ipopcmaes, initialPoint, expectedResult, 0.01, f.GetFinalObjective(), 0.1, 5);
}


TEST_CASE("IPOPCMAESRosenbrockFunctionTest", "[IPOPCMAESTest]")
{
  BoundaryBoxConstraint<> b(0, 2);

  BIPOP_CMAES<FullSelection, BoundaryBoxConstraint<>> bipopcmaes(
    15, // lambda
    b, // transformationPolicy
    32, // batchSize
    1000, // maxIterations
    1e-8, // tolerance
    FullSelection(), // selectionPolicy
    0.25, // stepSize
    1.5, // populationFactor
    5, // maxRestarts
    1e4 // maxFunctionEvaluations
  );

  FunctionTest<RosenbrockFunction>(bipopcmaes, 0.1, 0.1);
}

TEST_CASE("BIPOPCMAESRosenbrockFunctionTest", "[IPOPCMAESTest]")
{
  BoundaryBoxConstraint<> b(0, 2);

  BIPOP_CMAES<FullSelection, BoundaryBoxConstraint<>> bipopcmaes(
    15, // lambda
    b, // transformationPolicy
    32, // batchSize
    1000, // maxIterations
    1e-8, // tolerance
    FullSelection(), // selectionPolicy
    0.25, // stepSize
    1.5, // populationFactor
    5, // maxRestarts
    1e4 // maxFunctionEvaluations
  );

  FunctionTest<RosenbrockFunction>(bipopcmaes, 0.1, 0.1);
}