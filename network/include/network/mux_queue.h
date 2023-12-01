#pragma once

#include <mutex>
#include <queue>
#include <type_traits>

template <typename T>
class mux_queue {

public:
	T popFront()
	{
		std::lock_guard lg(_mux);
		T front;
		if constexpr (std::is_move_constructible<T>::value)
		{
			front = std::move(_queue.front());
		}
		else
		{
			front = _queue.front();
		}
		_queue.pop();
		return front;
	}

	void push(const T& obj)
	{
		std::lock_guard lg(_mux);
		_queue.push(obj);
	}

	void push(T&& obj)
	{
		std::lock_guard lg(_mux);
		_queue.push(obj);
	}

	size_t size() const
	{
		// no need to lock here because popFront() is called in the same thread, even if there is concurrency
		// issue with push(), the returned size will always be less than or equal to actual size, so there will
		// be no errors with popFront()
		return _queue.size();
	}

private:
	std::mutex _mux;
	std::queue<T> _queue;
};