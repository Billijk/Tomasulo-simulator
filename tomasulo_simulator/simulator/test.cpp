#include "simulator.h"
#include "reservation.h"
#include "instructions.h"
#include <string>
#include <cmath>
#include <iostream>
#include <cassert>

using namespace std;

vector<Instruction*> instrs;
vector<string> names;
map<string, float> regs;
map<string, string> reg_qs;
map<unsigned int, float> mem;
map<string, Reservation*> resvs;

// test simulator setup
void test_setup(Simulator& sim);

// memory and register test
void test_set_memory(Simulator& sim);
void test_set_register(Simulator& sim);

// instruction test
void test_add_instructions(Simulator& sim);
void test_remove_instructions(Simulator& sim);
void test_set_instructions(Simulator& sim);

// executing cycle test
void test_steps(Simulator& sim);

// test clear
void test_reset(Simulator& sim);

void clear_test(Simulator& sim);    // helper func: test clean status of all parts

int main() {
    cout << "Test Start!" << endl;
    Simulator simulator;
    test_setup(simulator);
    test_set_memory(simulator);
    test_set_register(simulator);
    test_add_instructions(simulator);
    test_remove_instructions(simulator);
    test_set_instructions(simulator);
    test_steps(simulator);
    test_reset(simulator);
    cout << "All Test Passed!" << endl;
    return 0;
}

void clear_test(Simulator& sim) {
    // instructions
    sim.get_all_instructions(instrs);
    assert(instrs.size() == 0);
    assert(sim.get_pc() == 0);
    cout << ".";
    // registers
    sim.get_all_registers(regs);
    sim.get_all_register_qs(reg_qs);
    for (const auto& item: regs)
        assert(item.second == 0.0);
    for (const auto& item: reg_qs)
        assert(item.second == RESERVATION_NONE);
    // memory
    sim.get_used_memory(mem);
    assert(mem.size() == 0);
    cout << ".";
    // reservations
    sim.get_all_reservations(resvs);
    for (const auto& item: resvs) {
        assert(item.second -> is_buzy() == false);
        assert(item.second -> get_vj() == 0);
        assert(item.second -> get_vk() == 0);
        assert(item.second -> get_a() == 0);
        assert(item.second -> get_qj() == RESERVATION_NONE);
        assert(item.second -> get_qk() == RESERVATION_NONE);
        assert(item.second -> get_rest_cycles() == 0);
    }
    cout << ".";
}

void test_setup(Simulator& sim) {
    cout << "Test basic simulator setup: ";
    clear_test(sim);
    // registers
    sim.get_register_names(names);
    assert(names.size() == 10);
    for (int i = 0; i < 10; ++ i)
        assert(names[i] == "F" + to_string(i));
    cout << ".";
    // reservations
    map<string, Reservation*> lb, sb, ar, mr;
    sim.get_load_buffers(lb);
    sim.get_save_buffers(sb);
    sim.get_add_reservations(ar);
    sim.get_mul_reservations(mr);
    sim.get_all_reservations(resvs);
    assert(resvs.size() == 11);
    cout << ".";

#define TEST_NEW_RESERVATION(prefix, num, container) {    \
    assert(container.size() == num);                      \
    for (int i = 0; i < num; ++ i) {                      \
        string name = prefix + to_string(i);              \
        assert(container.find(name) != container.end());  \
        assert(resvs.find(name) != resvs.end());          \
        assert(container[name] == resvs[name]);           \
        assert(container[name] -> get_name() == name);    \
    }                                                     \
}

    TEST_NEW_RESERVATION("LOAD_", 3, lb);
    TEST_NEW_RESERVATION("SAVE_", 3, sb);
    TEST_NEW_RESERVATION("ADD_", 3, ar);
    TEST_NEW_RESERVATION("MUL_", 2, mr);
    cout << "PASSED!" << endl;
}

void test_set_memory(Simulator& sim) {
    cout << "Test set memory: ";
    sim.get_used_memory(mem);
    assert(mem.size() == 0);
    assert(sim.set_memory(4096, 0.0) == -1);
    cout << ".";
    assert(sim.set_memory(0, 1.0) == 0);
    sim.get_used_memory(mem);
    assert(mem.size() == 1);
    assert(mem.find(0) != mem.end());
    assert(mem[0] == 1.0);
    cout << ".";
    float res;
    sim.get_memory(0, res);
    assert(res == 1.0);
    cout << "PASSED!" << endl;
}

void test_set_register(Simulator& sim) {
    cout << "Test set register: ";
    assert(sim.set_register("F10", 1.0) == -1);
    assert(sim.set_register("F0", 1.5) == 0);
    float res;
    sim.get_register("F0", res);
    assert(res == 1.5);
    cout << "PASSED!" << endl;
}

