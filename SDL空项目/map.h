#pragma once

#include<algorithm>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<SDL.h>

#define TILE_SIZE 96

enum class TileType
{
	Failed=-1,Idle,Selected,Start,End
};


class Map
{
public:
	Map(const char* file_path)
	{
		for(int i=0;i<8;++i)
			for(int j=0;j<8;++j)
				m_mp[i][j] = -1;
		std::ifstream file(file_path);
		if (!file.is_open())
		{
			SDL_Log("Map file not found");
			return;
		}
        std::string line;
		int cur_line = 0;
		while (std::getline(file, line))
		{
			//以逗号为分隔拆分字符串
			std::vector<std::string> vec;
			size_t pos = 0;
			while ((pos = line.find(',')) != std::string::npos)
			{
				vec.push_back(line.substr(0, pos));
				line.erase(0, pos + 1);
			}

			int limi=std::min<int>(8,vec.size());
			for (int i = 0; i < limi; i++)
			{
				m_mp[cur_line][i] = std::stoi(vec[i]);
				if (m_mp[cur_line][i] >= -1 && m_mp[cur_line][i] <= 3)
					continue;
				else m_mp[cur_line][i] = -1;
			}
			cur_line++;
		}
	}


public:
	int m_mp[8][8];


};