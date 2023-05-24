#include "mock_filesystem.h"

mock_filesystem::mock_filesystem(std::map<std::string, nlohmann::json> files_){
	files = files_;
}

bool mock_filesystem::create_directory(const std::string& path) { return true; }

bool mock_filesystem::write_file(const std::string& path, const nlohmann::json& data) {
	files[path] = data;
	return true;
}

bool mock_filesystem::read_file(const std::string& path, nlohmann::json& data) {
	if (files.find(path) != files.end()){
		data = files[path];
		return true;
	}
	return false;
}

bool mock_filesystem::delete_file(const std::string& path) {
	if (files.find(path) != files.end()){
		files.erase(path);
		return true;
	}
	return false;
}
	
bool mock_filesystem::list_files(const std::string& path, std::map<std::string, std::set<int>>& f) {
	for(auto i: files){
		if (i.first.find(path) != std::string::npos){
			f[path].insert(std::stoi(std::string(i.first.begin() + i.first.find_last_of('/') + 1, i.first.end())));
		}
	}
	return true;
}