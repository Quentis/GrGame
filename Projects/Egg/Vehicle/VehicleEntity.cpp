#include "DXUT.h"
#include "Vehicle/VehicleEntity.h"
#include "Physics/PhysicsMathConversions.h"

using namespace Egg;
using namespace Egg::Vehicle;
using namespace Egg::Physics;
using namespace Egg::Scene;
using namespace Egg::Math;
using namespace physx;
using namespace Egg::Math;
using namespace Egg::Physics;

VehicleEntity::~VehicleEntity()
{

}

Math::float4x4 VehicleEntity::getModelMatrix()
{
	PxTransform m = actor->getGlobalPose();

	//float4 rot = Egg::Physics::PxQuatToRadiansAndUnitAxis(m.q);

	float angle;
	float3 axis;
	m.q.toRadiansAndUnitAxis(angle, ~axis);
	float4 rot = {axis, angle};

	if ((rot.isinf().any() == false) && (rot.isnan().any() == false))
	{
		return float4x4::rotation(rot.xyz, rot.w) * float4x4::translation(~m.p);
	}
	else
	{
		return float4x4::translation(~m.p);
	}

	return float4x4::translation(~m.p);
}

Math::float4x4 VehicleEntity::getModelMatrixInverse()
{
	//original method here was to negate angle and position, but that didnt work...
	return getModelMatrix().invert();
}

Math::float3 VehicleEntity::getPosition()
{
	using namespace Egg::Math;

	PxTransform m = actor->getGlobalPose();
	return ~m.p;
}

Math::float4 VehicleEntity::getOrientation()
{
	using namespace Egg::Math;
	PxTransform m = actor->getGlobalPose();
	return ~m.q;
}

Math::float4x4 VehicleEntity::getRotationMatrix()
{
	using namespace Egg::Math;
	PxTransform m = actor->getGlobalPose();

	//converting pxquat
	//float4 rot = Egg::Physics::PxQuatToRadiansAndUnitAxis(m.q);

	float angle;
	float3 axis;
	m.q.toRadiansAndUnitAxis(angle, ~axis);
	float4 rot = { axis, angle };

	if ((rot.isinf().any() == false) && (rot.isnan().any() == false))
	{
		return float4x4::rotation(rot.xyz, rot.w);
	}
	else
	{
		return float4x4::rotation(float3(0,1,0),0);
	}
}

Math::float4x4 VehicleEntity::getRotationMatrixInverse()
{
	using namespace Egg::Math;
	PxTransform m = actor->getGlobalPose();

	//float4 rot = Egg::Physics::PxQuatToRadiansAndUnitAxis(m.q);

	float angle;
	float3 axis;
	m.q.toRadiansAndUnitAxis(angle, ~axis);
	float4 rot = { axis, angle };

	if ((rot.isinf().any() == false) && (rot.isnan().any() == false))
	{
		return float4x4::rotation(rot.xyz, -rot.w);
	}
	else
	{
		return float4x4::rotation(float3(0,1,0),0);
	}
}

void VehicleEntity::render(const Egg::Scene::RenderParameters& renderParameters)
{
	//we need to render all of the submeshes of the car
	wheel1->render(renderParameters);
	wheel2->render(renderParameters);
	wheel3->render(renderParameters);
	wheel4->render(renderParameters);
	chassis->render(renderParameters);
}

