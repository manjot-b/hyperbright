#include "Simulate.h"
#include "Controller.h"
#include "Vehicle.h"
#include "DevUI.h"

#include <vehicle/PxVehicleUtil.h>

#include <ctype.h>
#include <iostream>

#include "SnippetVehicleSceneQuery.h"
#include "SnippetVehicleFilterShader.h"
#include "SnippetVehicleTireFriction.h"
#include "SnippetVehicleCreate.h"

#include "SnippetPrint.h"
#include "SnippetPVD.h"
#include "SnippetUtils.h"

using namespace std;
using namespace physx;
using namespace snippetvehicle;
using namespace glm;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;

PxCooking* gCooking = NULL;

PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic* gGroundPlane = NULL;
PxVehicleDrive4W* gVehicle4W[4];


bool					gIsVehicleInAir = true;
std::shared_ptr<AudioPlayer> audioPlayer;

class CollisionCallBack : public physx::PxSimulationEventCallback {
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints);  PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors);  PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors);  PX_UNUSED(count); }
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {}
	void onTrigger(PxTriggerPair* pairs, PxU32 count) {

		//for (physx::PxU32 i = 0; i < count; i++) {
			std::cout << "PICKUP COLLISION \n";
			audioPlayer->playPickupCollision();
		//}
	}
	void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}
};
CollisionCallBack collisionCallBack;


Simulate::Simulate(vector<shared_ptr<IPhysical>>& _physicsModels, vector<shared_ptr<Vehicle>>& _vehicles, const Arena& arena) :
	physicsModels(_physicsModels), vehicles(_vehicles)
{
	initPhysics();

	for (const auto& wall : arena.getWalls())
	{
		cookMeshes(*wall, true);
	}
}

Simulate::~Simulate() {

}

PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f,		0.75f,
	5.0f,		0.75f,
	30.0f,		0.125f,
	120.0f,		0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxVehiclePadSmoothingData gPadSmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxVehicleDrive4WRawInputData gVehicleInputData[4];

enum DriveMode
{
	eDRIVE_MODE_ACCEL_FORWARDS = 0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE
};

PxF32					gVehicleModeLifetime = 4.0f;
PxF32					gVehicleModeTimer = 0.0f;
PxU32					gVehicleOrderProgress = 0;
bool					gVehicleOrderComplete = false;
bool					gMimicKeyInputs = false;

VehicleDesc initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const float vehScale = 1 / 3.5f;
	const PxF32 chassisMass = 800.0f; // default 1500
	const PxVec3 chassisDims(4.f * vehScale, 2.5f * vehScale, 10.1f * vehScale);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 50.0f; // default 10
	const PxF32 wheelRadius = 0.3f;
	const PxF32 wheelWidth = 0.4f;
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 6;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

namespace Driving {
	void startAccelerateForwardsMode(int v)
	{
		gVehicle4W[v]->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalAccel(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogAccel(1.0f);
		}
	}

	void startAccelerateReverseMode(int v)
	{
		gVehicle4W[v]->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalAccel(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogAccel(1.0f);
		}
	}

