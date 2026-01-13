#pragma once

#include"timer.h"
#include"map.h"

#include<vector>
#include<SDL.h>
#include<SDL_image.h>

class Animation
{
public:
	typedef std::function<void()>PlayBack;
	typedef std::vector<SDL_Texture*>TexList;

public:
	Animation()
	{
		timer.set_one_shot(false);
		timer.set_on_timeout([&]()
			{
				idx_frame++;
				if (idx_frame == rect_src_list.size())
				{
					idx_frame = is_loop ? 0 : rect_src_list.size() - 1;
					if (on_finished)
					{
						on_finished();
					}
				}
			});
		tex_list.clear();
	}
	~Animation() = default;

	void set_frame_seperate(bool flag)
	{
		is_frame_seperate = flag;
	}

	void add_texture(SDL_Texture* texture)
	{
		tex_list.push_back(texture);
		SDL_QueryTexture(texture, nullptr, nullptr, &width_frame, &height_frame);
		rect_src_list.emplace_back();
	}

	void set_animation(SDL_Texture* texture, int num_h, int num_v, const std::vector<int>& idx_list)
	{
		int tex_width, tex_height;

		if (texture == nullptr)
		{
			std::cout << "texture is null" << std::endl;
			return;
		}

		this->texture = texture;
		SDL_QueryTexture(texture, nullptr, nullptr, &tex_width, &tex_height);
		width_frame = tex_width / num_h;
		height_frame = tex_height / num_v;

		rect_src_list.resize(idx_list.size());
		for (size_t i = 0; i < idx_list.size(); i++)
		{
			int idx = idx_list[i];
			SDL_Rect& rect_src = rect_src_list[i];

			rect_src.x = idx % num_h * width_frame;
			rect_src.y = idx / num_h * height_frame;
			rect_src.w = width_frame, rect_src.h = height_frame;
		}
	}

	void on_render(SDL_Renderer* renderer, const SDL_Point& pos_dst, double angle = 0, int ratio = 1)const
	{
		static SDL_Rect rect_dst;

		rect_dst.x = pos_dst.x, rect_dst.y = pos_dst.y;
		rect_dst.w = width_frame * ratio, rect_dst.h = height_frame * ratio;

		if (!is_frame_seperate)
			SDL_RenderCopyEx(renderer, texture, &rect_src_list[idx_frame], &rect_dst, angle, nullptr, SDL_FLIP_NONE);
		else
		{
			SDL_RenderCopyEx(renderer, tex_list[idx_frame], nullptr, &rect_dst, angle, nullptr, SDL_FLIP_NONE);
		}
	}
	void on_update(double delta)
	{
		timer.on_update(delta);
	}

	void set_interval(double interval)
	{
		timer.set_wait_time(interval);
	}

	void set_on_finished(PlayBack on_finished)
	{
		this->on_finished = on_finished;
	}

	void set_loop(bool flag)
	{
		is_loop = flag;
	}

	void reset()
	{
		idx_frame = 0;
		timer.restart();
	}

private:
	Timer timer;
	PlayBack on_finished;

	int idx_frame = 0;
	bool is_loop = true;
	bool is_frame_seperate = false;

	TexList tex_list;
	SDL_Texture* texture = nullptr;
	std::vector<SDL_Rect>rect_src_list;
	int width_frame = 0, height_frame = 0;
};
