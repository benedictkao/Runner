#pragma once

#include <stdint.h>

namespace common
{
	static constexpr int PORT_NUMBER{ 7777 };

	namespace messages
	{
		enum class Type : uint32_t
		{
			PING
		};
	}
}