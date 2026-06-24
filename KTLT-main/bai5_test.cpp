#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "file_utils.h"
#include "question.h"
#include "question_list.h"
#include "report.h"
#include "text_utils.h"

using namespace std;

static int testsPassed = 0;
static int testsFailed = 0;

#define TEST(name) \
    do { \
        cout << "  TEST: " << name << " ... "; \
        try {

#define END_TEST \
            cout << "PASSED\n"; \
            testsPassed++; \
        } catch (const exception& e) { \
            cout << "FAILED (" << e.what() << ")\n"; \
            testsFailed++; \
        } catch (...) { \
            cout << "FAILED (unknown exception)\n"; \
            testsFailed++; \
        } \
    } while (0)

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            throw runtime_error( \
                string("Assertion failed: ") + #cond \
                + " (line " + to_string(__LINE__) + ")"); \
        } \
    } while (0)

#define ASSERT_EQ(a, b) \
    do { \
        auto va = (a); \
        auto vb = (b); \
        if (va != vb) { \
            ostringstream oss; \
            oss << "Expected " << #a << " == " << #b \
                << ", got " << va << " vs " << vb \
                << " (line " << __LINE__ << ")"; \
            throw runtime_error(oss.str()); \
        } \
    } while (0)

#define ASSERT_STREQ(a, b) \
    do { \
        if ((a) != (b)) { \
            throw runtime_error( \
                string("Expected string equality: ") + #a + " == " + #b \
                + ", got \"" + (a) + "\" vs \"" + (b) + "\"" \
                + " (line " + to_string(__LINE__) + ")"); \
        } \
    } while (0)

// ----------------------------------------------------------------------
// text_utils tests
// ----------------------------------------------------------------------
void test_text_utils() {
    cout << "\n=== text_utils.cpp ===\n";

    TEST("toUpperAscii converts lowercase") {
        ASSERT_EQ('A', toUpperAscii('a'));
        ASSERT_EQ('Z', toUpperAscii('z'));
        ASSERT_EQ('M', toUpperAscii('M'));
    }
    END_TEST;

    TEST("toUpperAscii leaves non-letter unchanged") {
        ASSERT_EQ('1', toUpperAscii('1'));
        ASSERT_EQ(' ', toUpperAscii(' '));
        ASSERT_EQ('\0', toUpperAscii('\0'));
    }
    END_TEST;

    TEST("toLowerAscii converts uppercase") {
        ASSERT_EQ('a', toLowerAscii('A'));
        ASSERT_EQ('z', toLowerAscii('Z'));
        ASSERT_EQ('m', toLowerAscii('m'));
    }
    END_TEST;

    TEST("toLowerAscii leaves non-letter unchanged") {
        ASSERT_EQ('5', toLowerAscii('5'));
        ASSERT_EQ('.', toLowerAscii('.'));
    }
    END_TEST;

    TEST("trimAsciiWhitespace removes spaces and tabs") {
        ASSERT_STREQ("hello", trimAsciiWhitespace("  hello"));
        ASSERT_STREQ("hello", trimAsciiWhitespace("hello  "));
        ASSERT_STREQ("hello", trimAsciiWhitespace("  hello  "));
        ASSERT_STREQ("he llo", trimAsciiWhitespace("  he llo  "));
    }
    END_TEST;

    TEST("trimAsciiWhitespace removes \\r and \\n") {
        ASSERT_STREQ("abc", trimAsciiWhitespace("\r\nabc\r\n"));
        ASSERT_STREQ("abc", trimAsciiWhitespace("\n\nabc\r"));
    }
    END_TEST;

    TEST("trimAsciiWhitespace empty result") {
        ASSERT_STREQ("", trimAsciiWhitespace(""));
        ASSERT_STREQ("", trimAsciiWhitespace("   "));
        ASSERT_STREQ("", trimAsciiWhitespace("\t\r\n "));
    }
    END_TEST;

    TEST("removeUtf8Bom removes BOM") {
        string bom = "\xEF\xBB\xBFhello";
        ASSERT_STREQ("hello", removeUtf8Bom(bom));
    }
    END_TEST;

    TEST("removeUtf8Bom no BOM") {
        ASSERT_STREQ("hello", removeUtf8Bom("hello"));
        ASSERT_STREQ("", removeUtf8Bom(""));
    }
    END_TEST;

    TEST("equalsIgnoreCaseAscii") {
        ASSERT(true == equalsIgnoreCaseAscii("hello", "HELLO"));
        ASSERT(true == equalsIgnoreCaseAscii("Hello", "hELLO"));
        ASSERT(true == equalsIgnoreCaseAscii("", ""));
        ASSERT(true != equalsIgnoreCaseAscii("hello", "world"));
        ASSERT(true != equalsIgnoreCaseAscii("hello", "hell"));
    }
    END_TEST;

    TEST("parseIntegerString valid") {
        int val = 0;
        ASSERT(true == parseIntegerString("123", val));
        ASSERT_EQ(123, val);
        ASSERT(true == parseIntegerString("0", val));
        ASSERT_EQ(0, val);
        ASSERT(true == parseIntegerString("-42", val));
        ASSERT_EQ(-42, val);
        ASSERT(true == parseIntegerString("+7", val));
        ASSERT_EQ(7, val);
    }
    END_TEST;

    TEST("parseIntegerString invalid") {
        int val = 0;
        ASSERT(true != parseIntegerString("", val));
        ASSERT(true != parseIntegerString("abc", val));
        ASSERT(true != parseIntegerString("12a", val));
        ASSERT(true != parseIntegerString(" 1", val));
        ASSERT(true != parseIntegerString("1 ", val));
    }
    END_TEST;

    TEST("parseIntegerString overflow rejected") {
        int val = 0;
        ASSERT(true != parseIntegerString("999999999999", val));
    }
    END_TEST;
}

