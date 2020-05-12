#ifndef CREAT_MAXIMA_SESSION_H
#define CREAT_MAXIMA_SESSION_H
#include <zip.h>
#include <iostream>
#include <fstream>
#include <sstream>

int createMaximaSession(const std::string &sessionName, const std::string &circuitFileName, const std::string &data);
int createKicad2SyciraMaximaFile(const std::string &circuitFileName, const std::string &data);
int createMinimalSession(const std::string &sessionName, const std::string &circuitFileName);
bool checkSessionExists(const std::string &sessionName);



#endif
