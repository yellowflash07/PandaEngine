#pragma once
#include <vector>
#include "MeshManager.h"
#include "PhysicsShapes.h"
#include "cAABB.h"
#include <map>

struct PhysicsBody
{
	cMesh* mesh;
	glm::vec3 acceleration;
	glm::vec3 velocity;
	float inverseMass;

	void setShape(PhysicsShapes::sSphere* pSphereProps);
	void setShape(PhysicsShapes::sPlane* pPlaneProps);
	void setShape(PhysicsShapes::sTriangle* pTriangleProps);
	void setShape(PhysicsShapes::sCapsule* pCapsuleProps);
	void setShape(PhysicsShapes::sMeshOfTriangles_Indirect* pTriangleMeshProps);
	void setShape(PhysicsShapes::sMeshOfTriangles_LocalVertices* pTriangleMeshProps);

	std::map<unsigned int, cAABB*> aabbs;

	PhysicsShapes::eShape shapeType;
	void* shape = NULL;
};

class PhysicsManager
{
public:
	PhysicsManager();
	PhysicsManager(MeshManager* meshManager);
	~PhysicsManager();

	void Update(float deltaTime);
	void CheckIntersections(float deltaTime);
	void AddMesh(PhysicsBody* physicsBody);
	std::vector<cAABB*> GenerateAABBs(PhysicsBody* body);
private:
	MeshManager* meshManager;
	std::vector<PhysicsBody*> bodies;
	glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);

	//intersection tests
	bool m_Sphere_Sphere_IntersectionTest(PhysicsBody* pSphereA, PhysicsBody* pShpereB);
	bool m_Sphere_Plane_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pPlane);
	bool m_Sphere_Triangle_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pTriangle);
	bool m_Sphere_AABB_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pAABB);
	bool m_Sphere_Capsule_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pCapsule);
	bool m_Sphere_TriMeshIndirect_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pTriMesh);
	bool m_Sphere_TriMeshLocal_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pTriMesh);

};

