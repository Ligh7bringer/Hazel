#pragma once

#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel
{

class OpenGLRendererAPI : public RendererAPI
{
public:
	virtual void Init() override;

	virtual void SetClearColor(const glm::vec4& col) override;
	virtual void Clear() override;
	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
};

} // namespace Hazel
