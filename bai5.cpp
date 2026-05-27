#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// Cấu trúc dữ liệu lưu trữ một câu hỏi trắc nghiệm
struct Question {
    int id;
    string subject;    // Môn / Học phần
    string content;    // Nội dung câu hỏi
    string A, B, C, D; // 4 đáp án lựa chọn
    char correctAns;   // Đáp án đúng (A/B/C/D)
    string difficulty; // Độ khó (Dễ/Khó)
};

// Cấu trúc một Node trong Danh sách liên kết đơn
struct Node {
    Question data;
    Node* next;
};

// Lớp quản lý ngân hàng câu hỏi bằng Danh sách liên kết đơn (Tự cài đặt, không dùng thư viện)
class QuestionList {
public:
    Node* head;
    int size;

    QuestionList() {
        head = nullptr;
        size = 0;
    }

    // Hàm hủy để giải phóng bộ nhớ, tránh rò rỉ (Memory Leak)
    ~QuestionList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    // Thêm câu hỏi vào cuối danh sách liên kết
    void addQuestion(Question q) {
        Node* newNode = new Node();
        newNode->data = q;
        newNode->next = nullptr;

        if (head == nullptr) {
            head = newNode;
        } else {
            Node* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        size++;
    }

    // Đọc dữ liệu từ file text vào danh sách liên kết
    void loadFromFile(string filename) {
        // Xóa danh sách cũ nếu có trước khi nạp dữ liệu mới
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        size = 0;

        ifstream file(filename);
        if (!file.is_open()) return;

        while (true) {
            Question q;
            string idStr;
            if (!getline(file, idStr) || idStr.empty()) break;
            q.id = stoi(idStr);
            getline(file, q.subject);
            getline(file, q.content);
            getline(file, q.A);
            getline(file, q.B);
            getline(file, q.C);
            getline(file, q.D);
            string ans;
            getline(file, ans);
            q.correctAns = ans[0];
            getline(file, q.difficulty);
            addQuestion(q);
        }
        file.close();
    }

    // Ghi toàn bộ dữ liệu từ danh sách liên kết xuống file text
    void saveToFile(string filename) {
        ofstream file(filename);
        if (!file.is_open()) return;
        Node* temp = head;
        while (temp != nullptr) {
            file << temp->data.id << "\n"
                 << temp->data.subject << "\n"
                 << temp->data.content << "\n"
                 << temp->data.A << "\n"
                 << temp->data.B << "\n"
                 << temp->data.C << "\n"
                 << temp->data.D << "\n"
                 << temp->data.correctAns << "\n"
                 << temp->data.difficulty << "\n";
            temp = temp->next;
        }
        file.close();
    }
};

// Thuật toán xáo trộn Fisher-Yates viết tay cho mảng các con trỏ câu hỏi (Sinh đề thi ngẫu nhiên)
void shuffleQuestions(Question* arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Question* temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

// Thuật toán xáo trộn Fisher-Yates viết tay cho mảng chuỗi (Xáo trộn thứ tự đáp án A, B, C, D)
void shuffleAnswers(string arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        string temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

// Hàm ghi lịch sử kết quả thi xuống file report.txt
void saveReport(string filename, string name, int score, int total) {
    ofstream file(filename, ios::app);
    if (file.is_open()) {
        file << "Người thi: " << name << " | Điểm: " << score << "/" << total << "\n";
        file.close();
    }
}

int main() {
    srand(time(0));
    
    // ĐÃ FIX KHAI BÁO CHUẨN Ở ĐÂY:
    string qFile = "questions.txt";
    string rFile = "report.txt";

    QuestionList bank;
    bank.loadFromFile(qFile);

    int choice;
    do {
        cout << "\n=== CHƯƠNG TRÌNH THI TRẮC NGHIỆM ===\n";
        cout << "1. Nhập thêm câu hỏi từ bàn phím (Lưu vào file text)\n";
        cout << "2. Thực hiện thi trắc nghiệm (Sinh đề, Xáo trộn & Tính giờ)\n";
        cout << "3. Xem báo cáo kết quả các lần thi\n";
        cout << "0. Thoát chương trình\n";
        cout << "Chọn chức năng: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            Question q;
            cout << "\n--- CHỨC NĂNG: NHẬP CÂU HỎI MỚI ---\n";
            cout << "ID câu hỏi (Số nguyên): "; cin >> q.id; cin.ignore();
            cout << "Môn / Học phần: "; getline(cin, q.subject);
            cout << "Nội dung câu hỏi: "; getline(cin, q.content);
            cout << "Đáp án A: "; getline(cin, q.A);
            cout << "Đáp án B: "; getline(cin, q.B);
            cout << "Đáp án C: "; getline(cin, q.C);
            cout << "Đáp án D: "; getline(cin, q.D);
            cout << "Đáp án đúng (Gõ ký tự A hoặc B hoặc C hoặc D): "; cin >> q.correctAns; cin.ignore();
            cout << "Độ khó (Dễ/Khó): "; getline(cin, q.difficulty);
            
            bank.addQuestion(q);
            bank.saveToFile(qFile);
            
            cout << "=> Hệ thống đã lưu câu hỏi mới xuống file '" << qFile << "' thành công!\n";
        }
        else if (choice == 2) {
            bank.loadFromFile(qFile);

            if (bank.size == 0) {
                cout << "=> Thông báo: Ngân hàng câu hỏi đang trống hoặc thiếu file '" << qFile << "'!\n";
                cout << "=> Hãy dùng tính năng 1 để nạp câu hỏi hoặc kiểm tra thư mục chứa chương trình.\n";
                continue;
            }

            string name;
            int numQ, timeLimit;
            cout << "\n--- CHỨC NĂNG: THỰC HIỆN THI TRẮC NGHIỆM ---\n";
            cout << "Nhập tên người tham gia thi: "; getline(cin, name);
            cout << "Nhập số lượng câu hỏi muốn thi (Hiện có " << bank.size << " câu): "; cin >> numQ;
            cout << "Nhập thời gian làm bài định mức (Đơn vị: giây): "; cin >> timeLimit;
            cin.ignore();

            if (numQ > bank.size) numQ = bank.size;

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
                
                if (currentTime - startTime >= timeLimit) {
                    cout << "\n*** QUÁ THỜI GIAN LÀM BÀI! HỆ THỐNG TỰ ĐỘNG THU BÀI ***\n";
                    break;
                }

                string ansArr[4] = {examQuestions[i]->A, examQuestions[i]->B, examQuestions[i]->C, examQuestions[i]->D};
                string correctStr;
                
                if (examQuestions[i]->correctAns == 'A' || examQuestions[i]->correctAns == 'a') correctStr = ansArr[0];
                else if (examQuestions[i]->correctAns == 'B' || examQuestions[i]->correctAns == 'b') correctStr = ansArr[1];
                else if (examQuestions[i]->correctAns == 'C' || examQuestions[i]->correctAns == 'c') correctStr = ansArr[2];
                else correctStr = ansArr[3];

                shuffleAnswers(ansArr, 4);

                char currentCorrect = 'A';
                for (int k = 0; k < 4; k++) {
                    if (ansArr[k] == correctStr) {
                        currentCorrect = 'A' + k;
                        break;
                    }
                }

                cout << "\nCâu " << i + 1 << " [" << examQuestions[i]->subject << " - " << examQuestions[i]->difficulty << "]: " << examQuestions[i]->content << "\n";
                cout << "A. " << ansArr[0] << "\n";
                cout << "B. " << ansArr[1] << "\n";
                cout << "C. " << ansArr[2] << "\n";
                cout << "D. " << ansArr[3] << "\n";
                
                cout << "Trả lời (Thời gian còn lại " << timeLimit - (currentTime - startTime) << " giây): ";
                char ans;
                cin >> ans;
                
                if (ans >= 'a' && ans <= 'z') ans -= 32; 
                if (ans == currentCorrect) score++;
            }

          // Tự động chấm điểm và phân loại lời nhắc theo kết quả
    double phanTram = (double)score / numQ * 100;

    cout << "\n============================================\n";
    cout << "   🎉 KẾT THÚC BÀI THI TRẮC NGHIỆM 🎉\n";
    cout << "============================================\n";
    cout << "Thí sinh: " << name << "\n";
    cout << "Kết quả đạt được: " << score << "/" << numQ << " câu đúng.\n";
    cout << "Tỷ lệ chính xác: " << phanTram << "%\n";
    cout << "Đánh giá: ";

    if (phanTram == 100) {
        cout << "🥇 Xuất sắc tuyệt đối! Hurayyy, bạn làm tốt quá dioo~ ✨💖\n";
    } else if (phanTram >= 80) {
        cout << "🌟 Siêu cấp đỉnh cao! Bạn học bài kỹ lắm đúng không nè? Quá giỏi! 🎉\n";
    } else if (phanTram >= 50) {
        cout << "👍 Kết quả rất tốt nhé! Cố lên một xíu nữa là đạt điểm tối đa luôn rùi~ 🥰\n";
    } else {
        cout << "💪 Không sao hết nè, vẫn còn nhiều cơ hội mà! Ôm một cái lấy động lực rồi lần sau làm lại tốt hơn nhaaa~ 💕\n";
    }
    cout << "============================================\n";

    saveReport(rFile, name, score, numQ);
    delete[] examQuestions;
        }
        else if (choice == 3) {
            cout << "\n=== BÁO CÁO KẾT QUẢ LỊCH SỬ CÁC LẦN THI ===\n";
            ifstream file(rFile);
            if (!file.is_open()) {
                cout << "=> Chưa có bất kỳ dữ liệu lịch sử thi nào được ghi nhận trong hệ thống.\n";
            } else {
                string line;
                while (getline(file, line)) {
                    cout << line << "\n";
                }
                file.close();
            }
        }
    } while (choice != 0);

    return 0;
}
