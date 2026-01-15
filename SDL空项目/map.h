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
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                m_mp[i][j] = -1;
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            SDL_Log("Map file not found");
            return;
        }
        std::string line;
        int cur_line = 0;
        while (std::getline(file, line) && cur_line < 8)
        {
            // 以逗号为分隔拆分字符串
            std::vector<std::string> vec;
            size_t start = 0;
            size_t end = line.find(',');

            while (end != std::string::npos)
            {
                vec.push_back(line.substr(start, end - start));
                start = end + 1;
                end = line.find(',', start);
            }

            // 添加最后一个字段（最后一个逗号之后的部分）
            if (start < line.length()) {
                vec.push_back(line.substr(start));
            }

            // 处理空行的情况
            if (vec.empty()) {
                vec.push_back("-1"); // 默认值为禁用格
            }

            int limi = std::min<int>(8, vec.size());
            for (int i = 0; i < limi; i++)
            {
                // 去除可能的空格
                std::string cell = vec[i];
                cell.erase(0, cell.find_first_not_of(" \t\n\r")); // 去除前导空白
                cell.erase(cell.find_last_not_of(" \t\n\r") + 1); // 去除尾部空白

                if (!cell.empty())
                {
                    try
                    {
                        m_mp[cur_line][i] = std::stoi(cell);
                        if (m_mp[cur_line][i] < -1 || m_mp[cur_line][i] > 3)
                        {
                            m_mp[cur_line][i] = -1; // 超出范围设为禁用格
                        }
                    }
                    catch (const std::invalid_argument& e)
                    {
                        m_mp[cur_line][i] = -1; // 转换失败设为禁用格
                    }
                    catch (const std::out_of_range& e)
                    {
                        m_mp[cur_line][i] = -1; // 超出范围设为禁用格
                    }
                }
                else
                {
                    m_mp[cur_line][i] = -1; // 空单元格设为禁用格
                }
            }
            cur_line++;
        }
    }

    Map() {
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                m_mp[i][j] = -1;
    }

	void check()
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				std::cout << m_mp[i][j] << "\t";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

public:
	int m_mp[8][8];


};