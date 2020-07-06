#include "Hazel.hpp"
#include "Hazel/Core/EntryPoint.hpp"

#include "EditorLayer.hpp"

namespace Hazel
{

class Hazelnut : public Application
{
public:
	Hazelnut()
		: Application("Hazelnut")
	{
		PushLayer(new EditorLayer());
	}

	~Hazelnut() = default;
};

Scope<Application> CreateApplication() { return MakeScope<Hazelnut>(); }

} // namespace Hazel