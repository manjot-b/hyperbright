#include "Simulate.h"
#include "engine/Controller.h"
#include "entity/Vehicle.h"
#include "entity/PickupManager.h"
#include "ui/DevUI.h"
#include "engine/TeamStats.h"

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

namespace hyperbright {
namespace physics {

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

int const number_of_vehicles = 4;
PxVehicleDrive4W* gVehicle4W[number_of_vehicles];

bool gIsVehicleInAir[4] = { true, true, true, true };
std::shared_ptr<audio::AudioPlayer> audioPlayer;
std::shared_ptr<entity::PickupManager> pum;
std::queue<std::shared_ptr<entity::Pickup>> toBeRemovedPickups;

class CollisionCallBack : public physx::PxSimulationEventCallback {
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints);  PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors);  PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors);  PX_UNUSED(count); }
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {}
	void onTrigger(PxTriggerPair* pairs, PxU32 count) {
		for (physx::PxU32 i = 0; i < count; i++) {
			if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
				PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;
			
			// Make sure actors that partake in collisions have their userData field set
			// to an IPhysical.
			IPhysical* first = static_cast<IPhysical*>(pairs[i].triggerActor->userData);
			IPhysical* second = static_cast<IPhysical*>(pairs[i].otherActor->userData);

			if (first->getTriggerType() == IPhysical::TriggerType::CHARGING_STATION) {
				audioPlayer->playPowerstationCollision();
				cout << "Station collision detected \n";
				entity::Vehicle* v = dynamic_cast<entity::Vehicle*>(second);
				v->restoreEnergy();
			}
			else if (first->getTriggerType() == IPhysical::TriggerType::PICKUP) {
				audioPlayer->playPickupCollision();
				cout << "Pickup collision detected" << endl;
				entity::Vehicle* v = dynamic_cast<entity::Vehicle*>(second);
				std::shared_ptr<entity::Pickup> p = pum->handlePickupOnCollision(v);
				if (p->pickupNumber != 0) {
					toBeRemovedPickups.push(p);
				}
			}
		}
	}
	void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}
};
CollisionCallBack collisionCallBack;

void addPickup(std::shared_ptr<entity::Pickup>& pickup) {
	PxFilterData obstFilterData(snippetvehicle::COLLISION_FLAG_OBSTACLE, snippetvehicle::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	PxShape* pickupBox = gPhysics->createShape(PxBoxGeometry(0.5f, 1.5f, 0.5f), *gMaterial, false);
	vec3 pickupLocation = pickup->getArenaLocation();
	PxRigidStatic* pickupActor = gPhysics->createRigidStatic(PxTransform(pickupLocation.x, pickupLocation.y, pickupLocation.z));

	pickupBox->setSimulationFilterData(obstFilterData);

	pickupBox->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);//FLAGS TO SET AS TRIGGER VOLUME
	pickupBox->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	pickupActor->attachShape(*pickupBox);
	pickupActor->userData = static_cast<IPhysical*>(pickup.get());
	pickupActor->setName("pickup");
	gScene->addActor(*pickupActor);
}

void removePickups() {
	while (toBeRemovedPickups.size() > 0) {
		std::shared_ptr<entity::Pickup>& pickup = toBeRemovedPickups.front();
		PxU32 numActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
		if (numActors) {
			std::vector<PxRigidActor*> actors(numActors);
			gScene->getActors(PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), numActors);

			for (int i = 0; i < numActors; i++)
			{
				if (actors[i]->userData) {
					IPhysical* pu = static_cast<IPhysical*>(actors[i]->userData);
					if (pu->getTriggerType() == IPhysical::TriggerType::PICKUP) {

						entity::Pickup* pickupPtr = dynamic_cast<entity::Pickup*>(pu);

						if (pickupPtr->pickupNumber == pickup->pickupNumber) {
							gScene->removeActor(*actors[i]);
							actors[i]->release();
							toBeRemovedPickups.pop();
							break;
						}
					}
				}
			}
		}
	}
}


