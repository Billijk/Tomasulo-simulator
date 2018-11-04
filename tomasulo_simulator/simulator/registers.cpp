#include "registers.h"
#include "reservation.h"
#include <iostream>

using namespace std;

Register::Register(): value(0), qi(RESERVATION_NONE) {}

int Register::write_back(string from, float value) {
    if (from != qi) return 0;
    int ret = 0;
    if ((ret = set_value(value)) != 0) return ret;
    if ((ret = set_reservation(RESERVATION_NONE)) != 0) return ret;
    return ret;
}

RegisterManager::RegisterManager(int size) {
    for (int i = 0; i < size; ++ i) {
        string name = string("F") + to_string(i);
        registers[name] = Register();
    }
}

RegisterManager::~RegisterManager() {}

int RegisterManager::get_register_names(vector<string>& res) {
    res.clear();
    for (const auto& it: registers) res.push_back(it.first);
    return 0;
}

int RegisterManager::get_registers(vector<string> names, vector<float>& res) {
    res.clear();
    for (const auto& name: names) {
        if (registers.find(name) == registers.end()) return -1;
        res.push_back(registers[name].get_value());
    }
    return 0;
}

int RegisterManager::get_register_qs(vector<string> names, vector<string>& res) {
    res.clear();
    for (const auto& name: names) {
        if (registers.find(name) == registers.end()) return -1;
        res.push_back(registers[name].get_reservation());
    }
    return 0;
}

Register* RegisterManager::get_register_pointer(string name) {
    if (registers.find(name) == registers.end()) return NULL;
    else return &registers[name];
}

int RegisterManager::reserve(string reg_name, string reservation_name) {
    if (registers.find(reg_name) == registers.end()) return -1;
    return registers[reg_name].set_reservation(reservation_name);
}

int RegisterManager::set_value(string reg_name, float value) {
    if (registers.find(reg_name) == registers.end()) return -1;
    return registers[reg_name].set_value(value);
}

int RegisterManager::write_back(string from, float value) {
    int ret = 0;
    for (auto& item: registers)
        if ((ret = item.second.write_back(from, value)) != 0)
            return ret;
    return 0;
}

int RegisterManager::clear() {
    for (auto& it: registers) {
        it.second.set_value(0.0f);
        it.second.set_reservation(RESERVATION_NONE);
    }
    return 0;
}