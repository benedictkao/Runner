#pragma once

#include <mutex>
#include <queue>
#include <type_traits>

#include "NullMutex.h"

template <typename T, typename _MutexType = std::mutex>
class ConcurrentQueue
{

public:
	T popFront()
	{
		T front;
		_mux.lock();
		if constexpr (std::is_move_constructible<T>::value)
		{
			front = std::move(_queue.front());
		}
		else
		{
			front = _queue.front();
		}
		_queue.pop();
		_mux.unlock();
		return front;
	}

	void push(const T& obj)
	{
		_mux.lock();
		_queue.push(obj);
		_mux.unlock();
	}

	void push(T&& obj)
	{
		_mux.lock();
		_queue.push(obj);
		_mux.unlock();
	}

	size_t size()
	{
		_mux.lock();
		auto size = _queue.size();
		_mux.unlock();
		return size;
	}

private:
	_MutexType _mux;
	std::queue<T> _queue;
};