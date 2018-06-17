#pragma once

#include "Element.h"
#include "Resource.h"

#include <Shared\Core\grimJson.h>
#include <Shared\Core\grimLmdb.h>
#include <vector>
#include <map>

namespace grim{
struct Header
{
	enum { kMaxGrimoireNameSize = 64 };
	char grimoireName[kMaxGrimoireNameSize];

	int numEntities;
	int totalElements;
	int numElementsOfType[kNumElementTypes];

	int totalResources;
	int numResourcesOfType[kNumResourceTypes];
};

class Grimoire
{
public:
	Grimoire(); 
	~Grimoire(); 

	void ClearGrimoire();
	void BuildForExport(bool optimized = false);
	
	void ReadFromFile_Json(const char* filename);
	void WriteToFile_Json(const char* filename);

	void ReadFromFile_Binary(const char* filename);
	void WriteToFile_Binary(const char* filename, bool optimized = false);

	void ReadFromLmdb(grimLmdb::Environment& lmdbEnvironment);
	void WriteToLmdb(grimLmdb::Environment& lmdbEnvironment);

	//void GenerateRandom(const int numObjects, uint64_t seed = 8675309);
private:

	void BuildOptimized();
	void BuildUnoptimized();
	
	void ReadHeader(const grimJson::JsonDocument& doc);
	void ReadResources(const grimJson::JsonDocument& doc);
	void ReadElements(const grimJson::JsonDocument& doc, grimJson::JsonAllocator& allocator);

	void WriteHeader(grimJson::JsonWriter& writer)const;
	void WriteResources(grimJson::JsonWriter& writer)const;
	void WriteElements(grimJson::JsonWriter& writer, grimJson::JsonAllocator& allocator)const;

	uint32_t GetSizeofBinaryBlob();
	uint32_t GetSizeofElementBlob();
	uint32_t GetSizeofResourceBlob();

	bool WriteToBinaryBlob(char* blob, uint32_t sizeOfBlob);
	bool ReadFromBinaryBlob(char* blob, uint32_t sizeOfBlob);

private:
	Header m_header;

	std::vector<Entity*> m_entities;
	std::map<grim::uuid, Entity*> m_entityMap;
	std::vector<Element*> m_elements[kNumElementTypes];
	std::vector<Resource*> m_resources[kNumResourceTypes];

	bool m_isBuilt;
};
}