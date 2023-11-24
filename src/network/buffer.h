#pragma once

#include <vector>
#include <cstring>
#include <stdint.h>

namespace network {

	class Buffer {

	public:
		Buffer();

	public:
		template <typename T>
		void write(const T& data) 
		{
			static_assert(std::is_standard_layout<T>::value, "Data is too complex");

			size_t currSize = _internalBuffer.size();
			size_t writeSize = sizeof(T);
			_internalBuffer.resize(currSize + writeSize);
			std::memcpy(_internalBuffer.data() + currSize, &data, writeSize);
		}

		template <typename T>
		void read(T& data)
		{
			static_assert(std::is_standard_layout<T>::value, "Data is too complex");

			size_t currSize = _internalBuffer.size();
			size_t readSize = sizeof(T);
			size_t newSize = currSize - readSize;
			std::memcpy(&data, _internalBuffer.data() + newSize, readSize);
			_internalBuffer.resize(newSize);
		}

		void fill(const void* data, size_t size);

		const void* data() const;
		size_t size() const;
		void clear();

	private:
		std::vector<char> _internalBuffer;
	};
}