#include "simulator.h"

using namespace std;

Simulator::Simulator(int register_num, int mem_size, 
        int add_reserv_num, int mul_reserv_num,
        int add_pipeline_seg, int mul_pipeline_seg, int div_pipeline_seg,
        int mul_seg_cycle, int div_seg_cycle,
        int load_buffer_size, int save_buffer_size) {
    total_cycles = 0;
    kernel = new CPUKernel(register_num, mem_size, 
        add_reserv_num, mul_reserv_num,
        add_pipeline_seg, mul_pipeline_seg, div_pipeline_seg,
        mul_seg_cycle, div_seg_cycle,
        load_buffer_size, save_buffer_size);
}

Simulator::~Simulator() {
    delete kernel;
}

int Simulator::get_all_instructions(vector<Instruction*>& res) {
    return kernel -> get_instructions(res);
}

int Simulator::get_waiting_instructions(vector<Instruction*>& res) {
    int pc = kernel -> get_pc();
    return kernel -> get_instructions(res, pc);
}

int Simulator::get_instruction(unsigned int addr, Instruction*& res) {
    vector<Instruction*> instrs;
    int ret;
    if ((ret = kernel -> get_instructions(instrs, addr, addr + 1)) != 0) return ret;
    res = instrs[0];
    return ret;
}

int Simulator::get_used_memory(map<unsigned int, float>& res) {
    res.clear();
    int ret = 0;
    vector<unsigned int> addrs;
    ret = kernel -> get_used_memory_addrs(addrs);
    if (ret != 0) return ret;
    vector<float> values;
    ret = kernel -> get_memory(addrs, values);
    if (ret != 0) return ret;
    for (size_t i = 0; i < addrs.size(); ++ i) {
        res[addrs[i]] = values[i];
    }
    return ret;
}

int Simulator::get_memory(unsigned int addr, float& res) {
    vector<unsigned int> addrs;
    vector<float> values;
    addrs.push_back(addr);
    int ret = kernel -> get_memory(addrs, values);
    res = values[0];
    return ret;
}

int Simulator::get_register_names(vector<string>& res) {
    return kernel -> get_register_names(res);
}

int Simulator::get_all_registers(map<string, float>& res) {
    res.clear();
    int ret = 0;
    vector<string> names;
    ret = kernel -> get_register_names(names);
    if (ret != 0) return ret;
    vector<float> values;
    ret = kernel -> get_registers(names, values);
    if (ret != 0) return ret;
    for (size_t i = 0; i < names.size(); ++ i) {
        res[names[i]] = values[i];
    }
    return ret;
}

int Simulator::get_register(string name, float& res) {
    vector<string> names;
    vector<float> values;
    names.push_back(name);
    int ret = kernel -> get_registers(names, values);
    res = values[0];
    return ret;
}

int Simulator::get_all_register_qs(map<string, string>& res) {
    res.clear();
    int ret = 0;
    vector<string> names;
    ret = kernel -> get_register_names(names);
    if (ret != 0) return ret;
    vector<string> values;
    ret = kernel -> get_register_qs(names, values);
    if (ret != 0) return ret;
    for (size_t i = 0; i < names.size(); ++ i) {
        res[names[i]] = values[i];
    }
    return ret;
}

int Simulator::get_register_q(string name, string& res) {
    vector<string> names;
    vector<string> values;
    names.push_back(name);
    int ret = kernel -> get_register_qs(names, values);
    res = values[0];
    return ret;
}

int Simulator::get_all_reservations(map<string, Reservation*>& res) {
    return kernel -> get_all_reservations(res);
}

int Simulator::get_load_buffers(map<string, Reservation*>& res) {
    return kernel -> get_load_buffers(res);
}

int Simulator::get_save_buffers(map<string, Reservation*>& res) {
    return kernel -> get_save_buffers(res);
}

int Simulator::get_add_reservations(map<string, Reservation*>& res) {
    return kernel -> get_add_reservations(res);
}

int Simulator::get_mul_reservations(map<string, Reservation*>& res) {
    return kernel -> get_mul_reservations(res);
}

int Simulator::step() {
    return step(1);
}

int Simulator::step(unsigned cycle) {
    int ret = 0;
    for (unsigned i = 0; i < cycle; ++ i) {
        if ((ret = kernel -> step()) != 0)
            return ret;
        total_cycles ++;
    }
    return ret;
}

int Simulator::add_instruction(string instr) {
    return kernel -> add_instruction(instr);
}

int Simulator::remove_instruction(unsigned int addr) {
    return kernel -> remove_instruction(addr);
}

int Simulator::set_instruction(unsigned int addr, string instr) {
    return kernel -> set_instruction(addr, instr);
}

int Simulator::set_memory(unsigned int addr, float v) {
    return kernel -> set_memory(addr, v);
}

int Simulator::set_register(string name, float v) {
    return kernel -> set_register(name, v);
}

int Simulator::reset() {
    total_cycles = 0;
    return kernel -> reset();
}