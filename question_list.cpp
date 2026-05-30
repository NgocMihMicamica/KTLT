#include <fstream>
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

struct Node {
    Question data;
    Node* next;
};

struct QuestionList {
    Node* head;
    int size;
};

string removeUtf8Bom(const string& text);
string trimAsciiWhitespace(const string& text);
bool parseIntegerString(const string& text, int& value);

char normalizeAnswer(char ans);
string normalizeDifficulty(const string& difficulty);
bool isValidQuestion(const Question& q);

void initQuestionList(QuestionList& list) {
    list.head = nullptr;
    list.size = 0;
}

void clearQuestionList(QuestionList& list) {
    Node* current = list.head;
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    list.head = nullptr;
    list.size = 0;
}

Question* findQuestionById(QuestionList& list, int id) {
    Node* temp = list.head;
    while (temp != nullptr) {
        if (temp->data.id == id) return &(temp->data);
        temp = temp->next;
    }
    return nullptr;
}

bool containsQuestionId(QuestionList& list, int id) {
    return findQuestionById(list, id) != nullptr;
}

void addQuestion(QuestionList& list, const Question& q) {
    Node* newNode = new Node();
    newNode->data = q;
    newNode->next = nullptr;

    if (list.head == nullptr) {
        list.head = newNode;
    } else {
        Node* temp = list.head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    list.size++;
}

void writeQuestionRecord(ofstream& file, const Question& q) {
    file << q.id << "\n"
         << q.subject << "\n"
         << q.content << "\n"
         << q.A << "\n"
         << q.B << "\n"
         << q.C << "\n"
         << q.D << "\n"
         << normalizeAnswer(q.correctAns) << "\n"
         << normalizeDifficulty(q.difficulty) << "\n";
}

void skipBrokenRecord(ifstream& file) {
    string ignored;
    for (int i = 0; i < 8; i++) {
        if (!getline(file, ignored)) break;
    }
}

bool appendQuestionToFile(const string& filename, const Question& q) {
    bool needNewLine = false;
    ifstream inputFile(filename.c_str(), ios::binary);
    if (inputFile.is_open()) {
        inputFile.seekg(0, ios::end);
        streamoff fileSize = inputFile.tellg();
        if (fileSize > 0) {
            inputFile.seekg(-1, ios::end);
            char lastChar;
            inputFile.get(lastChar);
            needNewLine = lastChar != '\n';
        }
        inputFile.close();
    }

    ofstream file(filename.c_str(), ios::app);
    if (!file.is_open()) return false;

    if (needNewLine) file << "\n";
    writeQuestionRecord(file, q);
    file.close();
    return true;
}

bool loadQuestionsFromFile(QuestionList& list, const string& filename) {
    clearQuestionList(list);

    ifstream file(filename.c_str());
    if (!file.is_open()) return false;

    string idStr;
    while (getline(file, idStr)) {
        idStr = trimAsciiWhitespace(removeUtf8Bom(idStr));
        if (idStr.empty()) continue;

        Question q;
        if (!parseIntegerString(idStr, q.id)) {
            skipBrokenRecord(file);
            continue;
        }

        string ans;
        if (!getline(file, q.subject)) break;
        if (!getline(file, q.content)) break;
        if (!getline(file, q.A)) break;
        if (!getline(file, q.B)) break;
        if (!getline(file, q.C)) break;
        if (!getline(file, q.D)) break;
        if (!getline(file, ans)) break;
        if (!getline(file, q.difficulty)) break;

        q.subject = trimAsciiWhitespace(q.subject);
        q.content = trimAsciiWhitespace(q.content);
        q.A = trimAsciiWhitespace(q.A);
        q.B = trimAsciiWhitespace(q.B);
        q.C = trimAsciiWhitespace(q.C);
        q.D = trimAsciiWhitespace(q.D);
        ans = trimAsciiWhitespace(ans);
        q.difficulty = normalizeDifficulty(trimAsciiWhitespace(q.difficulty));
        q.correctAns = ans.empty() ? '\0' : normalizeAnswer(ans[0]);

        if (isValidQuestion(q) && !containsQuestionId(list, q.id)) {
            addQuestion(list, q);
        }
    }

    file.close();
    return true;
}

bool saveQuestionsToFile(QuestionList& list, const string& filename) {
    ofstream file(filename.c_str());
    if (!file.is_open()) return false;

    Node* temp = list.head;
    while (temp != nullptr) {
        writeQuestionRecord(file, temp->data);
        temp = temp->next;
    }

    file.close();
    return true;
}