Simulate::Simulate(vector<shared_ptr<IPhysical>>& _physicsModels, vector<shared_ptr<entity::Vehicle>>& _vehicles, const entity::Arena& arena, std::shared_ptr<entity::PickupManager>& _pickupManager) :
	physicsModels(_physicsModels), vehicles(_vehicles)
{
	initPhysics();
	pum = _pickupManager;
	for (const auto& wall : arena.getWalls())
	{
		// Store nullptr for the userData because we don't ever do anything with the walls.
		// If we do eventually need to perform some logic on the walls when a collision happens,
		// then the walls will need to extend IPhysical and set the TriggerType to the appropriate value.
		cookMeshes(*wall, nullptr, true);
	}
	addChargingStations(arena.getChargingStations());
}

Simulate::~Simulate() {

}

void Simulate::addChargingStations(const entity::Arena::ChargingStationList& stations)
{

	PxFilterData obstFilterData(snippetvehicle::COLLISION_FLAG_OBSTACLE, snippetvehicle::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);

	for (const auto& station : stations)
	{
		const glm::vec3& worldCoords = station->getWorldCoords();


		PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.5f, 1.5f, 0.5f), *gMaterial, false);
		PxRigidStatic* actor = gPhysics->createRigidStatic(PxTransform(PxVec3(worldCoords.x, 1.5f, worldCoords.z)));

		shape->setSimulationFilterData(obstFilterData);

		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);//FLAGS TO SET AS TRIGGER VOLUME
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

		actor->attachShape(*shape);
		physics::IPhysical* stationPtr = station.get();
		actor->userData = (void*)stationPtr;
		actor->setName("station");
		gScene->addActor(*actor);
	}
}

PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f,		1.0f,
	4.0f,		0.6f,
	20.0f,		0.35f,
	90.0f,		0.2f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		3.0f,	//rise rate eANALOG_INPUT_ACCEL
		5.0f,	//rise rate eANALOG_INPUT_BRAKE		
		10.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		0.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		0.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		4.0f,	//fall rate eANALOG_INPUT_ACCEL
		5.0f,	//fall rate eANALOG_INPUT_BRAKE		
		12.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		3.f,	//fall rate eANALOG_INPUT_STEER_LEFT
		3.f	//fall rate eANALOG_INPUT_STEER_RIGHT
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
bool					gMimicKeyInputs = true;

VehicleDesc initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const float vehScale = 1 / 3.f;
	const PxF32 chassisMass = 1000.0f; // default 1500
	const PxVec3 chassisDims(3.7f * vehScale, 2.5f * vehScale, 10.1f * vehScale);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.5f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 10.0f; // default 10
	const PxF32 wheelRadius = 1.f * vehScale;
	const PxF32 wheelWidth = 0.1f;
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
	vehicleDesc.wheelSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

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

	void applyVehicleFlipImpulse(int v)
	{
		PxRigidBodyExt::addLocalForceAtLocalPos(*gVehicle4W[v]->getRigidDynamicActor(), PxVec3(-1000.f, -3500.f, 0.f), PxVec3(1.f, 0.f, 0.f), PxForceMode::eIMPULSE);
	}
}//namespace Driving

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
	sceneDesc.gravity = PxVec3(0.0f, -18.f, 0.0f);
	sceneDesc.simulationEventCallback = &collisionCallBack;

	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;
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
	PxRigidStatic* gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	
	// Store nullptr for the userData because we don't ever do anything with the ground.
	// If we do eventually need to perform some logic on the walls when a collision happens,
	// then we need to create a Ground class that extends IPhysical and sets the TriggerType to the appropriate value.
	gGroundPlane->userData = nullptr;
	gScene->addActor(*gGroundPlane);

	//Create Vehicle bodies
	VehicleDesc vehicleDesc = initVehicleDesc();
	vector<shared_ptr<entity::Vehicle>>::iterator iter = vehicles.begin();
	for (int i = 0; i < number_of_vehicles; i++, iter++) {
		gVehicle4W[i] = createVehicle4W(vehicleDesc, gPhysics, gCooking);
		entity::Vehicle* vehicle = iter->get();

		vec3 playerStartPos = vehicle->getPosition();
		quat playerOrientation = vehicle->getOrientation();
		PxQuat playerPxOrientation(playerOrientation.x, playerOrientation.y, playerOrientation.z, playerOrientation.w);
		PxTransform startTransform(PxVec3(playerStartPos.x, playerStartPos.y, playerStartPos.z), playerPxOrientation);
		gVehicle4W[i]->getRigidDynamicActor()->setGlobalPose(startTransform);

		// Convert the Vehicle to its base type for easy conversion.
		gVehicle4W[i]->getRigidDynamicActor()->userData = static_cast<IPhysical*>(vehicle);
		gVehicle4W[i]->getRigidDynamicActor()->setName(vehicle->getId());
		gScene->addActor(*gVehicle4W[i]->getRigidDynamicActor());

		std::cout << "car: " << vehicle->getId() << " initialized" << std::endl;
	}
	std::cout << "PhysX Initialized" << std::endl;
}

