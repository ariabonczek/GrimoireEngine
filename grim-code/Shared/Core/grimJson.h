#pragma once

#include <Shared/rapidjson/document.h>
#include <Shared/rapidjson/allocators.h>
#include <Shared/rapidjson/prettywriter.h>
#include <Shared/rapidjson/error/en.h>

#include <Shared/Core/Assert.h>

#include <stdint.h>

namespace grimJson 
{
	typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> JsonWriter;
	typedef rapidjson::Document JsonDocument;
	typedef rapidjson::Value JsonValue;
	typedef rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> JsonAllocator;

	void ParseDocument(JsonDocument& doc, const char* jsonText);

	bool serializeToJSON(const bool self, JsonValue& value, JsonAllocator& allocator);
	bool serializeToJSON(const char* self, JsonValue& value, JsonAllocator& allocator);
	bool serializeToJSON(const int self, JsonValue& value, JsonAllocator& allocator);
	bool serializeToJSON(const long self, JsonValue& value, JsonAllocator& allocator);
	bool serializeToJSON(const float self, JsonValue& value, JsonAllocator& allocator);
	bool serializeToJSON(const double self, JsonValue& value, JsonAllocator& allocator);
	bool serializeToJSON(const uint32_t self, JsonValue& value, JsonAllocator& allocator);
	bool serializeToJSON(const uint64_t self, JsonValue& value, JsonAllocator& allocator);

	bool initializeFromJSON(bool& self, const JsonValue& value, JsonAllocator& allocator);
	bool initializeFromJSON(char* self, const JsonValue& value, JsonAllocator& allocator);
	bool initializeFromJSON(int& self, const JsonValue& value, JsonAllocator& allocator);
	bool initializeFromJSON(long long& self, const JsonValue& value, JsonAllocator& allocator);
	bool initializeFromJSON(float& self, const JsonValue& value, JsonAllocator& allocator);
	bool initializeFromJSON(double& self, const JsonValue& value, JsonAllocator& allocator);
	bool initializeFromJSON(uint32_t& self, const JsonValue& value, JsonAllocator& allocator);
	bool initializeFromJSON(uint64_t& self, const JsonValue& value, JsonAllocator& allocator);
}