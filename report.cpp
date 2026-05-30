#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

struct ReportRecord {
    string name;
    int score;
    int total;
    double percent;
};

string removeUtf8Bom(const string& text);
string trimAsciiWhitespace(const string& text);
bool parseIntegerString(const string& text, int& value);

bool parseReportLine(const string& line, ReportRecord& record) {
    string cleanLine = trimAsciiWhitespace(removeUtf8Bom(line));
    if (cleanLine.empty()) return false;

    string nameLabel = "Người thi: ";
    string scoreLabel = " | Điểm: ";

    size_t namePos = cleanLine.find(nameLabel);
    size_t scorePos = cleanLine.find(scoreLabel);
    if (namePos != 0 || scorePos == string::npos || scorePos <= nameLabel.length()) {
        return false;
    }

    record.name = trimAsciiWhitespace(cleanLine.substr(nameLabel.length(),
                                                       scorePos - nameLabel.length()));
    if (record.name.empty()) return false;

    size_t scoreStart = scorePos + scoreLabel.length();
    size_t slashPos = cleanLine.find('/', scoreStart);
    if (slashPos == string::npos || slashPos <= scoreStart) return false;

    size_t totalStart = slashPos + 1;
    size_t totalEnd = totalStart;
    while (totalEnd < cleanLine.length() &&
           cleanLine[totalEnd] >= '0' && cleanLine[totalEnd] <= '9') {
        totalEnd++;
    }
    if (totalEnd <= totalStart) return false;

    string scoreText = trimAsciiWhitespace(cleanLine.substr(scoreStart, slashPos - scoreStart));
    string totalText = trimAsciiWhitespace(cleanLine.substr(totalStart, totalEnd - totalStart));

    if (!parseIntegerString(scoreText, record.score)) return false;
    if (!parseIntegerString(totalText, record.total)) return false;
    if (record.total <= 0 || record.score < 0 || record.score > record.total) return false;

    record.percent = (double)record.score / record.total * 100;
    return true;
}

void printFixedNumber(double value, int precision) {
    ios::fmtflags oldFlags = cout.flags();
    streamsize oldPrecision = cout.precision();

    cout << fixed << setprecision(precision) << value;

    cout.flags(oldFlags);
    cout.precision(oldPrecision);
}

void printPercent(double percent) {
    printFixedNumber(percent, 2);
    cout << "%";
}

void saveReport(const string& filename, const string& name, int score, int total) {
    ofstream file(filename.c_str(), ios::app);
    if (!file.is_open()) {
        cout << "=> Không thể ghi file báo cáo '" << filename << "'.\n";
        return;
    }

    double percent = total > 0 ? (double)score / total * 100 : 0;
    file << "Người thi: " << name
         << " | Điểm: " << score << "/" << total
         << " | Tỷ lệ: " << percent << "%\n";
    file.close();
}

void viewReport(const string& rFile) {
    cout << "\n=== BÁO CÁO KẾT QUẢ LỊCH SỬ CÁC LẦN THI ===\n";

    ifstream file(rFile.c_str());
    if (!file.is_open()) {
        cout << "=> Chưa có file lịch sử thi.\n";
        return;
    }

    string line;
    bool hasData = false;
    int validRecords = 0;
    int invalidRecords = 0;
    int totalCorrect = 0;
    int totalQuestions = 0;
    double totalPercent = 0;
    int excellentCount = 0;
    int veryGoodCount = 0;
    int passedCount = 0;
    int needPracticeCount = 0;
    ReportRecord bestRecord;
    ReportRecord worstRecord;

    while (getline(file, line)) {
        line = removeUtf8Bom(line);
        if (!trimAsciiWhitespace(line).empty()) {
            cout << line << "\n";
            hasData = true;

            ReportRecord record;
            if (parseReportLine(line, record)) {
                if (validRecords == 0 || record.percent > bestRecord.percent) {
                    bestRecord = record;
                }
                if (validRecords == 0 || record.percent < worstRecord.percent) {
                    worstRecord = record;
                }

                validRecords++;
                totalCorrect += record.score;
                totalQuestions += record.total;
                totalPercent += record.percent;

                if (record.score == record.total) {
                    excellentCount++;
                } else if (record.percent >= 80) {
                    veryGoodCount++;
                } else if (record.percent >= 50) {
                    passedCount++;
                } else {
                    needPracticeCount++;
                }
            } else {
                invalidRecords++;
            }
        }
    }
    file.close();

    if (!hasData) {
        cout << "=> Chưa có bất kỳ dữ liệu lịch sử thi nào.\n";
        return;
    }

    if (validRecords == 0) {
        cout << "\n=> Chưa có dòng dữ liệu hợp lệ để thống kê.\n";
        return;
    }

    cout << "\n--- THỐNG KÊ TỔNG HỢP ---\n";
    cout << "Tổng số lượt thi hợp lệ: " << validRecords << "\n";
    cout << "Tổng số câu đã làm: " << totalQuestions << "\n";
    cout << "Tổng số câu trả lời đúng: " << totalCorrect << "\n";
    cout << "Số câu đúng trung bình mỗi lượt: ";
    printFixedNumber((double)totalCorrect / validRecords, 2);
    cout << "\n";
    cout << "Tỷ lệ đúng toàn bộ: ";
    printPercent((double)totalCorrect / totalQuestions * 100);
    cout << "\n";
    cout << "Tỷ lệ đúng trung bình mỗi lượt: ";
    printPercent(totalPercent / validRecords);
    cout << "\n";

    cout << "Kết quả cao nhất: " << bestRecord.name << " - "
         << bestRecord.score << "/" << bestRecord.total << " (";
    printPercent(bestRecord.percent);
    cout << ")\n";

    cout << "Kết quả thấp nhất: " << worstRecord.name << " - "
         << worstRecord.score << "/" << worstRecord.total << " (";
    printPercent(worstRecord.percent);
    cout << ")\n";

    cout << "Phân loại: Xuất sắc " << excellentCount
         << " | Rất tốt " << veryGoodCount
         << " | Đạt yêu cầu " << passedCount
         << " | Cần ôn tập thêm " << needPracticeCount << "\n";

    if (invalidRecords > 0) {
        cout << "Lưu ý: Có " << invalidRecords
             << " dòng báo cáo không đúng định dạng nên không đưa vào thống kê.\n";
    }
}