void test_add_instructions(Simulator& sim) {
    cout << "Test add instructions to instruction queue: ";
    assert(sim.add_instruction(string("ADDD F0    ,  F1,    F2")) == 0);
    assert(sim.add_instruction(string("ADDD F0 F1,F2")) == -1);
    assert(sim.add_instruction(string("MULD F0,F1,F2,F3")) == -1);
    assert(sim.add_instruction(string("222333 2233 545 6")) == -1);
    assert(sim.add_instruction(string("LD F0,   F1 , F2")) == -1);
    assert(sim.add_instruction(string("ST F0, 234")) == 0);
    assert(sim.add_instruction(string("ST F0, 4096")) == -1);
    cout << ".";
    sim.get_all_instructions(instrs);
    assert(instrs.size() == 2);
    Instruction* instr_p;
    assert(sim.get_instruction(2, instr_p) == -1);
    sim.get_instruction(0, instr_p);
    assert(instrs[0] == instr_p);
    sim.get_instruction(1, instr_p);
    assert(instrs[1] == instr_p);
    cout << ".";
    assert(instrs[0] -> get_type() == ADDD);
    assert(instrs[0] -> get_dst() == "F0");
    assert(instrs[0] -> get_src1() == "F1");
    assert(instrs[0] -> get_src2() == "F2");
    assert(instrs[0] -> get_addr() == -1);
    cout << ".";
    assert(instrs[1] -> get_type() == ST);
    assert(instrs[1] -> get_src1() == "F0");
    assert(instrs[1] -> get_addr() == 234);
    assert(instrs[1] -> get_dst() == "INVALID");
    assert(instrs[1] -> get_src2() == "INVALID");
    cout << "PASSED!" << endl;
}

void test_remove_instructions(Simulator& sim) {
    cout << "Test remove instruction from instruction queue: ";
    assert(sim.add_instruction(string("ADDD F0, F0, F1")) == 0);
    assert(sim.add_instruction(string("SUBD F0, F0, F1")) == 0);
    assert(sim.add_instruction(string("MULD F0, F0, F1")) == 0);
    assert(sim.add_instruction(string("DIVD F0, F0, F1")) == 0);
    cout << ".";
    sim.get_all_instructions(instrs);
    assert(instrs.size() == 6);
    assert(sim.remove_instruction(0) == 0);
    sim.get_all_instructions(instrs);
    assert(instrs.size() == 5);
    assert(instrs[0] -> get_type() == ST);
    cout << ".";
    assert(sim.step() == 0);
    assert(sim.remove_instruction(0) == -1);
    sim.get_all_instructions(instrs);
    assert(instrs.size() == 5);
    sim.get_waiting_instructions(instrs);
    assert(instrs.size() == 4);
    cout << "PASSED!" << endl;
}

void test_set_instructions(Simulator& sim) {
    cout << "Test set instruction: ";
    string new_i = "LD F0, 0";
    sim.get_all_instructions(instrs);
    assert(instrs.size() == 5);
    assert(sim.set_instruction(0, new_i) == -1);
    cout << ".";
    assert(sim.set_instruction(1, new_i) == 0);
    Instruction* instr_p;
    sim.get_instruction(1, instr_p);
    assert(instr_p -> get_type() == LD);
    cout << ".";
    new_i = "LD F0, F1";        // error instruction
    assert(sim.set_instruction(2, new_i) == -1);
    sim.get_instruction(2, instr_p);
    assert(instr_p -> get_type() == SUBD);
    cout << "PASSED!" << endl;
}

