#pragma once

#include <string>
#include <filesystem>

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>

namespace cheat::feature::sniffer 
{
	class ProtoManager
	{
	public:
		ProtoManager(const std::string& idFilePath, const std::string& protoDir);

		std::optional<std::string> GetJson(uint32_t id, std::vector<byte>& data);
		std::optional<std::string> GetName(uint32_t id);

		void LoadIDFile(const std::string& filepath);
		void LoadProtoDir(const std::string& dirPath);

	private:
		std::mutex _mutex;
	    std::shared_ptr<google::protobuf::compiler::Importer> importer;
		std::shared_ptr<google::protobuf::DynamicMessageFactory> factory;
		std::shared_ptr<google::protobuf::compiler::DiskSourceTree> diskTree;

		std::map<uint16_t, std::string> nameMap;

		google::protobuf::Message* ParseMessage(const std::string& name, std::vector<byte> data);
	};
}


