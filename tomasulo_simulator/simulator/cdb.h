#pragma once

#include <string>
#include <vector>
#include <algorithm>

class CPUKernel;

class CDB {
    CPUKernel* kernel;
    std::vector<std::pair<std::string, float>> notifications;
public:
    CDB(CPUKernel* _kernel): kernel(_kernel) {}
    ~CDB() {}

    int write_back(std::string from, float value);
    int notify_reservations();
};