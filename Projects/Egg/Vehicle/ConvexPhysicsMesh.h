#pragma once
#include "Physics/PhysicsMathConversions.h"
#include "PxPhysicsAPI.h"
#include <boost/shared_ptr.hpp>
#include <assimp/importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postProcess.h> // Post processing flags

namespace Egg { namespace Vehicle
{
	using namespace physx;
	using namespace std;
	using namespace Assimp;

	//#######################################################
	///Helper class for creating PxConvexMeshes from file
	//#####################################################

	class MemoryInputDataCustom: public PxInputData
	{
	public:
		MemoryInputDataCustom(PxU8* data, PxU32 length) :	
			mSize	(length),
			mData	(data),
			mPos	(0)
		{
		}

		PxU32 read(void* dest, PxU32 count)
		{
			PxU32 length = PxMin<PxU32>(count, mSize-mPos);
			memcpy(dest, mData+mPos, length);
			mPos += length;
			return length;
		}

		PxU32 getLength() const {return mSize;}

		void		seek(PxU32 pos)
		{			
			mPos = PxMin<PxU32>(mSize, pos);
		}			

		PxU32 tell() const
		{
			return mPos;
		}

	private:
		PxU32		mSize;
		const PxU8*	mData;
		PxU32		mPos;
	};

	class MemoryOutputStream: public PxOutputStream
	{
	public:
		MemoryOutputStream() :
			mData		(NULL),
			mSize		(0),
			mCapacity	(0)
		{
		}


		~MemoryOutputStream()
		{
			if(mData)
				delete[] mData;
		}

		PxU32		write(const void* src, PxU32 size)
		{
			PxU32 expectedSize = mSize + size;
			if(expectedSize > mCapacity)
			{
				mCapacity = expectedSize + 4096;

				PxU8* newData = new PxU8[mCapacity];
				PX_ASSERT(newData!=NULL);

				if(newData)
				{
					memcpy(newData, mData, mSize);
					delete[] mData;
				}
				mData = newData;
			}
			memcpy(mData+mSize, src, size);
			mSize += size;
			return size;
		}

		PxU32		getSize()	const	{	return mSize; }
		PxU8*		getData()	const	{	return mData; }
	private:
		PxU8*		mData;
		PxU32		mSize;
		PxU32		mCapacity;
	};

	/// Application class with scene management
	class ConvexPhysicsMesh
	{
	private:

		PxConvexMesh* convexMesh;

		/// Local shorthand for shared pointer type.
		typedef boost::shared_ptr<ConvexPhysicsMesh> P;
		/// Local shorthand for weak pointer type.
		typedef boost::weak_ptr<ConvexPhysicsMesh> W;

		PxConvexMesh* getConvexMesh()
		{
			return convexMesh;
		}

		ConvexPhysicsMesh(PxScene* scene, PxCooking* cookinglib, std::string name)
		{
			convexMesh = NULL;

			int vertexCount;
			PxVec3* buffer;

			Importer importer;
			const aiScene* assScene = importer.ReadFile( name, 0);

			// if the import failed, the convex mesh will be NULL
			if( !assScene || !assScene->HasMeshes() || assScene->mNumMeshes == 0) 
			{
				std::string error = importer.GetErrorString();
				error.length();
				return;
			}

			vertexCount = assScene->mMeshes[0]->mNumVertices;
			buffer = new PxVec3[vertexCount];

			for (int i = 0; i < vertexCount; i++)
			{
				buffer[i] = PxVec3(assScene->mMeshes[0]->mVertices[i].x, assScene->mMeshes[0]->mVertices[i].y, assScene->mMeshes[0]->mVertices[i].z);
			}

			PxConvexMeshDesc convexDesc;
			convexDesc.points.count     = vertexCount;
			convexDesc.points.stride    = sizeof(PxVec3);
			convexDesc.points.data      = buffer;
			convexDesc.flags            = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;


			MemoryOutputStream buf;
			if(cookinglib->cookConvexMesh(convexDesc, buf))
			{
				MemoryInputDataCustom input(buf.getData(), buf.getSize());
				convexMesh = scene->getPhysics().createConvexMesh(input);
			}

			delete [] buffer;
		}

	public:


		~ConvexPhysicsMesh()
		{
			if (convexMesh != NULL)
			{
				//convexMesh->release();
			}
		}

		//factory for convex meshes
		static PxConvexMesh* create(PxScene* scene, PxCooking* cookinglib, std::string name)
		{
			return ConvexPhysicsMesh::P(new ConvexPhysicsMesh(scene, cookinglib, name))->getConvexMesh();
		}


	};

}
}