#ifndef __instruction
#define __instruction

#include "global.hpp"

inline bool isNum(char ch)
{
	if (ch >= '0' || ch <= '9')
		return true;
	if (ch >= 'A' || ch <= 'F')
		return true;
	return false;
}
inline unsigned int read()
{
	char tmp[8];
	tmp[1] = getchar();
	while (!isNum(tmp[1]))
		tmp[1] = getchar();
	tmp[0] = getchar();
	getchar();
	tmp[3] = getchar();
	tmp[2] = getchar();
	getchar();
	tmp[5] = getchar();
	tmp[4] = getchar();
	getchar();
	tmp[7] = getchar();
	tmp[6] = getchar();
	getchar();

	unsigned int ans = 0;
	for (int i = 0; i < 8; ++i)
	{
		if (tmp[i] < 'A')
			tmp[i] = tmp[i] - '0';
		else
			tmp[i] = tmp[i] - 'A' + 10;
		ans |= (unsigned int)(tmp[i] << (4 * i));
	}
	return ans;
}
//处理指令
class instruction
{
public:
	ins_name name;
	unsigned int imm, funct3, funct7, rs1, rs2, rd, opcode, str;

public:
	instruction()
	{
		name = NAME0;
		imm = funct3 = funct7 = rs1 = rs2 = rd = opcode = str = 0u;
	}
	void set(unsigned int STR)
	{
		name = NAME0;
		imm = funct3 = funct7 = rs1 = rs2 = rd = 0u;
		str = STR;
		opcode = str & 127u;

		switch (opcode)
		{
		case 0x37:
			name = LUI;
			rd = (str >> 7u) & 31u;
			imm = get_U_imm(str);
			break;
		case 0x17:
			name = AUIPC;
			rd = (str >> 7u) & 31u;
			imm = get_U_imm(str);
			break;
		case 0x6f:
			name = JAL;
			rd = (str >> 7u) & 31u;
			imm = get_J_imm(str);
			break;
		case 0x67:
			name = JALR;
			rd = (str >> 7u) & 31u;
			imm = get_I_imm(str);
			rs1 = (str >> 15u) & 31u;
			funct3 = 0u;
			break;
		case 0x63:
			imm = get_B_imm(str);
			rs1 = (str >> 15u) & 31u;
			rs2 = (str >> 20u) & 31u;
			funct3 = (str >> 12u) & 7u;
			switch (funct3)
			{
			case 0:
				name = BEQ;
				break;
			case 1:
				name = BNE;
				break;
			case 4:
				name = BLT;
				break;
			case 5:
				name = BGE;
				break;
			case 6:
				name = BLTU;
				break;
			case 7:
				name = BGEU;
				break;
			}
			break;
		case 0x03:
			imm = get_I_imm(str);
			rs1 = (str >> 15u) & 31u;
			rd = (str >> 7u) & 31u;
			funct3 = (str >> 12u) & 7u;
			switch (funct3)
			{
			case 0:
				name = LB;
				break;
			case 1:
				name = LH;
				break;
			case 2:
				name = LW;
				break;
			case 4:
				name = LBU;
				break;
			case 5:
				name = LHU;
				break;
			}
			break;
		case 0x23:
			imm = get_S_imm(str);
			rs1 = (str >> 15u) & 31u;
			rs2 = (str >> 20u) & 31u;
			funct3 = (str >> 12u) & 7u;
			switch (funct3)
			{
			case 0:
				name = SB;
				break;
			case 1:
				name = SH;
				break;
			case 2:
				name = SW;
				break;
			}
			break;
		case 0x13:
			imm = get_I_imm(str);
			rs1 = (str >> 15u) & 31u;
			rd = (str >> 7u) & 31u;
			funct3 = (str >> 12u) & 7u;
			switch (funct3)
			{
			case 0:
				name = ADDI;
				break;
			case 1:
				name = SLLI;
				break;
			case 2:
				name = SLTI;
				break;
			case 3:
				name = SLTIU;
				break;
			case 4:
				name = XORI;
				break;
			case 5:
				switch (str >> 30u)
				{
				case 0:
					name = SRLI;
					break;
				case 1:
					name = SRAI;
					break;
				}
				break;
			case 6:
				name = ORI;
				break;
			case 7:
				name = ANDI;
				break;
			}
			break;
		case 0x33:
			rs1 = (str >> 15u) & 31u;
			rs2 = (str >> 20u) & 31u;
			rd = (str >> 7u) & 31u;
			funct3 = (str >> 12u) & 7u;
			switch (funct3)
			{
			case 0:
				switch (str >> 30u)
				{
				case 0:
					name = ADD;
					break;
				case 1:
					name = SUB;
					break;
				}
				break;
			case 5:
				switch (str >> 30u)
				{
				case 0:
					name = SRL;
					break;
				case 1:
					name = SRA;
					break;
				}
			case 1:
				name = SLL;
				break;
			case 2:
				name = SLT;
				break;
			case 3:
				name = SLTU;
				break;
			case 4:
				name = XOR;
				break;
			case 6:
				name = OR;
				break;
			case 7:
				name = AND;
				break;
			}
			break;
		}
	}
	unsigned int get_U_imm(const unsigned int &str)
	{
		return str & 0xfffff000;
	}
	unsigned int get_I_imm(const unsigned int &str)
	{
		unsigned int ans = 0u;
		if ((str >> 31u) == 1u)
			ans |= 0xfffff800;
		ans |= (str >> 20u) & 2047u;
		return ans;
	}
	unsigned int get_S_imm(const unsigned int &str)
	{
		unsigned int ans = 0u;
		if ((str >> 31u) == 1u)
			ans |= 0xfffff800;
		ans |= ((str >> 25u) & 63u) << 5u;
		ans |= ((str >> 8u) & 15u) << 1u;
		ans |= (str >> 7u) & 1u;
		return ans;
	}
	unsigned int get_B_imm(const unsigned int &str)
	{
		unsigned int ans = 0u;
		if ((str >> 31u) == 1u)
			ans |= 0xfffff000;
		ans |= ((str >> 25u) & 63u) << 5u;
		ans |= ((str >> 7u) & 1u) << 11u;
		ans |= ((str >> 8u) & 15u) << 1u;
		return ans;
	}
	unsigned int get_J_imm(const unsigned int &str)
	{
		unsigned int imm = 0;
		if ((str >> 31u) == 1u)
			imm |= 0xfff00000UL;
		imm |= str & 0x000ff000UL;
		imm |= ((str >> 20u) & 1u) << 11u;
		imm |= ((str >> 21u) & 1023u) << 1u;
		return imm;
	}
	void print()
	{
		std::cout << std::endl;
		std::cout << "name:" << name << std::endl;
		std::cout << "imm:" << imm << std::endl;
		std::cout << "funct3:" << funct3 << std::endl;
		std::cout << "funct7:" << funct7 << std::endl;
		std::cout << "rs1:" << rs1 << std::endl;
		std::cout << "rd:" << rd << std::endl;
		std::cout << "opcode:" << opcode << std::endl;
		std::cout << "str:" << str << std::endl;
		std::cout << std::endl;
	}
	void EX()
	{
		switch (name)
		{
		case LUI:
			reg[rd] = imm;
			break;
		case AUIPC:
			reg[rd] += imm;
			break;

		case JAL:
			reg[rd] = pc + 4;
			break;
		case JALR:
			reg[rd] = pc + 4;
			break;

		case ADDI:
			reg[rd] = reg[rs1] + imm;
			break;
		case SLTI:
			reg[rd] = ((int)reg[rs1] < (int)imm) ? 1 : 0;
			break;
		case SLTIU:
			reg[rd] = (reg[rs1] < imm) ? 1 : 0;
			break;
		case XORI:
			reg[rd] = reg[rs1] ^ imm;
			break;
		case ORI:
			reg[rd] = reg[rs1] | imm;
			break;
		case ANDI:
			reg[rd] = reg[rs1] & imm;
			break;
		case SLLI:
			reg[rd] = reg[rs1] << (imm & 31u);
			break;
		case SRLI:
			reg[rd] = reg[rs1] >> (imm & 31u);
			break;
		case SRAI:
			reg[rd] = (int)(reg[rs1] >> (imm & 31u));
			break;

		case ADD:
			reg[rd] = reg[rs1] + reg[rs2];
			break;
		case SUB:
			reg[rd] = reg[rs1] - reg[rs2];
			break;

		case SLL:
			reg[rd] = reg[rs1] << (reg[rs2] & 31u);
			break;
		case SRL:
			reg[rd] = reg[rs1] >> (reg[rs2] & 31u);
			break;
		case SRA:
			reg[rd] = (int)(reg[rs1] >> (reg[rs2] & 31u));
			break;

		case SLT:
			reg[rd] = (int)reg[rs1] < (int)reg[rs2];
			break;
		case SLTU:
			reg[rd] = reg[rs1] < reg[rs2];
			break;

		case XOR:
			reg[rd] = reg[rs1] ^ reg[rs2];
			break;
		case OR:
			reg[rd] = reg[rs1] | reg[rs2];
			break;
		case AND:
			reg[rd] = reg[rs1] & reg[rs2];
			break;

		default:
			break;
		}
		reg[0] = 0;
	}
	void MEM()
	{
		switch (name)
		{
		case LB:
			char tmp_LB;
			memcpy(&tmp_LB, mem + (reg[rs1] + imm), sizeof(char));
			reg[rd] = (unsigned int)tmp_LB;
			break;
		case LH:
			short tmp_LH;
			memcpy(&tmp_LH, mem + (reg[rs1] + imm), sizeof(short));
			reg[rd] = (unsigned int)tmp_LH;
			break;
		case LW:
			memcpy(&reg[rd], mem + (reg[rs1] + imm), sizeof(unsigned int));
			break;
		case LBU:
			unsigned char tmp_LBU;
			memcpy(&tmp_LBU, mem + (reg[rs1] + imm), sizeof(unsigned char));
			reg[rd] = (unsigned int)tmp_LBU;
			break;
		case LHU:
			unsigned short tmp_LHU;
			memcpy(&tmp_LHU, mem + (reg[rs1] + imm), sizeof(unsigned short));
			reg[rd] = (unsigned int)tmp_LHU;
			break;

		case SB:
			char tmp_SB;
			tmp_SB = (char)reg[rs2];
			memcpy(mem + (reg[rs1] + imm), &tmp_SB, sizeof(char));
			break;
		case SH:
			short tmp_SH;
			tmp_SH = (short)reg[rs2];
			memcpy(mem + (reg[rs1] + imm), &tmp_SH, sizeof(short));
			break;
		case SW:
			memcpy(mem + (reg[rs1] + imm), &reg[rs2], sizeof(unsigned int));
			break;
			
			//跳转
		case BEQ:
			pc += (reg[rs1] == reg[rs2]) ? imm : 0;
			break;
		case BNE:
			pc += (reg[rs1] != reg[rs2]) ? imm : 0;
			break;
		case BLT:
			pc += ((int)reg[rs1] < (int)reg[rs2]) ? imm : 0;
			break;
		case BGE:
			pc += ((int)reg[rs1] >= (int)reg[rs2]) ? imm : 0;
			break;
		case BLTU:
			pc += (reg[rs1] < reg[rs2]) ? imm : 0;
			break;
		case BGEU:
			pc += (reg[rs1] >= reg[rs2]) ? imm : 0;
			break;
		default:
			break;
		}
	}
};
#endif