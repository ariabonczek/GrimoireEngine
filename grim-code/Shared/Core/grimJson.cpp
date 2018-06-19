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

	bool serializeToJSON(const Vector2 self, JsonValue& value, JsonAllocator& allocator)
	{
		bool ret;
		JsonValue jsonMember;
		ret = grimJson::serializeToJSON(self.x, jsonMember, allocator);
		value.AddMember("x", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.y, jsonMember, allocator);
		value.AddMember("y", jsonMember, allocator);
		return ret;
	}

	bool serializeToJSON(const Vector3 self, JsonValue& value, JsonAllocator& allocator)
	{
		bool ret;
		JsonValue jsonMember;
		ret = grimJson::serializeToJSON(self.x, jsonMember, allocator);
		value.AddMember("x", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.y, jsonMember, allocator);
		value.AddMember("y", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.z, jsonMember, allocator);
		value.AddMember("z", jsonMember, allocator);
		return ret;
	}

	bool serializeToJSON(const Vector4 self, JsonValue& value, JsonAllocator& allocator)
	{
		bool ret;
		JsonValue jsonMember;
		ret = grimJson::serializeToJSON(self.x, jsonMember, allocator);
		value.AddMember("x", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.y, jsonMember, allocator);
		value.AddMember("y", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.z, jsonMember, allocator);
		value.AddMember("z", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.w, jsonMember, allocator);
		value.AddMember("w", jsonMember, allocator);
		return ret;
	}

	bool serializeToJSON(const Matrix self, JsonValue& value, JsonAllocator& allocator)
	{
		bool ret;
		JsonValue jsonMember;
		ret = grimJson::serializeToJSON(self.row[0], jsonMember, allocator);
		value.AddMember("row0", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.row[1], jsonMember, allocator);
		value.AddMember("row1", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.row[2], jsonMember, allocator);
		value.AddMember("row2", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.row[3], jsonMember, allocator);
		value.AddMember("row3", jsonMember, allocator);
		return ret;
	}
	
	bool serializeToJSON(const Quaternion self, JsonValue& value, JsonAllocator& allocator)
	{
		bool ret;
		JsonValue jsonMember;
		ret = grimJson::serializeToJSON(self.x, jsonMember, allocator);
		value.AddMember("x", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.y, jsonMember, allocator);
		value.AddMember("y", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.z, jsonMember, allocator);
		value.AddMember("z", jsonMember, allocator);
		ret = grimJson::serializeToJSON(self.w, jsonMember, allocator);
		value.AddMember("w", jsonMember, allocator);
		return ret;
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

	bool initializeFromJSON(Vector2& self, const JsonValue& value, JsonAllocator& allocator)
	{
		bool ret;
		GRIM_ASSERT(value.HasMember("x"), "JSON input for type %s is missing attribute %s", "Vector2", "x");
		ret = grimJson::initializeFromJSON(self.x, value["x"], allocator);
		GRIM_ASSERT(value.HasMember("y"), "JSON input for type %s is missing attribute %s", "Vector2", "y");
		ret = grimJson::initializeFromJSON(self.y, value["y"], allocator);
		return true;
	}

	bool initializeFromJSON(Vector3& self, const JsonValue& value, JsonAllocator& allocator)
	{
		bool ret;
		GRIM_ASSERT(value.HasMember("x"), "JSON input for type %s is missing attribute %s", "Vector3", "x");
		ret = grimJson::initializeFromJSON(self.x, value["x"], allocator);
		GRIM_ASSERT(value.HasMember("y"), "JSON input for type %s is missing attribute %s", "Vector3", "y");
		ret = grimJson::initializeFromJSON(self.y, value["y"], allocator);
		GRIM_ASSERT(value.HasMember("z"), "JSON input for type %s is missing attribute %s", "Vector3", "z");
		ret = grimJson::initializeFromJSON(self.z, value["z"], allocator);
		return true;
	}

	bool initializeFromJSON(Vector4& self, const JsonValue& value, JsonAllocator& allocator)
	{
		bool ret;
		GRIM_ASSERT(value.HasMember("x"), "JSON input for type %s is missing attribute %s", "Vector4", "x");
		ret = grimJson::initializeFromJSON(self.x, value["x"], allocator);
		GRIM_ASSERT(value.HasMember("y"), "JSON input for type %s is missing attribute %s", "Vector4", "y");
		ret = grimJson::initializeFromJSON(self.y, value["y"], allocator);
		GRIM_ASSERT(value.HasMember("z"), "JSON input for type %s is missing attribute %s", "Vector4", "z");
		ret = grimJson::initializeFromJSON(self.z, value["z"], allocator);
		GRIM_ASSERT(value.HasMember("w"), "JSON input for type %s is missing attribute %s", "Vector4", "w");
		ret = grimJson::initializeFromJSON(self.w, value["w"], allocator);
		return true;
	}
	
	bool initializeFromJSON(Matrix& self, const JsonValue& value, JsonAllocator& allocator)
	{
		bool ret;
		GRIM_ASSERT(value.HasMember("row0"), "JSON input for type %s is missing attribute %s", "Matrix", "row0");
		ret = grimJson::initializeFromJSON(self.row[0], value["row0"], allocator);
		GRIM_ASSERT(value.HasMember("row1"), "JSON input for type %s is missing attribute %s", "Matrix", "row1");
		ret = grimJson::initializeFromJSON(self.row[1], value["row1"], allocator);
		GRIM_ASSERT(value.HasMember("row2"), "JSON input for type %s is missing attribute %s", "Matrix", "row2");
		ret = grimJson::initializeFromJSON(self.row[2], value["row2"], allocator);
		GRIM_ASSERT(value.HasMember("row3"), "JSON input for type %s is missing attribute %s", "Matrix", "row3");
		ret = grimJson::initializeFromJSON(self.row[3], value["row3"], allocator);
		return true;
	}

	bool initializeFromJSON(Quaternion& self, const JsonValue& value, JsonAllocator& allocator)
	{
		bool ret;
		GRIM_ASSERT(value.HasMember("x"), "JSON input for type %s is missing attribute %s", "Quaternion", "x");
		ret = grimJson::initializeFromJSON(self.x, value["x"], allocator);
		GRIM_ASSERT(value.HasMember("y"), "JSON input for type %s is missing attribute %s", "Quaternion", "y");
		ret = grimJson::initializeFromJSON(self.y, value["y"], allocator);
		GRIM_ASSERT(value.HasMember("z"), "JSON input for type %s is missing attribute %s", "Quaternion", "z");
		ret = grimJson::initializeFromJSON(self.z, value["z"], allocator);
		GRIM_ASSERT(value.HasMember("w"), "JSON input for type %s is missing attribute %s", "Quaternion", "z");
		ret = grimJson::initializeFromJSON(self.w, value["w"], allocator);
		return true;
	}
}