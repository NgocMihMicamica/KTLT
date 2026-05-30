#include <string>

using namespace std;

string removeUtf8Bom(const string& text) {
    if (text.length() >= 3 &&
        (unsigned char)text[0] == 0xEF &&
        (unsigned char)text[1] == 0xBB &&
        (unsigned char)text[2] == 0xBF) {
        return text.substr(3);
    }
    return text;
}

string trimAsciiWhitespace(const string& text) {
    int start = 0;
    int end = (int)text.length() - 1;

    while (start <= end &&
           (text[start] == ' ' || text[start] == '\t' ||
            text[start] == '\r' || text[start] == '\n')) {
        start++;
    }

    while (end >= start &&
           (text[end] == ' ' || text[end] == '\t' ||
            text[end] == '\r' || text[end] == '\n')) {
        end--;
    }

    if (start > end) return "";
    return text.substr(start, end - start + 1);
}

char toUpperAscii(char c) {
    if (c >= 'a' && c <= 'z') return c - 32;
    return c;
}

char toLowerAscii(char c) {
    if (c >= 'A' && c <= 'Z') return c + 32;
    return c;
}

bool equalsIgnoreCaseAscii(const string& a, const string& b) {
    if (a.length() != b.length()) return false;
    for (int i = 0; i < (int)a.length(); i++) {
        if (toLowerAscii(a[i]) != toLowerAscii(b[i])) return false;
    }
    return true;
}

bool parseIntegerString(const string& text, int& value) {
    string cleanText = removeUtf8Bom(text);
    if (cleanText.empty()) return false;

    int i = 0;
    int sign = 1;
    long long result = 0;

    if (cleanText[0] == '-') {
        sign = -1;
        i = 1;
    } else if (cleanText[0] == '+') {
        i = 1;
    }

    if (i >= (int)cleanText.length()) return false;

    for (; i < (int)cleanText.length(); i++) {
        if (cleanText[i] < '0' || cleanText[i] > '9') return false;
        result = result * 10 + (cleanText[i] - '0');
        if (result > 2147483648LL) return false;
    }

    result *= sign;
    if (result < -2147483648LL || result > 2147483647LL) return false;

    value = (int)result;
    return true;
}
