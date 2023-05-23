#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class file_system {
public:
	virtual bool create_directory(const std::string& path) = 0;
	virtual bool write_file(const std::string& path, const nlohmann::json& data) = 0;
	virtual bool read_file(const std::string& path, nlohmann::json& data) = 0;
	virtual bool delete_file(const std::string& path) = 0;
	virtual bool list_files(const std::string& path, std::vector<std::string>& files) = 0;
};


#endif