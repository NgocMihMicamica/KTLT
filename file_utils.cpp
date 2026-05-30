#include <fstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

bool fileExists(const string& filename) {
    ifstream file(filename.c_str());
    return file.good();
}

string getDirectoryName(const string& path) {
    size_t pos = path.find_last_of("\\/");
    if (pos == string::npos) return "";
    return path.substr(0, pos);
}

string joinPath(const string& directory, const string& filename) {
    if (directory.empty()) return filename;
    char last = directory[directory.length() - 1];
    if (last == '\\' || last == '/') return directory + filename;
#ifdef _WIN32
    return directory + "\\" + filename;
#else
    return directory + "/" + filename;
#endif
}

string getExecutableDirectory() {
#ifdef _WIN32
    char path[MAX_PATH];
    DWORD length = GetModuleFileNameA(nullptr, path, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) return "";
    return getDirectoryName(string(path, length));
#else
    char path[4096];
    ssize_t length = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (length <= 0) return "";
    path[length] = '\0';
    return getDirectoryName(string(path));
#endif
}

string resolveDataFile(const string& filename) {
    string executableDirectory = getExecutableDirectory();
    string executableFile = joinPath(executableDirectory, filename);
    if (fileExists(executableFile)) return executableFile;
    if (!executableDirectory.empty()) return executableFile;

    if (fileExists(filename)) return filename;
    return filename;
}
