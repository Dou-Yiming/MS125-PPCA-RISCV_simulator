#ifndef __pipeline
#define __pipeline

#include "instruction.hpp"

class IF_reg;
class ID_reg;
class EX_reg;
class MEM_reg;

class IF_reg
{
public:
    bool isEmpty;
    unsigned int cur_ins;
    unsigned int cur_pc;
    IF_reg()
    {
        isEmpty = 1;
    }
    void operate_IF()
    {
        isEmpty = 0;
        cur_pc = pc;
        memcpy(&cur_ins, mem + cur_pc, sizeof(unsigned int));
        pc += 4;
        //std::cout<<cur_ins<<std::endl;
    }
};
class ID_reg
{
public:
    bool isEmpty;
    instruction cur_ins;
    unsigned int cur_pc;
    unsigned int val1, val2;

    ID_reg()
    {
        isEmpty = 1;
    }
    void operate_ID(IF_reg &cur_IF)
    {
        cur_pc = cur_IF.cur_pc;
        if (cur_IF.cur_ins == 0x0ff00513)
        {
            isEnd = 1;
            cur_IF.isEmpty = 1;
            return;
        }
        cur_ins.set(cur_IF.cur_ins);

        //stall
        switch (cur_ins.name)
        {
        case JAL:
        case AUIPC:
        case LUI:
            if (used[cur_ins.rd])
                return;
            break;
        case JALR:
        case ADDI:
        case SLTI:
        case SLTIU:
        case XORI:
        case ORI:
        case ANDI:
        case SLLI:
        case SRLI:
        case SRAI:
        case LB:
        case LH:
        case LW:
        case LBU:
        case LHU:
            if (used[cur_ins.rd] || used[cur_ins.rs1])
                return;
            break;
        case SB:
        case SH:
        case SW:
            if (used[cur_ins.rs1] || used[cur_ins.rs2])
                return;
            break;
        default:
            if (used[cur_ins.rd] || used[cur_ins.rs1] || used[cur_ins.rs2])
                return;
            break;
        }

        isEmpty = 0;
        cur_IF.isEmpty = 1;
        if (cur_ins.rs1 == 0)
            val1 = 0;
        if (cur_ins.rs1 == 0)
            val2 = 0;
        /*
        //fowarding
        switch (cur_ins.name)
        {
        case LUI:
        case AUIPC:
        case JAL:
            if (cur_ins.rd)
                target[cur_ins.rd]++;
            break;
        case JALR:  case LB:  case LH:  case LW:  case LBU:
        case LHU:  case ADDI:  case SLTI:  case SLTIU:  case XORI:
        case ORI:  case ANDI:  case SLLI:  case SRLI:  case SRAI:    

        }
*/
        //no need to predict
        if (cur_ins.name == JAL)
            pc = cur_pc + cur_ins.imm;
        if (cur_ins.name == JALR)
            pc = (reg[cur_ins.rs1] + cur_ins.imm) & (-2u);

        //predict
        if (cur_ins.name >= 29 && cur_ins.name <= 34)
        {
            switch (cur_ins.name)
            {
            case BEQ:
                pc = cur_pc + ((reg[cur_ins.rs1] == reg[cur_ins.rs2]) ? cur_ins.imm : 4);
                break;
            case BNE:
                pc = cur_pc + ((reg[cur_ins.rs1] != reg[cur_ins.rs2]) ? cur_ins.imm : 4);
                break;
            case BLT:
                pc = cur_pc + (((int)reg[cur_ins.rs1] < (int)reg[cur_ins.rs2]) ? cur_ins.imm : 4);
                break;
            case BGE:
                pc = cur_pc + (((int)reg[cur_ins.rs1] >= (int)reg[cur_ins.rs2]) ? cur_ins.imm : 4);
                break;
            case BLTU:
                pc = cur_pc + ((reg[cur_ins.rs1] < reg[cur_ins.rs2]) ? cur_ins.imm : 4);
                break;
            case BGEU:
                pc = cur_pc + ((reg[cur_ins.rs1] >= reg[cur_ins.rs2]) ? cur_ins.imm : 4);
                break;
            }
        }
        else
        {
            used[cur_ins.rd] = 1;
            used[0] = 0;
        }
    }
};
class EX_reg
{
public:
    bool isEmpty;
    unsigned int vrd;
    unsigned int cur_pc;
    ins_name iname;
    unsigned int cur_imm;
    unsigned int cur_rs1;
    unsigned int cur_rs2;
    unsigned int cur_rd;
    EX_reg()
    {
        isEmpty = 1;
    }
    void operate_EX(ID_reg &cur_ID, IF_reg &cur_IF)
    {
        isEmpty = 0;
        cur_ID.isEmpty = 1;
        cur_pc = cur_ID.cur_pc;
        iname = cur_ID.cur_ins.name;
        cur_imm = cur_ID.cur_ins.imm;
        cur_rs1 = cur_ID.cur_ins.rs1;
        cur_rs2 = cur_ID.cur_ins.rs2;
        cur_rd = cur_ID.cur_ins.rd;
        switch (cur_ID.cur_ins.name)
        {
        case LUI:
            vrd = cur_ID.cur_ins.imm;
            break;
        case AUIPC:
            vrd += cur_ID.cur_ins.imm;
            break;

        case JAL:
            vrd = cur_pc + 4;
            break;
        case JALR:
            vrd = cur_pc + 4;
            break;

        case BEQ:
            judge_SB(reg[cur_rs1] == reg[cur_rs2], cur_IF);
            break;
        case BNE:
            judge_SB(reg[cur_rs1] != reg[cur_rs2], cur_IF);
            break;
        case BLT:
            judge_SB((int)reg[cur_rs1] < (int)reg[cur_rs2], cur_IF);
            break;
        case BGE:
            judge_SB((int)reg[cur_rs1] >= (int)reg[cur_rs2], cur_IF);
            break;
        case BLTU:
            judge_SB(reg[cur_rs1] < reg[cur_rs2], cur_IF);
            break;
        case BGEU:
            judge_SB(reg[cur_rs1] >= reg[cur_rs2], cur_IF);
            break;

        case ADDI:
            vrd = reg[cur_ID.cur_ins.rs1] + cur_ID.cur_ins.imm;
            break;
        case SLTI:
            vrd = ((int)reg[cur_ID.cur_ins.rs1] < (int)cur_ID.cur_ins.imm) ? 1 : 0;
            break;
        case SLTIU:
            vrd = (reg[cur_ID.cur_ins.rs1] < cur_ID.cur_ins.imm) ? 1 : 0;
            break;
        case XORI:
            vrd = reg[cur_ID.cur_ins.rs1] ^ cur_ID.cur_ins.imm;
            break;
        case ORI:
            vrd = reg[cur_ID.cur_ins.rs1] | cur_ID.cur_ins.imm;
            break;
        case ANDI:
            vrd = reg[cur_ID.cur_ins.rs1] & cur_ID.cur_ins.imm;
            break;
        case SLLI:
            vrd = reg[cur_ID.cur_ins.rs1] << (cur_ID.cur_ins.imm & 31u);
            break;
        case SRLI:
            vrd = reg[cur_ID.cur_ins.rs1] >> (cur_ID.cur_ins.imm & 31u);
            break;
        case SRAI:
            vrd = (int)(reg[cur_ID.cur_ins.rs1] >> (cur_ID.cur_ins.imm & 31u));
            break;

        case ADD:
            vrd = reg[cur_ID.cur_ins.rs1] + reg[cur_ID.cur_ins.rs2];
            break;
        case SUB:
            vrd = reg[cur_ID.cur_ins.rs1] - reg[cur_ID.cur_ins.rs2];
            break;

        case SLL:
            vrd = reg[cur_ID.cur_ins.rs1] << (reg[cur_ID.cur_ins.rs2] & 31u);
            break;
        case SRL:
            vrd = reg[cur_ID.cur_ins.rs1] >> (reg[cur_ID.cur_ins.rs2] & 31u);
            break;
        case SRA:
            vrd = (int)(reg[cur_ID.cur_ins.rs1] >> (reg[cur_ID.cur_ins.rs2] & 31u));
            break;

        case SLT:
            vrd = (int)reg[cur_ID.cur_ins.rs1] < (int)reg[cur_ID.cur_ins.rs2];
            break;
        case SLTU:
            vrd = reg[cur_ID.cur_ins.rs1] < reg[cur_ID.cur_ins.rs2];
            break;

        case XOR:
            vrd = reg[cur_ID.cur_ins.rs1] ^ reg[cur_ID.cur_ins.rs2];
            break;
        case OR:
            vrd = reg[cur_ID.cur_ins.rs1] | reg[cur_ID.cur_ins.rs2];
            break;
        case AND:
            vrd = reg[cur_ID.cur_ins.rs1] & reg[cur_ID.cur_ins.rs2];
            break;

        default:
            break;
        }
        reg[0] = 0;
    }
    void judge_SB(bool res, IF_reg &cur_IF)
    {
        if (res)
            pc = cur_pc + cur_imm;
        else
            pc = cur_pc + 4;
        cur_IF.isEmpty = 1;
    }
};
class MEM_reg
{
public:
    bool isEmpty;
    unsigned int cur_pc;
    unsigned int vrd;
    ins_name iname;
    unsigned int cur_imm;
    unsigned int cur_rs1;
    unsigned int cur_rs2;
    unsigned int cur_rd;
    MEM_reg()
    {
        isEmpty = 1;
    }
    void operate_MEM(EX_reg &cur_EX)
    {
        isEmpty = 0;
        cur_EX.isEmpty = 1;
        cur_pc = cur_EX.cur_pc;
        iname = cur_EX.iname;
        cur_imm = cur_EX.cur_imm;
        cur_rs1 = cur_EX.cur_rs1;
        cur_rs2 = cur_EX.cur_rs2;
        cur_rd = cur_EX.cur_rd;
        vrd = cur_EX.vrd;
        switch (iname)
        {
        case LB:
            char tmp_LB;
            memcpy(&tmp_LB, mem + (reg[cur_rs1] + cur_imm), sizeof(char));
            vrd = (unsigned int)tmp_LB;
            break;
        case LH:
            short tmp_LH;
            memcpy(&tmp_LH, mem + (reg[cur_rs1] + cur_imm), sizeof(short));
            vrd = (unsigned int)tmp_LH;
            break;
        case LW:
            memcpy(&vrd, mem + (reg[cur_rs1] + cur_imm), sizeof(unsigned int));
            break;
        case LBU:
            unsigned char tmp_LBU;
            memcpy(&tmp_LBU, mem + (reg[cur_rs1] + cur_imm), sizeof(unsigned char));
            vrd = (unsigned int)tmp_LBU;
            break;
        case LHU:
            unsigned short tmp_LHU;
            memcpy(&tmp_LHU, mem + (reg[cur_rs1] + cur_imm), sizeof(unsigned short));
            vrd = (unsigned int)tmp_LHU;
            break;

        case SB:
            char tmp_SB;
            tmp_SB = (char)reg[cur_rs2];
            memcpy(mem + (reg[cur_rs1] + cur_imm), &tmp_SB, sizeof(char));
            break;
        case SH:
            short tmp_SH;
            tmp_SH = (short)reg[cur_rs2];
            memcpy(mem + (reg[cur_rs1] + cur_imm), &tmp_SH, sizeof(short));
            break;
        case SW:
            memcpy(mem + (reg[cur_rs1] + cur_imm), &reg[cur_rs2], sizeof(unsigned int));
            break;
        default:
            break;
        }
    }
    void operate_WB()
    {
        switch (iname)
        {
        case BEQ:
        case BNE:
        case BLT:
        case BGE:
        case BLTU:
        case BGEU:
        case SB:
        case SH:
        case SW:
            break;
        default:
            if (cur_rd)
            {
                used[cur_rd] = 0;
                reg[cur_rd] = vrd;
            }
            break;
        }
        reg[0] = 0;
        isEmpty = 1;
    }
};

#endif