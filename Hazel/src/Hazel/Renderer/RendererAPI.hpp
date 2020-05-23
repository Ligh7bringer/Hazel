#pragma once

#include "VertexArray.hpp"

#include <glm/glm.hpp>

namespace Hazel
{

class RendererAPI
{
public:
	enum class API
	{
		None = 0,
		OpenGL = 1
	};

public:
	static Scope<RendererAPI> Create();

	virtual void Init() = 0;

	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t widht, uint32_t height) = 0;
	virtual void SetClearColor(const glm::vec4& col) = 0;
	virtual void Clear() = 0;

	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

	static API GetAPI() { return s_API; }

private:
	static API s_API;
};

} // namespace Hazel