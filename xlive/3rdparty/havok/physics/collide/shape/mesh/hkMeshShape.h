#pragma once
#include "havok/common/base/container/array/hkArray.h"
#include "havok/common/base/math/vector/hkVector4.h"
#include "havok/physics/collide/shape/compund/hkShapeCollection.h"
#include "havok/physics/collide/shape/compund/mesh/hkMeshMaterial.h"


template <unsigned N>
struct  hkMeshShape : hkShapeCollection
{
	struct Subpart
	{
		/// A pointer to triples of vertex indices.
		union SubpartIndex
		{
			// union's don't like hk typing just using base types
			const unsigned int* m_index32Base;
			const unsigned __int16* m_index16Base;
		};

		const hkReal* m_vertexBase;
		hkInt32 m_vertexStriding;
		hkInt32 m_numVertices;
		SubpartIndex m_indexBase;
		hkInt32 m_stridingType;
		hkInt32 m_indexStriding;
		hkInt32 m_numTriangles;
		const hkUint8* m_materialIndexBase;
		hkInt32 m_materialIndexStriding;
		const hkMeshMaterial* m_materialBase;
		hkInt32 m_materialStriding;
		hkInt32 m_numMaterials;
	};

	hkVector4 m_scaling;
	hkReal m_radius;
	hkInt32 m_numBitsForSubpartIndex;
	hkInplaceArray<Subpart, N> m_subparts;
};
