#include "persistent_filesystem.h"
#include <filesystem>
#include <fstream>
#include <boost/log/trivial.hpp>

bool persistent_filesystem::create_directory(const std::string& path) {
	return std::filesystem::create_directory(path);
}

bool persistent_filesystem::write_file(const std::string& path, const nlohmann::json& data) {
	std::ofstream f(path);
	if (!f){
		BOOST_LOG_TRIVIAL(error) << "Could not open file: " << path << std::endl;
		return false;
	}    f << data;
    f.close();
	return true;
}

bool persistent_filesystem::read_file(const std::string& path, nlohmann::json& data) {
	std::ifstream f(path);
	if (!f){
		BOOST_LOG_TRIVIAL(error) << "Could not open file: " << path << std::endl;
		return false;
	}
    std::stringstream buf;
    buf << f.rdbuf();
    try {
    	data = nlohmann::json::parse(buf.str());
    	f.close();
    	return true;
    } catch (const nlohmann::json::parse_error& e) {
    	BOOST_LOG_TRIVIAL(error) << "Could not parse file " << path << " into json\n";
    	return false;
    }
}

bool persistent_filesystem::delete_file(const std::string& path) {
	if (std::filesystem::remove(path)){
		BOOST_LOG_TRIVIAL(info) << "Deleted file " << path << std::endl;
		return true;
	}
	BOOST_LOG_TRIVIAL(error) << "Could not delete file " << path << std::endl;
	return false;
}

bool persistent_filesystem::list_files(const std::string& path, std::map<std::string, std::set<int>>& files) {
	if (!std::filesystem::is_directory(path)) {
		BOOST_LOG_TRIVIAL(error) << path << " is not a directory" << std::endl;
		return false;
	}
	std::string filename;
	std::string entity;
	for (const auto & entry : std::filesystem::directory_iterator(path)){
		filename = entry.path().filename().string();
		entity = std::string(filename.begin() + filename.find_last_of('/') + 1, filename.end());
        files.insert({entity, std::set<int>{}});
        for (const auto & stored : std::filesystem::directory_iterator(path + "/" + filename)) {
            files[entity].insert(std::stoi(stored.path().filename().string()));
        }
	}
	return true;
}
