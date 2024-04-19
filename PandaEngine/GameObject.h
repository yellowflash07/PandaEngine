#pragma once
#include <entt/entt.hpp>
#include <string>
#include "TransformComponent.h"
#include "IEditorUI.h"

class GameObject
{
public:
	GameObject(std::string name);
	~GameObject();

	entt::registry* m_Registry;
	entt::entity entity;

	template <class T, typename... Args>
	T& AddComponent(Args &&...args)
	{
		//a game object can only have one of each component
		assert(!m_Registry->all_of<T>(entity));
		T& comp = m_Registry->emplace<T>(entity, std::forward<Args>(args)...);
		IEditorUI* ui = dynamic_cast<IEditorUI*>(&comp);
		m_UIComponents.push_back(ui);
		return comp;
	}
	std::vector<IEditorUI*> m_UIComponents;

	template <class T>
	T* GetComponent()
	{
		//assert(m_Registry->all_of<T>(entity));
		if (!m_Registry->all_of<T>(entity))
			return nullptr;

		return &m_Registry->get<T>(entity);
	}
	std::string m_Name;

	std::vector<GameObject*> m_Children;

	void Destroy();

	bool LoadAsync = false;

private:
};

