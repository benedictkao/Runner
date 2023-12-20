template <typename _Connection, typename _Mutex>
bool network::EventReader::read(
	_Connection& connection, 
	ConcurrentQueue<InMessage, _Mutex>& inQueue,
	int timeout
)
{
	ENetHost* host = connection.getHost();
	while (auto event = readNext(host, timeout))
	{
		switch (event->type)
		{
			case ENET_EVENT_TYPE_CONNECT:
				connection.onConnected(event);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				connection.onReceive(event);
				InMessage msg = { event->peer };
				msg.raw.fill(event->packet->data, event->packet->dataLength);
				inQueue.push(std::move(msg));
			}
				break;
			case ENET_EVENT_TYPE_DISCONNECT: 
			{
				if (connection.onDisconnected(event)) // return true means we should stop reading
					return true;
				break;
			}
			default:
				break;
		}
	}
	return false;
}

inline ENetEvent* network::EventReader::readNext(ENetHost* host, int timeout)
{
	return enet_host_service(host, &_event, timeout) > 0 ? &_event : nullptr;
}