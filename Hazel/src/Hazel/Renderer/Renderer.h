#pragma once

namespace Hazel {

	enum class RendererAPI {
		None = 0,
		OpenGL = 1
	};

	class Renderer {
	public:
		inline static RendererAPI GetAPI() { return s_RendererAPI; }

		static void SetAPI(RendererAPI api) {
			s_RendererAPI = api;
		}

	private:
		static RendererAPI s_RendererAPI;
	};

	

}