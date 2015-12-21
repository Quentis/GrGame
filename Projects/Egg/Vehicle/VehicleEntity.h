#pragma once
#include "Vehicle/ConvexPhysicsMesh.h"
#include "Vehicle/VehicleSceneQueryData.h"
#include "Physics/PhysicsEntity.h"
#include "Physics/PhysicsMathConversions.h"
#include "Scene/Entity.h"
#include "Mesh/Multi.h"
#include "PxPhysicsAPI.h"
#include "Mesh/Indexed.h"
#include <unordered_map>
#include "vehicle\PxVehicleSDK.h"
#include"Vehicle/StaticPartEntity.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
}
#include "luabind/luabind.hpp"
#include "luabind/adopt_policy.hpp"
#include <boost/enable_shared_from_this.hpp>

using namespace physx;
using namespace Egg::Physics;

namespace Egg { namespace Vehicle
{
	enum
	{
		COLLISION_FLAG_GROUND			=	1 << 0,
		COLLISION_FLAG_WHEEL			=	1 << 1,
		COLLISION_FLAG_CHASSIS			=	1 << 2,
		COLLISION_FLAG_OBSTACLE			=	1 << 3,
		COLLISION_FLAG_DRIVABLE_OBSTACLE=	1 << 4,

		COLLISION_FLAG_GROUND_AGAINST	=			 											    COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_WHEEL_AGAINST	=									COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
		COLLISION_FLAG_CHASSIS_AGAINST	=			COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_OBSTACLE_AGAINST	=			COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST=	COLLISION_FLAG_GROUND 						 |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	};

	using namespace physx;

	/// An entity with a fixed model transform.
	class VehicleEntity : public Egg::Scene::Entity, public boost::enable_shared_from_this<VehicleEntity>
	{
	protected:

		static const int SAMPLEVEHICLE_UNDRIVABLE_SURFACE =  0x0000ffff;
		static const int SAMPLEVEHICLE_DRIVABLE_SURFACE =  0xffff0000;

		static const int WHEEL_NUM = 4;

		bool control_accel;
		bool control_brake;
		bool control_steerleft;
		bool control_steerright;
		bool control_gearup;
		bool control_geardown;

		StaticPartEntity::P chassis, wheel1, wheel2, wheel3, wheel4;

		PxVec3 wheelOffsets[WHEEL_NUM];
		physx::PxRigidDynamic* actor;
		luabind::object controlState;
		physx::PxShape *pxWheelShapes[WHEEL_NUM];
		PxVehicleDrive4W* car;
		PxVehicleDrivableSurfaceToTireFrictionPairs* mSurfaceTirePairs;
		PxVehicleDrivableSurfaceType mVehicleDrivableSurfaceTypes[256];
		PxBatchQuery* mSqWheelRaycastBatchQuery;
		PxRaycastQueryResult* mSqResults;
		VehicleSceneQueryData* mSqData;

