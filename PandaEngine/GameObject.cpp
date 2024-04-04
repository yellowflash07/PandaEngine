#include "GameObject.h"

GameObject::GameObject(std::string name)
{
	m_Name = name;
}

void GameObject::Destroy()
{
	m_Registry->destroy(entity);
}
