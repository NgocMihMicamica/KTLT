#ifndef QUESTION_LIST_H
#define QUESTION_LIST_H

#include <string>

#include "models.h"

void initQuestionList(QuestionList& list);
void clearQuestionList(QuestionList& list);
Question* findQuestionById(QuestionList& list, int id);
bool containsQuestionId(QuestionList& list, int id);
void addQuestion(QuestionList& list, const Question& q);
bool appendQuestionToFile(const std::string& filename, const Question& q);
bool loadQuestionsFromFile(QuestionList& list, const std::string& filename);
bool saveQuestionsToFile(QuestionList& list, const std::string& filename);

#endif
