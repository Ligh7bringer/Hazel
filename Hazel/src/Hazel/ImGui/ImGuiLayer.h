#pragma once

#include "Hazel/Layer.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace Hazel {

	class HAZEL_API ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		struct CameraProperties {
			glm::vec3 Position{ 0.f, 0.f, 0.f };
			float Rotation = 0.f;
			glm::vec4 SquareColour{ 0.2f, 0.3f, 0.8f, 1.0f };
		};

		const CameraProperties& GetCameraProperties() const { return m_CameraProps; }

	private:
		CameraProperties m_CameraProps;
		float m_Time = 0.0f;
	};

}