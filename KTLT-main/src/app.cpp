#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#include "app.h"
#include "console_utils.h"
#include "exam.h"
#include "file_utils.h"
#include "input_utils.h"
#include "question_list.h"
#include "report.h"

using namespace std;

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

    string qFile = resolveDataFile("data/questions.txt");
    string rFile = resolveDataFile("data/report.txt");

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
