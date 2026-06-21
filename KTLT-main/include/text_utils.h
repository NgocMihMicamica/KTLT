#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <string>

std::string removeUtf8Bom(const std::string& text);
std::string trimAsciiWhitespace(const std::string& text);
char toUpperAscii(char c);
char toLowerAscii(char c);
bool equalsIgnoreCaseAscii(const std::string& a, const std::string& b);
bool parseIntegerString(const std::string& text, int& value);

#endif
