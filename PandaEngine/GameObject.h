#pragma once
#include <entt/entt.hpp>
#include <string>

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
		return m_Registry->emplace<T>(entity, std::forward<Args>(args)...);
	}
private:
	std::string m_Name;
};

