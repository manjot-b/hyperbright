#include "Simulate.h"

#include <ctype.h>
#include <vehicle/PxVehicleUtil.h>
#include <iostream>

using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;

PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

PxCooking* gCooking = NULL;

// Actors needed
PxRigidStatic* gGroundPlane = NULL; // ground
PxRigidDynamic* boxCar = NULL;

Simulate::Simulate(std::vector<std::shared_ptr<Model>>& _physicsModels) :
	physicsModels(_physicsModels)
{
	initPhysics();
}

Simulate::~Simulate() {

}

//////////////////////////////////////////////////////////////////////////////////////////////////
void Simulate::initPhysics()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	gScene = gPhysics->createScene(sceneDesc);
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
	cookMeshes();

	gGroundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*gGroundPlane);

	// goal is to take this out and replace with cooked shapes
	PxShape* boxCarShape = gPhysics->createShape(PxBoxGeometry(PxReal(1), PxReal(0.5), PxReal(1.5)), *gMaterial);
	boxCar = gPhysics->createRigidDynamic(PxTransform(PxVec3(PxReal(0), PxReal(6), PxReal(0))));
	boxCar->setAngularVelocity(PxVec3(0, -1, -2));
	boxCar->attachShape(*boxCarShape);
	gScene->addActor(*boxCar);
	
	std::cout << "PhysX Initialized" << std::endl;
}

void Simulate::stepPhysics()
{
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);

	for (auto& model : physicsModels)
	{
		setModelPose(model);
	}
}

void Simulate::setModelPose(std::shared_ptr<Model>& model)
{
	PxU32 numActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (numActors)
	{
		std::vector<PxRigidActor*> actors(numActors);
		gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), numActors);
		
		PxShape* shapes[128]; // max number of shapes per actor is 128
		for (int i = 0; i < numActors; i++)
		{
			const PxU32 numShapes = actors[i]->getNbShapes();
			actors[i]->getShapes(shapes, numShapes);
			for (int j = 0; j < numShapes; j++)
			{
				// for now if the shape is a box (a.k.a. the boxcar) then grab its globalpose
				if (shapes[j]->getGeometryType() == PxGeometryType::eBOX) 
				{
					PxMat44 boxPose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
					glm::mat4 boxUpdate(glm::vec4(boxPose.column0.x, boxPose.column0.y, boxPose.column0.z, 0.0f),
										glm::vec4(boxPose.column1.x, boxPose.column1.y, boxPose.column1.z, 0.0f),
										glm::vec4(boxPose.column2.x, boxPose.column2.y, boxPose.column2.z, 0.0f),
										glm::vec4(boxPose.column3.x, boxPose.column3.y, boxPose.column3.z, 1.0f));
					model->updateModelMatrix(boxUpdate);
					model->setPosition(	glm::vec3(	boxPose.getPosition().x,
													boxPose.getPosition().y,
													boxPose.getPosition().z));
				}
			}
		}
	}
}

void Simulate::cookMeshes()
{
	for (auto& model : physicsModels)
	{
		if (!model->isDynamic())
		{
			std::vector<std::unique_ptr<Mesh>>& meshes = model->getMeshes();
			for (auto& mesh : meshes)
			{
				std::vector<PxVec3> pxVertices;
				std::vector<Vertex> meshVerts = mesh->getVertices();
				std::vector<unsigned int> indices = mesh->getIndices();

				// convert Vertex positions into PxVec3
				for (int i = 0; i < meshVerts.size(); i++)
				{
					pxVertices.push_back(PxVec3(meshVerts[i].position.x, meshVerts[i].position.y, meshVerts[i].position.z));
				}

				PxTriangleMeshDesc meshDesc;
				meshDesc.points.count = pxVertices.size();
				meshDesc.points.stride = sizeof(PxVec3);
				meshDesc.points.data = pxVertices.data();

				meshDesc.triangles.count = indices.size() / 3;
				meshDesc.triangles.stride = 3 * sizeof(PxU32);
				meshDesc.triangles.data = indices.data();

				PxDefaultMemoryOutputStream writeBuffer;
				gCooking->cookTriangleMesh(meshDesc, writeBuffer);

				PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
				PxTriangleMesh* triMesh = gPhysics->createTriangleMesh(readBuffer);

				PxTransform trans(PxVec3(0.f, 10.f, -2.f));
				PxRigidStatic* rigidStat = gPhysics->createRigidStatic(trans);
				PxShape* shape = PxRigidActorExt::createExclusiveShape(*rigidStat, PxTriangleMeshGeometry(triMesh), *gMaterial);
				shape->setLocalPose(trans);

				rigidStat = PxCreateStatic(*gPhysics, trans, *shape);
				rigidStat->attachShape(*shape);
				gScene->addActor(*rigidStat);

				// can't figure out how to create dynamic shapes with cooked mesh
				// Might revisit this issue if time allows.
				/*PxRigidDynamic* rigidDyn = PxCreateDynamic(*gPhysics, trans, PxTriangleMeshGeometry(triMesh), *gMaterial, 1.f);
				rigidDyn->setAngularVelocity(PxVec3(0.f, 0.f, 1.f));
				gScene->addActor(*rigidDyn);*/
			}
		}
	}
}

void Simulate::cleanupPhysics()
{
	gScene->release();
	gDispatcher->release();
	gPhysics->release();
	gCooking->release();
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		transport->release();
	}
	gFoundation->release();

	std::cout << "Cleaned up PhysX" << std::endl;
}
