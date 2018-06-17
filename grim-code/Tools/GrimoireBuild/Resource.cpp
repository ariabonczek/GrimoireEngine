#include "Precompiled.h"

#include "Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include <Shared/stb_image.h>
#include <Shared/assimp/scene.h>
#include <Shared/assimp/Importer.hpp>
#include <Shared/assimp/postprocess.h>

namespace grim{
char* g_resourceTypeNames[kNumResourceTypes] = { "mesh", "material" };

Resource::Resource():
	m_uuid(0)
#if _DEBUG
	,m_name("")
#endif
{}

MeshResource::MeshResource() :
	Resource(),
	m_numMeshes(0)
{
	memset(m_meshes, 0, sizeof(MeshData) * kMaxMeshesPerResource);
}

MeshResource::~MeshResource()
{
	for (int i = 0; i < (int)m_numMeshes; ++i)
	{
		MeshData& meshData = m_meshes[i];

		GRIM_ASSERT(meshData.m_positions, "No positions detected for mesh resource %s, memory will not be freed", m_name);
		if (meshData.m_positions)
			free(meshData.m_positions);
	}
	memset(m_meshes, 0, sizeof(MeshData) * kMaxMeshesPerResource);
}

bool MeshResource::serializeBinary(char* blob)const
{
	uint32_t bytesWritten = 0;

	for (int i = 0; i < (int)m_numMeshes; ++i)
	{
		const MeshData& meshData = m_meshes[i];

		if (meshData.m_positions)
		{
			const uint32_t size = sizeof(float) * 3 * meshData.m_numVertices;
			memcpy(blob, meshData.m_positions, size);
			blob += size;
			bytesWritten += size;
		}

		if (meshData.m_normals)
		{
			const uint32_t size = sizeof(float) * 3 * meshData.m_numVertices;
			memcpy(blob, meshData.m_normals, size);
			blob += size;
			bytesWritten += size;
		}

		if (meshData.m_tangents)
		{
			const uint32_t size = sizeof(float) * 3 * meshData.m_numVertices;
			memcpy(blob, meshData.m_tangents, size);
			blob += size;
			bytesWritten += size;
		}

		if (meshData.m_uvs)
		{
			const uint32_t size = sizeof(float) * 2 * meshData.m_numVertices;
			memcpy(blob, meshData.m_uvs, size);
			blob += size;
			bytesWritten += size;
		}

		if (meshData.m_indices)
		{
			const uint32_t size = sizeof(uint16_t) * meshData.m_numIndices;
			memcpy(blob, meshData.m_indices, size);
			blob += size;
			bytesWritten += size;
		}
	}

	GRIM_ASSERT(bytesWritten == m_totalSizeInBytes, "Bad estimation of mesh size when serializing, size %d bytes but serialized %d bytes", m_totalSizeInBytes, bytesWritten);
	return true;
}

bool MeshResource::deserializeBinary(char* buffer)
{
	return true;
}

static uint32_t CalculateMeshSize(aiMesh* assimpMesh)
{
	const int numVertices = assimpMesh->mNumVertices;
	uint32_t totalSize = 0;

	if(assimpMesh->HasPositions())
	{
		const int size = sizeof(float) * 3 * numVertices;
		totalSize += size;
	}

	if (assimpMesh->HasNormals())
	{
		const int size = sizeof(float) * 3 * numVertices;
		totalSize += size;
	}

	if (assimpMesh->mTangents)
	{
		const int size = sizeof(float) * 3 * numVertices;
		totalSize += size;
	}

	if (assimpMesh->mTextureCoords[0])
	{
		const int size = sizeof(float) * 2 * numVertices;
		totalSize += size;
	}

	if(assimpMesh->HasFaces())
	{
		const int numIndices = assimpMesh->mNumFaces * 3;
		const int size = sizeof(uint16_t) * numIndices;
		totalSize += size;
	}

	return totalSize;
}

static bool AssimpProcessMesh(aiMesh* assimpMesh, const aiScene* scene, MeshData& mesh, char* meshBlob)
{
	uint32_t bytesRead = 0;

	const int numVertices = assimpMesh->mNumVertices;
	mesh.m_numVertices = numVertices;
	
	// Vertices

	if (assimpMesh->HasPositions())
	{
		const int size = sizeof(float) * 3 * numVertices;
		mesh.m_positions = (tVector3*)(meshBlob);
		(char*)meshBlob += size;

		memcpy(mesh.m_positions, assimpMesh->mVertices, size);
		bytesRead += size;
	}

	if (assimpMesh->HasNormals())
	{
		const int size = sizeof(float) * 3 * numVertices;
		mesh.m_normals = (tVector3*)(meshBlob);
		(char*)meshBlob += size;

		memcpy(mesh.m_normals, assimpMesh->mNormals, size);
		bytesRead += size;
	}

	if(assimpMesh->mTangents)
	{
		const int size = sizeof(float) * 3 * numVertices;
		mesh.m_tangents = (tVector3*)(meshBlob);
		(char*)meshBlob += size;

		memcpy(mesh.m_tangents, assimpMesh->mTangents, size);
		bytesRead += size;
	}

	if(assimpMesh->mTextureCoords[0])
	{
		const int size = sizeof(float) * 2 * numVertices;
		mesh.m_uvs = (tVector2*)(meshBlob);
		(char*)meshBlob += size;

		// Need to copy these the slow way since assimp uses vector3 for uvs
		const aiVector3D* uvSet = assimpMesh->mTextureCoords[0];
		for (int i = 0; i < numVertices; ++i)
		{
			memcpy(&mesh.m_uvs[i], &uvSet[i], sizeof(float) * 2);
		}
		bytesRead += size;
	}

	// Indices

	const int numIndices = assimpMesh->mNumFaces * 3;
	mesh.m_numIndices = numIndices;
	
	if (assimpMesh->HasFaces())
	{
		const int size = sizeof(uint16_t) * numIndices;
		mesh.m_indices = (uint16_t*)(meshBlob);
		(char*)meshBlob += size;

		for (uint16_t i = 0; i < assimpMesh->mNumFaces; i++)
		{
			const aiFace face = assimpMesh->mFaces[i];
			for (uint16_t j = 0; j < face.mNumIndices; j++)
			{
				mesh.m_indices[i * 3 + j] = face.mIndices[j];
			}
		}
		bytesRead += size;
	}

	uint32_t calculatedSize = CalculateMeshSize(assimpMesh);
	GRIM_ASSERT(bytesRead == calculatedSize, "Size mismatch between calculated mesh size %d and bytes read %d", calculatedSize, bytesRead);

	return true;
}

static bool AssimpProcessScene(aiNode* node, const aiScene* scene, MeshResource& meshResource)
{
	GRIM_ASSERT(node->mNumMeshes <= MeshResource::kMaxMeshesPerResource, "Attempted to load a scene with more than %d meshes, check data then bump the limit", MeshResource::kMaxMeshesPerResource);

	bool success = false;

	for (int i = 0; i < (int)node->mNumMeshes; i++)
	{
		aiMesh* assimpMesh = scene->mMeshes[i];
		const uint32_t size = CalculateMeshSize(assimpMesh);

		meshResource.m_totalSizeInBytes = size;
		char* meshBlob = static_cast<char*>(malloc(size));

		success |= AssimpProcessMesh(assimpMesh, scene, meshResource.m_meshes[meshResource.m_numMeshes++], meshBlob);
	}

	for (int i = 0; i < (int)node->mNumChildren; i++)
	{
		success |= AssimpProcessScene(node->mChildren[i], scene, meshResource);
	}

	return success;
}

bool LoadMeshesFromFile(MeshResource& meshResource, const char* filepath)
{
#if _DEBUG
	GRIM_ASSERT(strlen(filepath) <= Resource::kMaxResourceNameSize, "Length of mesh filepath %s is greater than the maximum allowed size %d, consider bumping the size?", filepath, Resource::kMaxResourceNameSize);
	strcpy(meshResource.m_name, filepath);
#endif

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filepath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	GRIM_ASSERT(scene, "Failed to load mesh from filepath %s", filepath);

	return AssimpProcessScene(scene->mRootNode, scene, meshResource);
}

Material::Material() :
	Resource(),
	m_isShared(false)
{}

bool Material::serializeBinary(char* buffer)const
{
	return true;
}
bool Material::deserializeBinary(char* buffer)
{
	return true;
}
}