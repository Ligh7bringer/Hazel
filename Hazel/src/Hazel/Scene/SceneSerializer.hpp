#pragma once

namespace Hazel
{

class Scene;

class SceneSerializer
{
public:
	explicit SceneSerializer(const Ref<Scene>& scene);

	void Serialize(const std::string& filepath);
	void SerializeRuntime(const std::string& filepath);

	bool Deserialize(const std::string& filepath);
	bool DeserializeRuntime(const std::string& filepath);

private:
	Ref<Scene> m_Scene;
};

} // namespace Hazel
