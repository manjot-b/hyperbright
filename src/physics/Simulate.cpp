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

PxVehicleDrive4W* gVehicle4W[4];

bool gIsVehicleInAir[4] = { true, true, true, true };
std::shared_ptr<audio::AudioPlayer> audioPlayer;
std::shared_ptr<entity::PickupManager> pum;
std::queue<std::shared_ptr<entity::Pickup>> toBeRemovedPickups;
vector<shared_ptr<entity::Vehicle>>* vehs = NULL;
class CollisionCallBack : public physx::PxSimulationEventCallback {
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { cout << "constraint break\n"; PX_UNUSED(constraints);  PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { cout << "wake up\n"; PX_UNUSED(actors);  PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { cout << "sleep\n"; PX_UNUSED(actors);  PX_UNUSED(count); }
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) { 
		for (PxU32 i = 0; i < nbPairs; i++) {
			// only make collision sounds for player vehicle
			if (pairHeader.actors[0] == gVehicle4W[0]->getRigidDynamicActor() ||
				pairHeader.actors[1] == gVehicle4W[0]->getRigidDynamicActor()) {

				// quick and sloppy pair detection
				// the only way this call can be thrown is if a vehicle
				// hits a wall or another vehicle so just check for those 2 things
				if (pairHeader.actors[0]->userData == nullptr ||
					pairHeader.actors[1]->userData == nullptr) {
					// collided with wall, make wall collision sound
					audioPlayer->playCarCollisionSound();
				}
				else {
					// collided with vehicle, make vehicle collision sound
					audioPlayer->playCarCollisionSound();
				}
			}
		}
	}
	void onTrigger(PxTriggerPair* pairs, PxU32 count) {
		for (PxU32 i = 0; i < count; i++) {
			if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
				PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;
			
			// Make sure actors that partake in collisions have their userData field set
			// to an IPhysical.
			IPhysical* first = static_cast<IPhysical*>(pairs[i].triggerActor->userData);
			IPhysical* second = static_cast<IPhysical*>(pairs[i].otherActor->userData);

			if (first->getTriggerType() == IPhysical::TriggerType::CHARGING_STATION) {
				entity::Vehicle* v = dynamic_cast<entity::Vehicle*>(second);
				// This collision will repeat so only trigger it's effect if the 
				// vehicle needs to be recharged.
				if (!v->fullEnergy()) {

					if (v->getTeam() == vehs->at(0)->getTeam()) {
						audioPlayer->playPowerstationCollision();
					}
					v->restoreEnergy();
				}
			}
			else if (first->getTriggerType() == IPhysical::TriggerType::PICKUP) {
				entity::Vehicle* v = dynamic_cast<entity::Vehicle*>(second);

				if (!v->hasPickup()) {
					std::shared_ptr<entity::Pickup> p = pum->handlePickupOnCollision(v);
					if (v->getTeam() == vehs->at(0)->getTeam()) {
						audioPlayer->playPickupCollision();
					}

					if (p->pickupNumber != 0) {
						toBeRemovedPickups.push(p);
					}
				}
			}
		}
	}
	void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) { cout << "advance collision\n"; }
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
	arenaSize = (arena.getArenaSize()/2.f - 1.f) * arena.getTileWidth();
	vehs = &_vehicles;
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

		glm::vec3 dim = station->getDimensions();
		PxBoxGeometry dimensions = PxBoxGeometry(dim.x * .5f, dim.y * .5f, dim.z * .5f);

