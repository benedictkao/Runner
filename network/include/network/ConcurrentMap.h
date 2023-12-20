
#pragma once

#include <mutex>
#include <unordered_map>
#include <vector>

#include "NullMutex.h"

template <typename _Key, typename _Value, typename _MutexType = std::mutex>
class ConcurrentMap
{
public:
	ConcurrentMap(int initialCapacity): _map(initialCapacity) {}

public:
	/*
	* Creates a copy of the value
	*/
	_Value get(const _Key& key) const
	{
		_mux.lock();
		const auto val = _map[key];
		_mux.unlock();
		return val;
	}

	void set(const _Key& key, const _Value& value)
	{
		_mux.lock();
		_map[key] = value;
		_mux.unlock();
	}

	void erase(const _Key& key)
	{
		_mux.lock();
		_map.erase(key);
		_mux.unlock();
	}

	bool contains(const _Key& obj)
	{
		_mux.lock();
		bool result = _map.count(obj) > 0;
		_mux.unlock();
		return result;
	}

	std::vector<_Value> getKeys()
	{
		std::vector<_Value> list;
		_mux.lock();
		list.reserve(_map.size());
		for (const auto& pair : _map)
			list.push_back(pair.first);
		_mux.unlock();
		return list;
	}

private:
	_MutexType _mux;
	std::unordered_map<_Key, _Value> _map;
};