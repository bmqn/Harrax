#pragma once

#define ENABLE_LOGGING    1
#define ENABLE_ASSERTIONS 1

static constexpr double k_TimeStep = 1.0 / 60.0;

static constexpr size_t k_MaxComponents = 2 << 5;
static constexpr size_t k_MaxSystems = 2 << 5;