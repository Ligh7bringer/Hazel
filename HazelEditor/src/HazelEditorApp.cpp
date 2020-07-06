#include "Hazel.hpp"
#include "Hazel/Core/EntryPoint.hpp"

#include "EditorLayer.hpp"

namespace Hazel
{

class HazelEditor : public Application
{
public:
	HazelEditor()
		: Application("Hazel Editor")
	{
		PushLayer(new EditorLayer());
	}

	~HazelEditor() = default;
};

Scope<Application> CreateApplication() { return MakeScope<HazelEditor>(); }

} // namespace Hazel