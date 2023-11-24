#include "buffer.h"

static constexpr auto BUFFER_SIZE{ 128 };

network::Buffer::Buffer()
{
	_internalBuffer.reserve(BUFFER_SIZE);
}

void network::Buffer::fill(const void* data, size_t size)
{
	_internalBuffer.resize(size);
	std::memcpy(_internalBuffer.data(), data, size);
}

const void* network::Buffer::data() const
{
	return static_cast<const void*>(_internalBuffer.data());
}

size_t network::Buffer::size() const
{
	return static_cast<uint32_t>(_internalBuffer.size());
}

void network::Buffer::clear()
{
	_internalBuffer.clear();
}