bool VehicleEntity::animate(float dt)
{
	PxVehicleKeySmoothingData gKeySmoothingData =
	{
		{
			3.0f,	//rise rate eANALOG_INPUT_ACCEL		
				3.0f,	//rise rate eANALOG_INPUT_BRAKE		
				10.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
				2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT	
				2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT	
		},
		{
			5.0f,	//fall rate eANALOG_INPUT__ACCEL		
				5.0f,	//fall rate eANALOG_INPUT__BRAKE		
				10.0f,	//fall rate eANALOG_INPUT__HANDBRAKE	
				5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT	
				5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT	
			}
	};

	PxF32 gSteerVsForwardSpeedData[2*8]=
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
	PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData,4);

	PxVehicleWheels** vehicles = new PxVehicleWheels*[1];
	vehicles[0] = car;

	//Binding inputs to the car
	PxVehicleDrive4WRawInputData rawInputData;

	rawInputData.setDigitalAccel(control_accel);
	rawInputData.setDigitalBrake(control_brake);
	rawInputData.setDigitalSteerLeft(control_steerleft);
	rawInputData.setDigitalSteerRight(control_steerright);
	rawInputData.setGearUp(control_gearup);
	rawInputData.setGearDown(control_geardown);

	PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData,gSteerVsForwardSpeedTable,rawInputData,dt,*car);

	//Raycasting suspension, and updating vehicle physics
	if(NULL==mSqWheelRaycastBatchQuery)
	{
		mSqWheelRaycastBatchQuery= mSqData->setUpBatchedSceneQuery(actor->getScene());
	}
	PxVehicleSuspensionRaycasts(mSqWheelRaycastBatchQuery,1,vehicles,mSqData->getRaycastQueryResultBufferSize(),mSqData->getRaycastQueryResultBuffer());

	PxVehicleUpdates(dt,actor->getScene()->getGravity(),*mSurfaceTirePairs,1,vehicles);

	//Uncomment for debug information
	/*
	int a = car->mWheelsDynData.getTireDrivableSurfaceType(0);
	float b1 = car->mWheelsDynData.getTireFriction(0);
	int c = mSqData->getRaycastQueryResultBuffer()->nbHits;
	bool d = car->isInAir();

	float e = car->mWheelsDynData.getTireLongSlip(0);
	int f = car->mDriveDynData.getCurrentGear();
	float g = car->mDriveDynData.getEngineRotationSpeed();
	float h = car->mWheelsDynData.getTireLongSlip(0);

	_RPT4(0,"accel: %d  brake: %d, left: %d  right: %d\n",control_accel?1:0 ,control_brake?1:0 ,control_steerleft?1:0 ,control_steerright?1:0);
	_RPT3(0,"tfric: %f  nbhits: %d, inair: %d \n",b1, c, d?1:0);
	_RPT3(0,"tls: %f  enginerot: %f, gear: %d \n\n",h,g,f);
	*/

	//Updating position and rotation of the vehicle's meshes shown in the engine
	const int numShapes=actor->getNbShapes();
	PxShape* carShapes[WHEEL_NUM + 1]; //4 wheels + chassis
	actor->getShapes(carShapes,numShapes);

	if (~PxShapeExt::getGlobalPose(*carShapes[0]).isSane() && ~PxShapeExt::getGlobalPose(*carShapes[0]).isFinite())
	{
		wheel1->setPosition(~PxShapeExt::getGlobalPose(*carShapes[0]).p);
		wheel1->setRotation(~PxShapeExt::getGlobalPose(*carShapes[0]).q);
	}

	if (~PxShapeExt::getGlobalPose(*carShapes[1]).isSane() && ~PxShapeExt::getGlobalPose(*carShapes[1]).isFinite())
	{
		wheel2->setPosition(~PxShapeExt::getGlobalPose(*carShapes[1]).p);
		wheel2->setRotation(~PxShapeExt::getGlobalPose(*carShapes[1]).q);
	}

	if (~PxShapeExt::getGlobalPose(*carShapes[2]).isSane() && ~PxShapeExt::getGlobalPose(*carShapes[2]).isFinite())
	{
		wheel3->setPosition(~PxShapeExt::getGlobalPose(*carShapes[2]).p);
		wheel3->setRotation(~PxShapeExt::getGlobalPose(*carShapes[2]).q);
	}

	if (~PxShapeExt::getGlobalPose(*carShapes[3]).isSane() && ~PxShapeExt::getGlobalPose(*carShapes[3]).isFinite())
	{
		wheel4->setPosition(~PxShapeExt::getGlobalPose(*carShapes[3]).p);
		wheel4->setRotation(~PxShapeExt::getGlobalPose(*carShapes[3]).q);
	}

	if (~PxShapeExt::getGlobalPose(*carShapes[4]).isSane() && ~PxShapeExt::getGlobalPose(*carShapes[4]).isFinite())
	{
		chassis->setPosition(~PxShapeExt::getGlobalPose(*carShapes[4]).p);
		chassis->setRotation(~PxShapeExt::getGlobalPose(*carShapes[4]).q);
	}



	//resetting control state before reading new values in
	control_accel = false;
	control_brake = false;
	control_steerleft = false;
	control_steerright = false;
	control_geardown= false;
	control_gearup = false;

	if( luabind::type(controlState) == LUA_TTABLE)
	{
		if(luabind::type(controlState["script"]) == LUA_TFUNCTION)
			controlState["script"](shared_from_this(), controlState);
		if(luabind::type(controlState["killed"]) != LUA_TNIL)
			return false;
	}
	return true;

}

