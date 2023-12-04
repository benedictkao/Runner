#pragma once

#include <mutex>
#include <unordered_set>
#include <vector>

#include "NullMutex.h"

template <typename T, typename _MutexType = std::mutex>
class ConcurrentSet
{
public:
	ConcurrentSet(int initialCapacity): _set(initialCapacity) {}

public:
	void insert(const T& obj)
	{
		_mux.lock();
		_set.insert(obj);
		_mux.unlock();
	}

	void erase(const T& obj)
	{
		_mux.lock();
		_set.erase(obj);
		_mux.unlock();
	}

	bool contains(const T& obj)
	{
		_mux.lock();
		bool result = _set.count(obj) > 0;
		_mux.unlock();
		return result;
	}

	std::vector<T> toList()
	{
		std::vector<T> list;
		_mux.lock();
		list.reserve(_set.size());
		for (auto item : _set)
		{
			list.push_back(item);
		}
		_mux.unlock();
		return list;
	}

private:
	_MutexType _mux;
	std::unordered_set<T> _set;
};