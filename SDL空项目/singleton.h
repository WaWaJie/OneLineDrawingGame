#pragma once

template <typename T>
class Singleton
{
public:
	static T* get_instance()
	{
		if (instance == nullptr)
			instance = new T();
		return instance;
	}

protected:
	Singleton() = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;
	~Singleton() = default;

private:
	static T* instance;

};
template <typename T>
T* Singleton<T>::instance = nullptr;