		PxShape* shape = gPhysics->createShape(dimensions, *gMaterial, false);
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

float velStepOne = 0.f;
float stepOneTurnStr = 0.7f;
float velStepTwo = 7.f;
float stepTwoTurnStr = 0.6f;
float velStepThr = 18.f;
float stepThrTurnStr = 0.33f;
float velStepFou = 25.f;
float stepFouTurnStr = 0.15f;
PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	velStepOne,		stepOneTurnStr,
	velStepTwo,		stepTwoTurnStr,
	velStepThr,		stepThrTurnStr,
	velStepFou,		stepFouTurnStr,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		12.0f,	//rise rate eANALOG_INPUT_ACCEL
		5.0f,	//rise rate eANALOG_INPUT_BRAKE		
		20.f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		1.f,	//rise rate eANALOG_INPUT_STEER_LEFT
		1.f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		2.f,	//fall rate eANALOG_INPUT_ACCEL
		20.0f,	//fall rate eANALOG_INPUT_BRAKE		
		20.f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		10.f,	//fall rate eANALOG_INPUT_STEER_LEFT
		10.f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

float analogAccel = 14.0f;
float analogBrake = 3.0f;
float analogHandBrake = 20.f;
float analogSteer = 1.5f;

float analogAccelFall = 3.0f;
float analogBrakeFall = 20.0f;
float analogHandBrakeFall = 20.f;
float analogSteerFall = 10.f;

PxVehiclePadSmoothingData gPadSmoothingData =
{
	{
		analogAccel,	//rise rate eANALOG_INPUT_ACCEL
		analogBrake,	//rise rate eANALOG_INPUT_BRAKE		
		analogHandBrake,	//rise rate eANALOG_INPUT_HANDBRAKE	
		analogSteer,	//rise rate eANALOG_INPUT_STEER_LEFT
		analogSteer,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},		  
	{		  
		analogAccelFall,	//fall rate eANALOG_INPUT_ACCEL
		analogBrakeFall,	//fall rate eANALOG_INPUT_BRAKE		
		analogHandBrakeFall,	//fall rate eANALOG_INPUT_HANDBRAKE	
		analogSteerFall, 	//fall rate eANALOG_INPUT_STEER_LEFT
		analogSteerFall		//fall rate eANALOG_INPUT_STEER_RIGHT
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

bool		gMimicKeyInputs = true;
const PxF32 normalChassisMass = 1400.0f;
const PxF32 slowChassisMass = 3000.0f;
const PxF32 fastChassisMass = 600.0f;

VehicleDesc initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const float vehScale = 1 / 3.f;
	const PxF32 chassisMass = normalChassisMass;
	const PxVec3 chassisDims(3.7f * vehScale, 2.5f * vehScale, 10.1f * vehScale);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.42f, 0.2f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f; // default 10
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
	void startAccelerateForwardsMode(int v, float aDrive)
	{
		gVehicle4W[v]->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalAccel(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogAccel(aDrive);
		}
	}

	void startAccelerateReverseMode(int v, float aDrive)
	{
		gVehicle4W[v]->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalAccel(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogAccel(aDrive);
		}
	}

	void startBrakeMode(int v, float aDrive)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalBrake(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogBrake(aDrive);
		}
	}

	void startTurnHardLeftMode(int v, float aSteer)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalSteerLeft(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogSteer(-aSteer);
		}
	}

