#pragma once

namespace messages 
{
	enum class Type : uint32_t
	{
		PING, LOGIN, CHAT_MSG
	};

	namespace body 
	{
		struct Ping
		{
			// void test() {
			// 	auto t = std::chrono::system_clock::now().time_since_epoch();
			// 	uint64_t t2 = std::chrono::duration_cast<std::chrono::milliseconds>(t).count();
			// }
			uint64_t timeInMillis;
		};

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
	typedef TypedMessage<Type::PING, body::Ping> PingData;
}