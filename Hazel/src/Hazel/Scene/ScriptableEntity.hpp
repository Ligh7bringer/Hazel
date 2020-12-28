#pragma once

#include "Entity.hpp"

namespace Hazel
{

class ScriptableEntity
{
public:
	virtual ~ScriptableEntity() = default;

	template <typename T>
	T& GetComponent()
	{
		return m_Entity.GetComponent<T>();
	}

protected:
	virtual void OnCreate() { }
	virtual void OnDestroy() { }
	virtual void OnUpdate(Timestep ts) { }

private:
	Entity m_Entity;
	friend class Scene;
};

} // namespace Hazel