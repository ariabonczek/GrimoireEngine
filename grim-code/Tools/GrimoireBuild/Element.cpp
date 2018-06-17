#include "Precompiled.h"
#include "Element.h"

namespace grim{
char* g_elementTypeNames[kNumElementTypes] = { "model", "light"};

Entity::Entity(uint64_t uuid, const char* name):
	m_uuid(uuid)
{
	memset(m_name, 0, kMaxEntityNameSize);
	strcpy_s(m_name, kMaxEntityNameSize, name);
	memset(&m_elementList, 0, sizeof(Element*) * kNumElementTypes);
}

Element::Element(Entity*const entity, const ElementType type) :
	m_entity(entity),
	m_type(type)
{
	GRIM_ASSERT(m_entity->m_elementList[type] == nullptr, "Attempt made two assign two %s elements to entity %s", g_elementTypeNames[type], m_entity->m_name);
	m_entity->m_elementList[type] = this;
}

ModelElement::ModelElement(Entity* entity) :
	Element(entity, kModel),
	m_mesh(nullptr),
	m_material(nullptr),
	m_attributes()
{}

bool ModelElement::serializeJSON(grimJson::JsonValue& value, grimJson::JsonAllocator& allocator)const
{
	return grimJson::serializeToJSON(m_attributes, value, allocator);
}

bool ModelElement::deserializeJSON(const grimJson::JsonValue& value, grimJson::JsonAllocator& allocator)
{
	return grimJson::initializeFromJSON(m_attributes, value, allocator);
}

uint32_t ModelElement::getSerializedBinarySize()const
{
	return sizeof(ModelElement::binaryBlob);
}

bool ModelElement::serializeBinary(void* buffer)const
{
	binaryBlob blob;
	if (m_mesh)
		blob.meshResourceUuid = m_mesh->m_uuid;
	else
		blob.meshResourceUuid = 0;
	blob.m_attributes = m_attributes;

	memcpy(buffer, &blob, sizeof(binaryBlob));

	return true;
}

bool ModelElement::deserializeBinary(void* buffer)
{
	binaryBlob blob;
	memcpy(&blob, buffer, sizeof(binaryBlob));

	m_attributes = blob.m_attributes;
	// todo uuid to resource pointer

	return true;
}

Light::Light(Entity* entity) :
	Element(entity, kLight),
	m_attributes()
{}

bool Light::serializeJSON(grimJson::JsonValue& value, grimJson::JsonAllocator& allocator)const
{
	return grimJson::serializeToJSON(m_attributes, value, allocator);
}

bool Light::deserializeJSON(const grimJson::JsonValue& value, grimJson::JsonAllocator& allocator)
{
	return grimJson::initializeFromJSON(m_attributes, value, allocator);
}

uint32_t Light::getSerializedBinarySize()const
{
	return sizeof(Light::binaryBlob);
}

bool Light::serializeBinary(void* buffer)const
{
	binaryBlob blob;
	blob.m_attributes = m_attributes;

	memcpy(buffer, &blob, sizeof(binaryBlob));

	return true;
}

bool Light::deserializeBinary(void* buffer)
{
	binaryBlob blob;
	memcpy(&blob, buffer, sizeof(binaryBlob));
	
	m_attributes = blob.m_attributes;
	return true;
}
}