#pragma once

#include "instructions.h"
#include <string>

#define RESERVATION_NONE "None"

class ALU;
class Memory;

class Reservation {
protected:
    bool buzy;
    InstrType op;
    float vj, vk;
    unsigned int a;
    std::string qj, qk;
    std::string name;
    int rest_cycles;

    // NOTICE: this is not part of a standard reservation,
    // we record this for to show more information to users.
    Instruction* instruction;
public:
    Reservation(std::string _name): name(_name) {
        clear();
    }

    bool is_buzy() const { return buzy; }
    InstrType get_op() const { return op; }
    float get_vj() const { return vj; }
    float get_vk() const { return vk; }
    unsigned int get_a() const { return a; }
    std::string get_qj() const { return qj; }
    std::string get_qk() const { return qk; }
    std::string get_name() const { return name; }
    int get_rest_cycles() const { return rest_cycles; }

    Instruction* get_instruction() { return instruction; }

    // if all the operands is ready
    bool ready() { return buzy && qj == RESERVATION_NONE && qk == RESERVATION_NONE; }
    int set_rest_cycles(int _rest_cycles) { rest_cycles = _rest_cycles; return 0; }
    int decrease_cycle() { return --rest_cycles; }
    int finish_work() { buzy = false; rest_cycles = 0; return 0; }
    // get needed value from cdb
    int fill_qj(float _vj) { vj = _vj; qj = RESERVATION_NONE; return 0; }
    int fill_qk(float _vk) { vk = _vk; qk = RESERVATION_NONE; return 0; }

    int clear() {
        buzy = false;
        vj = vk = a = 0;
        qj = RESERVATION_NONE;
        qk = RESERVATION_NONE;
        rest_cycles = 0;
        return 0;
    }
};

class ALUReservation : public Reservation {
    ALU* owner;
public:
    ALUReservation(std::string _name, ALU* _owner): Reservation(_name), owner(_owner) {}
    ~ALUReservation() {}

    // occupy this reservation
    int reserve(InstrType _op, float _vj, float _vk, std::string _qj, std::string _qk, Instruction* instr) {
        buzy = true;
        op = _op;
        vj = _vj;
        vk = _vk;
        qj = _qj;
        qk = _qk;
        instruction = instr;
        return 0;
    }
};

class MemoryBuffer : public Reservation {
    Memory* owner;
public:
    MemoryBuffer(std::string _name, Memory* _owner): Reservation(_name), owner(_owner) {}
    ~MemoryBuffer() {}

    // occupy this reservation
    int reserve(InstrType _op, float _vj, int _a, std::string _qj, Instruction* instr) {
        buzy = true;
        op = _op;
        vj = _vj;
        a = _a;
        qj = _qj;
        qk = RESERVATION_NONE;
        instruction = instr;
        return 0;
    }
};