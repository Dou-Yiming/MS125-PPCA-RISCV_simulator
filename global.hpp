#ifndef __global
#define __global

#include <iostream>
#include <cstdio>
#include <cstring>

unsigned int reg[32];		//register simulator
unsigned char mem[9000000]; //memory simulator
unsigned int pc = 0;
bool isEnd = 0;
unsigned int target[32] = {0};
bool used[32] = {0};
unsigned int totalPrediction = 0;
unsigned int correctPrediction = 0;
unsigned int counter[32] = {0};

const char *REGI_name[] = {"zero", "ra", "sp", "qp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
const char *INS_name[] = {"NAME0", "ADD", "SLT", "SLTU", "AND", "OR", "XOR", "SLL", "SRL", "SUB", "SRA", "JALR", "LB", "LH", "LW", "LBU", "LHU", "ADDI", "SLTI", "SLTIU", "ANDI", "ORI", "XORI", "SLLI", "SRLI", "SRAI", "SB", "SH", "SW", "BEQ", "BNE", "BLT", "BGE", "BLTU", "BGEU", "LUI", "AUIPC", "JAL"};
enum ins_name
{
	NAME0, ADD, SLT, SLTU, AND, OR, XOR, SLL, SRL, SUB, SRA, //R
	JALR, LB, LH, LW, LBU, LHU, ADDI, SLTI, SLTIU, ANDI, ORI, XORI, SLLI, SRLI, SRAI, //I
	SB, SH, SW, //S
	BEQ, BNE, BLT, BGE, BLTU, BGEU, //B
	LUI, AUIPC, //U
	JAL	   //J
};
#endif