#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <ctime>
#include <string>

#include "models.h"

void clearInputLine();
bool readIntInRange(const std::string& prompt, int& value, int minValue, int maxValue);
bool readIntAtLeast(const std::string& prompt, int& value, int minValue);
bool readNonEmptyLine(const std::string& prompt, std::string& value);
bool readAnswerInput(const std::string& prompt, char& answer);
int getRemainingSeconds(std::time_t startTime, int timeLimit);
bool isInteractiveInput();
TimedAnswerStatus readAnswerCharacterWithTimer(char& answer, std::time_t startTime, int timeLimit);
TimedAnswerStatus readAnswerInputWithTimer(const std::string& prompt, char& answer,
                                           std::time_t startTime, int timeLimit);
bool readDifficultyInput(const std::string& prompt, std::string& difficulty);

#endif
