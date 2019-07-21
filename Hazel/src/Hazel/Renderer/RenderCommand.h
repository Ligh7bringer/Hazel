#pragma once

#include "RendererAPI.h"

namespace Hazel {

	class RenderCommand {
	public:
		inline static void SetClearColor(const glm::vec4& col) {
			s_RendererAPI->SetClearColor(col);
		}

		inline static void Clear() {
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}