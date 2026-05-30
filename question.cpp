#include <cstdlib>
#include <string>

using namespace std;

struct Question {
    int id;
    string subject;
    string content;
    string A, B, C, D;
    char correctAns;
    string difficulty;
};

struct AnswerOption {
    string text;
    char originalOption;
};

string trimAsciiWhitespace(const string& text);
char toUpperAscii(char c);
bool equalsIgnoreCaseAscii(const string& a, const string& b);

char normalizeAnswer(char ans) {
    return toUpperAscii(ans);
}

bool isValidAnswer(char ans) {
    ans = normalizeAnswer(ans);
    return ans == 'A' || ans == 'B' || ans == 'C' || ans == 'D';
}

bool isEasyDifficulty(const string& difficulty) {
    string cleanDifficulty = trimAsciiWhitespace(difficulty);
    return equalsIgnoreCaseAscii(cleanDifficulty, "De") ||
           cleanDifficulty == "Dễ" ||
           cleanDifficulty == "dễ" ||
           cleanDifficulty == "DỄ";
}

bool isHardDifficulty(const string& difficulty) {
    string cleanDifficulty = trimAsciiWhitespace(difficulty);
    return equalsIgnoreCaseAscii(cleanDifficulty, "Kho") ||
           cleanDifficulty == "Khó" ||
           cleanDifficulty == "khó" ||
           cleanDifficulty == "KHÓ";
}

bool isValidDifficulty(const string& difficulty) {
    return isEasyDifficulty(difficulty) || isHardDifficulty(difficulty);
}

string normalizeDifficulty(const string& difficulty) {
    if (isEasyDifficulty(difficulty)) return "Dễ";
    if (isHardDifficulty(difficulty)) return "Khó";
    return difficulty;
}

bool isValidQuestion(const Question& q) {
    return q.id > 0 &&
           !q.subject.empty() &&
           !q.content.empty() &&
           !q.A.empty() &&
           !q.B.empty() &&
           !q.C.empty() &&
           !q.D.empty() &&
           isValidAnswer(q.correctAns) &&
           isValidDifficulty(q.difficulty);
}

void shuffleQuestions(Question* arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Question* temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void shuffleAnswers(AnswerOption arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        AnswerOption temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}
