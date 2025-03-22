#include<iostream>
#include"manager/manager.h"
#include "manager/Game_manager.h"




int main(int argc, char* argv[])
{
	return GameManager::instance()->run(argc, argv); // 运行游戏
}