	void startTurnHardRightMode(int v, float aSteer)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalSteerRight(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogSteer(-aSteer);
		}
	}

	void startHandbrakeTurnLeftMode(int v, float aSteer)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalSteerLeft(true);
			gVehicleInputData[v].setDigitalHandbrake(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogSteer(-aSteer);
			gVehicleInputData[v].setAnalogHandbrake(1.0f);
		}
	}

	void startHandbrakeTurnRightMode(int v, float aSteer)
	{
		if (gMimicKeyInputs)
		{
			gVehicleInputData[v].setDigitalSteerRight(true);
			gVehicleInputData[v].setDigitalHandbrake(true);
		}
		else
		{
			gVehicleInputData[v].setAnalogSteer(-aSteer);
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
		PxF32 mass = gVehicle4W[v]->getRigidDynamicActor()->getMass();
		PxRigidBodyExt::addLocalForceAtLocalPos(*gVehicle4W[v]->getRigidDynamicActor(), PxVec3(-mass * 2.f, -3.f * mass, 0.f), PxVec3(1.f, 0.f, 0.f), PxForceMode::eIMPULSE);
	}
	void applyVehicleBoost(int v)
	{
		PxF32 mass = gVehicle4W[v]->getRigidDynamicActor()->getMass();
		if (gVehicle4W[v]->computeForwardSpeed() < 40) 
			PxRigidBodyExt::addLocalForceAtLocalPos(*gVehicle4W[v]->getRigidDynamicActor(), PxVec3(0.f, 0.f, mass * 0.75f), PxVec3(0.f, -0.1f, 0.2f), PxForceMode::eIMPULSE);
	}
	void applyVehicleTrap(int v)
	{
		PxF32 mass = gVehicle4W[v]->getRigidDynamicActor()->getMass();
		if (gVehicle4W[v]->computeForwardSpeed() > 5)
			PxRigidBodyExt::addLocalForceAtLocalPos(*gVehicle4W[v]->getRigidDynamicActor(), PxVec3(0.f, 0.f, -mass * 1.5f), PxVec3(0.f, 0.1f, -1.f), PxForceMode::eIMPULSE);
	}
}//namespace Driving

//////////////////////////////////////////////////////////////////////////////////////////////////
void Simulate::initPhysics()
{
	if (!gFoundation) gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

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
	PxRigidStatic* gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics, PxVec3(0, 1, 0), 0.f);
	
	// Store nullptr for the userData because we don't ever do anything with the ground.
	// If we do eventually need to perform some logic on the walls when a collision happens,
	// then we need to create a Ground class that extends IPhysical and sets the TriggerType to the appropriate value.
	gGroundPlane->userData = nullptr;
	gScene->addActor(*gGroundPlane);
	

	PxRigidStatic* barrier1 = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics, PxVec3(1, 0, 0), arenaSize.x + 0.1f);
	barrier1->userData = nullptr;
	gScene->addActor(*barrier1);

	PxRigidStatic* barrier2 = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics, PxVec3(-1, 0, 0), arenaSize.x + 0.1f);
	barrier2->userData = nullptr;
	gScene->addActor(*barrier2);

	PxRigidStatic* barrier3 = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics, PxVec3(0, 0, 1), arenaSize.y + 0.1f);
	barrier3->userData = nullptr;
	gScene->addActor(*barrier3);

	PxRigidStatic* barrier4 = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics, PxVec3(0, 0, -1), arenaSize.y + 0.1f);
	barrier4->userData = nullptr;
	gScene->addActor(*barrier4);

	//Create Vehicle bodies
	VehicleDesc vehicleDesc = initVehicleDesc();
	vector<shared_ptr<entity::Vehicle>>::iterator iter = vehicles.begin();
	for (int i = 0; i < vehicles.size(); i++, iter++) {
		gVehicle4W[i] = createVehicle4W(vehicleDesc, gPhysics, gCooking);
		entity::Vehicle* vehicle = iter->get();

		vec3 playerStartPos = vehicle->getPosition();
		quat playerOrientation = vehicle->getOrientation();
		PxQuat playerPxOrientation(playerOrientation.x, playerOrientation.y, playerOrientation.z, playerOrientation.w);
		PxTransform startTransform(PxVec3(playerStartPos.x, playerStartPos.y, playerStartPos.z), playerPxOrientation);
		gVehicle4W[i]->getRigidDynamicActor()->setGlobalPose(startTransform);

		// Convert the Vehicle to its base type for easy conversion.
		gVehicle4W[i]->getRigidDynamicActor()->userData = static_cast<IPhysical*>(vehicle);
		const std::string& name = engine::teamStats::names[vehicle->getTeam()];
		gVehicle4W[i]->getRigidDynamicActor()->setName(name.c_str());
		gScene->addActor(*gVehicle4W[i]->getRigidDynamicActor());

		std::cout << "car: " << name << " initialized" << std::endl;
	}
	std::cout << "PhysX Initialized" << std::endl;
}

