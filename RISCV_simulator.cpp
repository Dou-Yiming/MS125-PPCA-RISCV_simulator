#include "pipeline.hpp"
//主函数
int main()
{
	freopen("data\\riscv-testcases\\testcases\\pi.data", "r", stdin);
	//读入内存
	unsigned int tmp = 0;
	instruction ins;
	char buffer[20] = {0};
	unsigned int pos_mem = 0;
	while (scanf("%s", buffer) != EOF)
	{
		if (buffer[0] == '@')
		{
			sscanf(buffer + 1, "%x", &pos_mem);
		}
		else
		{
			sscanf(buffer, "%x", &mem[pos_mem]);
			scanf("%x", &mem[pos_mem + 1]);
			scanf("%x", &mem[pos_mem + 2]);
			scanf("%x", &mem[pos_mem + 3]);
			pos_mem += 4;
		}
		tmp = 0;
	}
	IF_reg cur_IF;
	ID_reg cur_ID;
	EX_reg cur_EX;
	MEM_reg cur_MEM;
	register int i;
	do
	{
		if (!cur_MEM.isEmpty)
			cur_MEM.operate_WB();
		if (cur_MEM.isEmpty && !cur_EX.isEmpty)
			cur_MEM.operate_MEM(cur_EX);
		if (cur_EX.isEmpty && !cur_ID.isEmpty)
			cur_EX.operate_EX(cur_ID, cur_IF);
		if (cur_ID.isEmpty && !cur_IF.isEmpty)
			cur_ID.operate_ID(cur_IF, cur_EX, cur_MEM);
		if (cur_IF.isEmpty && !isEnd)
			cur_IF.operate_IF();
		// for (i = 0; i < 32; ++i)
			// printf("%d ",(reg[i]));
		// printf("\n");
		// std::cout << std::endl;
	} while (!cur_IF.isEmpty || !cur_ID.isEmpty || !cur_EX.isEmpty || !cur_MEM.isEmpty);

	unsigned int ans = (unsigned int)reg[10];
	std::cout << (ans & 255u) << std::endl;
	std::cout << "correct Prediction:" << correctPrediction << std::endl;
	std::cout << "total Prediction:" << totalPrediction << std::endl;
	std::cout << "accuracy:" << ((double)correctPrediction / (double)totalPrediction) * 100 << "%" << std::endl;
	return 0;
}
