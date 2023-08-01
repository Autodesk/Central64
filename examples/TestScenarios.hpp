#pragma once
#ifndef CENTRAL64EXAMPLES_TEST_SCENARIOS
#define CENTRAL64EXAMPLES_TEST_SCENARIOS

#include <central64/PathPlanner.hpp>

template <int L>
void TestScenario(central64::CellAlignment alignment,
                  central64::SearchMethod searchMethod,
                  central64::SmoothingMethod smoothingMethod,
                  bool centralize,
                  central64::Offset2D sourceCoords,
                  central64::Offset2D sampleCoords);

void TestScenarios();

#endif
