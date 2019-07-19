#include <Hazel.h>

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
};

class Sandbox : public Hazel::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		PushOverlay(new Hazel::ImGuiLayer());
	}

	~Sandbox() {}
};

Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox();
}