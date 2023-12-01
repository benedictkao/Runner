#pragma once

#include <stdint.h>

namespace messages
{
	enum class Type : uint32_t
	{
		PING, LOGIN, CHAT_MSG
	};

	namespace body
	{
		struct Login
		{
			int id;
		};

		struct ChatMsg
		{
			int id;
			char message[64] = { 0 };	// message has max length of 64 chars
		};
	}

	template <Type type, typename V>
	struct TypedMessage
	{
		static constexpr Type getType()
		{
			return type;
		}

		V body;
	};

	// define typed messages here
	typedef TypedMessage<Type::LOGIN, body::Login> LoginData;
	typedef TypedMessage<Type::CHAT_MSG, body::ChatMsg> ChatMsgData;
}