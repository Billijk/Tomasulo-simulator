#pragma once

#include <map>
#include <string>
#include <vector>

class Register {
    float value;
    std::string qi;     // corresponding reservation name
public:
    Register();
    ~Register() {}
    float get_value() { return value; }
    int set_value(float float_value) { value = float_value; return 0; }
    std::string get_reservation() { return qi; }
    int set_reservation(std::string name) { qi = name; return 0; }

    int write_back(std::string from, float float_value);
};

class RegisterManager {
    std::map<std::string, Register> registers;
public:
    RegisterManager(int size);
    ~RegisterManager();

    int get_register_names(std::vector<std::string>& res);
    int get_registers(std::vector<std::string> names, std::vector<float>& res);
    int get_register_qs(std::vector<std::string> names, std::vector<std::string>& res);
    Register* get_register_pointer(std::string name); // return null if not found

    int reserve(std::string reg_name, std::string reservation_name);
    int set_value(std::string reg_name, float value);
    int write_back(std::string from, float value);

    int clear();
};