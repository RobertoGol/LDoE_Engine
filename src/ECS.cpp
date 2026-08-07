#include "ECS.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/SpriteRendererComponent.hpp"
#include "BoxColliderComponent.hpp"
#include "Components/ColliderComponent.hpp"

json Entity::Serialize() {
    json j;
    j["name"] = name;
    j["components"] = json::array();
    for (auto& [type, comp] : components) {
        j["components"].push_back(comp->Serialize());
    }
    return j;
}

void Entity::Deserialize(const json& j) {
    name = j.value("name", "Unknown");
    components.clear();

    if (!j.contains("components")) {
        return;
    }

    for (const auto& componentJson : j["components"]) {
        const std::string type = componentJson.value("type", "");
        if (type == "TransformComponent" || type == "Transform") {
            AddComponent<TransformComponent>()->Deserialize(componentJson);
        } else if (type == "SpriteRendererComponent" || type == "SpriteRenderer") {
            AddComponent<SpriteRendererComponent>()->Deserialize(componentJson);
        } else if (type == "TilemapComponent") {
            AddComponent<TilemapComponent>()->Deserialize(componentJson);
        } else if (type == "BoxColliderComponent") {
            AddComponent<BoxColliderComponent>()->Deserialize(componentJson);
        } else if (type == "ColliderComponent") {
            AddComponent<ColliderComponent>()->Deserialize(componentJson);
        }
    }
}