	void startBrakeMode(int v)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalBrake(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogBrake(1.0f);
		}
	}

	void startTurnHardLeftMode(int v)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalSteerLeft(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogSteer(-1.0f);
		}
	}

	void startTurnHardRightMode(int v)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalSteerRight(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogSteer(1.0f);
		}
	}

	void startHandbrakeTurnLeftMode(int v)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalSteerLeft(true);
			gVehicleInputData[v].setDigitalHandbrake(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogSteer(-1.0f);
			gVehicleInputData[v].setAnalogHandbrake(1.0f);
		}
	}

	void startHandbrakeTurnRightMode(int v)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalSteerRight(true);
			gVehicleInputData[v].setDigitalHandbrake(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogSteer(1.0f);
			gVehicleInputData[v].setAnalogHandbrake(1.0f);
		}
	}

	void releaseAllControls(int v)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalAccel(false);
			gVehicleInputData[v].setDigitalSteerLeft(false);
			gVehicleInputData[v].setDigitalSteerRight(false);
			gVehicleInputData[v].setDigitalBrake(false);
			gVehicleInputData[v].setDigitalHandbrake(false);
		}
		else
		{
			gVehicleInputData[v].setAnalogAccel(0.0f);
			gVehicleInputData[v].setAnalogSteer(0.0f);
			gVehicleInputData[v].setAnalogBrake(0.0f);
			gVehicleInputData[v].setAnalogHandbrake(0.0f);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void Simulate::initPhysics()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	if (!gFoundation) {
		std::cout << "PxCreateFoundation failed!\n";
	}

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.simulationEventCallback = &collisionCallBack;

	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;
	sceneDesc.simulationEventCallback = &collisionCallBack; // SET OUR COLLISION DETECTION
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

	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	gGroundPlane->userData = NULL;
	gScene->addActor(*gGroundPlane);

	//Create Vehicle bodies
	VehicleDesc vehicleDesc = initVehicleDesc();
	vector<shared_ptr<Vehicle>>::iterator iter = vehicles.begin();
	for (int i = 0;  i < 2; i++, iter++) {
		gVehicle4W[i] = createVehicle4W(vehicleDesc, gPhysics, gCooking);
		Vehicle* vehicle = iter->get();

		vec3 playerStartPos = vehicle->getPosition();
		quat playerOrientation = vehicle->getOrientation();
		std::cout << "car: " << vehicle->getId() << " initialized" << std::endl;
		PxQuat playerPxOrientation(playerOrientation.x, playerOrientation.y, playerOrientation.z, playerOrientation.w);
		PxTransform startTransform(PxVec3(playerStartPos.x, playerStartPos.y, playerStartPos.z), playerPxOrientation);
		gVehicle4W[i]->getRigidDynamicActor()->setGlobalPose(startTransform);

		// TO-DO: Rather than storing a string id for each vehicle, see if we can store the pointer
		// to the vehicle itself.
		gVehicle4W[i]->getRigidDynamicActor()->userData = (void*)vehicle->getId();
		gScene->addActor(*gVehicle4W[i]->getRigidDynamicActor());
	}

	///////////////////////////////////// BOX
	/*PxFilterData obstFilterData(snippetvehicle::COLLISION_FLAG_OBSTACLE, snippetvehicle::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	PxShape* boxWall = gPhysics->createShape(PxBoxGeometry(0.7f, 0.7f, 0.7f), *gMaterial, false);
	PxRigidStatic* wallActor = gPhysics->createRigidStatic(PxTransform(PxVec3(0,0,0)));
	boxWall->setSimulationFilterData(obstFilterData);

	boxWall->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);//FLAGS TO SET AS TRIGGER VOLUME
	boxWall->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	wallActor->setGlobalPose(PxTransform(PxVec3(0,0.7f,5)));
	wallActor->attachShape(*boxWall);
	wallActor->userData = (void*)physicsModels[2]->getId().c_str();
	gScene->addActor(*wallActor);*/

	std::cout << "PhysX Initialized" << std::endl;
}

void setDriveMode(VehicleController ctrl)
{
	int vNum = ctrl.contrId;

	Driving::releaseAllControls(vNum);
	//FORWARD OR BACKWARD
	if (ctrl.input[0]) {
		Driving::startAccelerateForwardsMode(vNum);
	}
	else if (ctrl.input[1]) {
		Driving::startAccelerateReverseMode(vNum);
	}

	//LEFT OR RIGHT
	if (ctrl.input[2]) {
		Driving::startTurnHardLeftMode(vNum);
	}
	else if (ctrl.input[3]) {
		Driving::startTurnHardRightMode(vNum);
	}
}

void smoothControlValues(int vNum, float frameRate) {
	if (gMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData[vNum], frameRate, gIsVehicleInAir, *gVehicle4W[vNum]);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData[vNum], frameRate, gIsVehicleInAir, *gVehicle4W[vNum]);
	}
}

void Simulate::stepPhysics(float frameRate)
{
	//Cycle through the vehicles and set there driving mode
	for (auto& vehicle : vehicles) {
		VehicleController& ctrl = vehicle->getController();
		setDriveMode(ctrl);
		smoothControlValues(ctrl.contrId, frameRate);
	}

	//////////////////// PLAYER
	//Raycasts.
	PxVehicleWheels* pxVehicles[1] = { gVehicle4W[0] };
	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, 1, pxVehicles, raycastResultsSize, raycastResults);
	
	//Vehicle update.
	const PxVec3 grav = gScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W[0]->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(frameRate, grav, *gFrictionPairs, 1, pxVehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	gIsVehicleInAir = gVehicle4W[0]->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	////////////////////// AI
		//Raycasts.
	PxVehicleWheels* pxVehicles_ai[1] = { gVehicle4W[1] };
	PxRaycastQueryResult* raycastResults_ai = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize_ai = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, 1, pxVehicles_ai, raycastResultsSize_ai, raycastResults_ai);

	//Vehicle update.
	const PxVec3 grav_ai = gScene->getGravity();
	PxWheelQueryResult wheelQueryResults_ai[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults_ai[1] = { {wheelQueryResults_ai, gVehicle4W[1]->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(frameRate, grav_ai, *gFrictionPairs, 1, pxVehicles_ai, vehicleQueryResults_ai);

	//Work out if the vehicle is in the air.
	gIsVehicleInAir = gVehicle4W[1]->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	//Scene update.
	gScene->simulate(frameRate);
	gScene->fetchResults(true);

	for (auto& model : physicsModels)
	{
		setModelPose(model);
	}

	vehicles[0]->updatePositionAndDirection();
	/*for (auto& vehicle : vehicles) {
		vehicle->updatePositionAndOrientation();
	}*/
}

void Simulate::setModelPose(std::shared_ptr<IPhysical>& model)
{
	PxU32 numActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (numActors)
	{
		std::vector<PxRigidActor*> actors(numActors);
		gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), numActors);
		//std::cout << "number of actors: " << numActors << std::endl;


		PxShape* shapes[128]; // max number of shapes per actor is 128
		for (int i = 0; i < numActors; i++)
		{
			const PxU32 numShapes = actors[i]->getNbShapes();
			actors[i]->getShapes(shapes, numShapes);
			
			if (actors[i]->userData != NULL) {
				const char* actorName = reinterpret_cast<const char*>(actors[i]->userData);
				const char* modelName = model->getId();
				
				if (actorName == modelName) {
					PxMat44 boxPose;
					if (numShapes > 0) {
						boxPose = (PxShapeExt::getGlobalPose(*shapes[6], *actors[i]));
					}
					else {
						boxPose = (PxShapeExt::getGlobalPose(*shapes[0], *actors[i]));
					}
					mat4 modelMatrix;
					memcpy(&modelMatrix, &boxPose, sizeof(PxMat44));
					model->setModelMatrix(modelMatrix);

					vec3 modelPos;
					memcpy(&modelPos, &(boxPose.getPosition()), sizeof(PxVec3));
					model->setPosition(modelPos);
					
				}
			}
		}
	}
}