void setDriveMode(entity::VehicleController* ctrl)
{
	int vNum = ctrl->contrId;

	if (ctrl->flipImpulse) {
		Driving::applyVehicleFlipImpulse(vNum);
		ctrl->flipImpulse = false;
	}

	Driving::releaseAllControls(vNum);
	//BRAKE OR FORWARD OR BACKWARD
	if (ctrl->input[5]) {
		Driving::startBrakeMode(vNum);
	} 
	else if (ctrl->input[0]) {
		Driving::startAccelerateForwardsMode(vNum);
	}
	else if (ctrl->input[1]) {
		Driving::startAccelerateReverseMode(vNum);
	}

	//LEFT OR RIGHT
	if (ctrl->input[2]) {
		if (ctrl->input[4]) {
			Driving::startHandbrakeTurnLeftMode(vNum);
		}
		else {
			Driving::startTurnHardLeftMode(vNum);
		}
	}
	else if (ctrl->input[3]) {
		if (ctrl->input[4]) {
			Driving::startHandbrakeTurnRightMode(vNum);
		}
		else {
			Driving::startTurnHardRightMode(vNum);
		}
	}
}

void smoothControlValues(int vNum, float frameRate) {
	if (gMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData[vNum], frameRate, gIsVehicleInAir[vNum], *gVehicle4W[vNum]);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData[vNum], frameRate, gIsVehicleInAir[vNum], *gVehicle4W[vNum]);
	}
}

void Simulate::stepPhysics(float frameRate)
{
	//Cycle through the vehicles and set there driving mode
	for (auto& vehicle : vehicles) {
		entity::VehicleController* ctrl = &vehicle->getController();
		setDriveMode(ctrl);
		smoothControlValues(ctrl->contrId, frameRate);
	}

	// Vehicle wheel raycasts and state updating
	for (int i = 0; i < number_of_vehicles; i++) {
		//Raycasts.
		PxVehicleWheels* pxVehicles[1] = { gVehicle4W[i] };
		PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
		PxVehicleSuspensionRaycasts(gBatchQuery, 1, pxVehicles, raycastResultsSize, raycastResults);

		//Vehicle update.
		const PxVec3 grav = gScene->getGravity();
		PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
		PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W[i]->mWheelsSimData.getNbWheels()} };
		PxVehicleUpdates(frameRate, grav, *gFrictionPairs, 1, pxVehicles, vehicleQueryResults);

		//Work out if the vehicle is in the air.
		gIsVehicleInAir[i] = gVehicle4W[i]->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
	}

	//Scene update.
	gScene->simulate(frameRate);
	gScene->fetchResults(true);

	// Update graphics models based on their phsyX model
	for (auto& model : physicsModels)
	{
		setModelPose(model);
	}

	// update the stats of each vehicle
	for (auto& vehicle : vehicles) {
		vehicle->updateOrientation();
		vehicle->updateSpeedometer(frameRate);
		if (vehicle->getTeam() == engine::teamStats::Teams::TEAM0) {
			// print out aspects of the player vehicle per frame here

		}
	}
}