void setDriveMode(entity::VehicleController* ctrl)
{
	int vNum = ctrl->contrId;
	float aDrive;
	float aSteer;
	if (ctrl->analogController) {
		gMimicKeyInputs = false;
		aDrive = ctrl->analogDrive;
		aSteer = ctrl->analogSteer;
	}
	else {
		gMimicKeyInputs = true;
		aDrive = 1.f;
		aSteer = 1.f;
	}

	//APPLY FLIP PULSE
	if (ctrl->flipImpulse) {
		Driving::applyVehicleFlipImpulse(vNum);
		ctrl->flipImpulse = false;
	}

	//APPLY TRAP
	if (ctrl->trap.second) {
		Driving::applyVehicleTrap(vNum);
		ctrl->trap.first -= 1;
		if (ctrl->trap.first <= 0) ctrl->trap.second = false;
	}

	//APPLY BOOST
	if (ctrl->boost.second) {
		if (vehs->at(vNum)->isUpright()) {
			Driving::applyVehicleBoost(vNum);
			ctrl->boost.first -= 1;
			if (ctrl->boost.first <= 0) ctrl->boost.second = false;
		}
	}

	Driving::releaseAllControls(vNum);
	//BRAKE
	if (ctrl->input[5]) {
		Driving::startBrakeMode(vNum, aDrive);
	} 
	//FORWARD
	else if (ctrl->input[0]) {
		Driving::startAccelerateForwardsMode(vNum, aDrive);
	}
	//REVERSE
	else if (ctrl->input[1]) {
		Driving::startAccelerateReverseMode(vNum, aDrive);
	}

	//LEFT
	if (ctrl->input[2]) {
		if (ctrl->input[4]) {
			Driving::startHandbrakeTurnLeftMode(vNum, aSteer);
		}
		else {
			Driving::startTurnHardLeftMode(vNum, aSteer);
		}
	}
	//RIGHT
	if (ctrl->input[3]) {
		if (ctrl->input[4]) {
			Driving::startHandbrakeTurnRightMode(vNum, aSteer);
		}
		else {
			Driving::startTurnHardRightMode(vNum, aSteer);
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

void Simulate::applyIntroForce(float maxSpeed)
{
	PxF32 mass = gVehicle4W[0]->getRigidDynamicActor()->getMass();
	if (gVehicle4W[0]->computeForwardSpeed() < maxSpeed)
		PxRigidBodyExt::addLocalForceAtLocalPos(*gVehicle4W[0]->getRigidDynamicActor(), PxVec3(0.f, 0.f, mass * 0.75f), PxVec3(0.f, -0.1f, 0.2f), PxForceMode::eIMPULSE);
}

float Simulate::applyStoppingForce(int vNum)
{
	PxF32 mass = gVehicle4W[vNum]->getRigidDynamicActor()->getMass();
	float speed = gVehicle4W[vNum]->computeForwardSpeed();
	if (speed > 0.25f)
		PxRigidBodyExt::addLocalForceAtLocalPos(*gVehicle4W[vNum]->getRigidDynamicActor(), PxVec3(0.f, 0.f, -mass * 0.5f), PxVec3(0.f, -0.1f, 0.2f), PxForceMode::eIMPULSE);
	return speed;
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
	for (int i = 0; i < vehicles.size(); i++) {
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
		vehicle->updateSpeedometerAndAcceleration(frameRate);
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
				// Actor must have IPhysical in its userData
				IPhysical* actorPtr = static_cast<IPhysical*>(actors[i]->userData);

				if (actorPtr == model.get()) {
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
					PxVec3 pxModelPos = boxPose.getPosition();
					memcpy(&modelPos, &pxModelPos, sizeof(PxVec3));
					model->setPosition(modelPos);

					// Check if this actor is a vehicle and if so, set its wheels pose.
					entity::Vehicle* vehicle = dynamic_cast<entity::Vehicle*>(actorPtr);
					
					if (vehicle)
					{
						PxMat44 frontRight = (PxShapeExt::getGlobalPose(*shapes[0], *actors[i]));
						PxMat44 frontLeft = (PxShapeExt::getGlobalPose(*shapes[1], *actors[i]));
						PxMat44 rearRight = (PxShapeExt::getGlobalPose(*shapes[2], *actors[i]));
						PxMat44 rearLeft = (PxShapeExt::getGlobalPose(*shapes[3], *actors[i]));

						glm::mat4 fr, fl, rr, rl;
						memcpy(&fr, &frontRight, sizeof(PxMat44));
						memcpy(&fl, &frontLeft, sizeof(PxMat44));
						memcpy(&rr, &rearRight, sizeof(PxMat44));
						memcpy(&rl, &rearLeft, sizeof(PxMat44));

						vehicle->setWheelsModelMatrix(fl, fr, rr, rl);
					}
					break;
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
			
			// We don't want to change the tile color if another vehicle
			// is already on the tile.
			bool onlyVehicleOnTile = true;
			for (const auto& otherVehicle : vehicles) {
				if (otherVehicle->getTeam() != vehicle->getTeam() && otherVehicle->currentTile == vehicle->currentTile) {
					onlyVehicleOnTile = false;
					break;
				}
			}

			if (arena.isTrap(vehicle->currentTile)) {
				
				if (vehicle->getTeam() == vehicles.at(0)->getTeam()) {
					audioPlayer->playTrapHitSound();
				}
				vehicle->applyTrap(300.f);
				//std::cout << "TRAP TRIGGERED\n";
				arena.removeTrap(vehicle->currentTile);
			} else if(vehicle->syphonActive){
				std::optional<engine::teamStats::Teams> team = arena.getTeamOnTile(*tileCoords);
			
				if (team) {
					// Decrement the teams score and remove the team
					arena.setTileTeam(*tileCoords, std::nullopt);
					engine::teamStats::scores[*team]--;
					vehicle->increaseEnergy();
				}
			
			
			} else if (vehicle->enoughEnergy() && onlyVehicleOnTile && !arena.tileHasChargingStation(*tileCoords)) {
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
			} // INSERT CODE FOR TRAP
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

	for (int i = 0; i < vehicles.size(); i++) {
		gVehicle4W[i]->free();
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

void Simulate::setConfigs(ui::DevUI::Settings::Handling turn)
{
	gSteerVsForwardSpeedData[0] = turn.velStepOne;
	gSteerVsForwardSpeedData[1] = turn.stepOneTurnStr;
	gSteerVsForwardSpeedData[2] = turn.velStepTwo;
	gSteerVsForwardSpeedData[3] = turn.stepTwoTurnStr;
	gSteerVsForwardSpeedData[4] = turn.velStepThr;
	gSteerVsForwardSpeedData[5] = turn.stepThrTurnStr;
	gSteerVsForwardSpeedData[6] = turn.velStepFou;
	gSteerVsForwardSpeedData[7] = turn.stepFouTurnStr;
	gSteerVsForwardSpeedTable = { gSteerVsForwardSpeedData, 4 };

	gPadSmoothingData.mRiseRates[0] = turn.analogAccel;
	gPadSmoothingData.mRiseRates[1] = turn.analogBrake;
	gPadSmoothingData.mRiseRates[2] = turn.analogHandBrake;
	gPadSmoothingData.mRiseRates[3] = turn.analogSteer;
	gPadSmoothingData.mRiseRates[4] = turn.analogSteer;
	gPadSmoothingData.mRiseRates[5] = turn.analogAccelFall;
	gPadSmoothingData.mRiseRates[6] = turn.analogBrakeFall;
	gPadSmoothingData.mRiseRates[7] = turn.analogHandBrakeFall;
	gPadSmoothingData.mRiseRates[8] = turn.analogSteerFall;
	gPadSmoothingData.mRiseRates[9] = turn.analogSteerFall;
}

}	// namespace physics
}	// namespace hyperbright
