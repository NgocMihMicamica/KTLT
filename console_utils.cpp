#include <clocale>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

#ifdef _WIN32
class Utf8ConsoleBuffer : public streambuf {
private:
    HANDLE outputHandle;
    bool isConsole;

    void writeText(const char* text, streamsize length) {
        if (length <= 0) return;

        if (!isConsole) {
            DWORD written = 0;
            WriteFile(outputHandle, text, (DWORD)length, &written, nullptr);
            return;
        }

        int wideLength = MultiByteToWideChar(CP_UTF8, 0, text, (int)length, nullptr, 0);
        if (wideLength <= 0) {
            DWORD written = 0;
            WriteFile(outputHandle, text, (DWORD)length, &written, nullptr);
            return;
        }

        wstring wideText(wideLength, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, text, (int)length, &wideText[0], wideLength);

        DWORD written = 0;
        WriteConsoleW(outputHandle, wideText.c_str(), (DWORD)wideText.length(), &written, nullptr);
    }

protected:
    int overflow(int ch) override {
        if (ch == traits_type::eof()) return traits_type::not_eof(ch);

        char c = (char)ch;
        writeText(&c, 1);
        return ch;
    }

    streamsize xsputn(const char* text, streamsize length) override {
        writeText(text, length);
        return length;
    }

public:
    Utf8ConsoleBuffer() {
        outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        isConsole = outputHandle != nullptr &&
                    outputHandle != INVALID_HANDLE_VALUE &&
                    GetConsoleMode(outputHandle, &mode);
    }
};
#endif

void setupVietnameseConsole() {
    setlocale(LC_ALL, "");
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    static Utf8ConsoleBuffer* utf8OutputBuffer = new Utf8ConsoleBuffer();
    cout.rdbuf(utf8OutputBuffer);
#endif
}
