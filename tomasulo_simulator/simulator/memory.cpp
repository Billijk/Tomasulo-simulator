#include "memory.h"
#include "registers.h"
#include <algorithm>
#include <iterator>
#include <iostream>
#include <cstdlib>

using namespace std;

#define GET_BUFFERS(buffers, res) { \
    for (auto& r: buffers)          \
        res.push_back(&r);          \
}
#define IS_BUFFER_EMPTY(buffers, res) { \
    res = true;                         \
    for (const auto& r: buffers)        \
        if (!(r.is_buzy())) {        \
            res = false;                \
            break;                      \
        }                               \
}

Memory::Memory(int size, int load_buffer_size, int save_buffer_size, CDB* _cdb) {
    // set up memory
    for (int i = 0; i < size; ++ i) values.push_back(0.0f);
    // set up buffers
    for (int i = 0; i < load_buffer_size; ++ i)
        load_buffers.emplace_back("LOAD_" + to_string(i), this);
    for (int i = 0; i < save_buffer_size; ++ i)
        save_buffers.emplace_back("SAVE_" + to_string(i), this);
    cdb = _cdb;
    current_buffer = NULL;
}

Memory::~Memory() {}

int Memory::get_used_addrs(vector<unsigned int>& res) {
    res.clear();
    copy(used_addrs.begin(), used_addrs.end(), back_inserter(res));
    return 0;
}

int Memory::get_memory(vector<unsigned int> addrs, vector<float>& res) {
    res.clear();
    for (unsigned int a: addrs) {
        if (a > values.size()) {
            res.clear();
            return -1;
        } else {
            res.push_back(values[a]);
        }
    }
    return 0;
}

int Memory::set_memory(unsigned int addr, float value) {
    if (addr >= values.size()) return -1;
    used_addrs.insert(addr);
    values[addr] = value;
    return 0;
}

int Memory::get_save_buffers(vector<Reservation*>& res) {
    res.clear();
    GET_BUFFERS(save_buffers, res);
    return 0;
}

int Memory::get_load_buffers(vector<Reservation*>& res) {
    res.clear();
    GET_BUFFERS(load_buffers, res);
    return 0;
}

bool Memory::is_save_buffers_buzy() {
    bool res;
    IS_BUFFER_EMPTY(save_buffers, res);
    return res;
}

bool Memory::is_load_buffers_buzy() {
    bool res;
    IS_BUFFER_EMPTY(load_buffers, res);
    return res;
}

int Memory::do_write_back() {
    int ret;
    for (const auto& item: results) {
        item.first -> get_instruction() -> next_status();
        item.first -> finish_work();
        if ((ret = cdb -> write_back(item.first -> get_name(), item.second)) != 0)
            return ret;
    }
    results.clear();
    return 0;
}

int Memory::do_save(MemoryInstruction* instr, Register* src) {
    cout << "do_save(): " + instr -> get_instruction() << endl;
    // find an empty buffer
    MemoryBuffer* selected_buffer;
    for (auto& buffer: save_buffers) {
        if (!buffer.is_buzy()) {
            selected_buffer = &buffer;
            break;
        }
    }
    // handle source operand
    float vj;
    string qj;
    if (src -> get_reservation() == RESERVATION_NONE) {
        // operand is ready
        vj = src -> get_value();
        qj = RESERVATION_NONE;
    } else {
        vj = 0;
        qj = src -> get_reservation();
    }
    return selected_buffer -> reserve(ST, vj, instr -> get_addr(), qj, instr);
}

int Memory::do_load(MemoryInstruction* instr, Register* dst) {
    cout << "do_load(): " + instr -> get_instruction() << endl;
    // find an empty buffer
    MemoryBuffer* selected_buffer;
    for (auto& buffer: load_buffers) {
        if (!buffer.is_buzy()) {
            selected_buffer = &buffer;
            break;
        }
    }
    // handle target operand
    int ret;
    if ((ret = dst -> set_reservation(selected_buffer -> get_name())) != 0) return ret;
    if ((ret = selected_buffer -> reserve(LD, 0, instr -> get_addr(), RESERVATION_NONE, instr)) != 0)
        return ret;
    return ret;
}

int Memory::do_cycle() {
    int ret;
    if ((ret = do_write_back()) != 0) return ret;
    if (current_buffer == NULL) {
        // get all ready buffers and randomly choose one
        vector<MemoryBuffer*> ready_buffers;
        for (auto& buffer: load_buffers)
            if (buffer.ready() && buffer.get_rest_cycles() == 0) 
                ready_buffers.push_back(&buffer);
        for (auto& buffer: save_buffers)
            if (buffer.ready() && buffer.get_rest_cycles() == 0)
                ready_buffers.push_back(&buffer);
        if (ready_buffers.size() > 0) {
            current_buffer = ready_buffers[rand() % ready_buffers.size()];
            current_buffer -> set_rest_cycles(Instruction::needed_cycle(current_buffer -> get_op()));
            current_buffer -> get_instruction() -> next_status();
        }
    }
    if (current_buffer != NULL) {
        int rest_cycles = current_buffer -> decrease_cycle();
        if (!rest_cycles) {
            // finish executing
            current_buffer -> get_instruction() -> next_status();   // update status
            if (current_buffer -> get_op() == LD) {
                results.push_back(make_pair(current_buffer, values[current_buffer -> get_a()]));
            } else {
                current_buffer -> finish_work();
                current_buffer -> get_instruction() -> next_status();   // no WB cycle
                used_addrs.insert(current_buffer -> get_a());
                values[current_buffer -> get_a()] = current_buffer -> get_vj();
            }
            current_buffer = NULL;
        }
    }
    return 0;
}

int Memory::clear() {
    current_buffer = NULL;
    int ret = 0;
    for (auto& r: load_buffers) 
        if ((ret = r.clear()) != 0) return ret;
    for (auto& r: save_buffers) 
        if ((ret = r.clear()) != 0) return ret;
    for (size_t i = 0; i < values.size(); ++ i)
        values[i] = 0.0f;
    used_addrs.clear();
    return 0;
}