#pragma once

#include "cdb.h"
#include "registers.h"
#include "reservation.h"
#include "instructions.h"
#include <set>
#include <vector>
#include <algorithm>

class Memory {
    std::set<unsigned int> used_addrs;
    std::vector<float> values;
    std::vector<MemoryBuffer> save_buffers;
    std::vector<MemoryBuffer> load_buffers;
    std::vector<std::pair<MemoryBuffer*, float>> results;
    MemoryBuffer* current_buffer;
    CDB* cdb;

    int do_write_back();
    
public:
    Memory(int size, int load_buffer_size, int save_buffer_size, CDB* cdb);
    ~Memory();

    int get_used_addrs(std::vector<unsigned int>& res);
    int get_memory(std::vector<unsigned int> addrs, std::vector<float>& res);
    int set_memory(unsigned int addr, float value);

    int get_save_buffers(std::vector<Reservation*>& res);
    int get_load_buffers(std::vector<Reservation*>& res);

    bool is_save_buffers_buzy();
    bool is_load_buffers_buzy();
    int do_save(MemoryInstruction* instr, Register* dst);
    int do_load(MemoryInstruction* instr, Register* src);
    int do_cycle();

    int clear();
};