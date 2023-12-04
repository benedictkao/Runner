#pragma once

class NullMutex
{
public:
	void lock() {}
	void unlock() {}
};