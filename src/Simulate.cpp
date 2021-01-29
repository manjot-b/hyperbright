#include "Simulate.h"

#include <ctype.h>
#include <PxPhysicsAPI.h>
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

Simulate::Simulate() {
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

	gGroundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*gGroundPlane);

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
}

void Simulate::setModelPose(std::unique_ptr<Model>& model)
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

void Simulate::cleanupPhysics()
{
	gScene->release();
	gDispatcher->release();
	gPhysics->release();
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		transport->release();
	}
	gFoundation->release();

	std::cout << "Cleaned up PhysX" << std::endl;
}
