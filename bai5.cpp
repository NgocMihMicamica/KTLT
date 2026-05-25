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
    string difficulty; // Độ khó (De/Kho)
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

// Thuật toán xáo trộn Fisher-Yates viết chay cho mảng các con trỏ câu hỏi (Sinh đề thi ngẫu nhiên)
void shuffleQuestions(Question* arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Question* temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

// Thuật toán xáo trộn Fisher-Yates viết chay cho mảng chuỗi (Xáo trộn thứ tự đáp án A, B, C, D)
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
        file << "Nguoi thi: " << name << " | Diem: " << score << "/" << total << "\n";
        file.close();
    }
}

int main() {
    srand(time(0));
    
    // ĐƯỜNG DẪN TƯƠNG ĐỐI NGUYÊN BẢN: Đọc trực tiếp file ở cùng thư mục, không chứa ổ C:\
    string qFile = "questions.txt";
    string rFile = "report.txt";

    QuestionList bank;
    bank.loadFromFile(qFile);

    int choice;
    do {
        cout << "\n=== CHUONG TRINH THI TRAC NGHIEM ===\n";
        cout << "1. Nhap them cau hoi tu ban phim (Luu vao file text)\n";
        cout << "2. Thuc hien thi trac nghiem (Sinh de, Xao tron & Tinh gio)\n";
        cout << "3. Xem bao cao ket qua cac lan thi\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "Chon chuc nang: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            Question q;
            cout << "\n--- CHUC NANG: NHAP CAU HOI MOI ---\n";
            cout << "ID cau hoi (So nguyen): "; cin >> q.id; cin.ignore();
            cout << "Mon / Hoc phan: "; getline(cin, q.subject);
            cout << "Noi dung cau hoi: "; getline(cin, q.content);
            cout << "Dap an A: "; getline(cin, q.A);
            cout << "Dap an B: "; getline(cin, q.B);
            cout << "Dap an C: "; getline(cin, q.C);
            cout << "Dap an D: "; getline(cin, q.D);
            cout << "Dap an dung (Go ky tu A hoac B hoac C hoac D): "; cin >> q.correctAns; cin.ignore();
            cout << "Do kho (De/Kho): "; getline(cin, q.difficulty);
            
            bank.addQuestion(q);
            bank.saveToFile(qFile);
            
            cout << "=> He thong da luu cau hoi moi xuong file '" << qFile << "' thanh cong!\n";
        }
        else if (choice == 2) {
            bank.loadFromFile(qFile);

            if (bank.size == 0) {
                cout << "=> Thong bao: Ngan hang cau hoi dang trong hoac thieu file '" << qFile << "'!\n";
                cout << "=> Hay dung tinh nang 1 de nap cau hoi hoac kiem tra thu muc chua chuong trinh.\n";
                continue;
            }

            string name;
            int numQ, timeLimit;
            cout << "\n--- CHUC NANG: THUC HIEN THI TRAC NGHIEM ---\n";
            cout << "Nhap ten nguoi tham gia thi: "; getline(cin, name);
            cout << "Nhap so luong cau hoi muon thi (Hien co " << bank.size << " cau): "; cin >> numQ;
            cout << "Nhap thoi gian lam bai dinh muc (Don vi: giay): "; cin >> timeLimit;
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
                    cout << "\n*** QUA THOI GIAN LAM BAI! HE THONG TU DONG THU BAI ***\n";
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

                cout << "\nCau " << i + 1 << " [" << examQuestions[i]->subject << " - " << examQuestions[i]->difficulty << "]: " << examQuestions[i]->content << "\n";
                cout << "A. " << ansArr[0] << "\n";
                cout << "B. " << ansArr[1] << "\n";
                cout << "C. " << ansArr[2] << "\n";
                cout << "D. " << ansArr[3] << "\n";
                
                cout << "Tra loi (Thoi gian con lai " << timeLimit - (currentTime - startTime) << " giay): ";
                char ans;
                cin >> ans;
                
                if (ans >= 'a' && ans <= 'z') ans -= 32; 
                if (ans == currentCorrect) score++;
            }

            cout << "\n=> KET THUC BAI THI! Ket qua cua " << name << " dat duoc: " << score << "/" << numQ << "\n";
            
            saveReport(rFile, name, score, numQ);
            delete[] examQuestions; 
        }
        else if (choice == 3) {
            cout << "\n=== BAO CAO KET QUA LICH SU CAC LAN THI ===\n";
            ifstream file(rFile);
            if (!file.is_open()) {
                cout << "=> Chua co bat ky du lieu lich su thi nao duoc ghi nhan trong he thong.\n";
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