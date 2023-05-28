#include "UUID.hpp"

#include <random>

namespace Hazel
{

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine{s_RandomDevice()};
static std::uniform_int_distribution<uint64_t> s_Dist;

UUID::UUID()
	: m_UUID(s_Dist(s_Engine))
{ }

UUID::UUID(uint64_t uuid)
	: m_UUID(uuid)
{ }

} // namespace Hazel
