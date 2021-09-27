#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>

#include <array>
#include <filesystem>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Hazel/Core/Log.hpp"
#include "Hazel/Debug/Instrumentor.hpp"

#ifdef HZ_PLATFORM_WINDOWS
#	include <Windows.h>
#endif
