#pragma once

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>

class JsonReader
{
public:
	JsonReader();
	~JsonReader();

	bool LoadJsonFile(const char* filename, rapidjson::Document &document);
	void WriteJsonFile(const char* filename, rapidjson::Document &document);

};

