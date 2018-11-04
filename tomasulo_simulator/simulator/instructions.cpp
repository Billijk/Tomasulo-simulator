#include "instructions.h"
#include <vector>
#include <iostream>

using namespace std;

int Instruction::needed_cycle(InstrType type) {
    switch (type) {
        case ADDD: 
        case SUBD:
        case LD:
        case ST:
            return 2;
        case MULD:
            return 10;
        case DIVD:
            return 40;
        default: return -1; 
    }
}

Instruction* Instruction::parse(string instr, const set<string>& valid_reg_names, int valid_memory_size) {
    // split
    size_t split_pos = instr.find(" ");
    if (split_pos == string::npos) return new ErrorInstruction();
    string op = instr.substr(0, split_pos);
    vector<string> tokens;
    size_t new_pos;
    do {
        // ignore possible spaces
        while (instr[split_pos] == ' ' && split_pos < instr.size()) split_pos ++;
        new_pos = instr.find(",", split_pos);
        int len = new_pos - split_pos;
        while (instr[split_pos + len - 1] == ' ' && len > 0) len --;
        tokens.push_back(instr.substr(split_pos, len));
        split_pos = new_pos + 1;
    } while (new_pos != string::npos);
    // check instrucion format
    if (tokens.size() == 3) {
        for (int i = 0; i < 3; ++ i)
            if (valid_reg_names.find(tokens[i]) == valid_reg_names.end())
                return new ErrorInstruction();
        
        // operands valid, check op
        if (op == string("ADDD")) {
            return new ADDDi(tokens[0], tokens[1], tokens[2]);
        } else if (op == string("SUBD")) {
            return new SUBDi(tokens[0], tokens[1], tokens[2]);
        } else if (op == string("MULD")) {
            return new MULDi(tokens[0], tokens[1], tokens[2]);
        } else if (op == string("DIVD")) {
            return new DIVDi(tokens[0], tokens[1], tokens[2]);
        } else return new ErrorInstruction();
    } else if (tokens.size() == 2) {
        // first operand as register name
        if (valid_reg_names.find(tokens[0]) == valid_reg_names.end())
            return new ErrorInstruction();
        // second operand as int addr
        for (size_t i = 0; i < tokens[1].size(); ++ i)
            if (tokens[1][i] < '0' || tokens[1][i] > '9')
                return new ErrorInstruction();
        int addr = stoi(tokens[1]);
        if (addr >= valid_memory_size)
            return new ErrorInstruction();

        // operands valid, check op
        if (op == string("LD")) {
            return new LDi(tokens[0], addr);
        } else if (op == string("ST")) {
            return new STi(tokens[0], addr);
        } else return new ErrorInstruction();
    } else return new ErrorInstruction();
}
