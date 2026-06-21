#ifndef QUESTION_H
#define QUESTION_H

#include <string>

#include "models.h"

char normalizeAnswer(char ans);
bool isValidAnswer(char ans);
bool isEasyDifficulty(const std::string& difficulty);
bool isHardDifficulty(const std::string& difficulty);
bool isValidDifficulty(const std::string& difficulty);
std::string normalizeDifficulty(const std::string& difficulty);
bool isValidQuestion(const Question& q);
void shuffleQuestions(Question* arr[], int n);
void shuffleAnswers(AnswerOption arr[], int n);

#endif