// ----------------------------------------------------------------------
// question tests
// ----------------------------------------------------------------------
void test_question() {
    cout << "\n=== question.cpp ===\n";

    TEST("normalizeAnswer uppercases") {
        ASSERT_EQ('A', normalizeAnswer('a'));
        ASSERT_EQ('B', normalizeAnswer('b'));
        ASSERT_EQ('A', normalizeAnswer('A'));
    }
    END_TEST;

    TEST("isValidAnswer accepts A-D") {
        ASSERT(true == isValidAnswer('A'));
        ASSERT(true == isValidAnswer('b'));
        ASSERT(true != isValidAnswer('E'));
        ASSERT(true != isValidAnswer('1'));
        ASSERT(true != isValidAnswer('\0'));
    }
    END_TEST;

    TEST("isEasyDifficulty") {
        ASSERT(true == isEasyDifficulty("De"));
        ASSERT(true == isEasyDifficulty("de"));
        ASSERT(true == isEasyDifficulty("Dễ"));
        ASSERT(true == isEasyDifficulty("dễ"));
        ASSERT(true == isEasyDifficulty("DỄ"));
        ASSERT(true != isEasyDifficulty("Kho"));
        ASSERT(true != isEasyDifficulty("Khó"));
        ASSERT(true != isEasyDifficulty(""));
    }
    END_TEST;

    TEST("isHardDifficulty") {
        ASSERT(true == isHardDifficulty("Kho"));
        ASSERT(true == isHardDifficulty("kho"));
        ASSERT(true == isHardDifficulty("Khó"));
        ASSERT(true == isHardDifficulty("khó"));
        ASSERT(true == isHardDifficulty("KHÓ"));
        ASSERT(true != isHardDifficulty("De"));
        ASSERT(true != isHardDifficulty("Dễ"));
        ASSERT(true != isHardDifficulty(""));
    }
    END_TEST;

    TEST("isValidDifficulty") {
        ASSERT(true == isValidDifficulty("De"));
        ASSERT(true == isValidDifficulty("Kho"));
        ASSERT(true != isValidDifficulty("Trung binh"));
        ASSERT(true != isValidDifficulty(""));
    }
    END_TEST;

    TEST("normalizeDifficulty returns Dễ or Khó") {
        ASSERT_STREQ("Dễ", normalizeDifficulty("de"));
        ASSERT_STREQ("Dễ", normalizeDifficulty("Dễ"));
        ASSERT_STREQ("Khó", normalizeDifficulty("KHÓ"));
        ASSERT_STREQ("Khó", normalizeDifficulty("kho"));
    }
    END_TEST;

    TEST("isValidQuestion valid") {
        Question q = {1, "Math", "Q?", "A1", "B1", "C1", "D1", 'A', "Dễ"};
        ASSERT(true == isValidQuestion(q));
    }
    END_TEST;

    TEST("isValidQuestion invalid cases") {
        Question q;
        q.id = 0; q.subject = "M"; q.content = "C"; q.A = "A"; q.B = "B";
        q.C = "C"; q.D = "D"; q.correctAns = 'A'; q.difficulty = "Dễ";
        ASSERT(true != isValidQuestion(q));

        q.id = 1; q.subject = "";
        ASSERT(true != isValidQuestion(q));

        q.subject = "M"; q.correctAns = 'E';
        ASSERT(true != isValidQuestion(q));

        q.correctAns = 'A'; q.difficulty = "Trung binh";
        ASSERT(true != isValidQuestion(q));
    }
    END_TEST;

    TEST("shuffleQuestions preserves all elements") {
        Question a = {1, "S", "C", "A", "B", "C", "D", 'A', "Dễ"};
        Question b = {2, "S", "C", "A", "B", "C", "D", 'B', "Khó"};
        Question c = {3, "S", "C", "A", "B", "C", "D", 'C', "Dễ"};
        Question* arr[3] = {&a, &b, &c};
        srand(123);
        shuffleQuestions(arr, 3);
        int ids[3] = {arr[0]->id, arr[1]->id, arr[2]->id};
        ASSERT(true == (ids[0]==1 || ids[1]==1 || ids[2]==1));
        ASSERT(true == (ids[0]==2 || ids[1]==2 || ids[2]==2));
        ASSERT(true == (ids[0]==3 || ids[1]==3 || ids[2]==3));
    }
    END_TEST;

    TEST("shuffleAnswers preserves all elements") {
        AnswerOption arr[3] = {{"A1", 'A'}, {"B1", 'B'}, {"C1", 'C'}};
        srand(456);
        shuffleAnswers(arr, 3);
        bool foundA = false, foundB = false, foundC = false;
        for (int i = 0; i < 3; i++) {
            if (arr[i].originalOption == 'A') foundA = true;
            if (arr[i].originalOption == 'B') foundB = true;
            if (arr[i].originalOption == 'C') foundC = true;
        }
        ASSERT(true == foundA);
        ASSERT(true == foundB);
        ASSERT(true == foundC);
    }
    END_TEST;
}

