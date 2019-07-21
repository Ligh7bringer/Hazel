#include <Hazel.h>

#include "imgui/imgui.h"

class ExampleLayer : public Hazel::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override {
		//HZ_INFO("ExampleLayer::Update()");

		if (Hazel:: Input::IsKeyPressed(HZ_KEY_SPACE)) {
			HZ_TRACE("Space pressed!");
		}
	}

	void OnEvent(Hazel::Event& evt) override {
		if (evt.GetEventType() == Hazel::EventType::KeyPressed) {
			Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)evt;
			HZ_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

	void OnImGuiRender() override {
		ImGui::Begin("FPS");
		ImGui::Text("%.3f ms/frame\n%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
};

class Sandbox : public Hazel::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};

Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox();
}