#pragma once

#include <cstdint>
#include <functional>

namespace Hazel
{

class UUID
{
	uint64_t m_UUID;

public:
	UUID();
	explicit UUID(uint64_t uuid);
	UUID(const UUID&) = default;

	operator uint64_t() const { return m_UUID; }
};

} // namespace Hazel

namespace std
{
template <typename T>
struct hash;

template <>
struct hash<Hazel::UUID>
{
	std::size_t operator()(const Hazel::UUID& uuid) const { return static_cast<uint64_t>(uuid); }
};

} // namespace std
