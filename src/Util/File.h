#ifndef FILE_H
#define FILE_H

#include <string>

class File {
public:
	//创建路径
	//Create path
	static bool create_path(const std::string& file, unsigned int mod);

	//新建文件，目录文件夹自动生成
	//Create a new file,and the directory folder will be generated automatically
	static FILE* create_file(const std::string& file, const std::string& mode);

	/**
	 * 加载文件内容至string
	 * @param path 加载的文件路径
	 * @return 文件内容
	 * Load file content to string
	 * @param path The path of the file to load
	 * @return The file content
	 */
	static std::string loadFile(const std::string& path);

	/**
	 * 保存内容至文件
	 * @param data 文件内容
	 * @param path 保存的文件路径
	 * @return 是否保存成功
	 * Save content to file
	 * @param data The file content
	 * @param path The path to save the file
	 * @return Whether the save was successful
	 */
	static bool saveFile(const std::string& data, const std::string& path);


private:
	File();
	~File();
};

#endif //FILE_H	