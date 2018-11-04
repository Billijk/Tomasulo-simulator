#pragma once

#include <set>
#include <string>

enum InstrStatus { WAITING, ISSUED, EXECUTING, EXECUTED, DONE };
enum InstrType { ERROR, ADDD, SUBD, MULD, DIVD, LD, ST };

class Instruction {
protected:
    InstrStatus status;
    InstrType type;
public:
    Instruction(InstrType _type): status(WAITING), type(_type) {}
    virtual ~Instruction() {}

    // return needed cycles given instruction type
    static int needed_cycle(InstrType type);
    // factory method, return a new instruction given string format
    static Instruction* parse(std::string instr, 
        const std::set<std::string>& valid_reg_names, int valid_memory_size);

    InstrStatus get_status() const { return status; }
    void next_status() {
        if (status != DONE)
            status = static_cast<InstrStatus>(((int) status) + 1);
    }
    InstrType get_type() const { return type; }
    virtual std::string get_instruction() = 0;
    virtual std::string get_dst() = 0;      // for Arith and LD
    virtual std::string get_src1() = 0;     // for Arith and ST
    virtual std::string get_src2() = 0;     // for Arith
    virtual int get_addr() = 0;             // for LD and ST
};

class ArithmeticInstruction: public Instruction {
protected:
    std::string f1, f2, f3;
public:
    ArithmeticInstruction(std::string _f1, std::string _f2, std::string _f3, 
        InstrType _type): Instruction(_type), f1(_f1), f2(_f2), f3(_f3) {}
    virtual ~ArithmeticInstruction() {}
    virtual std::string get_instruction() = 0;
    std::string get_dst() { return f1; }
    std::string get_src1() { return f2; }
    std::string get_src2() { return f3; }
    int get_addr() { return -1; }
    virtual float calculate(float src1, float src2) = 0;
};

class ADDDi : public ArithmeticInstruction {
public:
    ADDDi(std::string _f1, std::string _f2, std::string _f3) : ArithmeticInstruction(_f1, _f2, _f3, ADDD) {}
    ~ADDDi() {}
    std::string get_instruction() { return "ADDD " + f1 + ", " + f2 + ", " + f3; }
    float calculate(float src1, float src2) { return src1 + src2; }
};

class SUBDi : public ArithmeticInstruction {
public:
    SUBDi(std::string _f1, std::string _f2, std::string _f3) : ArithmeticInstruction(_f1, _f2, _f3, SUBD) {}
    ~SUBDi() {}
    std::string get_instruction() { return "SUBD " + f1 + ", " + f2 + ", " + f3; }
    float calculate(float src1, float src2) { return src1 - src2; }
};

class MULDi : public ArithmeticInstruction {
public:
    MULDi(std::string _f1, std::string _f2, std::string _f3) : ArithmeticInstruction(_f1, _f2, _f3, MULD) {}
    ~MULDi() {}
    std::string get_instruction() { return "MULD " + f1 + ", " + f2 + ", " + f3; }
    float calculate(float src1, float src2) { return src1 * src2; }
};

class DIVDi : public ArithmeticInstruction {
public:
    DIVDi(std::string _f1, std::string _f2, std::string _f3) : ArithmeticInstruction(_f1, _f2, _f3, DIVD) {}
    ~DIVDi() {}
    std::string get_instruction() { return "DIVD " + f1 + ", " + f2 + ", " + f3; }
    float calculate(float src1, float src2) { return src1 / src2; }
};

class MemoryInstruction : public Instruction {
protected:
    std::string f1;
    int addr;
public:
    MemoryInstruction(std::string _f1, int _addr, InstrType _type):
         Instruction( _type), f1(_f1), addr(_addr)  {}
    virtual ~MemoryInstruction() {}
    virtual std::string get_instruction() = 0;
    virtual std::string get_dst() = 0;      // for LD
    virtual std::string get_src1() = 0;     // for ST
    std::string get_src2() { return "INVALID"; }
    int get_addr() { return addr; }
};

class LDi : public MemoryInstruction {
public:
    LDi(std::string _f1, int _addr) : MemoryInstruction(_f1, _addr, LD) {}
    ~LDi() {}
    std::string get_instruction() { return "LD " + f1 + ", " + std::to_string(addr); }
    std::string get_dst() { return f1; }
    std::string get_src1() { return "INVALID"; }
};

class STi : public MemoryInstruction {
public:
    STi(std::string _f1, int _addr) : MemoryInstruction(_f1, _addr, ST) {}
    ~STi() {}
    std::string get_instruction() { return "ST " + f1 + ", " + std::to_string(addr); }
    std::string get_dst() { return "INVALID"; }
    std::string get_src1() { return f1; }
};

class ErrorInstruction : public Instruction {
public:
    ErrorInstruction() : Instruction(ERROR) {}
    ~ErrorInstruction() {}
    std::string get_instruction() { return "ERROR"; }
    std::string get_dst() { return "INVALID"; }
    std::string get_src1() { return "INVALID"; }
    std::string get_src2() { return "INVALID"; }
    int get_addr() { return -1; }
};
