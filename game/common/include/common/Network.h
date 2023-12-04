#pragma once

#include <stdint.h>

namespace net
{
	static constexpr int PORT{ 7777 };

	namespace messages
	{
		enum class Type : uint32_t
		{
			PING
		};
	}
}