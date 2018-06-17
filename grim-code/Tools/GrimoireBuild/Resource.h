#pragma once

#include <stdint.h>
#include <Generated/generated_Vector2.h>
#include <Generated/generated_Vector3.h>

#include <Shared/Core/Uuid.h>

namespace grim{
enum ResourceType
{
	kMesh = 0,
	kMaterial,
	kNumResourceTypes
};

extern char* g_resourceTypeNames[kNumResourceTypes];

class __declspec(novtable) Resource
{
public:
	Resource();

	virtual bool serializeBinary(char* buffer)const = 0;
	virtual bool deserializeBinary(char* buffer) = 0;

	grim::uuid m_uuid;
	ResourceType m_type;
	uint32_t m_totalSizeInBytes;

#if _DEBUG
	enum { kMaxResourceNameSize = 256 };
	char m_name[kMaxResourceNameSize];
#endif
};

struct MeshData
{
	tVector3* m_positions;
	tVector3* m_normals;
	tVector3* m_tangents;
	tVector2* m_uvs;

	uint16_t* m_indices;

	uint32_t m_numVertices;
	uint32_t m_numIndices;
};

class MeshResource : public Resource
{
public:
	enum { kMaxMeshesPerResource = 4 };

	MeshResource();
	~MeshResource();

	bool serializeBinary(char* buffer)const;
	bool deserializeBinary(char* buffer);

	MeshData m_meshes[kMaxMeshesPerResource];
	uint32_t m_numMeshes;
};

bool LoadMeshesFromFile(MeshResource& meshResource, const char* filepath);

class Material : public Resource
{
public:
	Material();

	bool serializeBinary(char* buffer)const;
	bool deserializeBinary(char* buffer);

	bool IsSharedMaterial()const { return m_isShared; }

	bool m_isShared;
};
}