void Simulate::setModelPose(std::shared_ptr<IPhysical>& model)
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
			
			if (actors[i]->userData != NULL) {
				//const char* actorName = reinterpret_cast<const char*>(actors[i]->userData);
				const char* actorName = actors[i]->getName();
				const char* modelName = model->getId();

				if (actorName == modelName) {
					PxMat44 boxPose;
					if (numShapes > 0) {
						// as of now the only actors with more than 1 shape are the vehicles
						// if more complex actors are created then we need another condition to
						// update the correct model in the actor
						boxPose = (PxShapeExt::getGlobalPose(*shapes[6], *actors[i])); // shapes[6] = vehicle chassis 
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

					// Check if this actor is a vehicle and if so, set its wheels pose.
					for (const auto& name : engine::teamStats::names)
					{
						if (name.second.c_str() == actorName)
						{
							entity::Vehicle& vehicle = static_cast<entity::Vehicle&>(*model);

							PxMat44 frontRight = (PxShapeExt::getGlobalPose(*shapes[0], *actors[i]));
							PxMat44 frontLeft = (PxShapeExt::getGlobalPose(*shapes[1], *actors[i]));
							PxMat44 rearRight = (PxShapeExt::getGlobalPose(*shapes[2], *actors[i]));
							PxMat44 rearLeft = (PxShapeExt::getGlobalPose(*shapes[3], *actors[i]));

							glm::mat4 fr, fl, rr, rl;
							memcpy(&fr, &frontRight, sizeof(PxMat44));
							memcpy(&fl, &frontLeft, sizeof(PxMat44));
							memcpy(&rr, &rearRight, sizeof(PxMat44));
							memcpy(&rl, &rearLeft, sizeof(PxMat44));

							vehicle.setWheelsModelMatrix(fl, fr, rr, rl);
							break;
						}
					}
				}
			}
		}
	}
}

void Simulate::checkVehiclesOverTile(entity::Arena& arena, const std::vector<std::shared_ptr<entity::Vehicle>>& vehicles)
{
	for (const auto& vehicle : vehicles)
	{
		std::optional<glm::vec2> tileCoords = arena.isOnTile(vehicle->getPosition());
		if (tileCoords)
		{
			vehicle->currentTile = *tileCoords;

			if (vehicle->enoughEnergy() && !arena.tileHasChargingStation(*tileCoords))
			{
				std::optional<engine::teamStats::Teams> old = arena.getTeamOnTile(*tileCoords);

				if (old && *old != vehicle->getTeam())
				{
					engine::teamStats::scores[*old]--;
					engine::teamStats::scores[vehicle->getTeam()]++;
					arena.setTileTeam(*tileCoords, vehicle->getTeam());
					vehicle->reduceEnergy();
				}
				else if (!old)
				{
					engine::teamStats::scores[vehicle->getTeam()]++;
					arena.setTileTeam(*tileCoords, vehicle->getTeam());
					vehicle->reduceEnergy();
				}
			}
		}
	}
}

void Simulate::cookMeshes(const model::Model& model, void* _userData, bool useModelMatrix)
{
	const std::vector<std::unique_ptr<model::Mesh>>& meshes = model.getMeshes();
	for (auto& mesh : meshes)
	{
		std::vector<PxVec3> pxVertices;
		const std::vector<model::Vertex>& meshVerts = mesh->getVertices();
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
		rigidStat->userData = _userData;
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
	PxU32 numActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (numActors)
	{
		std::vector<PxRigidActor*> actors(numActors);
		gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), numActors);
		for (int i = 0; i < numActors; i++) {
			actors[i]->release();
		}
	}

	for (auto& vehicle : gVehicle4W) {
		vehicle->free();
	}

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

void Simulate::setAudioPlayer(std::shared_ptr<audio::AudioPlayer> player) {
	audioPlayer = player;
}

}	// namespace physics
}	// namespace hyperbright