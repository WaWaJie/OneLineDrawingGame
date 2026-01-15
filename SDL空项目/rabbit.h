#pragma once

#include"animation.h"
#include"resources_manager.h"

enum class FacingDirection
{
	Left, Right, Up, Down
};

class Rabbit
{
public:
	Rabbit()
	{
		anim_left=new Animation();
		anim_left->set_animation(ResourcesManager::get_instance()->find_texture("player"), 4, 4, { 8,9,10,11 });
		anim_left->set_interval(0.2);
		anim_left->set_loop(true);
		
		anim_right = new Animation();
		anim_right->set_animation(ResourcesManager::get_instance()->find_texture("player"), 4, 4, { 12,13,14,15 });
		anim_right->set_interval(0.2);
		anim_right->set_loop(true);

		anim_up = new Animation();
		anim_up->set_animation(ResourcesManager::get_instance()->find_texture("player"), 4, 4, { 4,5,6,7});
		anim_up->set_interval(0.2);
		anim_up->set_loop(true);
			
		anim_down = new Animation();
		anim_down->set_animation(ResourcesManager::get_instance()->find_texture("player"), 4, 4, { 0,1,2,3 });
		anim_down->set_interval(0.2);
		anim_down->set_loop(true);

		current_anim = anim_down;
	}

	void on_update(float delta)
	{
		if (current_anim)
			current_anim->on_update(delta);
	}

	void on_render(SDL_Renderer* renderer)
	{
		if (current_anim)
		{
			current_anim->on_render(renderer, pos_dst, 0, scale);
		}
	}
	
	void set_direction(FacingDirection dir)
	{
		switch (dir)
		{
		case FacingDirection::Left:
			current_anim = anim_left;
			facing = FacingDirection::Left;
			break;
		case FacingDirection::Right:
			current_anim = anim_right;
			facing = FacingDirection::Right;
			break;
		case FacingDirection::Up:
			current_anim = anim_up;
			facing = FacingDirection::Up;
			break;
		case FacingDirection::Down:
			current_anim = anim_down;
			facing = FacingDirection::Down;
			break;
		default:
			break;

		}
	}

	void set_pos(int x, int y)
	{
		pos_dst.x = x;
		pos_dst.y = y;
	}

	void set_scale(float scale) { this->scale = scale; }

private:
	Animation* current_anim = nullptr;
	Animation* anim_left = nullptr;
	Animation* anim_right = nullptr;
	Animation* anim_up = nullptr;
	Animation* anim_down = nullptr;

	FacingDirection facing = FacingDirection::Down;

	SDL_Point pos_dst = { 0 };
	float scale = 2.0f;
};