#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

struct Node;

struct QuestionList {
    Node* head;
    int size;
};

void setupVietnameseConsole();
string resolveDataFile(const string& filename);

void initQuestionList(QuestionList& list);
void clearQuestionList(QuestionList& list);
bool loadQuestionsFromFile(QuestionList& list, const string& filename);

bool readIntInRange(const string& prompt, int& value, int minValue, int maxValue);
bool inputNewQuestion(QuestionList& bank, const string& qFile);
bool takeExam(QuestionList& bank, const string& qFile, const string& rFile);
void viewReport(const string& rFile);

void printMenu() {
    cout << "\n=== CHƯƠNG TRÌNH THI TRẮC NGHIỆM ===\n";
    cout << "1. Nhập thêm câu hỏi từ bàn phím (lưu vào file text)\n";
    cout << "2. Thực hiện thi trắc nghiệm (sinh đề, xáo trộn, tính giờ)\n";
    cout << "3. Xem báo cáo kết quả các lần thi\n";
    cout << "0. Thoát chương trình\n";
}

int runProgram() {
    setupVietnameseConsole();
    srand((unsigned int)time(0));

    string qFile = resolveDataFile("questions.txt");
    string rFile = resolveDataFile("report.txt");

    QuestionList bank;
    initQuestionList(bank);
    loadQuestionsFromFile(bank, qFile);

    int choice;
    do {
        printMenu();
        if (!readIntInRange("Chọn chức năng: ", choice, 0, 3)) {
            cout << "\n=> Kết thúc chương trình do không còn dữ liệu nhập.\n";
            break;
        }

        if (choice == 1) {
            inputNewQuestion(bank, qFile);
        } else if (choice == 2) {
            takeExam(bank, qFile, rFile);
        } else if (choice == 3) {
            viewReport(rFile);
        }
    } while (choice != 0);

    clearQuestionList(bank);
    cout << "=> Đã thoát chương trình.\n";
    return 0;
}
