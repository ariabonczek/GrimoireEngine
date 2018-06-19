#pragma once

#include <stdlib.h>
#include <string>
#include <vector>

#include <Shared/Core/Uuid.h>
#include "Resource.h"

#include <Generated\generated_Model.h>
#include <Generated\generated_Light.h>

namespace grim {
class Element;

enum ElementType
{
	kModel,
	kLight,
	kNumElementTypes
};

extern char* g_elementTypeNames[kNumElementTypes];

struct ResourceEntry
{
	char* name;
};

class Entity
{
public:
	enum { kMaxEntityNameSize = 64 };
	Entity(const uint64_t uuid, const char* name);

	struct binaryBlob
	{
		grim::uuid uuid;
		char name[kMaxEntityNameSize];
	};

	grim::uuid m_uuid;
	char m_name[kMaxEntityNameSize];
	Element* m_elementList[kNumElementTypes];
};

class __declspec(novtable) Element
{
public:
	Element(Entity*const entity, const ElementType type);

	virtual bool serializeJSON(grimJson::JsonValue& value, grimJson::JsonAllocator& allocator) const = 0;
	virtual bool deserializeJSON(const grimJson::JsonValue& value, grimJson::JsonAllocator& allocator) = 0;
	
	virtual uint32_t getSerializedBinarySize()const = 0;
	virtual bool serializeBinary(void* buffer)const = 0;
	virtual bool deserializeBinary(void* buffer) = 0;

	// The entity this Element "belongs to"
	Entity* m_entity;

	ElementType m_type;
};

class ModelElement : public Element
{
public:
	struct binaryBlob
	{
		grim::uuid meshResourceUuid;
		gd::tModel m_attributes;
	};

	ModelElement(Entity* const entity);

	bool serializeJSON(grimJson::JsonValue& value, grimJson::JsonAllocator& allocator)const;
	bool deserializeJSON(const grimJson::JsonValue& value, grimJson::JsonAllocator& allocator);
	
	uint32_t getSerializedBinarySize()const;
	bool serializeBinary(void* buffer)const;
	bool deserializeBinary(void* buffer);

	// Resource Definitions
	MeshResource* m_mesh;
	Material* m_material;

	gd::tModel m_attributes;
};

class Light : public Element
{
public:
	struct binaryBlob
	{
		gd::tLight m_attributes;
	};

	Light(Entity*const entity);

	bool serializeJSON(grimJson::JsonValue& value, grimJson::JsonAllocator& allocator)const;
	bool deserializeJSON(const grimJson::JsonValue& value, grimJson::JsonAllocator& allocator);

	uint32_t getSerializedBinarySize()const;
	bool serializeBinary(void* buffer)const;
	bool deserializeBinary(void* buffer);

	gd::tLight m_attributes;
};
}