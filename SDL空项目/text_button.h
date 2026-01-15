#pragma once

#include"resources_manager.h"
#include<SDL.h>
#include<SDL_ttf.h>
#include<string>
#include<iostream>
#include<algorithm>
#include<vector>
#include<cmath>

class TextButton
{
public:
	typedef std::vector<std::string>TextList;

public:
	TextButton(const std::string& str_text, const SDL_Rect& rect_dst, const SDL_Rect& rect_offset) :
		str_text(str_text), rect_dst(rect_dst), rect_offset(rect_offset), rect_dst_final(rect_dst)
	{
		SplitString(str_text);
	}
	~TextButton() = default;
	void on_render(SDL_Renderer* renderer)
	{
		TTF_SetFontSize(ResourcesManager::get_instance()->find_font("ipix"), size);
		//渲染
		SDL_Rect rect_tmp = rect_dst_final;
		int height_line = rect_dst_final.h / text_list.size();
		rect_tmp.h = height_line;
		for (const auto& text_line : text_list)
		{
			SDL_DestroyTexture(tex_text);
			suf_text = TTF_RenderUTF8_Blended(ResourcesManager::get_instance()->find_font("ipix"), text_line.c_str(), color);
			tex_text = SDL_CreateTextureFromSurface(renderer, suf_text);
			SDL_RenderCopy(renderer, tex_text, nullptr, &rect_tmp);
			SDL_FreeSurface(suf_text);

			SDL_SetRenderDrawColor(renderer, color_boarder.r, color_boarder.g, color_boarder.b, color_boarder.a);
			rect_tmp.y += height_line;
		}
		SDL_RenderDrawRect(renderer, &rect_dst_final);

	}
	void on_update(double delta, const SDL_Point& pos_cursor)
	{
		color = (current_state == State::Idle) ? color_text : color_text_hovered;

		if (can_hovered && SDL_PointInRect(&pos_cursor, &rect_dst))
			current_state = State::Hovered;
		else
			current_state = State::Idle;

		rect_dst_final = rect_dst;
		if (current_state == State::Hovered)
		{
			rect_dst_final.x = rect_dst.x + rect_offset.x;
			rect_dst_final.y = rect_dst.y + rect_offset.y;
			rect_dst_final.w = rect_dst.w + rect_offset.w;
			rect_dst_final.h = rect_dst.h + rect_offset.h;
		}
	}
	void on_input(const SDL_Event& event, const SDL_Point& pos_cursor)
	{
		switch (event.type)
		{
		case SDL_MOUSEBUTTONUP:
			if (SDL_PointInRect(&pos_cursor, &rect_dst))
				is_pushed = true;
		}
	}

	bool get_pushed()const
	{
		return is_pushed;
	}
	void reset()
	{
		is_pushed = false;
		current_state = State::Idle;
	}
	void set_color_text(const SDL_Color& color) { this->color_text = color; }
	void set_color_border(const SDL_Color& color) { this->color_boarder = color; }
	void set_color_text_hovered(const SDL_Color& color) { this->color_text_hovered = color; }
	void set_text(const std::string& str_text) { this->str_text = str_text; SplitString(str_text); }
	void set_size(int size) { this->size = size; }
	void set_can_hovered(bool flag) { can_hovered = flag; }
	void set_rect_dst(const SDL_Rect& rect_dst) { this->rect_dst = rect_dst; }

private:
	void SplitString(const std::string& text)
	{
		text_list.clear();
		size_t start = 0;
		size_t end = text.find('\n');

		while (end != std::string::npos)
		{
			text_list.push_back(text.substr(start, end - start));
			start = end + 1;
			end = text.find('\n', start);
		}
		text_list.push_back(text.substr(start));
	}

private:
	enum class State
	{
		Idle,
		Hovered
	};

private:
	std::string str_text;
	SDL_Surface* suf_text;
	SDL_Texture* tex_text;
	SDL_Rect rect_dst;
	SDL_Rect rect_offset;
	SDL_Rect rect_dst_final;
	SDL_Color color;
	SDL_Color color_text = { 255,255,255,255 };
	SDL_Color color_text_hovered = { 255, 215, 0, 255 };
	SDL_Color color_boarder = { 215, 105, 195, 255 };
	State current_state = State::Idle;
	bool is_pushed = false;
	bool can_hovered = true;
	int size = 50;
	TextList text_list;
};
