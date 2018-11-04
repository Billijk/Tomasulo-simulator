#pragma once

#include "reservation.h"
#include "instructions.h"
#include "registers.h"
#include "alu.h"
#include "cdb.h"
#include "memory.h"
#include <vector>
#include <set>
#include <map>

class CPUKernel {
    std::vector<Instruction*> instr_queue;    // instruction queue
    Memory* memory;                          // memory
    RegisterManager* registers;                 // registers
    ALU *adder, *multiplier;                 // float point adder and multiplier
    CDB* cdb;                                // common data bus
    unsigned int pc;

    std::map<std::string, Reservation*> reservations;    // reservation lookup table
    std::set<std::string> register_names;
    int max_mem_size;

    void find_reservation_by_prefix(const std::string& prefix,
         std::map<std::string, Reservation*>& res);
    
    int send_to_reservations();
    
    friend class CDB;
public:
    CPUKernel(int register_num, int mem_size,
        int add_reserv_num, int mul_reserv_num,
        int add_pipeline_seg, int mul_pipeline_seg, int div_pipeline_seg,
        int mul_seg_cycle, int div_seg_cycle,
        int load_buffer_size, int save_buffer_size);
    ~CPUKernel();

    // the pipeline is simply defined as IF -> EX -> WB
    // and EX segment is also a pipeline in which different function part has different segments
    int step();
    
    int get_instructions(std::vector<Instruction*>& res, int start = 0, int end = -1);
    int get_used_memory_addrs(std::vector<unsigned int>& res);
    int get_memory(std::vector<unsigned int> addrs, std::vector<float>& res);
    int get_register_names(std::vector<std::string>& res);
    int get_registers(std::vector<std::string> names, std::vector<float>& res);
    int get_register_qs(std::vector<std::string> names, std::vector<std::string>& res);

    int get_all_reservations(std::map<std::string, Reservation*>& res);
    int get_load_buffers(std::map<std::string, Reservation*>& res);
    int get_save_buffers(std::map<std::string, Reservation*>& res);
    int get_add_reservations(std::map<std::string, Reservation*>& res);
    int get_mul_reservations(std::map<std::string, Reservation*>& res);
    int get_reservation(std::string name, Reservation*& res);

    int get_pc() { return pc; }

    int add_instruction(std::string instr);
    int remove_instruction(unsigned int addr);
    int set_instruction(unsigned int addr, std::string instr);
    int set_memory(unsigned int addr, float v);
    int set_register(std::string name, float v);

    int reset();

};