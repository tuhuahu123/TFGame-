#pragma once

#include"manager/manager.h"        // 包含单例模式管理器，用于全局资源管理
#include"tile.h"           // 包含地图块(Tile)定义及TileMap类型
#include <SDL_video.h>     // 包含SDL视频相关功能，用于图形渲染
#include <string>          // 包含标准字符串类，用于路径和文本处理
#include <fstream>         // 包含文件流操作，用于读取地图文件
#include<sstream>          // 包含字符串流，用于解析地图数据
#include <SDL.h>           // 包含SDL库，用于基础功能
#include<unordered_map>	  
#include "route.h"

class Map
{
public:
	typedef std::unordered_map<int, Route>SpawnerRountePool;

	Map() = default;
	~Map() = default;

	//加载地图
	bool load(const std::string& path)  
	{

		//打开文件
		std::ifstream file(path);  

		//检查文件是否打开成功
		if (!file.good())         
		{
			return false;
		}  

		TileMap tile_map_temp;       //临时地图
		int idx_x = -1, idx_y = -1;  //地图索引
		std::string str_line;		 //行

		// 地图文件解析循环：从文件中按行读取地图数据并构建地图二维结构
		while (std::getline(file, str_line))
		{
			str_line = trim_str(str_line);  // 清理行数据，移除多余空格

			// 跳过空行
			if (str_line.empty())
				continue;

			// 初始化新行处理
			idx_x = -1;                              // 重置列计数器
			idx_y++;                                 // 移至下一行
			tile_map_temp.emplace_back();            // 在地图中创建新行
			std::string str_tile = str_line;         // 保存原始行数据
			std::stringstream str_stream(str_line);  // 准备分割处理

			// 解析行数据：按逗号分割提取每个地图块
			while (std::getline(str_stream, str_line, ','))
			{
				idx_x++;                                  // 移至下一列
				tile_map_temp[idx_y].emplace_back();      // 在当前行添加新地图块
				Tile& tile = tile_map_temp[idx_y][idx_x]; // 获取新建地图块引用
				load_tile_from_string(tile, str_tile);    // 解析地图块属性
			}
		}

		file.close();  //关闭文件

		//检查地图是否为空
		if (tile_map_temp.empty()||tile_map_temp[0].empty())
		{
			return false;
		}

		tile_map = tile_map_temp;  //赋值地图
		generate_map_cache();     //生成房屋
		 
		return true;						 
	}

	//获取地图宽度
	size_t get_width() const   
	{
		if (tile_map.empty())
		{
			return 0;
		}
		return tile_map[0].size();
	}

	//获取地图高度
	size_t get_height() const 
	{
		return tile_map.size();
	}

	
	const TileMap& get_tile_map()
	{
		return tile_map;
	}

	const SDL_Point& get_home_idx()
	{
		return idx_home;
	}

	const SpawnerRountePool& get_spawner_route_pool()
	{
		return spwaner_route_pool;
	}

	////检查地图块是否有塔
	//void place_tower(const SDL_Point& idx)
	//{
	//	tile_map[idx.y][idx.x].has_tower = true;
	//}

private:
	TileMap tile_map; //地图
	SDL_Point idx_home = { 0 }; //家的索引
	SpawnerRountePool spwaner_route_pool; //生成器路径池

	//检查地图块是否有塔
	void place_tower(const SDL_Point& idx__tile) 
	{
		tile_map[idx__tile.y][idx__tile.x].has_tower = true; 
	}

	//去除首位空白字符      C++常用常用写法
	std::string trim_str(const std::string& str)			
	{
		size_t begin_idx = str.find_first_not_of(" \t");   //找到第一个非空格字符
		if (begin_idx == std::string::npos)
		{
			return "";
		}
		size_t end_idx = str.find_last_not_of(" \t");	//找到最后一个非空格字符
		size_t idx_range = end_idx - begin_idx + 1;		//计算字符长度
        return str.substr(begin_idx, idx_range);        //返回去除空格的字符串
	}

	
	// 地图块属性解析器：将格式为"值1\值2\值3\值4"的字符串转换为Tile对象的各项属性
	void load_tile_from_string(Tile& tile, const std::string& str)
	{
		// 步骤1：准备数据
		std::string str_tidy = trim_str(str);     // 清理字符串，删除首尾空格
		std::string str_value;                    // 临时存储单个解析值
		std::string values;                       // 存储所有有效值的结果集
		std::stringstream str_stream(str_tidy);   // 创建字符串流对象用于分割字符串

		// 步骤2：分割并解析数值
		while (std::getline(str_stream, str_value, '\\')) // 使用'\'作为分隔符逐个提取值
		{
			int value;
			try {
				value = std::stoi(str_value);  // 字符串转整数
			}
			catch (const std::invalid_argument&) {
				value = -1;  // 转换失败时用-1代替
			}
			values.push_back(value);  // 将数值添加到结果字符串
		}

		// 步骤3：设置地图块属性
		// 根据解析到的值设置Tile对象的各项属性，处理可能的数据缺失情况
		tile.terrian = (values.size() < 1 || values[0] < 0) ? 0 : values[0];
		tile.decoration = (values.size() < 2 || values[1] < 0) ? -1 : values[1];
		tile.direction = (Tile::Direction)((values.size() < 3 || values[2] < 0) ? 0 : values[2]);
		tile.Special_flag = (values.size() <= 3) ? -1 : values[3];
	}


	/**
	* 初始化游戏地图中的关键元素：防守目标(家)和怪物生成点
	*
	* 该函数遍历整个地图，处理特殊标记的地图块：
	* - Special_flag = 0：标识为玩家基地(防守目标)
	* - Special_flag > 0：标识为怪物生成点，并计算从该点到基地的路径
	* - Special_flag < 0：普通地图块，无特殊处理
	*/
	//生成地图缓存：遍历地图，处理特殊标记的地图块
	void generate_map_cache()  
	{
		for (int y = 0; y < get_height(); y++)    
		{
			// 双重循环遍历整个地图的所有格子
			for (int x = 0; x < get_width(); x++)  
			{
				const Tile& tile = tile_map[y][x]; 
				if (tile.Special_flag < 0)		   // 忽略没有特殊标记的普通地图块		 
				{
					continue;
				}
				if (tile.Special_flag == 0)       // 处理玩家基地：记录其位置坐标
				{
					idx_home.x = x;
					idx_home.y = y;
				}
				else                             // 处理怪物生成点：计算并存储从该点到基地的路径
				{
					//通过Route生成怪物行进路径列表
                    spwaner_route_pool[tile.Special_flag] = Route(tile_map, { x, y });
				}
			}
		}
	}
};