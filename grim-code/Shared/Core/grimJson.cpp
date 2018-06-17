#include <Shared\Core\grimJson.h>

#include <string>

namespace grimJson 
{
	void ParseDocument(JsonDocument& doc, const char* jsonText)
	{
		rapidjson::ParseResult res = doc.Parse(jsonText);
		GRIM_ASSERT(!res.IsError(), "JSON parse error: %s (%zu)", rapidjson::GetParseError_En(res.Code()), res.Offset());
	}

	bool serializeToJSON(const bool self, JsonValue& value, JsonAllocator& allocator)
	{
		value.SetBool(self);
		return true;
	}

	bool serializeToJSON(const char* self, JsonValue& value, JsonAllocator& allocator)
	{
		value.SetString(self, allocator);
		return true;
	}

	bool serializeToJSON(const int self, JsonValue& value, JsonAllocator& allocator)
	{
		value.SetInt(self);
		return true;
	}

	bool serializeToJSON(const long self, JsonValue& value, JsonAllocator& allocator)
	{
		value.SetInt64(self);
		return true;
	}

	bool serializeToJSON(const float self, JsonValue& value, JsonAllocator& allocator)
	{
		value.SetFloat(self);
		return true;
	}

	bool serializeToJSON(const double self, JsonValue& value, JsonAllocator& allocator)
	{
		value.SetDouble(self);
		return true;
	}

	bool serializeToJSON(const uint32_t self, JsonValue& value, JsonAllocator& allocator)
	{
		value.SetUint(self);
		return true;
	}

	bool serializeToJSON(const uint64_t self, JsonValue& value, JsonAllocator& allocator)
	{
		value.SetUint64(self);
		return true;
	}

	bool initializeFromJSON(bool& self, const JsonValue& value, JsonAllocator& allocator)
	{
		self = value.GetBool();
		return true;
	}

	bool initializeFromJSON(char* self, const JsonValue& value, JsonAllocator& allocator)
	{
		const char* temp = value.GetString();
		self = static_cast<char*>(allocator.Malloc(strlen(temp)));
		strcpy(self, temp);
		return true;
	}

	bool initializeFromJSON(int& self, const JsonValue& value, JsonAllocator& allocator)
	{
		self = value.GetInt();
		return true;
	}

	bool initializeFromJSON(long long& self, const JsonValue& value, JsonAllocator& allocator)
	{
		self = value.GetInt64();
		return true;
	}

	bool initializeFromJSON(float& self, const JsonValue& value, JsonAllocator& allocator)
	{
		self = value.GetFloat();
		return true;
	}

	bool initializeFromJSON(double& self, const JsonValue& value, JsonAllocator& allocator)
	{
		self = value.GetDouble();
		return true;
	}

	bool initializeFromJSON(uint32_t& self, const JsonValue& value, JsonAllocator& allocator)
	{
		self = value.GetUint();
		return true;
	}

	bool initializeFromJSON(uint64_t& self, const JsonValue& value, JsonAllocator& allocator)
	{
		self = value.GetUint64();
		return true;
	}
}