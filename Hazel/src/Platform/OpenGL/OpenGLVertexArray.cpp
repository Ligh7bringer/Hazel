#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Hazel
{

static GLenum ShaderDataTypeToOGLType(ShaderDataType type)
{
	switch(type)
	{
	case ShaderDataType::Float: return GL_FLOAT;
	case ShaderDataType::Float2: return GL_FLOAT;
	case ShaderDataType::Float3: return GL_FLOAT;
	case ShaderDataType::Float4: return GL_FLOAT;
	case ShaderDataType::Mat3: return GL_FLOAT;
	case ShaderDataType::Mat4: return GL_FLOAT;
	case ShaderDataType::Int: return GL_INT;
	case ShaderDataType::Int2: return GL_INT;
	case ShaderDataType::Int3: return GL_INT;
	case ShaderDataType::Int4: return GL_INT;
	case ShaderDataType::Bool: return GL_BOOL;
	case ShaderDataType::None: return -1;
	}

	HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

OpenGLVertexArray::OpenGLVertexArray()
{
	HZ_PROFILE_FUNCTION();

	glCreateVertexArrays(1, &m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	HZ_PROFILE_FUNCTION();

	glDeleteVertexArrays(1, &m_RendererID);
}

void OpenGLVertexArray::Bind() const
{
	HZ_PROFILE_FUNCTION();

	glBindVertexArray(m_RendererID);
}

void OpenGLVertexArray::Unbind() const
{
	HZ_PROFILE_FUNCTION();

	glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	HZ_PROFILE_FUNCTION();

	HZ_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "VertexBuffer has no layout!");

	glBindVertexArray(m_RendererID);
	vertexBuffer->Bind();

	uint32_t index = 0;
	const auto& layout = vertexBuffer->GetLayout();
	for(const auto& element : layout)
	{
		glEnableVertexAttribArray(m_VertexBufferIndex);
		glVertexAttribPointer(m_VertexBufferIndex,
							  element.GetComponentCount(),
							  ShaderDataTypeToOGLType(element.Type),
							  element.Normalised ? GL_TRUE : GL_FALSE,
							  layout.GetStride(),
							  (const void*)(intptr_t)element.Offset);
		m_VertexBufferIndex++;
	}

	m_VertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
	HZ_PROFILE_FUNCTION();

	glBindVertexArray(m_RendererID);
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}

} // namespace Hazel
