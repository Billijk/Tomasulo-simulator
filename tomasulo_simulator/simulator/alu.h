#pragma once

#include "cdb.h"
#include "registers.h"
#include "reservation.h"
#include "instructions.h"
#include <vector>
#include <string>
#include <algorithm>

class ALU {
protected:
    std::vector<ALUReservation> reservations;
    std::vector<ALUReservation*> current_reservation;
    std::vector<std::pair<ALUReservation*, float>> results;
    CDB* cdb;

    // do the write back operation
    int do_write_back();

public:
    ALU(std::string reserv_name_prefix, int num_reservations, int num_pipeline_seg, CDB* cdb);
    virtual ~ALU();

    int get_reservations(std::vector<Reservation*>& res);

    bool is_reservations_buzy();
    int do_alu(Instruction* instr, Register* src1, Register* src2, Register* dst);
    virtual int do_cycle() = 0;

    int clear();
};

class Adder : public ALU {
public:
    Adder(std::string reserv_name_prefix, int num_reservations, int num_pipeline_seg, CDB* cdb);
    ~Adder();
    int do_cycle();
};

class Multiplier : public ALU {
    std::vector<int> rest_cycles;
    int mul_seg_cycle, div_seg_cycle;
    int num_div_pipeline_seg, num_mul_pipeline_seg;
public:
    Multiplier(std::string reserv_name_prefix, int num_reservations, 
        int num_mul_pipeline_seg, int num_div_pipeline_seg,
        int mul_seg_cycle, int div_seg_cycle, CDB* cdb);
    ~Multiplier();
    int do_cycle();
};