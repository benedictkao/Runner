#pragma once

#include <mutex>
#include <queue>

struct message {
	char Buffer[128];	// can send up to 128 bytes of data
	int size;
};

class message_queue {

private:
	std::mutex _mux;
	std::queue<message> _queue;

public:
	message popFront();
	void push(const message&);
	bool empty() const;
};