# Hướng dẫn chạy chương trình thi trắc nghiệm C++

Chương trình chạy trên giao diện console. Ngân hàng câu hỏi được đọc từ `data/questions.txt`, còn lịch sử kết quả thi được ghi vào `data/report.txt`.

## Chức năng chính

- Đọc ngân hàng câu hỏi từ file text, mỗi câu hỏi gồm 9 dòng liên tiếp.
- Nhập thêm câu hỏi mới, kiểm tra ID trùng, đáp án đúng và độ khó.
- Tổ chức bài thi trắc nghiệm, xáo trộn câu hỏi và đáp án, giới hạn thời gian làm bài.
- Chấm điểm tự động, lưu kết quả và xem thống kê lịch sử thi.

## Cấu trúc thư mục

```text
KTLT-main/
├── data/
│   ├── questions.txt          # Ngân hàng câu hỏi
│   └── report.txt             # Lịch sử kết quả thi
├── include/
│   ├── app.h                  # Khai báo luồng chương trình chính
│   ├── console_utils.h        # Khai báo cấu hình console UTF-8
│   ├── exam.h                 # Khai báo chức năng nhập câu hỏi và thi
│   ├── file_utils.h           # Khai báo xử lý đường dẫn file dữ liệu
│   ├── input_utils.h          # Khai báo nhập liệu an toàn và nhập có hẹn giờ
│   ├── models.h               # Struct dùng chung: Question, Node, QuestionList...
│   ├── question.h             # Khai báo kiểm tra/xử lý câu hỏi
│   ├── question_list.h        # Khai báo danh sách liên kết và đọc/ghi câu hỏi
│   ├── report.h               # Khai báo lưu và xem báo cáo
│   └── text_utils.h           # Khai báo xử lý chuỗi
├── src/
│   ├── bai5.cpp               # Hàm main
│   ├── app.cpp                # Menu chính và luồng điều khiển
│   ├── console_utils.cpp      # Cấu hình console tiếng Việt UTF-8
│   ├── exam.cpp               # Nhập câu hỏi, tổ chức thi, chấm điểm
│   ├── file_utils.cpp         # Xác định đường dẫn file dữ liệu
│   ├── input_utils.cpp        # Nhập liệu và nhập đáp án có giới hạn thời gian
│   ├── question.cpp           # Kiểm tra đáp án, độ khó và xáo trộn
│   ├── question_list.cpp      # Danh sách liên kết, đọc/ghi ngân hàng câu hỏi
│   ├── report.cpp             # Đọc lịch sử thi và in thống kê
│   └── text_utils.cpp         # Xử lý chuỗi, BOM UTF-8, khoảng trắng, parse số
└── README.md
```

## Cách build và chạy

### Windows với MinGW/MSYS2

```bash
g++ -std=c++17 -Iinclude src/*.cpp -o bai5.exe
./bai5.exe
```

### Linux, macOS hoặc GitHub Codespaces

```bash
g++ -std=c++17 -Iinclude src/*.cpp -o bai5
./bai5
```

Nếu dùng VS Code, có thể chạy task `Run bai5`; cấu hình trong `.vscode/tasks.json` đã trỏ tới `src` và `include`.

## Định dạng file dữ liệu

`data/questions.txt`: mỗi câu hỏi gồm 9 dòng:

```text
ID
Môn / Học phần
Nội dung câu hỏi
Đáp án A
Đáp án B
Đáp án C
Đáp án D
Đáp án đúng
Độ khó
```

`data/report.txt`: mỗi dòng là một kết quả thi theo dạng:

```text
Người thi: <name> | Điểm: <score>/<total> | Tỷ lệ: <percent>%
```