// ----------------------------------------------------------------------
// question_list tests
// ----------------------------------------------------------------------
void test_question_list() {
    cout << "\n=== question_list.cpp ===\n";

    TEST("initQuestionList sets head to nullptr and size 0") {
        QuestionList list;
        initQuestionList(list);
        ASSERT(nullptr == list.head);
        ASSERT_EQ(0, list.size);
    }
    END_TEST;

    TEST("addQuestion adds to empty list") {
        QuestionList list;
        initQuestionList(list);
        Question q = {1, "S", "C", "A", "B", "C", "D", 'A', "Dễ"};
        addQuestion(list, q);
        ASSERT(nullptr != list.head);
        ASSERT_EQ(1, list.size);
        ASSERT_EQ(1, list.head->data.id);
    }
    END_TEST;

    TEST("addQuestion appends multiple questions") {
        QuestionList list;
        initQuestionList(list);
        Question q1 = {1, "S", "C", "A", "B", "C", "D", 'A', "Dễ"};
        Question q2 = {2, "S", "C", "A", "B", "C", "D", 'B', "Khó"};
        addQuestion(list, q1);
        addQuestion(list, q2);
        ASSERT_EQ(2, list.size);
        ASSERT_EQ(1, list.head->data.id);
        ASSERT_EQ(2, list.head->next->data.id);
    }
    END_TEST;

    TEST("findQuestionById finds existing") {
        QuestionList list;
        initQuestionList(list);
        Question q1 = {10, "S", "C", "A", "B", "C", "D", 'A', "Dễ"};
        Question q2 = {20, "S", "C", "A", "B", "C", "D", 'B', "Khó"};
        addQuestion(list, q1);
        addQuestion(list, q2);
        ASSERT(nullptr != findQuestionById(list, 10));
        ASSERT(nullptr != findQuestionById(list, 20));
        ASSERT_EQ(10, findQuestionById(list, 10)->id);
    }
    END_TEST;

    TEST("findQuestionById returns null for missing") {
        QuestionList list;
        initQuestionList(list);
        Question q = {5, "S", "C", "A", "B", "C", "D", 'A', "Dễ"};
        addQuestion(list, q);
        ASSERT(nullptr == findQuestionById(list, 99));
    }
    END_TEST;

    TEST("containsQuestionId") {
        QuestionList list;
        initQuestionList(list);
        Question q = {42, "S", "C", "A", "B", "C", "D", 'A', "Dễ"};
        addQuestion(list, q);
        ASSERT(true == containsQuestionId(list, 42));
        ASSERT(true != containsQuestionId(list, 1));
    }
    END_TEST;

    TEST("clearQuestionList frees memory") {
        QuestionList list;
        initQuestionList(list);
        addQuestion(list, {1, "S", "C", "A", "B", "C", "D", 'A', "Dễ"});
        addQuestion(list, {2, "S", "C", "A", "B", "C", "D", 'B', "Khó"});
        clearQuestionList(list);
        ASSERT(nullptr == list.head);
        ASSERT_EQ(0, list.size);
    }
    END_TEST;

    TEST("loadQuestionsFromFile loads valid data") {
        ofstream tmp("test_questions.txt");
        tmp << "1\nMath\nQ?\nA\nB\nC\nD\nA\nDễ\n";
        tmp << "2\nScience\nQ2?\nA\nB\nC\nD\nB\nKhó\n";
        tmp.close();

        QuestionList list;
        initQuestionList(list);
        bool ok = loadQuestionsFromFile(list, "test_questions.txt");
        ASSERT(true == ok);
        ASSERT_EQ(2, list.size);
        clearQuestionList(list);
        remove("test_questions.txt");
    }
    END_TEST;

    TEST("loadQuestionsFromFile skips invalid records") {
        ofstream tmp("test_bad.txt");
        tmp << "1\nMath\nQ?\nA\nB\nC\nD\nA\nDễ\n";
        tmp << "notanid\nbad\n";
        tmp << "3\nMath\nQ?\nA\nB\nC\nD\nC\nDễ\n";
        tmp.close();

        QuestionList list;
        initQuestionList(list);
        bool ok = loadQuestionsFromFile(list, "test_bad.txt");
        ASSERT(true == ok);
        ASSERT_EQ(2, list.size);
        clearQuestionList(list);
        remove("test_bad.txt");
    }
    END_TEST;

    TEST("loadQuestionsFromFile returns false if file missing") {
        QuestionList list;
        initQuestionList(list);
        bool ok = loadQuestionsFromFile(list, "nonexistent_file_123.txt");
        ASSERT(true != ok);
    }
    END_TEST;

    TEST("saveQuestionsToFile and reload") {
        QuestionList list;
        initQuestionList(list);
        addQuestion(list, {1, "Subj", "Cont?", "A1", "B1", "C1", "D1", 'C', "Khó"});
        addQuestion(list, {2, "Subj2", "Cont2?", "A2", "B2", "C2", "D2", 'A', "Dễ"});

        bool ok = saveQuestionsToFile(list, "test_save.txt");
        ASSERT(true == ok);

        QuestionList list2;
        initQuestionList(list2);
        ok = loadQuestionsFromFile(list2, "test_save.txt");
        ASSERT(true == ok);
        ASSERT_EQ(2, list2.size);
        ASSERT_EQ(1, list2.head->data.id);
        ASSERT_EQ('C', list2.head->data.correctAns);
        ASSERT_STREQ("Khó", list2.head->data.difficulty);
        ASSERT_EQ(2, list2.head->next->data.id);

        clearQuestionList(list);
        clearQuestionList(list2);
        remove("test_save.txt");
    }
    END_TEST;

    TEST("appendQuestionToFile adds to new file") {
        Question q = {99, "S", "C", "A1", "B1", "C1", "D1", 'D', "Khó"};
        remove("test_append.txt");
        ASSERT(true == appendQuestionToFile("test_append.txt", q));

        QuestionList list;
        initQuestionList(list);
        ASSERT(true == loadQuestionsFromFile(list, "test_append.txt"));
        ASSERT_EQ(1, list.size);
        ASSERT_EQ(99, list.head->data.id);
        ASSERT_EQ('D', list.head->data.correctAns);
        ASSERT_STREQ("Khó", list.head->data.difficulty);
        clearQuestionList(list);
        remove("test_append.txt");
    }
    END_TEST;

    TEST("appendQuestionToFile appends to existing file") {
        remove("test_append2.txt");
        Question q1 = {10, "S", "C", "A", "B", "C", "D", 'A', "Dễ"};
        Question q2 = {20, "S", "C", "A", "B", "C", "D", 'B', "Khó"};
        ASSERT(true == appendQuestionToFile("test_append2.txt", q1));
        ASSERT(true == appendQuestionToFile("test_append2.txt", q2));

        QuestionList list;
        initQuestionList(list);
        ASSERT(true == loadQuestionsFromFile(list, "test_append2.txt"));
        ASSERT_EQ(2, list.size);
        clearQuestionList(list);
        remove("test_append2.txt");
    }
    END_TEST;
}

