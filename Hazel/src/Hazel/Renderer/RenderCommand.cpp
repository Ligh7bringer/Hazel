#include "RenderCommand.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

namespace Hazel
{

Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}
