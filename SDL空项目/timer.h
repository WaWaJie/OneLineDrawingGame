#pragma once

#include<functional>

class Timer
{
public:
	Timer() = default;
	~Timer() = default;

	void pause()
	{
		paused = true;
	}

	void resume()
	{
		paused = true;
	}

	void on_update(double delta)
	{
		if (paused)return;

		pass_time += delta;

		if (pass_time >= wait_time)
		{
			bool can_shot = (one_shot && !shotted) || (!one_shot);
			if (can_shot && on_timeout)
				on_timeout();
			shotted = true;
			pass_time -= wait_time;
		}
	}

	void restart()
	{
		pass_time = 0;
		shotted = false;
	}

	void set_wait_time(double wait_time)
	{
		this->wait_time = wait_time;
	}

	void set_one_shot(bool flag)
	{
		one_shot = flag;
	}

	void set_on_timeout(std::function<void()>on_timeout)
	{
		this->on_timeout = on_timeout;
	}

private:
	std::function<void()>on_timeout = nullptr;
	double wait_time = 0;
	double pass_time = 0;
	bool one_shot = false;
	bool shotted = false;
	bool paused = false;
};
