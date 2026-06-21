#ifndef EXAM_H
#define EXAM_H

#include <string>

#include "models.h"

bool inputNewQuestion(QuestionList& bank, const std::string& qFile);
void printEvaluation(double percent);
bool takeExam(QuestionList& bank, const std::string& qFile, const std::string& rFile);

#endif
