#ifndef REPORT_H
#define REPORT_H

#include <string>

void saveReport(const std::string& filename, const std::string& name, int score, int total);
void viewReport(const std::string& rFile);

#endif