// ----------------------------------------------------------------------
// report tests
// ----------------------------------------------------------------------
void test_report() {
    cout << "\n=== report.cpp ===\n";

    TEST("saveReport writes correct format") {
        saveReport("test_report.txt", "Nguyen Van A", 8, 10);

        ifstream f("test_report.txt");
        string line;
        getline(f, line);
        f.close();

        ASSERT(true != line.empty());
        ASSERT(string::npos != line.find("Nguyen Van A"));
        ASSERT(string::npos != line.find("8/10"));
        remove("test_report.txt");
    }
    END_TEST;

    TEST("viewReport handles missing file") {
        ostringstream capture;
        streambuf* oldCout = cout.rdbuf(capture.rdbuf());

        viewReport("nonexistent_report_file_xyz.txt");

        cout.rdbuf(oldCout);
        string output = capture.str();
        ASSERT(string::npos != output.find("chưa có"));
    }
    END_TEST;

    TEST("viewReport with real data shows statistics") {
        remove("test_rpt_data.txt");
        saveReport("test_rpt_data.txt", "Nguyen Van A", 8, 10);
        saveReport("test_rpt_data.txt", "Le Van B", 5, 10);

        ostringstream capture;
        streambuf* oldCout = cout.rdbuf(capture.rdbuf());
        viewReport("test_rpt_data.txt");
        cout.rdbuf(oldCout);

        string output = capture.str();
        ASSERT(string::npos != output.find("THỐNG KÊ"));
        ASSERT(string::npos != output.find("Nguyen Van A"));
        ASSERT(string::npos != output.find("Le Van B"));
        ASSERT(string::npos != output.find("2"));
        ASSERT(string::npos != output.find("13")); // 8+5 = 13 câu đúng
        ASSERT(string::npos != output.find("20")); // 10+10 = 20 câu tổng
        remove("test_rpt_data.txt");
    }
    END_TEST;
}