void VehicleEntity::onContact(PhysicsEntity::P other)
{
	/*if( luabind::type(controlState) == LUA_TTABLE)
	if(luabind::type(controlState["onContact"]) == LUA_TFUNCTION)
	controlState["onContact"](shared_from_this(), controlState, other->getControlState());
	*/
}

//Creates the vehicle simulation data according to the given parameters
//The vehicle's properties (like speed, brake strength, gears, etc.) can be customized here
void VehicleEntity::createVehicle4WSimulationData
	(const PxF32 chassisMass, PxConvexMesh* chassisConvexMesh,
	const PxF32 wheelMass, PxConvexMesh** wheelConvexMeshes, const PxVec3* wheelCentreOffsets,
	PxVehicleWheelsSimData& wheelsData, PxVehicleDriveSimData4W& driveData, PxVehicleChassisData& chassisData)
{

	using namespace physx;

	//Extract the chassis AABB dimensions from the chassis convex mesh.

	const PxVec3 chassisDims=computeChassisAABBDimensions(chassisConvexMesh);

	//The origin is at the center of the chassis mesh.
	//Set the center of mass to be below this point and a little towards the front.
	const PxVec3 chassisCMOffset=PxVec3(0.0f,-chassisDims.y*0.5f+0.65f,0.25f);

	//Now compute the chassis mass and moment of inertia.
	//Use the moment of inertia of a cuboid as an approximate value for the chassis moi.
	PxVec3 chassisMOI
		((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass/12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*chassisMass/12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass/12.0f);
	//A bit of tweaking here.  The car will have more responsive turning if we reduce the
	//y-component of the chassis moment of inertia.
	chassisMOI.y*=0.8f;

	//Let's set up the chassis data structure now.
	chassisData.mMass=chassisMass;
	chassisData.mMOI=chassisMOI;
	chassisData.mCMOffset=chassisCMOffset;

	//Work out the front/rear mass split from the cm offset.
	//This is a very approximate calculation with lots of assumptions.
	//massRear*zRear + massFront*zFront = mass*cm           (1)
	//massRear       + massFront        = mass                      (2)
	//Rearrange (2)
	//massFront = mass - massRear                                           (3)
	//Substitute (3) into (1)
	//massRear(zRear - zFront) + mass*zFront = mass*cm      (4)
	//Solve (4) for massRear
	//massRear = mass(cm - zFront)/(zRear-zFront)           (5)
	//Now we also have
	//zFront = (z-cm)/2                                                                     (6a)
	//zRear = (-z-cm)/2                                                                     (6b)
	//Substituting (6a-b) into (5) gives
	//massRear = 0.5*mass*(z-3cm)/z                                         (7)
	const PxF32 massRear=0.5f*chassisMass*(chassisDims.z-3*chassisCMOffset.z)/chassisDims.z;
	const PxF32 massFront=chassisMass-massRear;

	//Extract the wheel radius and width from the wheel convex meshes.
	PxF32 wheelWidths[WHEEL_NUM];
	PxF32 wheelRadii[WHEEL_NUM];
	computeWheelWidthsAndRadii(wheelConvexMeshes,wheelWidths,wheelRadii,4);

	//Now compute the wheel masses and inertias components around the axle's axis.
	//http://en.wikipedia.org/wiki/List_of_moments_of_inertia
	PxF32 wheelMOIs[WHEEL_NUM];
	for(PxU32 i=0;i<WHEEL_NUM;i++)
	{
		wheelMOIs[i]=0.5f*wheelMass*wheelRadii[i]*wheelRadii[i]; //default: wheelMOIs[i]=0.5f*wheelMass*wheelRadii[i]*wheelRadii[i];
	}
	//Let's set up the wheel data structures now with radius, mass, and moi.
	PxVehicleWheelData wheels[WHEEL_NUM];
	for(PxU32 i=0;i<4;i++)
	{
		wheels[i].mRadius=wheelRadii[i];
		wheels[i].mMass=wheelMass;
		wheels[i].mMOI=wheelMOIs[i];
		wheels[i].mWidth=wheelWidths[i];
	}

	//mDampingRate = 0.25f (default)
	//Disable the handbrake from the front wheels and enable for the rear wheels
	wheels[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mMaxHandBrakeTorque=0.0f;
	wheels[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mMaxHandBrakeTorque=0.0f;
	wheels[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mMaxHandBrakeTorque=4000.0f;
	wheels[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mMaxHandBrakeTorque=4000.0f;
	//Brakes
	wheels[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mMaxBrakeTorque = 9000.0f;
	wheels[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mMaxBrakeTorque =  9000.0f;
	wheels[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mMaxBrakeTorque =  9000.0f;
	wheels[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mMaxBrakeTorque = 9000.0f;
	//Enable steering for the front wheels and disable for the front wheels.
	wheels[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mMaxSteer=PxPi*0.3333f;
	wheels[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mMaxSteer=PxPi*0.3333f;
	wheels[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mMaxSteer=0.0f;
	wheels[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mMaxSteer=0.0f;

	//Let's set up the tire data structures now.
	//Put slicks on the front tires and wets on the rear tires.
	PxVehicleTireData tires[WHEEL_NUM];
	tires[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mType=0;
	tires[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mType=0;
	tires[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mType=0;
	tires[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mType=0;

	//Let's set up the suspension data structures now.
	PxVehicleSuspensionData susps[WHEEL_NUM];
	for(PxU32 i=0;i<4;i++)
	{
		susps[i].mMaxCompression=0.6f; //amennyit a rugó mozog.									(rest position)
		susps[i].mMaxDroop=0.35f; //droop: amennyit a kerék szabadon (rugó nélkül) mozoghat fel-le   (föld) |<-droop->|<------ compression ------>| (felfüggesztés)
		susps[i].mSpringStrength= 2200.0f; //default: 3500
		susps[i].mSpringDamperRate=600.0f; //default: 450, 4500
	}
	susps[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mSprungMass=massFront*0.5f;
	susps[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mSprungMass=massFront*0.5f;
	susps[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mSprungMass=massRear*0.5f;
	susps[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mSprungMass=massRear*0.5f;

	//We need to set up geometry data for the suspension, wheels, and tires.
	//We already know the wheel centers described as offsets from the rigid body centre of mass.
	//From here we can approximate application points for the tire and suspension forces.
	//Lets assume that the suspension travel directions are absolutely vertical.
	//Also assume that we apply the tire and suspension forces 30cm below the centre of mass.
	PxVec3 suspTravelDirections[WHEEL_NUM]={PxVec3(0,-1,0),PxVec3(0,-1,0),PxVec3(0,-1,0),PxVec3(0,-1,0)};
	PxVec3 wheelCentreCMOffsets[WHEEL_NUM];
	PxVec3 suspForceAppCMOffsets[WHEEL_NUM];
	PxVec3 tireForceAppCMOffsets[WHEEL_NUM];
	for(PxU32 i=0;i<4;i++)
	{
		wheelCentreCMOffsets[i]=wheelCentreOffsets[i]-chassisCMOffset;
		suspForceAppCMOffsets[i]=PxVec3(wheelCentreCMOffsets[i].x,-0.3f,wheelCentreCMOffsets[i].z);
		tireForceAppCMOffsets[i]=PxVec3(wheelCentreCMOffsets[i].x,-0.3f,wheelCentreCMOffsets[i].z);
	}


	//Now add the wheel, tire and suspension data.
	for(PxU32 i=0;i<4;i++)
	{
		wheelsData.setWheelData(i,wheels[i]);
		wheelsData.setTireData(i,tires[i]);
		wheelsData.setSuspensionData(i,susps[i]);
		wheelsData.setSuspTravelDirection(i,suspTravelDirections[i]);
		wheelsData.setWheelCentreOffset(i,wheelCentreCMOffsets[i]);
		wheelsData.setSuspForceAppPointOffset(i,suspForceAppCMOffsets[i]);
		wheelsData.setTireForceAppPointOffset(i,tireForceAppCMOffsets[i]);
	}

	//Now set up the differential, engine, gears, clutch, and ackermann steering.

	//Diff
	PxVehicleDifferential4WData diff;
	diff.mType=PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
	driveData.setDiffData(diff);

	//Engine
	PxVehicleEngineData engine;
	engine.mPeakTorque=700.0f;// Nm (Newton metre) default: 3000
	engine.mMaxOmega=600.0f;//default: 600=approx 6000 rpm
	driveData.setEngineData(engine);

	//Gears
	PxVehicleGearsData gears;
	gears.mSwitchTime=0.5f;
	driveData.setGearsData(gears);

	//Clutch
	PxVehicleClutchData clutch;
	clutch.mStrength=10.0f;
	driveData.setClutchData(clutch);

	//Ackermann steer accuracy
	PxVehicleAckermannGeometryData ackermann;
	ackermann.mAccuracy=1.0f;
	ackermann.mAxleSeparation=wheelCentreOffsets[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].z-wheelCentreOffsets[PxVehicleDrive4W::eREAR_LEFT_WHEEL].z;
	ackermann.mFrontWidth=wheelCentreOffsets[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].x-wheelCentreOffsets[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].x;
	ackermann.mRearWidth=wheelCentreOffsets[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].x-wheelCentreOffsets[PxVehicleDrive4W::eREAR_LEFT_WHEEL].x;
	driveData.setAckermannGeometryData(ackermann);

}

//Initializes the actor.
void VehicleEntity::setupActor
	(PxRigidDynamic* vehActor,
	const PxFilterData& vehQryFilterData,
	const PxGeometry** wheelGeometries, const PxTransform* wheelLocalPoses, const PxU32 numWheelGeometries, const PxMaterial* wheelMaterial, const PxFilterData& wheelCollFilterData,
	const PxGeometry** chassisGeometries, const PxTransform* chassisLocalPoses, const PxU32 numChassisGeometries, const PxMaterial* chassisMaterial, const PxFilterData& chassisCollFilterData,
	const PxVehicleChassisData& chassisData,
	PxPhysics* physics)
{
	//Add all the wheel shapes to the actor.
	for(PxU32 i=0;i<numWheelGeometries;i++)
	{
		PxShape* wheelShape=vehActor->createShape(*wheelGeometries[i],*wheelMaterial);
		wheelShape->setQueryFilterData(vehQryFilterData);
		wheelShape->setSimulationFilterData(wheelCollFilterData);
		wheelShape->setLocalPose(wheelLocalPoses[i]);

		pxWheelShapes[i] = wheelShape;
	}

	//Add the chassis shapes to the actor.
	for(PxU32 i=0;i<numChassisGeometries;i++)
	{
		PxShape* chassisShape=vehActor->createShape(*chassisGeometries[i],*chassisMaterial);
		chassisShape->setQueryFilterData(vehQryFilterData);
		chassisShape->setSimulationFilterData(chassisCollFilterData);
		chassisShape->setLocalPose(chassisLocalPoses[i]);
	}

	vehActor->setMass(chassisData.mMass);
	vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
	vehActor->setCMassLocalPose(PxTransform(chassisData.mCMOffset,PxQuat::createIdentity()));
}

//sets up the filtering data of the vehicle
void VehicleEntity::SampleVehicleSetupVehicleShapeQueryFilterData(PxFilterData* qryFilterData)
{

	qryFilterData->word3 = (PxU32)SAMPLEVEHICLE_UNDRIVABLE_SURFACE;
}


//Creates the actor of the Vehicle object
PxRigidDynamic* VehicleEntity::createVehicleActor4W
	(const PxVehicleChassisData& chassisData,
	PxConvexMesh** wheelConvexMeshes, PxConvexMesh* chassisConvexMesh, 
	PxScene& scene, PxPhysics& physics, const PxMaterial& material)
{
	//We need a rigid body actor for the vehicle.
	//Don't forget to add the actor the scene after setting up the associated vehicle.
	PxRigidDynamic* vehActor=physics.createRigidDynamic(PxTransform::createIdentity());

	//We need to add wheel collision shapes, their local poses, a material for the wheels, and a simulation filter for the wheels.
	PxConvexMeshGeometry frontLeftWheelGeom(wheelConvexMeshes[0]);
	PxConvexMeshGeometry frontRightWheelGeom(wheelConvexMeshes[1]);
	PxConvexMeshGeometry rearLeftWheelGeom(wheelConvexMeshes[2]);
	PxConvexMeshGeometry rearRightWheelGeom(wheelConvexMeshes[3]);
	const PxGeometry* wheelGeometries[WHEEL_NUM]={&frontLeftWheelGeom,&frontRightWheelGeom,&rearLeftWheelGeom,&rearRightWheelGeom};
	const PxTransform wheelLocalPoses[WHEEL_NUM]={PxTransform(wheelOffsets[0]),PxTransform(wheelOffsets[1]),PxTransform(wheelOffsets[2]),PxTransform(wheelOffsets[3])};
	const PxMaterial& wheelMaterial=material;
	PxFilterData wheelCollFilterData;
	wheelCollFilterData.word0=COLLISION_FLAG_WHEEL;
	wheelCollFilterData.word1=COLLISION_FLAG_WHEEL_AGAINST;

	//We need to add chassis collision shapes, their local poses, a material for the chassis, and a simulation filter for the chassis.
	PxConvexMeshGeometry chassisConvexGeom(chassisConvexMesh);
	const PxGeometry* chassisGeoms[1]={&chassisConvexGeom};
	const PxTransform chassisLocalPoses[1]={PxTransform::createIdentity()};
	const PxMaterial& chassisMaterial=material;
	PxFilterData chassisCollFilterData;
	chassisCollFilterData.word0= COLLISION_FLAG_CHASSIS;
	chassisCollFilterData.word1= COLLISION_FLAG_CHASSIS_AGAINST;//GEOMETRY_FLAG_GROUND | GEOMETRY_FLAG_RAY_CAST_WHEEL |	GEOMETRY_FLAG_VEHICLE_CHASSIS | GEOMETRY_FLAG_DYNAMIC_OBJECT;

	//Create a query filter data for the car to ensure that cars
	//do not attempt to drive on themselves.
	PxFilterData vehQryFilterData;
	SampleVehicleSetupVehicleShapeQueryFilterData(&vehQryFilterData);

	//Set up the physx rigid body actor with shapes, local poses, and filters.
	setupActor
		(vehActor,
		vehQryFilterData,
		wheelGeometries,wheelLocalPoses,4,&wheelMaterial,wheelCollFilterData,
		chassisGeoms,chassisLocalPoses,1,&chassisMaterial,chassisCollFilterData,
		chassisData,
		&physics);

	return vehActor;
}

void VehicleEntity::computeWheelWidthsAndRadii(PxConvexMesh** wheelConvexMeshes, PxF32* wheelWidths, PxF32* wheelRadii, PxU32 numWheels)
{
	for(PxU32 i=0;i<numWheels;i++)
	{
		const PxU32 numWheelVerts=wheelConvexMeshes[i]->getNbVertices();
		const PxVec3* wheelVerts=wheelConvexMeshes[i]->getVertices();
		PxVec3 wheelMin(PX_MAX_F32,PX_MAX_F32,PX_MAX_F32);
		PxVec3 wheelMax(-PX_MAX_F32,-PX_MAX_F32,-PX_MAX_F32);
		for(PxU32 j=0;j<numWheelVerts;j++)
		{
			wheelMin.x=PxMin(wheelMin.x,wheelVerts[j].x);
			wheelMin.y=PxMin(wheelMin.y,wheelVerts[j].y);
			wheelMin.z=PxMin(wheelMin.z,wheelVerts[j].z);
			wheelMax.x=PxMax(wheelMax.x,wheelVerts[j].x);
			wheelMax.y=PxMax(wheelMax.y,wheelVerts[j].y);
			wheelMax.z=PxMax(wheelMax.z,wheelVerts[j].z);
		}
		wheelWidths[i] = wheelMax.x - wheelMin.x;
		wheelRadii[i] = PxMax(wheelMax.y,wheelMax.z) * 0.975f;
	}
}

PxVec3 VehicleEntity::computeChassisAABBDimensions(const PxConvexMesh* chassisConvexMesh)
{
	const PxU32 numChassisVerts=chassisConvexMesh->getNbVertices();
	const PxVec3* chassisVerts=chassisConvexMesh->getVertices();
	PxVec3 chassisMin(PX_MAX_F32,PX_MAX_F32,PX_MAX_F32);
	PxVec3 chassisMax(-PX_MAX_F32,-PX_MAX_F32,-PX_MAX_F32);
	for(PxU32 i=0;i<numChassisVerts;i++)
	{
		chassisMin.x=PxMin(chassisMin.x,chassisVerts[i].x);
		chassisMin.y=PxMin(chassisMin.y,chassisVerts[i].y);
		chassisMin.z=PxMin(chassisMin.z,chassisVerts[i].z);
		chassisMax.x=PxMax(chassisMax.x,chassisVerts[i].x);
		chassisMax.y=PxMax(chassisMax.y,chassisVerts[i].y);
		chassisMax.z=PxMax(chassisMax.z,chassisVerts[i].z);
	}
	const PxVec3 chassisDims=chassisMax-chassisMin;
	return chassisDims;
}
