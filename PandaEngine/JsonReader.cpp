#include "JsonReader.h"
#include <iostream>
#include <fstream>

JsonReader::JsonReader()
{
}

JsonReader::~JsonReader()
{
}

bool JsonReader::LoadJsonFile(const char* filename, rapidjson::Document& document)
{
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open filename\n";
        return false;
    }

    std::string jsonString;
    std::string line;
    while (std::getline(inputFile, line)) {
        jsonString += line;
    }
    inputFile.close();

    rapidjson::Document doc;
    document.Parse(jsonString.c_str());

    if (document.HasParseError()) {
		std::cerr << "Failed to parse JSON data\n";
		return false;
	}

    return true;
}

void JsonReader::WriteJsonFile(const char* filename, rapidjson::Document &document)
{

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string jsonString = buffer.GetString();

    std::ofstream outputFile(filename);
    if (outputFile.is_open()) {
        outputFile << jsonString;
        outputFile.close();
        std::cout << "JSON data has been written to output.json\n";
    }
    else {
        std::cerr << "Failed to open output.json for writing\n";
    }

}
