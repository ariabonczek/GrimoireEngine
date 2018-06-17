#include "Precompiled.h"

#include "Grimoire.h"
#include "Element.h"

#include <fstream>
#include <random>

static std::mt19937_64 s_rand;

static const int kNameLength = 10;
static const int kMaxDecimalPlaces = 3;

static const float kMinPosition = -100.0f;
static const float kMaxPosition = 100.0f;
static const float kMinRotation = 0.0f;
static const float kMaxRotation = 360.0f;
static const float kMinScale = 0.01f;
static const float kMaxScale = 3.0f;

namespace grim{

static const char* kLmdbKeyPrefix = "__grimoire__";
static const int kLmdbKeySize = Header::kMaxGrimoireNameSize + sizeof(kLmdbKeyPrefix);

Grimoire::Grimoire()
{
	m_entities.reserve(100);
	for (int i = 0; i < kNumElementTypes; ++i)
	{
		m_elements[i].reserve(100);
	}
	memset(&m_header, 0, sizeof Header);
}

Grimoire::~Grimoire()
{}

void Grimoire::ClearGrimoire()
{
	// Entities
	for (int i = 0; i < m_entities.size(); ++i)
	{
		const Entity* entity = m_entities[i];

		delete entity;
	}

	// Elements
	for (int i = 0; i < kNumElementTypes; ++i)
	{
		const std::vector<Element*>& elementList = m_elements[i];

		for (int j = 0; j < elementList.size(); ++j)
		{
			const Element* element = elementList[j];
			
			delete element;
		}
	}

	// Resources
	for (int i = 0; i < kNumResourceTypes; ++i)
	{
		const std::vector<Resource*>& resourceList = m_resources[i];

		for (int j = 0; j < resourceList.size(); ++j)
		{
			const Resource* resource = resourceList[j];

			delete resource;
		}
	}

	memset(&m_header, 0, sizeof Header);
	m_entities.clear();
	m_entityMap.clear();
	for (int i = 0; i < kNumElementTypes; ++i)
		m_elements[i].clear();
	for (int i = 0; i < kNumResourceTypes; ++i)
		m_resources[i].clear();
}

void Grimoire::BuildForExport(bool optimized)
{
	if (optimized)
		BuildOptimized();
	else
		BuildUnoptimized();
}

void Grimoire::BuildOptimized()
{
	GRIM_ASSERT(false, "BuildOptimized unimplemented");
}

void Grimoire::BuildUnoptimized()
{
	for (int i = 0; i < kNumElementTypes; ++i)
		m_elements[i].clear();

	for (int i = 0; i < m_header.numEntities; ++i)
	{
		const Entity* entity = m_entities[i];
		for (int j = 0; j < kNumElementTypes; ++j)
		{
			if (entity->m_elementList[j])
				m_elements[j].push_back(entity->m_elementList[j]);
		}
	}
}

void Grimoire::ReadHeader(const grimJson::JsonDocument& doc)
{
	const grimJson::JsonValue& jsonHeader = doc["header"];
	{
		m_header.numEntities = jsonHeader["numEntities"].GetInt();
		m_header.totalElements = jsonHeader["totalElements"].GetInt();
		const grimJson::JsonValue& jsonElements = jsonHeader["numElements"];
		GRIM_ASSERT(jsonElements.IsObject(), "numElements is not a JSON Object");
		for (int i = 0; i < kNumElementTypes; ++i)
		{
			m_header.numElementsOfType[i] = jsonElements[g_elementTypeNames[i]].GetInt();
		}
		 
		m_header.totalResources = jsonHeader["numResources"].GetInt();

		const grimJson::JsonValue& jsonResourcesOfType = jsonHeader["numResourcesOfType"];
		GRIM_ASSERT(jsonResourcesOfType.IsObject(), "numResourcesOfType is not a JSON Object");
		for (int i = 0; i < kNumResourceTypes; ++i)
		{
			m_header.numResourcesOfType[i] = jsonResourcesOfType[g_resourceTypeNames[i]].GetInt();
		}
	}
}

void Grimoire::ReadResources(const grimJson::JsonDocument& doc)
{

}

static Entity* CreateNewEntity(const std::string name)
{
	uint64_t uuid = s_rand();
	Entity* entity = new Entity(uuid, name.c_str());

	return entity;
}

static Element* CreateNewElement(Entity*const entity, const ElementType type)
{
	switch (type)
	{
		case kModel:
		{
			return new ModelElement(entity);
		}
		case kLight:
		{
			return new Light(entity);
		}
		default:
		{
			GRIM_ASSERT(false, "ERROR: Attempted to create an entity of an unknown type");
			return nullptr;
		}
	}
}

static Resource* CreateNewResource(const ResourceType type)
{
	switch (type)
	{
		case kMesh:
		{
			return new MeshResource();
		}
		case kMaterial:
		{
			return new Material();
		}
		default:
		{
			GRIM_ASSERT(false, "ERROR: Attempted to create an resource of an unknown type");
			return nullptr;
		}
	}
}

void Grimoire::ReadElements(const grimJson::JsonDocument& doc, grimJson::JsonAllocator& allocator)
{
	const grimJson::JsonValue& jsonElements = doc["elements"];
	{	
		m_entities.reserve(m_header.numEntities);
		for (int i = 0; i < kNumElementTypes; ++i)
		{
			m_elements[i].reserve(m_header.numElementsOfType[i]);
			
			const grimJson::JsonValue& jsonThisElementList = jsonElements[g_elementTypeNames[i]];
	
			GRIM_ASSERT(jsonThisElementList.IsArray(), "Non-array found for element list, are you sure you're reading the file correctly?")
			for (int j = 0; j < m_header.numElementsOfType[i]; ++j)
			{
				const grimJson::JsonValue& jsonThisElement = jsonThisElementList[j];
	
				uint64_t uuid = jsonThisElement["uuid"].GetUint64();
				std::string name = jsonThisElement["name"].GetString();
	
				Entity* entity = nullptr;
				if (m_entityMap.find(uuid) == m_entityMap.end())
				{
					entity = new Entity(uuid, name.c_str());
					m_entities.push_back(entity);
					m_entityMap[uuid] = entity;
				}
				else
					entity = m_entityMap[uuid];
	
				const grimJson::JsonValue& attributes = jsonThisElement["attributes"];
				GRIM_ASSERT(attributes.IsObject(), "Object %s attributes not a JSON object", entity->m_name);
				GRIM_ASSERT(entity->m_elementList[i] == nullptr, "Found two of the same element type attached to the same entity; name: %s guid: %ull", entity->m_name, entity->m_uuid);
				
				Element* element = CreateNewElement(entity, (ElementType)(i));
				element->deserializeJSON(attributes, allocator);
				entity->m_elementList[i] = element;	
				m_elements[i].push_back(element);
			}
		}
	}
}

void Grimoire::ReadFromFile_Json(const char* filename)
{
	ClearGrimoire();
	
	// Read .grimoire file
	std::ifstream file(filename);
	if (!file.is_open())
		return;

	file.seekg(0, std::ios::end);
	int size = (int)file.tellg();
	file.seekg(0, std::ios::beg);

	char* fileContents = new char[size];
	memset(fileContents, 0, size);

	file.read(fileContents, size);

	file.close();

	// Parse json into Grimoire
	grimJson::JsonDocument doc;
	doc.Parse(fileContents);
	if (doc.HasParseError())
	{
		GRIM_ASSERT(false, "Attempt to parse grimoire file %s gave the following error code: %d", filename, doc.GetParseError());
		return;
	}
	// Header
	ReadHeader(doc);
	ReadResources(doc);
	ReadElements(doc, doc.GetAllocator());

	GRIM_ASSERT(m_header.numEntities == m_entities.size(), "Total entities in header does not match total entities in the read JSON document");
	for (int i = 0; i < kNumElementTypes; ++i)
		GRIM_ASSERT(m_header.numElementsOfType[i] == m_elements[i].size(), "Total elements in header does not match total elements in the read JSON document");

	delete fileContents;
}

void Grimoire::WriteHeader(grimJson::JsonWriter& writer)const
{
	writer.Key("header");
	{
		writer.StartObject(); // start header

		writer.Key("numEntities"); writer.Int(m_header.numEntities);
		writer.Key("numElements"); writer.StartObject();
		for (int i = 0; i < kNumElementTypes; ++i)
		{
			writer.Key(g_elementTypeNames[i]); writer.Int(m_header.numElementsOfType[i]);
		}
		writer.EndObject();
		writer.Key("totalElements"); writer.Int(m_header.totalElements);

		writer.Key("numResources"); writer.Int(m_header.totalResources);
		writer.Key("numResourcesOfType"); writer.StartObject();
		for (int i = 0; i < kNumResourceTypes; ++i)
		{
			writer.Key(g_resourceTypeNames[i]); writer.Int(m_header.numResourcesOfType[i]);
		}
		writer.EndObject();

		writer.EndObject(); // end header
	}
}

void Grimoire::WriteResources(grimJson::JsonWriter& writer)const
{
	writer.Key("resources");
	{
		writer.StartArray();

		writer.Key("dummy key");
		writer.String("dummy value");

		writer.EndArray();
	}
}

void Grimoire::WriteElements(grimJson::JsonWriter& writer, grimJson::JsonAllocator& allocator)const
{
	writer.Key("elements");
	{
		writer.StartObject();

		for (int i = 0; i < kNumElementTypes; ++i)
		{
			const std::vector<Element*>& currElementList = m_elements[i];
			writer.Key(g_elementTypeNames[i]);
			 
			writer.StartArray();
			{
				for (int j = 0; j < m_header.numElementsOfType[i]; ++j)
				{
					const Element* currElement = currElementList[j];
					writer.StartObject();
					writer.Key("uuid"); writer.Uint64(currElement->m_entity->m_uuid);
					writer.Key("name"); writer.String(currElement->m_entity->m_name);

					writer.Key("attributes");
					grimJson::JsonValue value(rapidjson::kObjectType);
					currElement->serializeJSON(value, allocator);
					value.Accept(writer);

					writer.EndObject();
				}
			}
			writer.EndArray();
		}

		writer.EndObject(); // end objects
	}
}

void Grimoire::WriteToFile_Json(const char* filename)
{
	GRIM_ASSERT(m_header.numEntities > 0, "Trying to write a grimoire file without any entities");
	GRIM_ASSERT(m_entities.size() == m_header.numEntities, "Trying to write a grimoire file without any entities");
	GRIM_ASSERT(m_header.totalElements > 0, "Trying to write a grimoire file without any elements");

	rapidjson::StringBuffer s;
	grimJson::JsonWriter writer(s);
	grimJson::JsonAllocator allocator;

	writer.SetMaxDecimalPlaces(kMaxDecimalPlaces);

	// Write json
	writer.StartObject();

	WriteHeader(writer);
	WriteResources(writer);
	WriteElements(writer, allocator);

	writer.EndObject();

	// Write to file
	std::ofstream out;
	out.open(filename);
	out << s.GetString();
	out.close();
}

uint32_t Grimoire::GetSizeofBinaryBlob()
{
	const uint32_t blobSize =(uint32_t)(
		sizeof(Header) +
		sizeof(Entity::binaryBlob) * m_entities.size() + 
		GetSizeofElementBlob() + 
		GetSizeofResourceBlob());

	return blobSize;
}

uint32_t Grimoire::GetSizeofElementBlob()
{
	uint32_t totalSize = 0;

	for (int i = 0; i < kNumElementTypes; ++i)
	{
		const std::vector<Element*>& elementList = m_elements[i];

		for (int j = 0; j < elementList.size(); ++j)
		{
			const Element* element = elementList[j];
			totalSize += sizeof(grim::uuid);
			totalSize += sizeof(ElementType);
			totalSize += element->getSerializedBinarySize();
		}
	}

	return totalSize;
}

uint32_t Grimoire::GetSizeofResourceBlob()
{
	uint32_t totalSize = 0;

	for (int i = 0; i < kNumResourceTypes; ++i)
	{
		const std::vector<Resource*>& resourceList = m_resources[i];

		for (int j = 0; j < resourceList.size(); ++j)
		{
			const Resource* resource = resourceList[j];
			totalSize += sizeof(ResourceType);
			totalSize += resource->m_totalSizeInBytes;
		}
	}

	return totalSize;
}

void Grimoire::ReadFromFile_Binary(const char* filename)
{
	// Read from file
	FILE* file = fopen(filename, "rb");
	fseek(file, 0L, SEEK_END);
	const uint32_t fileSize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	char* blob = static_cast<char*>(malloc(fileSize));
	fread(blob, fileSize, 1, file);

	fclose(file);

	bool success = ReadFromBinaryBlob(blob, fileSize);
	GRIM_ASSERT(success, "Failed to read binary grimoire file %s", filename);

	free(blob);
}

void Grimoire::WriteToFile_Binary(const char* filename, bool optimized)
{
	const uint32_t blobSize = GetSizeofBinaryBlob();
	char* blob = static_cast<char*>(malloc(blobSize));

	WriteToBinaryBlob(blob, blobSize);
	
	// Write to file
	FILE* file = fopen(filename, "wb");
	fwrite(blob, blobSize, 1, file);
	fclose(file);

	free(blob);
}

void Grimoire::ReadFromLmdb(grimLmdb::Environment& lmdbEnvironment)
{
	char lmdbKey[kLmdbKeySize];
	snprintf(lmdbKey, kLmdbKeySize, "%s%s", kLmdbKeyPrefix, m_header.grimoireName);

}

void Grimoire::WriteToLmdb(grimLmdb::Environment& lmdbEnvironment)
{
	char lmdbKey[kLmdbKeySize];
	snprintf(lmdbKey, kLmdbKeySize, "%s%s", kLmdbKeyPrefix, m_header.grimoireName);

	const uint32_t blobSize = GetSizeofBinaryBlob();
	char* blob = static_cast<char*>(malloc(blobSize));

	WriteToBinaryBlob(blob, blobSize);

	bool success = lmdbEnvironment.WriteBuffer(lmdbKey, blob, blobSize);
	GRIM_ASSERT(success, "Failed to write blob to lmdb for key value %s and blob size %d", blob, blobSize);
}

bool Grimoire::WriteToBinaryBlob(char* blob, uint32_t blobSize)
{
	// key needs to be unique, add a prefix?

	uint32_t bytesWritten = 0;

	memcpy(blob, &m_header, sizeof(Header));
	blob += sizeof(Header);
	bytesWritten += sizeof(Header);

	// Entities
	for (int i = 0; i < m_entities.size(); ++i)
	{
		const Entity* entity = m_entities[i];

		Entity::binaryBlob entityBlob;
		
		entityBlob.uuid = entity->m_uuid;
		strcpy_s(entityBlob.name, Entity::kMaxEntityNameSize, entity->m_name);

		const uint32_t size = sizeof(Entity::binaryBlob);
		memcpy(blob, &entityBlob, size);
		blob += size;
		bytesWritten += size;
	}

	// Elements
	for (int i = 0; i < kNumElementTypes; ++i)
	{
		const std::vector<Element*>& elementList = m_elements[i];

		for (int j = 0; j < elementList.size(); ++j)
		{
			const Element* element = elementList[j];

			{
				const uint32_t size = sizeof(grim::uuid);
				memcpy(blob, &element->m_entity->m_uuid, size);
				blob += size;
				bytesWritten += size;
			}

			{
				const uint32_t size = sizeof(ElementType);
				memcpy(blob, &element->m_type, size);
				blob += size;
				bytesWritten += size;
			}

			{
				const int32_t elementSize = element->getSerializedBinarySize();
				bool success = element->serializeBinary(blob);
				GRIM_ASSERT(success, "Failed to serialize %s element belonging to entity %s", g_elementTypeNames[element->m_type], element->m_entity->m_name);
				blob += elementSize;
				bytesWritten += elementSize;
			}
		}
	}

	// Resources
	for (int i = 0; i < kNumResourceTypes; ++i)
	{
		const std::vector<Resource*>& resourceList = m_resources[i];

		for (int j = 0; j < resourceList.size(); ++j)
		{
			const Resource* resource = resourceList[j];

			{
				const uint32_t size = sizeof(grim::uuid);
				memcpy(blob, &resource->m_uuid, size);
				blob += size;
				bytesWritten += size;
			}

			{
				const uint32_t size = sizeof(ResourceType);
				memcpy(blob, &resource->m_type, size);
				blob += size;
				bytesWritten += size;
			}

			{
				const uint32_t resourceSize = resource->m_totalSizeInBytes;

				bool success = resource->serializeBinary(blob);
				GRIM_ASSERT(success, "Failed to serialize %s resource with name %s", g_resourceTypeNames[resource->m_type], resource->m_name);
				blob += resourceSize;
				bytesWritten += resourceSize;
			}
		}
	}

	GRIM_ASSERT(bytesWritten == blobSize, "Bad estimation of blobSize, estimated size of %d bytes but serialized %d bytes", blobSize, bytesWritten);

	return true;
}

bool Grimoire::ReadFromBinaryBlob(char* blob, uint32_t blobSize)
{
	bool success = true;
	uint32_t bytesRead = 0;

	// First we have the header, which is easy
	memcpy(&m_header, blob, sizeof(Header));
	blob += sizeof(Header);
	bytesRead += sizeof(Header);

	// Reserve space in lists
	m_entities.reserve(m_header.numEntities);
	for (int i = 0; i < kNumElementTypes; ++i)
	{
		m_elements[i].reserve(m_header.numElementsOfType[i]);
	}
	for (int i = 0; i < kNumResourceTypes; ++i)
	{
		m_resources[i].reserve(m_header.numResourcesOfType[i]);
	}

	// Then we load the entity list
	for (int i = 0; i < m_header.numEntities; ++i)
	{
		Entity::binaryBlob entityBlob;
		const uint32_t size = sizeof(Entity::binaryBlob);
		memcpy(&entityBlob, blob, size);
		blob += size;
		bytesRead += size;

		GRIM_ASSERT(m_entityMap.find(entityBlob.uuid) == m_entityMap.end(), "Found duplicate uuid in entity map for entity name %s", entityBlob.name);

		Entity* entity = new Entity(entityBlob.uuid, entityBlob.name);
		m_entities.push_back(entity);
		m_entityMap[entityBlob.uuid] = entity;
	}

	// Then we load the element lists
	for (int i = 0; i < m_header.totalElements; ++i)
	{
		grim::uuid entityUuid;
		{
			const uint32_t size = sizeof(grim::uuid);
			memcpy(&entityUuid, blob, size);
			blob += size;
			bytesRead += size;
		}

		ElementType elementType;
		{
			const uint32_t size = sizeof(ElementType);
			memcpy(&elementType, blob, size);
			blob += size;
			bytesRead += size;
		}

		//entity stuff
		GRIM_ASSERT(m_entityMap.find(entityUuid) != m_entityMap.end(), "Element of type %s found with no entity corresponding to its uuid", g_elementTypeNames[elementType]);
		Entity* entity = m_entityMap[entityUuid];

		Element* element = CreateNewElement(entity, elementType);
		{
			const int32_t elementSize = element->getSerializedBinarySize();
			success &= element->deserializeBinary(blob);
			blob += elementSize;
			bytesRead += elementSize;
		}

		std::vector<Element*>& elementList = m_elements[elementType];
		elementList.push_back(element);
	}

	// Then we load the resource lists
	for (int i = 0; i < m_header.totalResources; ++i)
	{
		grim::uuid resourceUuid;
		{
			const uint32_t size = sizeof(grim::uuid);
			memcpy(&resourceUuid, blob, size);
			blob += size;
			bytesRead += size;
		}

		ResourceType resourceType;
		{
			const uint32_t size = sizeof(ResourceType);
			memcpy(&resourceType, blob, size);
			blob += size;
			bytesRead += size;
		}

		std::vector<Resource*> resourceList = m_resources[resourceType];
		Resource* resource = CreateNewResource(resourceType);
		{
			success &= resource->deserializeBinary(blob);
			blob += resource->m_totalSizeInBytes;
			bytesRead += resource->m_totalSizeInBytes;
		}
		resourceList.push_back(resource);
	}

	GRIM_ASSERT(bytesRead == blobSize, "Bad estimation of blobSize, estimated size of %d bytes but deserialized %d bytes", blobSize, bytesRead);

	return true;
}

//static float RandomFloat(float min, float max)
//{
//	return min + s_rand() / (s_rand.max() / (max - min));
//}
//
//static tVector3 RandomVector3(float min, float max)
//{
//	tVector3 v;
//
//	v.x = RandomFloat(min, max);
//	v.y = RandomFloat(min, max);
//	v.z = RandomFloat(min, max);
//	return v;
//
//}
//
//static tColorRGBA RandomColor()
//{
//	tColorRGBA c;
//
//	c.r = RandomFloat(0.0f, 1.0f);
//	c.g = RandomFloat(0.0f, 1.0f);
//	c.b = RandomFloat(0.0f, 1.0f);
//	c.a = RandomFloat(0.0f, 1.0f);
//	return c;
//}
//
//void Grimoire::GenerateRandom(const int numEntities, const uint64_t seed)
//{
//	s_rand.seed(seed);
//
//	GRIM_ASSERT(m_header.numEntities == 0, "Trying to generate into a grimoire file that already has objects");
//	GRIM_ASSERT(m_entities.size() == 0, "Trying to generate into a grimoire file that has an object heap.");
//
//	m_entities.resize(numEntities);
//	m_header.numEntities = numEntities;
//
//	char buffer[kNameLength];
//
//	for (int i = 0; i < numEntities; ++i)
//	{
//		for (int j = 0; j < kNameLength - 1; ++j)
//		{
//			buffer[j] = s_rand() % 26 + 97;
//		}
//		buffer[kNameLength - 1] = '\0';
//
//		m_entities[i] = CreateNewEntity(buffer);
//		Entity* entity = m_entities[i];
//
//		const int numElements = s_rand() % kNumElementTypes + 1; // minimum 1 element
//		int index = s_rand() % kNumElementTypes;
//		for (int j = 0; j < numElements; ++j)
//		{
//			index %= kNumElementTypes;
//			Element* element = CreateNewElement(entity, (ElementType)index);
//
//			switch (index)
//			{
//				case kModel:
//				{
//					ModelElement* model = static_cast<ModelElement*>(element);
//					model->m_attributes.transform.position = RandomVector3(kMinPosition, kMaxPosition);
//					model->m_attributes.transform.rotation = RandomVector3(kMinRotation, kMaxRotation);
//					model->m_attributes.transform.scale = RandomVector3(kMinScale, kMaxScale);
//					break;
//				}
//				case kLight:
//				{
//					Light* light = static_cast<Light*>(element);
//					light->m_attributes.color = RandomColor();
//					light->m_attributes.intensity = RandomFloat(-10.0f, 20.0f);
//					light->m_attributes.type = (grimDat::tLight::eLightType) (s_rand() % grimDat::tLight::kNumLightTypes);
//					light->m_attributes.range = RandomFloat(0.01f, 50.0f);
//					break;
//				}
//				default:
//					GRIM_ASSERT(false, "Somehow got invalid element type index %d", index);
//					break;
//			}
//
//			entity->m_elementList[index] = element;
//			++m_header.numElements[index];
//			++m_header.totalElements;
//
//			++index;
//		}
//
//	}
//}
}