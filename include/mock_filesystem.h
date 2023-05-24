#ifndef MOCK_FILESYSTEM_H
#define MOCK_FILESYSTEM_H

#include <nlohmann/json.hpp>
#include "file_system.h"
#include <map>
#include <set>

class mock_filesystem: public file_system {
public:
	mock_filesystem(){}
	mock_filesystem(std::map<std::string, nlohmann::json> files_);
	bool create_directory(const std::string& path) override;
	bool write_file(const std::string& path, const nlohmann::json& data) override;
	bool read_file(const std::string& path, nlohmann::json& data) override;
	bool delete_file(const std::string& path) override;
	bool list_files(const std::string& path, std::map<std::string, std::set<int>>& f) override;

private:
	std::map<std::string, nlohmann::json> files;
};

#endif