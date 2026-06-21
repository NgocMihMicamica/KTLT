#ifndef MODELS_H
#define MODELS_H

#include <string>

struct Question {
    int id;
    std::string subject;
    std::string content;
    std::string A, B, C, D;
    char correctAns;
    std::string difficulty;
};

struct AnswerOption {
    std::string text;
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

#endif
