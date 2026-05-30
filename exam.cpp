#include <ctime>
#include <iostream>
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

struct Node {
    Question data;
    Node* next;
};

struct QuestionList {
    Node* head;
    int size;
};

enum TimedAnswerStatus {
    ANSWER_READ_OK,
    ANSWER_READ_TIMEOUT,
    ANSWER_READ_EOF
};

char normalizeAnswer(char ans);
void shuffleQuestions(Question* arr[], int n);
void shuffleAnswers(AnswerOption arr[], int n);

bool loadQuestionsFromFile(QuestionList& list, const string& filename);
bool containsQuestionId(QuestionList& list, int id);
void addQuestion(QuestionList& list, const Question& q);
bool appendQuestionToFile(const string& filename, const Question& q);

bool readIntInRange(const string& prompt, int& value, int minValue, int maxValue);
bool readIntAtLeast(const string& prompt, int& value, int minValue);
bool readNonEmptyLine(const string& prompt, string& value);
bool readAnswerInput(const string& prompt, char& answer);
bool readDifficultyInput(const string& prompt, string& difficulty);
TimedAnswerStatus readAnswerInputWithTimer(const string& prompt, char& answer,
                                           time_t startTime, int timeLimit);

void saveReport(const string& filename, const string& name, int score, int total);

bool inputNewQuestion(QuestionList& bank, const string& qFile) {
    Question q;
    cout << "\n--- CHỨC NĂNG: NHẬP CÂU HỎI MỚI ---\n";

    loadQuestionsFromFile(bank, qFile);

    while (true) {
        if (!readIntAtLeast("ID câu hỏi (số nguyên dương): ", q.id, 1)) return false;
        if (!containsQuestionId(bank, q.id)) break;
        cout << "=> ID này đã tồn tại. Vui lòng nhập ID khác.\n";
    }

    if (!readNonEmptyLine("Môn / Học phần: ", q.subject)) return false;
    if (!readNonEmptyLine("Nội dung câu hỏi: ", q.content)) return false;
    if (!readNonEmptyLine("Đáp án A: ", q.A)) return false;
    if (!readNonEmptyLine("Đáp án B: ", q.B)) return false;
    if (!readNonEmptyLine("Đáp án C: ", q.C)) return false;
    if (!readNonEmptyLine("Đáp án D: ", q.D)) return false;
    if (!readAnswerInput("Đáp án đúng (A/B/C/D): ", q.correctAns)) return false;
    if (!readDifficultyInput("Độ khó (Dễ/Khó): ", q.difficulty)) return false;

    if (!appendQuestionToFile(qFile, q)) {
        cout << "=> Không thể lưu câu hỏi xuống file '" << qFile << "'.\n";
        return false;
    }

    addQuestion(bank, q);
    cout << "=> Đã lưu câu hỏi mới xuống file '" << qFile << "'.\n";
    return true;
}

void printEvaluation(double percent) {
    cout << "Đánh giá: ";
    if (percent == 100) {
        cout << "Xuất sắc.\n";
    } else if (percent >= 80) {
        cout << "Rất tốt.\n";
    } else if (percent >= 50) {
        cout << "Đạt yêu cầu, cần tiếp tục luyện tập.\n";
    } else {
        cout << "Cần ôn tập thêm.\n";
    }
}

bool takeExam(QuestionList& bank, const string& qFile, const string& rFile) {
    bool loaded = loadQuestionsFromFile(bank, qFile);
    if (!loaded || bank.size == 0) {
        cout << "=> Ngân hàng câu hỏi đang trống hoặc không mở được file '" << qFile << "'.\n";
        cout << "=> Hãy dùng chức năng 1 để nhập câu hỏi trước khi thi.\n";
        return false;
    }

    string name;
    int numQ;
    int timeLimit;

    cout << "\n--- CHỨC NĂNG: THỰC HIỆN THI TRẮC NGHIỆM ---\n";
    if (!readNonEmptyLine("Nhập tên người tham gia thi: ", name)) return false;
    if (!readIntInRange("Nhập số lượng câu hỏi muốn thi (1-" + to_string(bank.size) + "): ",
                        numQ, 1, bank.size)) return false;
    if (!readIntAtLeast("Nhập thời gian làm bài định mức (giây, tối thiểu 1): ",
                        timeLimit, 1)) return false;

    Question** examQuestions = new Question*[bank.size];
    Node* temp = bank.head;
    for (int i = 0; i < bank.size; i++) {
        examQuestions[i] = &(temp->data);
        temp = temp->next;
    }

    shuffleQuestions(examQuestions, bank.size);

    int score = 0;
    time_t startTime = time(0);

    for (int i = 0; i < numQ; i++) {
        time_t currentTime = time(0);
        int elapsed = (int)(currentTime - startTime);
        int remaining = timeLimit - elapsed;

        if (remaining <= 0) {
            cout << "\n*** QUÁ THỜI GIAN LÀM BÀI! HỆ THỐNG TỰ ĐỘNG THU BÀI ***\n";
            break;
        }

        AnswerOption answers[4] = {
            {examQuestions[i]->A, 'A'},
            {examQuestions[i]->B, 'B'},
            {examQuestions[i]->C, 'C'},
            {examQuestions[i]->D, 'D'}
        };
        shuffleAnswers(answers, 4);

        char currentCorrect = 'A';
        char originalCorrect = normalizeAnswer(examQuestions[i]->correctAns);
        for (int j = 0; j < 4; j++) {
            if (answers[j].originalOption == originalCorrect) {
                currentCorrect = (char)('A' + j);
                break;
            }
        }

        cout << "\nCâu " << i + 1 << " ["
             << examQuestions[i]->subject << " - "
             << examQuestions[i]->difficulty << "]: "
             << examQuestions[i]->content << "\n";
        cout << "A. " << answers[0].text << "\n";
        cout << "B. " << answers[1].text << "\n";
        cout << "C. " << answers[2].text << "\n";
        cout << "D. " << answers[3].text << "\n";

        char userAnswer;
        TimedAnswerStatus answerStatus = readAnswerInputWithTimer("Trả lời", userAnswer,
                                                                   startTime, timeLimit);
        if (answerStatus == ANSWER_READ_TIMEOUT) {
            cout << "\n*** QUÁ THỜI GIAN LÀM BÀI! HỆ THỐNG TỰ ĐỘNG THU BÀI ***\n";
            break;
        }
        if (answerStatus == ANSWER_READ_EOF) {
            delete[] examQuestions;
            return false;
        }

        if (userAnswer == currentCorrect) score++;
    }

    double percent = (double)score / numQ * 100;
    cout << "\n============================================\n";
    cout << "        KẾT THÚC BÀI THI TRẮC NGHIỆM\n";
    cout << "============================================\n";
    cout << "Thí sinh: " << name << "\n";
    cout << "Kết quả đạt được: " << score << "/" << numQ << " câu đúng.\n";
    cout << "Tỷ lệ chính xác: " << percent << "%\n";
    printEvaluation(percent);
    cout << "============================================\n";

    saveReport(rFile, name, score, numQ);
    delete[] examQuestions;
    return true;
}
