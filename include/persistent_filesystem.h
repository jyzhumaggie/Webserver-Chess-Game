#ifndef PERSISTENT_FILESYSTEM_H
#define PERSISTENT_FILESYSTEM_H

#include "file_system.h"
#include <map>
#include <set>

class persistent_filesystem: public file_system {
public:
	persistent_filesystem(){}
	bool create_directory(const std::string& path) override;
	bool write_file(const std::string& path, const nlohmann::json& data) override;
	bool read_file(const std::string& path, nlohmann::json& data) override;
	bool delete_file(const std::string& path) override;
	bool list_files(const std::string& path, std::map<std::string, std::set<int>>& files) override;
};


#endif