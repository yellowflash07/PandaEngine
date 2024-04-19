#include "GameObject.h"

GameObject::GameObject(std::string name)
{
	m_Name = name;
}

GameObject::~GameObject()
{
	m_Registry->destroy(entity);
}
