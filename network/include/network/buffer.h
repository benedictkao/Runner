#pragma once

#include <vector>
#include <stdint.h>

namespace network {

	class Buffer {

	public:
		Buffer();
		Buffer(int initialCapacity);

		Buffer(const Buffer&);
		Buffer(Buffer&&) noexcept;

		Buffer& operator=(const Buffer&);
		Buffer& operator=(Buffer&&) noexcept;

		~Buffer() = default;

	public:
		template <typename _Data>
		Buffer& write(const _Data& data);

		template <typename _Data>
		Buffer& read(_Data& data);

		void fill(const void* data, size_t size);

		const void* data() const;
		size_t size() const;
		void clear();

	private:
		std::vector<char> _internalBuffer;
	};
}

template <typename _Data>
network::Buffer& network::Buffer::write(const _Data& data)
{
	static_assert(std::is_standard_layout<_Data>::value, "Data is too complex");
	size_t currSize = _internalBuffer.size();
	size_t writeSize = sizeof(_Data);
	_internalBuffer.resize(currSize + writeSize);
	std::memcpy(_internalBuffer.data() + currSize, &data, writeSize);
	return *this;
}

template <typename _Data>
network::Buffer& network::Buffer::read(_Data& data)
{
	static_assert(std::is_standard_layout<_Data>::value, "Data is too complex");
	size_t currSize = _internalBuffer.size();
	size_t readSize = sizeof(_Data);
	size_t newSize = currSize - readSize;

	// if (newSize < 0)
	// 	return;
				
	std::memcpy(&data, _internalBuffer.data() + newSize, readSize);
	_internalBuffer.resize(newSize);
	return *this;
}