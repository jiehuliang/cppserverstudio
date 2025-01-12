#include "File.h"
#include "Logging.h"

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdio>

bool File::create_path(const std::string& file, unsigned int mod) {
	std::string path = file;
	std::string dir;
	size_t index = 1;
	while (true){
		index = path.find('/', index) + 1;
		dir = path.substr(0, index);
		if (dir.length() == 0) {
			break;
		}
		if (access(dir.data(), 0) == -1) {//access函数是查看是不是存在
			if (mkdir(dir.data(), mod) == -1) { //如果不存在就用mkdir函数来创建
				LOG_WARN << "mkdir " << dir << " failed ";
				return false;
			}
		}
	}
	return true;
}

FILE* File::create_file(const std::string& file, const std::string& mode) {
	std::string path = file;
	std::string dir;
	size_t index = 1;
	FILE* ret = nullptr;
	while (true) {
		index = path.find('/', index) + 1;
		dir = path.substr(0, index);
		if (dir.length() == 0) {
			break;
		}
		if (access(dir.data(), 0) == -1) {//access函数是查看是不是存在
			if (mkdir(dir.data(), 0777) == -1) {//如果不存在就用mkdir函数来创建
				LOG_WARN << "mkdir " << dir << " failed ";
				return nullptr;
			}
		}
	}
	if (path[path.size() - 1] != '/') {
		ret = fopen(file.data(),mode.data());
	}
	return ret;
}

std::string File::loadFile(const std::string& path) {
	FILE* fp = fopen(path.data(), "rb");
	if (!fp) {
		return "";
	}
	fseek(fp, 0, SEEK_END);
	auto len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	std::string str(len, '\0');
	if (len != (decltype(len))fread((char*)str.data(), 1, str.size(), fp)) {
		LOG_WARN << "fread" << path << " failed: ";
	}
	fclose(fp);
	return str;
}

bool File::saveFile(const std::string& data, const std::string& path) {
	FILE* fp = fopen(path.data(), "wb");
	if (!fp) {
		return false;
	}
	fwrite(data.data(), data.size(), 1, fp);
	fclose(fp);
	return true;
}