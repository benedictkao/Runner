#include "network/buffer.h"

#include <utility>

network::Buffer::Buffer() {}

network::Buffer::Buffer(int initialCapacity)
{
	_internalBuffer.reserve(initialCapacity);
}

network::Buffer::Buffer(const Buffer & buffer): _internalBuffer(buffer._internalBuffer) {}

network::Buffer::Buffer(Buffer && buffer) noexcept: _internalBuffer(std::move(buffer._internalBuffer)) {}

network::Buffer& network::Buffer::operator=(const Buffer& buffer)
{
	_internalBuffer = buffer._internalBuffer;
	return *this;
}

network::Buffer& network::Buffer::operator=(Buffer&& buffer) noexcept
{
	_internalBuffer = std::move(buffer._internalBuffer);
	return *this;
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