void Simulate::checkVehiclesOverTile(Arena& arena, const std::vector<std::shared_ptr<Vehicle>>& vehicles)
{
	for (const auto& vehicle : vehicles)
	{
		std::optional<glm::vec2> tileCoords = arena.isOnTile(vehicle->getPosition());
		if (tileCoords)
		{
			vehicle->currentTile = *tileCoords;
			arena.setTileColor(*tileCoords, vehicle->getColor());
			
			
			glm::vec3 worldCoords = arena.getTilePos(*tileCoords);
		}
	}
}

void Simulate::cookMeshes(const Model& model, bool useModelMatrix)
{
	const std::vector<std::unique_ptr<Mesh>>& meshes = model.getMeshes();
	for (auto& mesh : meshes)
	{
		std::vector<PxVec3> pxVertices;
		const std::vector<Vertex>& meshVerts = mesh->getVertices();
		const std::vector<unsigned int>& indices = mesh->getIndices();

		// convert Vertex positions into PxVec3
		for (int i = 0; i < meshVerts.size(); i++)
		{
			glm::vec4 mpos = glm::vec4(meshVerts[i].position, 1.f);
			if (useModelMatrix)
			{
				mpos = mpos * model.getModelMatrix();
			}
			PxVec3 pos;
			std::memcpy(&pos, &mpos, sizeof(PxVec3));
			pxVertices.push_back(pos);
		}

		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = pxVertices.size();
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = pxVertices.data();

		meshDesc.triangles.count = indices.size() / 3;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = indices.data();

		PxDefaultMemoryOutputStream writeBuffer;


		if (!gCooking->cookTriangleMesh(meshDesc, writeBuffer))
		{
			std::cout << "Error: Could not cook triangle mesh.\n";
		}

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxTriangleMesh* triMesh = gPhysics->createTriangleMesh(readBuffer);

		PxVec3 pos;
		std::memcpy(&pos, &(model.getPosition()), sizeof(PxVec3));
		PxTransform trans(pos);

		PxRigidStatic* rigidStat = gPhysics->createRigidStatic(PxTransform(pos));
		PxShape* shape = gPhysics->createShape(PxTriangleMeshGeometry(triMesh), *gMaterial, true);
		// This is needed for collisions to work.
		PxFilterData obstFilterData(snippetvehicle::COLLISION_FLAG_OBSTACLE, snippetvehicle::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);

		shape->setSimulationFilterData(obstFilterData);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		rigidStat->attachShape(*shape);
		gScene->addActor(*rigidStat);

		// can't figure out how to create dynamic shapes with cooked mesh
		// Might revisit this issue if time allows.
		/*PxRigidDynamic* rigidDyn = PxCreateDynamic(*gPhysics, trans, PxTriangleMeshGeometry(triMesh), *gMaterial, 1.f);
		rigidDyn->setAngularVelocity(PxVec3(0.f, 0.f, 1.f));
		gScene->addActor(*rigidDyn);*/
	}
}

void Simulate::cleanupPhysics()
{
	gVehicle4W[0]->getRigidDynamicActor()->release();
	gVehicle4W[0]->free();
	gVehicle4W[1]->getRigidDynamicActor()->release();
	gVehicle4W[1]->free();

	PX_RELEASE(gGroundPlane);
	PX_RELEASE(gBatchQuery);
	gVehicleSceneQueryData->free(gAllocator);
	PX_RELEASE(gFrictionPairs);
	PxCloseVehicleSDK();

	PX_RELEASE(gMaterial);
	PX_RELEASE(gCooking);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);

	std::cout << "Cleaned up PhysX" << std::endl;
}

void Simulate::setAudioPlayer(std::shared_ptr<AudioPlayer> player) {
	audioPlayer = player;
}