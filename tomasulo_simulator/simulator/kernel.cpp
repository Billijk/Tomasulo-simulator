#include "kernel.h"
#include <algorithm>
#include <iostream>

using namespace std;

#define REGISTER_RESERVATIONS(unit, getter) {   \
    unit -> getter(reserv_pointers);            \
    for (Reservation* p: reserv_pointers)       \
        reservations[p -> get_name()] = p;      \
}                                               \

CPUKernel::CPUKernel(int register_num, int mem_size, 
        int add_reserv_num, int mul_reserv_num,
        int add_pipeline_seg, int mul_pipeline_seg, int div_pipeline_seg,
        int mul_seg_cycle, int div_seg_cycle,
        int load_buffer_size, int save_buffer_size) {
    cdb = new CDB(this);
    memory = new Memory(mem_size, load_buffer_size, save_buffer_size, cdb);
    registers = new RegisterManager(register_num);
    adder = new Adder("ADD_", add_reserv_num, add_pipeline_seg, cdb);
    multiplier = new Multiplier("MUL_", mul_reserv_num, mul_pipeline_seg, 
        div_pipeline_seg, mul_seg_cycle, div_seg_cycle, cdb);
    pc = 0;

    // register reservations
    vector<Reservation*> reserv_pointers;
    REGISTER_RESERVATIONS(memory, get_load_buffers);
    REGISTER_RESERVATIONS(memory, get_save_buffers);
    REGISTER_RESERVATIONS(adder, get_reservations);
    REGISTER_RESERVATIONS(multiplier, get_reservations);

    max_mem_size = mem_size;
    vector<string> register_names_vec;
    registers -> get_register_names(register_names_vec);
    for (string name: register_names_vec)
        register_names.insert(name);
}

CPUKernel::~CPUKernel() {
    delete cdb;
    delete memory;
    delete registers;
    delete adder;
    delete multiplier;
}

void CPUKernel::find_reservation_by_prefix(const string& prefix, map<string, Reservation*>& res) {
    auto i = reservations.lower_bound(prefix);
    while (i != reservations.end()) {
        const string& key = i->first;
        if (key.compare(0, prefix.size(), prefix) == 0) // Really a prefix?
            res[key] = i -> second;
        else return;
        i ++;
    }
    return;
}

int CPUKernel::send_to_reservations() {
    Instruction* current = instr_queue[pc];
    Register *src1 = registers -> get_register_pointer(current -> get_src1()),
         *src2 = registers -> get_register_pointer(current -> get_src2()),
         *dst = registers -> get_register_pointer(current -> get_dst());
    switch (current -> get_type()) {
        case ADDD:
        case SUBD:
            if (adder -> is_reservations_buzy()) return -1;
            else adder -> do_alu(static_cast<ArithmeticInstruction*>(current), src1, src2, dst);
            break;
        case MULD:
        case DIVD:
            if (multiplier -> is_reservations_buzy()) return -1;
            else multiplier -> do_alu(static_cast<ArithmeticInstruction*>(current), src1, src2, dst);
            break;
        case LD:
            if (memory -> is_load_buffers_buzy()) return -1;
            else memory -> do_load(static_cast<MemoryInstruction*>(current), dst);
            break;
        case ST:
            if (memory -> is_save_buffers_buzy()) return -1;
            else memory -> do_save(static_cast<MemoryInstruction*>(current), src1);
            break;
        default: return -1;
    }
    current -> next_status();
    return 0;
}

int CPUKernel::step() {
    int ret = 0;
    if ((ret = adder -> do_cycle()) != 0) return ret;
    if ((ret = multiplier -> do_cycle()) != 0) return ret;
    if ((ret = memory -> do_cycle()) != 0) return ret;
    if ((ret = cdb -> notify_reservations()) != 0) return ret;
    if (pc != instr_queue.size())
        if (send_to_reservations() == 0) pc ++;
    return ret;
}

int CPUKernel::get_instructions(vector<Instruction*>& res, int start, int end) {
    res.clear();
    if (start < 0 || end > (int) instr_queue.size() || (end != -1 && start >= end))
        return -1;
    auto start_it = instr_queue.begin() + start;
    auto end_it = end == -1 ? instr_queue.end() : instr_queue.begin() + end;
    res.insert(res.begin(), start_it, end_it);
    return 0;
}

int CPUKernel::get_used_memory_addrs(vector<unsigned int>& res) {
    return memory -> get_used_addrs(res);
}

int CPUKernel::get_memory(vector<unsigned int> addrs, vector<float>& res) {
    return memory -> get_memory(addrs, res);
}

int CPUKernel::get_register_names(vector<string>& res) {
    return registers -> get_register_names(res);
}

int CPUKernel::get_registers(vector<string> names, vector<float>& res) {
    return registers -> get_registers(names, res);
}

int CPUKernel::get_register_qs(vector<string> names, vector<string>& res) {
    return registers -> get_register_qs(names, res);
}

int CPUKernel::get_all_reservations(map<string, Reservation*>& res) {
    res = reservations;
    return 0;
}

int CPUKernel::get_load_buffers(map<string, Reservation*>& res) {
    res.clear();
    find_reservation_by_prefix(string("LOAD"), res); 
    return 0;
}

int CPUKernel::get_save_buffers(map<string, Reservation*>& res) {
    res.clear();
    find_reservation_by_prefix(string("SAVE"), res); 
    return 0;
}

int CPUKernel::get_add_reservations(map<string, Reservation*>& res) {
    res.clear();
    find_reservation_by_prefix(string("ADD"), res); 
    return 0;
}

int CPUKernel::get_mul_reservations(map<string, Reservation*>& res) {
    res.clear();
    find_reservation_by_prefix(string("MUL"), res); 
    return 0;
}

int CPUKernel::get_reservation(string name, Reservation*& res) {
    if (reservations.find(name) == reservations.end())
        return -1;
    else res = reservations[name];
    return 0;
}

int CPUKernel::add_instruction(string instr) {
    Instruction* instr_p = Instruction::parse(instr, register_names, max_mem_size);
    if (instr_p -> get_type() == ERROR) {
        delete instr_p;
        return -1;
    }
    else instr_queue.push_back(instr_p);
    return 0;
}

int CPUKernel::remove_instruction(unsigned int addr) {
    if (addr >= instr_queue.size()) return -1;
    if (addr < pc) return -1;
    delete instr_queue[addr];
    instr_queue.erase(instr_queue.begin() + addr);
    return 0;
}

int CPUKernel::set_instruction(unsigned int addr, string instr) {
    if (addr > instr_queue.size()) return -1;
    if (addr < pc) return -1;
    Instruction* instr_p = Instruction::parse(instr, register_names, max_mem_size);
    if (instr_p -> get_type() == ERROR) {
        delete instr_p;
        return -1;
    }
    instr_queue[addr] = instr_p;
    return 0;
}

int CPUKernel::set_memory(unsigned int addr, float v) {
    return memory -> set_memory(addr, v);
}

int CPUKernel::set_register(string name, float v) {
    return registers -> set_value(name, v);
}

int CPUKernel::reset() {
    int ret = 0;
    pc = 0;
    for (auto& instr_p: instr_queue) delete instr_p;
    instr_queue.clear();
    if ((ret = memory -> clear()) != 0) return ret;
    if ((ret = registers -> clear()) != 0) return ret;
    if ((ret = adder -> clear()) != 0) return ret;
    if ((ret = multiplier -> clear()) != 0) return ret;
    return 0;
}
