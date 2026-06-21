#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>

bool fileExists(const std::string& filename);
std::string getDirectoryName(const std::string& path);
std::string joinPath(const std::string& directory, const std::string& filename);
std::string getExecutableDirectory();
std::string resolveDataFile(const std::string& filename);

#endif
