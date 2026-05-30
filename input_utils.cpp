#include <ctime>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

const int INPUT_IGNORE_LIMIT = 10000;

enum TimedAnswerStatus {
    ANSWER_READ_OK,
    ANSWER_READ_TIMEOUT,
    ANSWER_READ_EOF
};

char normalizeAnswer(char ans);
bool isValidAnswer(char ans);
bool isValidDifficulty(const string& difficulty);
string normalizeDifficulty(const string& difficulty);

#ifndef _WIN32
class TerminalRawMode {
private:
    termios originalMode;
    bool enabled;

public:
    TerminalRawMode() {
        enabled = false;
        if (isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &originalMode) == 0) {
            termios rawMode = originalMode;
            rawMode.c_lflag &= ~(ICANON | ECHO);
            rawMode.c_cc[VMIN] = 0;
            rawMode.c_cc[VTIME] = 0;
            enabled = tcsetattr(STDIN_FILENO, TCSANOW, &rawMode) == 0;
        }
    }

    ~TerminalRawMode() {
        if (enabled) {
            tcsetattr(STDIN_FILENO, TCSANOW, &originalMode);
        }
    }

    bool isEnabled() const {
        return enabled;
    }
};
#endif

void clearInputLine() {
    cin.ignore(INPUT_IGNORE_LIMIT, '\n');
}

bool readIntInRange(const string& prompt, int& value, int minValue, int maxValue) {
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clearInputLine();
            if (value >= minValue && value <= maxValue) return true;
            cout << "=> Giá trị phải nằm trong khoảng [" << minValue << ", " << maxValue << "].\n";
        } else {
            if (cin.eof()) return false;
            cin.clear();
            clearInputLine();
            cout << "=> Dữ liệu nhập không hợp lệ. Vui lòng nhập số nguyên.\n";
        }
    }
}

bool readIntAtLeast(const string& prompt, int& value, int minValue) {
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clearInputLine();
            if (value >= minValue) return true;
            cout << "=> Giá trị phải lớn hơn hoặc bằng " << minValue << ".\n";
        } else {
            if (cin.eof()) return false;
            cin.clear();
            clearInputLine();
            cout << "=> Dữ liệu nhập không hợp lệ. Vui lòng nhập số nguyên.\n";
        }
    }
}

bool readNonEmptyLine(const string& prompt, string& value) {
    while (true) {
        cout << prompt;
        if (!getline(cin, value)) return false;
        if (!value.empty()) return true;
        cout << "=> Không được để trống. Vui lòng nhập lại.\n";
    }
}

bool readAnswerInput(const string& prompt, char& answer) {
    string text;
    while (true) {
        cout << prompt;
        if (!getline(cin, text)) return false;
        if (!text.empty()) {
            answer = normalizeAnswer(text[0]);
            if (isValidAnswer(answer)) return true;
        }
        cout << "=> Đáp án chỉ được là A, B, C hoặc D.\n";
    }
}

int getRemainingSeconds(time_t startTime, int timeLimit) {
    time_t currentTime = time(0);
    int elapsed = (int)(currentTime - startTime);
    return timeLimit - elapsed;
}

bool isInteractiveInput() {
#ifdef _WIN32
    return _isatty(_fileno(stdin)) != 0;
#else
    return isatty(STDIN_FILENO) != 0;
#endif
}

TimedAnswerStatus readAnswerCharacterWithTimer(char& answer, time_t startTime, int timeLimit) {
    if (!isInteractiveInput()) {
        string text;
        if (!getline(cin, text)) return ANSWER_READ_EOF;
        if (getRemainingSeconds(startTime, timeLimit) <= 0) return ANSWER_READ_TIMEOUT;
        answer = text.empty() ? '\0' : normalizeAnswer(text[0]);
        return ANSWER_READ_OK;
    }

#ifdef _WIN32
    while (true) {
        if (getRemainingSeconds(startTime, timeLimit) <= 0) {
            cout << "\n";
            return ANSWER_READ_TIMEOUT;
        }

        if (_kbhit()) {
            int key = _getch();
            if (key == 0 || key == 224) {
                if (_kbhit()) _getch();
                continue;
            }
            if (key == '\r' || key == '\n' || key == '\b') continue;
            if (key < 32 || key > 126) continue;

            answer = normalizeAnswer((char)key);
            cout << (char)key << "\n";
            return ANSWER_READ_OK;
        }

        Sleep(50);
    }
#else
    TerminalRawMode rawMode;
    if (!rawMode.isEnabled()) {
        string text;
        if (!getline(cin, text)) return ANSWER_READ_EOF;
        if (getRemainingSeconds(startTime, timeLimit) <= 0) return ANSWER_READ_TIMEOUT;
        answer = text.empty() ? '\0' : normalizeAnswer(text[0]);
        return ANSWER_READ_OK;
    }

    while (true) {
        if (getRemainingSeconds(startTime, timeLimit) <= 0) {
            cout << "\n";
            return ANSWER_READ_TIMEOUT;
        }

        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(STDIN_FILENO, &readSet);

        timeval waitTime;
        waitTime.tv_sec = 0;
        waitTime.tv_usec = 50000;

        int ready = select(STDIN_FILENO + 1, &readSet, nullptr, nullptr, &waitTime);
        if (ready < 0) return ANSWER_READ_EOF;

        if (ready > 0 && FD_ISSET(STDIN_FILENO, &readSet)) {
            char key;
            ssize_t bytesRead = read(STDIN_FILENO, &key, 1);
            if (bytesRead <= 0) return ANSWER_READ_EOF;
            if (key == '\r' || key == '\n' || key == '\b' || key == 127) continue;
            if (key < 32 || key > 126) continue;

            answer = normalizeAnswer(key);
            cout << key << "\n";
            return ANSWER_READ_OK;
        }
    }
#endif
}

TimedAnswerStatus readAnswerInputWithTimer(const string& prompt, char& answer,
                                           time_t startTime, int timeLimit) {
    while (true) {
        int remaining = getRemainingSeconds(startTime, timeLimit);
        if (remaining <= 0) return ANSWER_READ_TIMEOUT;

        cout << prompt << " (còn lại " << remaining << " giây): " << flush;
        TimedAnswerStatus status = readAnswerCharacterWithTimer(answer, startTime, timeLimit);
        if (status != ANSWER_READ_OK) return status;

        if (isValidAnswer(answer)) return ANSWER_READ_OK;
        cout << "=> Đáp án chỉ được là A, B, C hoặc D.\n";
    }
}

bool readDifficultyInput(const string& prompt, string& difficulty) {
    while (true) {
        if (!readNonEmptyLine(prompt, difficulty)) return false;
        if (isValidDifficulty(difficulty)) {
            difficulty = normalizeDifficulty(difficulty);
            return true;
        }
        cout << "=> Độ khó chỉ được là Dễ hoặc Khó.\n";
    }
}