		/// Private constructor. Initializes the model transformation to indentity.
		/// @param shadedMesh the ShadedMesh which the Entity uses for displaying itself.
		VehicleEntity(physx::PxScene* scene, PxCooking* cookinglib, PxMaterial* physicsMaterial, Egg::Math::float3 position, Egg::Math::float4 orientation, Mesh::Multi::P echassis, Mesh::Multi::P ewheel, std::string chassisMesh, std::string wheelMesh):
			Scene::Entity(echassis)
		{
			//Weight parameters
			float density = 500.0f;
			float chassisWeight = 600.0f;
			float wheelWeight = 20.0f;

			mSqWheelRaycastBatchQuery = NULL;
			mSqData = VehicleSceneQueryData::allocate(WHEEL_NUM);

			chassis = StaticPartEntity::create(echassis);
			wheel1 = StaticPartEntity::create(ewheel);
			wheel2 = StaticPartEntity::create(ewheel);
			wheel3 = StaticPartEntity::create(ewheel);
			wheel4 = StaticPartEntity::create(ewheel);

			wheelOffsets[0]  = PxVec3(-3,-3, 6); //Front Left wheel
			wheelOffsets[1]  = PxVec3( 3,-3, 6); //Front Right wheel
			wheelOffsets[2]  = PxVec3(-3,-3,-5); //Rear left wheel
			wheelOffsets[3]  = PxVec3( 3,-3,-5); //Rear right wheel

			PxConvexMesh* chassisConvexMesh = ConvexPhysicsMesh::create(scene, cookinglib, chassisMesh);
			PxConvexMesh** wheelConvexMeshes = new PxConvexMesh*[4];
			wheelConvexMeshes[0] = ConvexPhysicsMesh::create(scene, cookinglib, wheelMesh);
			wheelConvexMeshes[1] = ConvexPhysicsMesh::create(scene, cookinglib, wheelMesh);
			wheelConvexMeshes[2] = ConvexPhysicsMesh::create(scene, cookinglib, wheelMesh);
			wheelConvexMeshes[3] = ConvexPhysicsMesh::create(scene, cookinglib, wheelMesh);

			PxVehicleWheelsSimData* wheelsSimData=PxVehicleWheelsSimData::allocate(4);
			PxVehicleDriveSimData4W driveSimData;
			PxVehicleChassisData chassisData;
			createVehicle4WSimulationData
				(chassisWeight, chassisConvexMesh,
				wheelWeight,wheelConvexMeshes, wheelOffsets,
				*wheelsSimData,driveSimData,chassisData);


			actor = createVehicleActor4W(chassisData,wheelConvexMeshes,chassisConvexMesh,*scene,(scene->getPhysics()),*physicsMaterial);

			PxU32 n = actor->getNbShapes();

			car = PxVehicleDrive4W::allocate(4);
			car->setup(&(scene->getPhysics()),actor,*wheelsSimData,driveSimData,0);

			//we won't need this anymore
			wheelsSimData->free();

			//Setting up materials.

			int materialCount = scene->getPhysics().getNbMaterials();
			PxMaterial** materials = new PxMaterial*[materialCount];
			scene->getPhysics().getMaterials(materials, materialCount);
			// Specifying which materials will be drivable. A specific friction value can be applied using mSurfaceTirePairs->setTypePairFriction;
			PxMaterial** drivableSurfaceMaterials = new PxMaterial*[materialCount];
			for(int i = 0; i < materialCount; i++)
			{
				drivableSurfaceMaterials[i] = materials[i];
				mVehicleDrivableSurfaceTypes[i].mType = i;
			}

			mSurfaceTirePairs=PxVehicleDrivableSurfaceToTireFrictionPairs::create(1, materialCount,(const PxMaterial**)drivableSurfaceMaterials,mVehicleDrivableSurfaceTypes);

			// You can specify the friction between each tire type, and each PxMaterial.
			for(int i = 0; i < materialCount; i++)
			{
				mSurfaceTirePairs->setTypePairFriction(i,0,1.0f);
			}

			//actor->setSolverIterationCounts(8,3);
			//PxRigidBodyExt::updateMassAndInertia(*actor, density);
			scene->addActor(*actor);

			PxVehicleDrive4W* vehDrive4W=(PxVehicleDrive4W*)car;
			vehDrive4W->setToRestState();
			vehDrive4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
			actor->setGlobalPose(PxTransform(~(position)));

			//sets Automatic or Manual gear switching
			car->mDriveDynData.setUseAutoGears(true);
		}
	public:
		/// Local shorthand for shared pointer type.
		typedef boost::shared_ptr<VehicleEntity> P;
		/// Local shorthand for weak pointer type.
		typedef boost::weak_ptr<VehicleEntity> W;

		/// Factory method for instantiation. Initializes the model transformation to indentity.
		/// @param shadedMesh the ShadedMesh which the Entity uses for displaying itself.
		/// @return shared pointer referencing created instance
		static VehicleEntity::P create(PxScene* scene, PxCooking* cookinglib, PxMaterial* physicsMaterial, Egg::Math::float3 position, Egg::Math::float4 orientation, Mesh::Multi::P echassis, Mesh::Multi::P ewheel, std::string chassisname, std::string wheelname)
		{
			return VehicleEntity::P(new VehicleEntity(scene, cookinglib, physicsMaterial, position, orientation, echassis, ewheel, chassisname, wheelname));
		}

		virtual ~VehicleEntity();

