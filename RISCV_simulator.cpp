//#define _CRT_SECURE_NO_DEPRECATE
#include "pipeline.hpp"
//主函数
int main()
{
	//freopen("testcases\\basicopt1.data", "r", stdin);
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
			/*for (int i = 0; i < 4; ++i)
				tmp |= ((unsigned int)mem[pos_mem + i]) << (8 * i);
			ins.set(tmp);
			std::cout << INS_name[ins.name] << std::endl;*/
			pos_mem += 4;
		}
		tmp = 0;
	}
	//std::cout << "input success" << std::endl;
	IF_reg cur_IF;
	ID_reg cur_ID;
	EX_reg cur_EX;
	MEM_reg cur_MEM;
	do
	{
		//puts("======================================");
		if (!cur_MEM.isEmpty)
		{
			cur_MEM.operate_WB();
			//std::cout << "WB:" << INS_name[cur_MEM.iname] << std::endl;
		}
		//std::cout << INS_name[cur_MEM.iname] << std::endl;
		if (cur_MEM.isEmpty && !cur_EX.isEmpty)
		{
			cur_MEM.operate_MEM(cur_EX);
			//std::cout << "MEM:" << INS_name[cur_MEM.iname] << std::endl;
		}
		//std::cout << INS_name[cur_MEM.iname] << std::endl;
		if (cur_EX.isEmpty && !cur_ID.isEmpty)
		{
			cur_EX.operate_EX(cur_ID, cur_IF);
			//std::cout << "EX:" << INS_name[cur_EX.iname] << std::endl;
		}
		//std::cout << INS_name[cur_EX.iname] << std::endl;
		if (cur_ID.isEmpty && !cur_IF.isEmpty)
		{
			cur_ID.operate_ID(cur_IF);
			/*std::cout << "ID:" << INS_name[cur_ID.cur_ins.name] << std::endl;
			std::cout<<"cur_IF isEmpty:"<<cur_IF.isEmpty<<std::endl;
            std::cout<<"cur_ID isEmpty:"<<cur_ID.isEmpty<<std::endl;
            std::cout<<"cur_EX isEmpty:"<<cur_EX.isEmpty<<std::endl;
            std::cout<<"cur_MEM isEmpty:"<<cur_MEM.isEmpty<<std::endl;*/
		}
		//std::cout << INS_name[cur_ID.cur_ins.name] << std::endl;
		if (cur_IF.isEmpty && !isEnd)
		{
			cur_IF.operate_IF();
			//std::cout<<cur_IF.cur_ins<<std::endl;
		}
		//std::cout<<"cur_IF isEmpty:"<<cur_IF.isEmpty<<std::endl;
		//std::cout<<"cur_ID isEmpty:"<<cur_ID.isEmpty<<std::endl;
		//std::cout<<"cur_EX isEmpty:"<<cur_EX.isEmpty<<std::endl;
		//std::cout<<"cur_MEM isEmpty:"<<cur_MEM.isEmpty<<std::endl;
		//std::cout << "isEnd:" << isEnd << std::endl;
		//std::cout << (reg[10] & 255u) << std::endl;
		//std::cout<<"counter:"<<counter<<std::endl;
		//for (int i = 0; i < 32; ++i)
			//std::cout << used[i] << "  ";
		//std::cout << std::endl;
	} while (!cur_IF.isEmpty || !cur_ID.isEmpty || !cur_EX.isEmpty || !cur_MEM.isEmpty);

	unsigned int ans = (unsigned int)reg[10];
	std::cout << (ans & 255u) << std::endl;
	return 0;
}