void test_steps(Simulator& sim) {
    cout << "Test Steps: ";
    // F0 should be 1.5 now
    float res;
    sim.get_register("F0", res);
    assert(res == 1.5);
    // remove redundant instructions
    sim.remove_instruction(4);
    sim.remove_instruction(3);
    sim.remove_instruction(2);
    sim.remove_instruction(1);
    // check current save buffer
    sim.get_save_buffers(resvs);
    assert(resvs["SAVE_0"] -> is_buzy());
    assert(resvs["SAVE_0"] -> get_op() == ST);
    assert(resvs["SAVE_0"] -> get_vj() == 1.5);
    assert(resvs["SAVE_0"] -> get_a() == 234);
    cout << ".";

    // routine A:
    // ST F0, 234
    // LD F1, 234
    // F1 and F0 should both be 1.5
    sim.add_instruction("LD F1, 234");
    assert(sim.step(5) == 0);
    sim.get_register("F0", res);
    assert(fabs(res - 1.5) < 1e-6);
    sim.get_register("F1", res);
    assert(fabs(res - 1.5) < 1e-6);
    cout << ".";

    assert(sim.get_total_cycles() == 6);

    // routine B:
    // ADDD F2, F0, F1
    // MULD F0, F2, F0
    // DIVD F3, F1, F1
    // ST F0, 1
    // LD F0, 234
    // res: F0 = 1.5, F1 = 1.5, F2 = 3.0, F3 = 1.0, mem[1] = 4.5
    sim.add_instruction("ADDD F2, F0, F1");
    sim.add_instruction("MULD F0, F2, F0");
    sim.add_instruction("DIVD F3, F1, F1");
    sim.add_instruction("ST F0, 1");
    sim.add_instruction("LD F0, 234");
    assert(sim.step() == 0);        // ADDD is issued
    sim.get_add_reservations(resvs);
    assert(resvs["ADD_0"] -> is_buzy());
    assert(resvs["ADD_0"] -> get_op() == ADDD);
    assert(resvs["ADD_0"] -> get_vj() == 1.5);
    assert(resvs["ADD_0"] -> get_vk() == 1.5);
    assert(resvs["ADD_0"] -> get_qj() == RESERVATION_NONE);
    assert(resvs["ADD_0"] -> get_qk() == RESERVATION_NONE);
    assert(resvs["ADD_0"] -> get_instruction() -> get_status() == ISSUED);
    assert(resvs["ADD_0"] -> get_rest_cycles() == 0);
    string q;
    sim.get_register_q("F2", q);
    assert(q == "ADD_0");
    cout << ".";
    assert(sim.step() == 0);        // ADDD executes, MULD issued
    sim.get_all_reservations(resvs);
    assert(resvs["MUL_0"] -> is_buzy());
    assert(resvs["MUL_0"] -> get_op() == MULD);
    assert(resvs["MUL_0"] -> get_vj() == 0);
    assert(resvs["MUL_0"] -> get_vk() == 1.5);
    assert(resvs["MUL_0"] -> get_qj() == "ADD_0");
    assert(resvs["MUL_0"] -> get_qk() == RESERVATION_NONE);
    assert(resvs["ADD_0"] -> get_rest_cycles() == 1);
    assert(resvs["MUL_0"] -> get_instruction() -> get_status() == ISSUED);
    sim.get_register_q("F0", q);
    assert(q == "MUL_0");
    cout << ".";
    assert(sim.step() == 0);        // ADDD finishes, MULD waits ADDD wb, DIVD issued
    sim.get_all_reservations(resvs);
    assert(resvs["ADD_0"] -> get_instruction() -> get_status() == EXECUTED);
    assert(resvs["MUL_0"] -> get_instruction() -> get_status() == ISSUED);
    assert(resvs["MUL_1"] -> get_instruction() -> get_status() == ISSUED);
    assert(sim.step() == 0);        // ADDD wb, MULD waits, DIVD executes, ST issued
    sim.get_all_reservations(resvs);
    assert(resvs["ADD_0"] -> is_buzy() == false);
    assert(resvs["MUL_0"] -> get_vj() == 3.0);
    assert(resvs["MUL_0"] -> get_qj() == RESERVATION_NONE);
    assert(resvs["ADD_0"] -> get_instruction() -> get_status() == DONE);
    assert(resvs["MUL_0"] -> get_instruction() -> get_status() == ISSUED);
    assert(resvs["MUL_1"] -> get_instruction() -> get_status() == EXECUTING);
    assert(resvs["MUL_0"] -> get_rest_cycles() == 0);
    assert(resvs["MUL_1"] -> get_rest_cycles() == 39);
    assert(resvs["SAVE_0"] -> is_buzy());
    assert(resvs["SAVE_0"] -> get_op() == ST);
    assert(resvs["SAVE_0"] -> get_vj() == 0);
    assert(resvs["SAVE_0"] -> get_qj() == "MUL_0");
    assert(resvs["SAVE_0"] -> get_a() == 1);
    cout << ".";
    assert(sim.step() == 0);        // MULD executes, ST waits, LD issued
    sim.get_mul_reservations(resvs);
    assert(resvs["MUL_0"] -> get_instruction() -> get_status() == EXECUTING);
    assert(resvs["MUL_0"] -> get_rest_cycles() == 9);
    assert(resvs["MUL_1"] -> get_rest_cycles() == 38);
    sim.get_register_q("F0", q);
    assert(q == "LOAD_0");
    cout << ".";
    assert(sim.step(39) == 0);      // all done
    sim.get_register("F0", res);
    assert(fabs(res - 1.5) < 1e-6);
    sim.get_register("F1", res);
    assert(fabs(res - 1.5) < 1e-6);
    sim.get_register("F2", res);
    assert(fabs(res - 3.0) < 1e-6);
    sim.get_register("F3", res);
    assert(fabs(res - 1.0) < 1e-6);
    sim.get_memory(1, res);
    assert(fabs(res - 4.5) < 1e-6);
    cout << ".";

    assert(sim.get_total_cycles() == 50);

    // routine C:
    // ADDD F3, F3, F3
    // ADDD F3, F3, F3
    // ADDD F3, F3, F3
    // ADDD F3, F3, F3
    // res: F3 = 16.0
    sim.add_instruction("ADDD F3, F3, F3");
    sim.add_instruction("ADDD F3, F3, F3");
    sim.add_instruction("ADDD F3, F3, F3");
    sim.add_instruction("ADDD F3, F3, F3");
    assert(sim.step(13) == 0);
    sim.get_register("F3", res);
    assert(fabs(res - 16.0) < 1e-6);

    sim.get_all_instructions(instrs);
    for (Instruction* p: instrs)
        assert(p -> get_status() == DONE);
    cout << "PASSED!" << endl;
}

void test_reset(Simulator& sim) {
    cout << "Test clear oper: ";
    assert(sim.reset() == 0);
    clear_test(sim);
    cout << "PASSED!" << endl;
}