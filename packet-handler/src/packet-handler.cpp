#include "packet-handler.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <map>
#include <vector>

#include <gcclib/util.h>
#include <gcclib/PipeTransfer.h>
#include <gcclib/data/PacketData.h>
#include <gcclib/data/PacketModifyData.h>
#include <gcclib/Globals.h>
#include <gcclib/Logger.h>

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>

#include <json.hpp>

using namespace std::chrono_literals;
namespace fs = std::filesystem;

static CSimpleIni ini;

static google::protobuf::compiler::Importer* importer;
static google::protobuf::DynamicMessageFactory* factory;
static google::protobuf::compiler::DiskSourceTree* diskTree;

static std::map<uint16_t, std::string> packetNameMap;

class ErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector 
{
	// Inherited via MultiFileErrorCollector
	virtual void AddError(const std::string& filename, int line, int column, const std::string& message) override
	{
		LOG_ERROR("Error while parsing %s file, line %d, column %d, error: %s\n", filename.c_str(), line, column, message.c_str());
	}
};

static void InitializeImporter(const std::string& protoDir) 
{
	diskTree = new google::protobuf::compiler::DiskSourceTree();
	diskTree->MapPath("", protoDir);

	auto errorCollector = new ErrorCollector();
	importer = new google::protobuf::compiler::Importer(diskTree, errorCollector);
	factory = new google::protobuf::DynamicMessageFactory(importer->pool());
}

static bool ParsePacketIDFile(const std::string& filepath) 
{
	std::ifstream file;
	file.open(filepath);
	if (!file.is_open())
		return false;

	auto content = nlohmann::json::parse(file);
	for (nlohmann::json::iterator it = content.begin(); it != content.end(); ++it)
	{
		auto id = std::stoi(it.key().c_str());
		packetNameMap[id] = it.value();
	}
	file.close();
	return true;
}

static google::protobuf::Message* ParseMessage(const std::string& name, std::vector<byte> data)
{
	auto fileDescriptor = importer->Import(name + ".proto");
	if (fileDescriptor == nullptr || fileDescriptor->message_type_count() == 0)
		return nullptr;

	auto message = factory->GetPrototype(fileDescriptor->message_type(0))->New();

	std::string stringData((char*)data.data(), data.size());
	message->ParseFromString(stringData);
	return message;
}

static std::string GetJSON(const std::string& name, std::vector<byte> data) 
{
	auto message = ParseMessage(name, data);
	if (message == nullptr)
		return "";
	std::string jsonMessage = {};
	google::protobuf::util::MessageToJsonString(*message, &jsonMessage);
	return jsonMessage;
}

int main(int argc, char* argv[])
{
	auto path = std::filesystem::path(argv[0]).parent_path();
	std::filesystem::current_path(path);

	Logger::SetLevel(Logger::Level::Debug, Logger::LoggerType::ConsoleLogger);

	ini.LoadFile(Globals::configFileName.c_str());

	auto protoDir = GetOrSelectPath(ini, "Network", "ProtoDirPath", "proto directory", nullptr);
	if (protoDir.empty())
		return 1;

	auto packetIdsPath = GetOrSelectPath(ini, "Network", "PacketIDsJson", "Packet IDs json file", "Json file\0*.json\0");
	if (packetIdsPath.empty())
		return 1;

	InitializeImporter(protoDir);

	if (!ParsePacketIDFile(packetIdsPath))
	{
		LOG_ERROR("Failed to parse packet ids file.");
		return 1;
	}

	ini.SaveFile(Globals::configFileName.c_str());

	auto pipe = PipeTransfer(Globals::packetPipeName);
	if (!pipe.Create())
	{
		LogLastError("Failed to create pipe.");
		return 1;
	}

	LOG_DEBUG("Waiting for connection...");
	if (!pipe.WaitForConnection()) 
	{
		LogLastError("Wait for connection failed.");
		return 1;
	}

	LOG_INFO("Client connected.");
	while (pipe.IsPipeOpened()) 
	{
		PacketData packetData = {};
		pipe.ReadObject(packetData);
		if (!pipe.IsPipeOpened())
			break;

		if (packetNameMap.count(packetData.messageId))
		{
			auto protoName = packetNameMap[packetData.messageId];
			// std::cout << "Head: " << GetJSON("PacketHead", packetData.headData) << std::endl;
			auto type = packetData.type == PacketType::Receive ? "Receive" : "Send";
			std::cout << type << " | Message name: " << protoName << std::endl;
			std::cout << type << " | Message: " << GetJSON(protoName, packetData.messageData) << std::endl;
		}

		if (packetData.waitForModifyData) 
		{
			LOG_INFO("Write data");
			PacketModifyData data {};
			// TODO: do modification data or block packet
			pipe.WriteObject(data);
		}
	}
}