#pragma once

namespace Hazel
{

class Timestep
{
public:
	Timestep() = default;
	Timestep(float time)
		: m_Time(time)
	{ }

	operator float() const { return m_Time; }

	float GetSeconds() const { return m_Time; }
	float GetMilliseconds() const { return m_Time * 1000.f; }

private:
	float m_Time;
};

} // namespace Hazel
