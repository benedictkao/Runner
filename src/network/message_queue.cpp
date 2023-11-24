#include "message_queue.h"

message message_queue::popFront()
{
	std::lock_guard(_mux);
	message m = _queue.front();
	_queue.pop();
	return m;
}

void message_queue::push(const message& m)
{
	std::lock_guard(_mux);
	_queue.push(m);
}

bool message_queue::empty() const
{
	// no need to lock here because popFront() is called in the same thread, even if there is concurrency
	// issue with push(), we will get the new message in the next loop iteration anyway
	return _queue.empty();
}