		/// Returns the model matrix. To be used for rendering, and positioning of light sources and cameras attached to the entity.
		virtual Egg::Math::float4x4 getModelMatrix();

		/// Returns the inverse of the model matrix.
		virtual Egg::Math::float4x4 getModelMatrixInverse();

		/// Returns reference point in world space.
		virtual Egg::Math::float3 getPosition();

		/// Returns orientation as a quaternion in world space.
		virtual Egg::Math::float4 getOrientation();

		/// Returns the rotation matrix.
		virtual Egg::Math::float4x4 getRotationMatrix();

		/// Returns the inverse of the rotation matrix. To be used for the view transformation of attached cameras.
		virtual Egg::Math::float4x4 getRotationMatrixInverse();

		virtual void render(const Egg::Scene::RenderParameters& renderParameters);

		physx::PxRigidActor* getActor(){return actor;}

		physx::PxRigidDynamic* makeActorDynamic(physx::PxScene* scene);

		void setControlState(luabind::object controlState)
		{
			this->controlState = controlState;
		}

		void releaseControlState()
		{
			this->controlState = luabind::object();
		}

		luabind::object getControlState()
		{
			return this->controlState;
		}

		virtual bool animate(float dt);

		virtual void onContact(Egg::Physics::PhysicsEntity::P other);
		
		//Creates the actor of the Vehicle object
		PxRigidDynamic* VehicleEntity::createVehicleActor4W
			(const PxVehicleChassisData& chassisData,
			PxConvexMesh** wheelConvexMeshes, PxConvexMesh* chassisConvexMesh, 
			PxScene& scene, PxPhysics& physics, const PxMaterial& material);

		//Computes wheel geometry
		void computeWheelWidthsAndRadii(PxConvexMesh** wheelConvexMeshes, PxF32* wheelWidths, PxF32* wheelRadii, PxU32 numWheels);

		//Computes AABB of the chassis, that is used for mass calculations
		PxVec3 computeChassisAABBDimensions(const PxConvexMesh* chassisConvexMesh);

		//Methods for setting up vehicle in physx


		//Creates the vehicle simulation data according to the given parameters
		//The vehicle's properties (like speed, brake strength, gears, etc.) can be customized here
		void createVehicle4WSimulationData
			(const PxF32 chassisMass, PxConvexMesh* chassisConvexMesh,
			const PxF32 wheelMass, PxConvexMesh** wheelConvexMeshes, const PxVec3* wheelCentreOffsets,
			PxVehicleWheelsSimData& wheelsData, PxVehicleDriveSimData4W& driveData, PxVehicleChassisData& chassisData);

		//Initialized the actor
		void setupActor
			(PxRigidDynamic* vehActor,
			const PxFilterData& vehQryFilterData,
			const PxGeometry** wheelGeometries, const PxTransform* wheelLocalPoses, const PxU32 numWheelGeometries, const PxMaterial* wheelMaterial, const PxFilterData& wheelCollFilterData,
			const PxGeometry** chassisGeometries, const PxTransform* chassisLocalPoses, const PxU32 numChassisGeometries, const PxMaterial* chassisMaterial, const PxFilterData& chassisCollFilterData,
			const PxVehicleChassisData& chassisData,
			PxPhysics* physics);

		//sets up the filtering data of the vehicle
		void SampleVehicleSetupVehicleShapeQueryFilterData(PxFilterData* qryFilterData);

		void setBrake(bool brake)
		{
			control_brake = brake;
		}

		void setAccelerate(bool accelerate)
		{
			control_accel = accelerate;
		}

		void setTurnLeft(bool turnLeft)
		{
			control_steerleft = turnLeft;
		}

		void setTurnRight(bool turnRight)
		{
			control_steerright = turnRight;
		}

		void setGearUp(bool gearUp)
		{
			control_gearup = gearUp;
		}

		void setGearDown(bool gearDown)
		{
			control_geardown = gearDown;
		}

		void resetVehicle()
		{
			car->setToRestState();
			car->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
			
			//actor->addForce(PxVec3(0,10,0),PxForceMode::eFORCE);
			actor->setGlobalPose(PxTransform( actor->getGlobalPose().p + PxVec3(0,1,0)));

		}

	};

}}