// ----------------------------------------------------------------------
// file_utils tests
// ----------------------------------------------------------------------
void test_file_utils() {
    cout << "\n=== file_utils.cpp ===\n";

    TEST("fileExists returns true for existing file") {
        ofstream tmp("test_exists.txt");
        tmp << "data";
        tmp.close();
        ASSERT(true == fileExists("test_exists.txt"));
        remove("test_exists.txt");
    }
    END_TEST;

    TEST("fileExists returns false for missing file") {
        ASSERT(true != fileExists("test_nonexistent_abc.txt"));
    }
    END_TEST;

    TEST("getDirectoryName extracts directory") {
        ASSERT_STREQ("foo", getDirectoryName("foo/bar.txt"));
        ASSERT_STREQ("foo\\bar", getDirectoryName("foo\\bar\\baz.txt"));
    }
    END_TEST;

    TEST("getDirectoryName empty when no separator") {
        ASSERT_STREQ("", getDirectoryName("file.txt"));
    }
    END_TEST;

    TEST("joinPath combines directory and file") {
        string result = joinPath("dir", "file.txt");
        ASSERT(true ==
            (result == "dir/file.txt" || result == "dir\\file.txt"));
    }
    END_TEST;

    TEST("joinPath returns filename if directory empty") {
        ASSERT_STREQ("file.txt", joinPath("", "file.txt"));
    }
    END_TEST;
}

// ----------------------------------------------------------------------
// main
// ----------------------------------------------------------------------
int main() {
    cout << "========================================\n";
    cout << "  UNIT TESTS - KTLT Exam Program\n";
    cout << "========================================\n";

    test_text_utils();
    test_question();
    test_question_list();
    test_report();
    test_file_utils();

    cout << "\n========================================\n";
    cout << "  RESULTS: " << (testsPassed + testsFailed)
         << " total, " << testsPassed << " passed, "
         << testsFailed << " failed\n";
    cout << "========================================\n";

    return testsFailed > 0 ? 1 : 0;
}
