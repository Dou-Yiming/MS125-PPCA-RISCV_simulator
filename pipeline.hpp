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
    IF_reg():isEmpty(true) {}
    void operate_IF()
    {
        isEmpty = 0;
        cur_pc = pc;
        memcpy(&cur_ins, mem + cur_pc, sizeof(unsigned int));
        pc += 4;
    }
};
class ID_reg
{
public:
    bool isEmpty;
    instruction cur_ins;
    unsigned int cur_pc;
    unsigned int val1, val2;
    bool isBranched;

    ID_reg():isEmpty(true){}
    void operate_ID(IF_reg &cur_IF, EX_reg &cur_EX, MEM_reg &cur_MEM);
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
    unsigned int val1, val2;
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
        val1 = cur_ID.val1;
        val2 = cur_ID.val2;
        switch (iname)
        {
        case LUI:
            vrd = cur_imm;
            break;
        case AUIPC:
            vrd = cur_pc + cur_imm;
            break;

        case JAL:
            vrd = cur_pc + 4;
            break;
        case JALR:
            vrd = cur_pc + 4;
            break;

        case BEQ:
            judge_SB(val1 == val2, cur_IF, cur_ID);
            break;
        case BNE:
            judge_SB(val1 != val2, cur_IF, cur_ID);
            break;
        case BLT:
            judge_SB((int)val1 < (int)val2, cur_IF, cur_ID);
            break;
        case BGE:
            judge_SB((int)val1 >= (int)val2, cur_IF, cur_ID);
            break;
        case BLTU:
            judge_SB(val1 < val2, cur_IF, cur_ID);
            break;
        case BGEU:
            judge_SB(val1 >= val2, cur_IF, cur_ID);
            break;

        case ADDI:
            vrd = val1 + cur_imm;
            break;
        case SLTI:
            vrd = ((int)val1 < (int)cur_imm) ? 1 : 0;
            break;
        case SLTIU:
            vrd = (val1 < cur_imm) ? 1 : 0;
            break;
        case XORI:
            vrd = val1 ^ cur_imm;
            break;
        case ORI:
            vrd = val1 | cur_imm;
            break;
        case ANDI:
            vrd = val1 & cur_imm;
            break;
        case SLLI:
            vrd = val1 << (cur_imm & 31u);
            break;
        case SRLI:
            vrd = val1 >> (cur_imm & 31u);
            break;
        case SRAI:
            vrd = (int)(val1 >> (cur_imm & 31u));
            break;

        case ADD:
            vrd = val1 + val2;
            break;
        case SUB:
            vrd = val1 - val2;
            break;

        case SLL:
            vrd = val1 << (val2 & 31u);
            break;
        case SRL:
            vrd = val1 >> (val2 & 31u);
            break;
        case SRA:
            vrd = (int)(val1 >> (val2 & 31u));
            break;

        case SLT:
            vrd = (int)val1 < (int)val2;
            break;
        case SLTU:
            vrd = val1 < val2;
            break;

        case XOR:
            vrd = val1 ^ val2;
            break;
        case OR:
            vrd = val1 | val2;
            break;
        case AND:
            vrd = val1 & val2;
            break;

        default:
            break;
        }
        reg[0] = 0;
    }
    void judge_SB(bool res, IF_reg &cur_IF, ID_reg &cur_ID)
    {
        if (res)
        {
            if (!cur_ID.isBranched)
            {
                pc = cur_pc + cur_imm;
                cur_IF.isEmpty = 1;
            }
            if (counter[cur_pc % 32] < 3u)
                ++counter[cur_pc % 32];
        }
        else
        {
            if (cur_ID.isBranched)
            {
                pc = cur_pc + 4;
                cur_IF.isEmpty = 1;
            }
            else
                ++correctPrediction;
            if (counter[cur_pc % 32] > 0)
                --counter[cur_pc % 32];
        }
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
    unsigned int cur_period = 0;
    bool isAccessed = 0;
    unsigned int val1, val2;
    MEM_reg()
    {
        isEmpty = 1;
        cur_period = 0;
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
        cur_period = 1;
        val1 = cur_EX.val1;
        val2 = cur_EX.val2;
        switch (iname)
        {
        case LB:
            char tmp_LB;
            memcpy(&tmp_LB, mem + (val1 + cur_imm), sizeof(char));
            vrd = (unsigned int)tmp_LB;
            break;
        case LH:
            short tmp_LH;
            memcpy(&tmp_LH, mem + (val1 + cur_imm), sizeof(short));
            vrd = (unsigned int)tmp_LH;
            break;
        case LW:
            memcpy(&vrd, mem + (val1 + cur_imm), sizeof(unsigned int));
            break;
        case LBU:
            unsigned char tmp_LBU;
            memcpy(&tmp_LBU, mem + (val1 + cur_imm), sizeof(unsigned char));
            vrd = (unsigned int)tmp_LBU;
            break;
        case LHU:
            unsigned short tmp_LHU;
            memcpy(&tmp_LHU, mem + (val1 + cur_imm), sizeof(unsigned short));
            vrd = (unsigned int)tmp_LHU;
            break;

        case SB:
            char tmp_SB;
            tmp_SB = (char)val2;
            memcpy(mem + (val1 + cur_imm), &tmp_SB, sizeof(char));
            break;
        case SH:
            short tmp_SH;
            tmp_SH = (short)val2;
            memcpy(mem + (val1 + cur_imm), &tmp_SH, sizeof(short));
            break;
        case SW:
            memcpy(mem + (val1 + cur_imm), &val2, sizeof(unsigned int));
            break;
        default:
            break;
        }
    }
    void operate_WB()
    {
        switch (iname)
        {
        case LB:case LH:case LW:case LBU:case LHU:
        case SB:case SH:case SW:
            if (cur_period != 3)
            {
                ++cur_period;
                if (cur_period == 3)
                    isAccessed = 1;
                return;
            }
        }
        switch (iname)
        {
        case BEQ:case BNE:case BLT:case BGE:case BLTU:case BGEU:
        case SB:case SH:case SW:
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
void ID_reg::operate_ID(IF_reg &cur_IF, EX_reg &cur_EX, MEM_reg &cur_MEM)
{
    cur_pc = cur_IF.cur_pc;
    if (cur_IF.cur_ins == 0x0ff00513)
    {
        isEnd = 1;
        cur_IF.isEmpty = 1;
        return;
    }
    cur_ins.set(cur_IF.cur_ins);
    //forwarding
    switch (cur_ins.name)
    {
    case LUI: case AUIPC: case JAL:
        if (used[cur_ins.rd])
            return;
        used[cur_ins.rd] = 1;
        used[0] = 0;
        break;
    case JALR:case LB:case LH:case LW:case LBU:case LHU:
    case ADDI:case SLTI:case SLTIU:
    case XORI:case ORI:case ANDI:
    case SLLI:case SRLI:case SRAI: //rd&rs1
        if (used[cur_ins.rd])
            return;
        if (used[cur_ins.rs1])
        {
            if (cur_EX.iname >= 12 && cur_EX.iname <= 16 && !cur_EX.isEmpty && cur_EX.cur_rd == cur_ins.rs1)
                return;
            if (!cur_EX.isEmpty && cur_EX.cur_rd == cur_ins.rs1)
                val1 = cur_EX.vrd;
            else if (cur_MEM.isAccessed)
                val1 = cur_MEM.vrd;
            else
                return;
        }
        else
            val1 = reg[cur_ins.rs1];
        used[cur_ins.rd] = 1;
        used[0] = 0;
        break;
    case ADD:case SUB:case SLL:case SLT:case SLTU:
    case XOR:case SRL:case SRA:case OR:case AND:
    case SB:case SH:case SW:
    case BEQ:case BNE:case BLT:case BGE:case BLTU:case BGEU: //rd&rs1&rs2
        if (used[cur_ins.rd])
            return;
        if (used[cur_ins.rs1])
        {
            if (cur_EX.iname >= 12 && cur_EX.iname <= 16 && !cur_EX.isEmpty && cur_EX.cur_rd == cur_ins.rs1)
                return;
            if (!cur_EX.isEmpty && cur_EX.cur_rd == cur_ins.rs1)
                val1 = cur_EX.vrd;
            else if (cur_MEM.isAccessed)
                val1 = cur_MEM.vrd;
            else
                return;
            if (used[cur_ins.rs2])
            {
                if (cur_EX.iname >= 12 && cur_EX.iname <= 16 && !cur_EX.isEmpty && cur_EX.cur_rd == cur_ins.rs2)
                    return;
                if (!cur_EX.isEmpty && cur_EX.cur_rd == cur_ins.rs2)
                    val2 = cur_EX.vrd;
                else if (cur_MEM.isAccessed)
                    val2 = cur_MEM.vrd;
                else
                    return;
            }
            else
                val2 = reg[cur_ins.rs2];
        }
        else
        {
            val1 = reg[cur_ins.rs1];
            if (used[cur_ins.rs2])
            {
                if (cur_EX.iname >= 12 && cur_EX.iname <= 16 && !cur_EX.isEmpty && cur_EX.cur_rd == cur_ins.rs2)
                    return;
                if (!cur_EX.isEmpty && cur_EX.cur_rd == cur_ins.rs2)
                    val2 = cur_EX.vrd;
                else if (cur_MEM.isAccessed)
                    val2 = cur_MEM.vrd;
                else
                    return;
            }
            else
                val2 = reg[cur_ins.rs2];
        }
        switch (cur_ins.name)
        {
        case ADD:case SUB:case SLL:case SLT:case SLTU:
        case XOR:case SRL:case SRA:case OR:case AND:
            used[cur_ins.rd] = 1;
            used[0] = 0;
        }
        break;
    }

    isEmpty = 0;
    cur_IF.isEmpty = 1;
    if (cur_ins.rs1 == 0)
        val1 = 0;
    if (cur_ins.rs2 == 0)
        val2 = 0;
    isBranched = 0;

    //no need to predict
    if (cur_ins.name == JAL)
        pc = cur_pc + cur_ins.imm;
    if (cur_ins.name == JALR)
        pc = (val1 + cur_ins.imm) & (-2u);

    //branch prediction
    if (cur_ins.name >= 29 && cur_ins.name <= 34)
    {
        if (counter[cur_pc % 32] <= 1)
        {
            pc = cur_pc + 4;
            isBranched = 0;
        }
        else
        {
            pc = cur_pc + cur_ins.imm;
            isBranched = 1;
        }
        ++totalPrediction;
    }
}
#endif