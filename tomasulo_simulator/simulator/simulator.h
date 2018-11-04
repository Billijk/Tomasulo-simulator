#pragma once

#include "kernel.h"
#include <string>
#include <vector>
#include <map>

class Simulator {
    CPUKernel* kernel;
    int total_cycles;

public:
    Simulator(int register_num = 10, int mem_size = 4096, 
        int add_reserv_num = 3, int mul_reserv_num = 2,
        int add_pipeline_seg = 2, int mul_pipeline_seg = 5, int div_pipeline_seg = 1,
        int mul_seg_cycle = 2, int div_seg_cycle = 40,
        int load_buffer_size = 3, int save_buffer_size = 3);
    ~Simulator();

    // getters
    int get_all_instructions(std::vector<Instruction*>& res);
    int get_waiting_instructions(std::vector<Instruction*>& res);
    int get_instruction(unsigned int addr, Instruction*& res);

    int get_used_memory(std::map<unsigned int, float>& res);
    int get_memory(unsigned int addr, float& res);

    int get_register_names(std::vector<std::string>& res);
    int get_all_registers(std::map<std::string, float>& res);           // name -> value
    int get_register(std::string name, float& res);
    int get_all_register_qs(std::map<std::string, std::string>& res);
    int get_register_q(std::string name, std::string& res);

    int get_all_reservations(std::map<std::string, Reservation*>& res);  // name -> Reservation
    int get_load_buffers(std::map<std::string, Reservation*>& res);
    int get_save_buffers(std::map<std::string, Reservation*>& res);
    int get_add_reservations(std::map<std::string, Reservation*>& res);
    int get_mul_reservations(std::map<std::string, Reservation*>& res);

    int get_pc() { return kernel -> get_pc(); }
    int get_total_cycles() { return total_cycles; }

    // run cycles
    int step();
    int step(unsigned cycle);

    // modify instructions
    int add_instruction(std::string instr);
    int remove_instruction(unsigned int addr);
    int set_instruction(unsigned int addr, std::string instr);

    // modify memory and registers
    int set_memory(unsigned int addr, float v);
    int set_register(std::string name, float v);

    int reset();

};