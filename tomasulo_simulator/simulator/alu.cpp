#include "alu.h"
#include <iostream>
#include <cstdlib>

using namespace std;

ALU::ALU(string reserv_name_prefix, int num_reservations, int num_pipeline_seg, CDB* cdb) {
    this -> cdb = cdb;
    for (int i = 0; i < num_reservations; ++ i)
        reservations.emplace_back(reserv_name_prefix + to_string(i), this);
    for (int i = 0; i < num_pipeline_seg; ++ i)
        current_reservation.push_back(NULL);
}

ALU::~ALU() {}

int ALU::get_reservations(vector<Reservation*>& res) {
    res.clear();
    //for (int i = 0; i < reservations.size(); ++ i)
    //    res.push_back(&reservations[i]);
    for (auto& r: reservations)
        res.push_back(&r);
    return 0;
}

bool ALU::is_reservations_buzy() {
    for (const auto& r: reservations)
        if (!(r.is_buzy())) return false;
    return true;
}

int ALU::do_alu(Instruction* instr, Register* src1, Register* src2, Register* dst) {
    cout << "do_alu(): " <<  instr -> get_instruction() << endl;
    // find an empty reservation
    ALUReservation* selected;
    for (auto& reservation: reservations) {
        if (!reservation.is_buzy()) {
            selected = &reservation;
            break;
        }
    }
    // handle source operand
    float vj, vk;
    string qj, qk;
    if (src1 -> get_reservation() == RESERVATION_NONE) {
        // operand is ready
        vj = src1 -> get_value();
        qj = RESERVATION_NONE;
    } else {
        vj = 0;
        qj = src1 -> get_reservation();
    }
    if (src2 -> get_reservation() == RESERVATION_NONE) {
        // operand is ready
        vk = src2 -> get_value();
        qk = RESERVATION_NONE;
    } else {
        vk = 0;
        qk = src2 -> get_reservation();
    }
    dst -> set_reservation(selected -> get_name());
    selected -> reserve(instr -> get_type(), vj, vk, qj, qk, instr);
    return 0;
}

int ALU::do_write_back() {
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

int ALU::clear() {
    int ret = 0;
    for (auto& r: reservations) 
        if ((ret = r.clear()) != 0) return ret;
    for (size_t i = 0; i < current_reservation.size(); ++ i)
        current_reservation[i] = NULL;
    return 0;
}

Adder::Adder(string reserv_name_prefix, int num_reservations, int num_pipeline_seg, CDB* cdb):
    ALU(reserv_name_prefix, num_reservations, num_pipeline_seg, cdb) {
}

Adder::~Adder() {}

int Adder::do_cycle() {
    int ret;
    if ((ret = do_write_back()) != 0) return ret;
    if (current_reservation[0] == NULL) {
        // get all ready reservations and randomly choose one
        vector<ALUReservation*> ready_reservations;
        for (auto& reservation: reservations)
            if (reservation.ready() && reservation.get_rest_cycles() == 0)
                ready_reservations.push_back(&reservation);
        if (ready_reservations.size() > 0) {
            current_reservation[0] = ready_reservations[rand() % ready_reservations.size()];
            current_reservation[0] -> get_instruction() -> next_status();
            current_reservation[0] -> set_rest_cycles(Instruction::needed_cycle(current_reservation[0] -> get_op()));
        }
    }
    ALUReservation* final_one = current_reservation[current_reservation.size() - 1];
    if (final_one != NULL) {
        // finish executing
        final_one -> get_instruction() -> next_status();
        float src1 = final_one -> get_vj();
        float src2 = final_one -> get_vk();
        if (final_one -> get_op() == ADDD)
            results.push_back(make_pair(final_one, src1 + src2));
        else if (final_one -> get_op() == SUBD)
            results.push_back(make_pair(final_one, src1 - src2));
        else return -1;
    }
    for (size_t i = current_reservation.size() - 1; i; -- i) {
        current_reservation[i] = current_reservation[i - 1];
        if (current_reservation[i] != NULL) current_reservation[i] -> decrease_cycle();
        current_reservation[i - 1] = NULL;
    }
    return 0;
}

Multiplier::Multiplier(string reserv_name_prefix, int num_reservations, 
        int num_mul_pipeline_seg, int num_div_pipeline_seg,
        int mul_seg_cycle, int div_seg_cycle, CDB* cdb) :
        ALU(reserv_name_prefix, num_reservations, num_mul_pipeline_seg + num_div_pipeline_seg, cdb) {
    this -> num_mul_pipeline_seg = num_mul_pipeline_seg;
    this -> num_div_pipeline_seg = num_div_pipeline_seg;
    this -> mul_seg_cycle = mul_seg_cycle;
    this -> div_seg_cycle = div_seg_cycle;
    for (size_t i = 0; i < current_reservation.size(); ++ i)
        rest_cycles.push_back(0);
}

Multiplier::~Multiplier() {}

int Multiplier::do_cycle() {
    int ret;
    if ((ret = do_write_back()) != 0) return ret;
    // separately handle MUL and DIV
    for (size_t i = 0; i < 2; ++ i) {
        InstrType op_type = (i == 0) ? MULD : DIVD;
        size_t reserv_start = (i == 0) ? 0 : num_mul_pipeline_seg;
        size_t reserv_end = (i == 0) ? num_mul_pipeline_seg : current_reservation.size();
        int num_cycles = (i == 0) ? mul_seg_cycle : div_seg_cycle;
        if (current_reservation[reserv_start] == NULL) {
            // get all ready reservations for MULD and randomly choose one
            vector<ALUReservation*> ready_reservations;
            for (auto& reservation: reservations)
                if (reservation.ready() && reservation.get_op() == op_type && 
                        reservation.get_rest_cycles() == 0)
                    ready_reservations.push_back(&reservation);
            if (ready_reservations.size() > 0) {
                current_reservation[reserv_start] = ready_reservations[rand() % ready_reservations.size()];
                rest_cycles[reserv_start] = num_cycles;
                current_reservation[reserv_start] -> get_instruction() -> next_status();
                current_reservation[reserv_start] -> set_rest_cycles(Instruction::needed_cycle(op_type));
            }
        }
        for (size_t j = reserv_start; j < reserv_end; ++ j)
            if (rest_cycles[j] > 0) {
                current_reservation[j] -> decrease_cycle();
                rest_cycles[j] --;
            }
        if (current_reservation[reserv_end - 1] != NULL && rest_cycles[reserv_end - 1] <= 0) {
            // finish executing
            ALUReservation* final_one = current_reservation[reserv_end - 1];
            final_one -> get_instruction() -> next_status();
            float src1 = final_one -> get_vj();
            float src2 = final_one -> get_vk();
            if (op_type == MULD)
                results.push_back(make_pair(final_one, src1 * src2));
            else if (op_type == DIVD)
                results.push_back(make_pair(final_one, src1 / src2));
            else return -1;
            current_reservation[reserv_end - 1] = NULL;
        }
        for (size_t j = reserv_end - 1; j > reserv_start; -- j) {
            if (current_reservation[j] == NULL && rest_cycles[j - 1] <= 0) {
                current_reservation[j] = current_reservation[j - 1];
                if (current_reservation[j] != NULL) rest_cycles[j] = num_cycles;
                current_reservation[j - 1] = NULL;
            }
        }
    }
    return 0;
}