#pragma once
#include <entt/entt.hpp>
#include <string>
#include "TransformComponent.h"

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
		return m_Registry->emplace<T>(entity, std::forward<Args>(args)...);
	}

	template <class T>
	T& GetComponent()
	{
		assert(m_Registry->all_of<T>(entity));
		return m_Registry->get<T>(entity);
	}

private:
	std::string m_Name;
};

