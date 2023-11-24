#pragma once

namespace messages 
{
	enum class Type : uint32_t
	{
		LOGIN, CHAT_MSG
	};

	namespace body 
	{

		struct Login 
		{
			int id;
		};

		struct ChatMsg
		{
			ENetPeer* sender;
			char message[64] = { 0 };	// message has max length of 64 chars
		};
	}
}