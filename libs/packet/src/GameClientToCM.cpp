#include "packet_stdafx.hpp"

constexpr std::int32_t GameClientToCMPacketServerList::maxServers;
constexpr std::size_t  GameClientToCMPacketToken::tokenLength;

GameClientToCMPacket::GameClientToCMPacket() : pck(), __padding()
{
}

std::unique_ptr<std::uint8_t[]>
    GameClientToCMPacket::serialize(std::size_t &sizeToFill) const
{
  // Set up cursor
  std::size_t cursor = sizeToFill;
  sizeToFill += sizeof(GameClientToCMPacket);

  // Allocate buffer
  std::unique_ptr<std::uint8_t[]> serial =
      std::make_unique<std::uint8_t[]>(sizeToFill);

  // Fill buffer
  std::memcpy(&serial[cursor], &pck, sizeof(GameClientToCMPacketRaw));

  GameClientToCMPacketRaw *data =
      reinterpret_cast<GameClientToCMPacketRaw *>(&serial[cursor]);
  data->eventType = static_cast<GameClientToCMEvent>(
      htons(static_cast<std::uint16_t>(data->eventType)));

  if (pck.eventType == GameClientToCMEvent::REQUEST_EVENT)
    {
      if (data->eventData.intEvent.event ==
          static_cast<std::uint16_t>(
              GameClientToCMPacketSimpleEvent::GET_TOKEN))
	{
	  // There are a few more datas to serialize
	  data->eventData.tokenRequ.port =
	      htons(data->eventData.tokenRequ.port);
	}
      data->eventData.intEvent.event = htons(data->eventData.intEvent.event);
    }
  else if (pck.eventType == GameClientToCMEvent::LIST_SERVERS_EVENT)
    {
      data->eventData.serverList.nbServers =
          std::min<std::int32_t>(data->eventData.serverList.nbServers,
                                 GameClientToCMPacketServerList::maxServers);
      for (std::int32_t i = 0; i < data->eventData.serverList.nbServers; ++i)
	{
	  GameClientToCMPacketStatus &cur =
	      data->eventData.serverList.servers[static_cast<std::size_t>(i)];

	  cur.port = htons(cur.port);
	  cur.currentClients = htons(cur.currentClients);
	  cur.maxClients = htons(cur.maxClients);
	}
      data->eventData.serverList.nbServers = static_cast<std::int32_t>(htonl(
          static_cast<std::uint32_t>(data->eventData.serverList.nbServers)));
    }
  else if (pck.eventType == GameClientToCMEvent::GET_TOKEN_EVENT)
    {
      data->eventData.token.valid = htons(data->eventData.token.valid);
    }

  return (serial);
}

void GameClientToCMPacket::deserialize(std::size_t, std::uint8_t *data)
{
  std::memcpy(&pck, data, sizeof(pck));

  pck.eventType = static_cast<GameClientToCMEvent>(
      ntohs(static_cast<std::uint16_t>(pck.eventType)));
  if (pck.eventType == GameClientToCMEvent::REQUEST_EVENT)
    {
      pck.eventData.intEvent.event = ntohs(pck.eventData.intEvent.event);
      if (pck.eventData.intEvent.event ==
          static_cast<std::uint16_t>(
              GameClientToCMPacketSimpleEvent::GET_TOKEN))
	{
	  // There are a few more datas to deserialize
	  pck.eventData.tokenRequ.port = ntohs(pck.eventData.tokenRequ.port);
	}
    }
  else if (pck.eventType == GameClientToCMEvent::LIST_SERVERS_EVENT)
    {
      pck.eventData.serverList.nbServers = static_cast<std::int32_t>(ntohl(
          static_cast<std::uint32_t>(pck.eventData.serverList.nbServers)));

      for (std::int32_t i = 0; i < pck.eventData.serverList.nbServers; ++i)
	{
	  GameClientToCMPacketStatus &cur =
	      pck.eventData.serverList.servers[static_cast<std::size_t>(i)];

	  cur.port = ntohs(cur.port);
	  cur.currentClients = ntohs(cur.currentClients);
	  cur.maxClients = ntohs(cur.maxClients);
	}
      assert(pck.eventData.serverList.nbServers <=
             GameClientToCMPacketServerList::maxServers);
    }
  else if (pck.eventType == GameClientToCMEvent::GET_TOKEN_EVENT)
    {
      pck.eventData.token.valid = ntohs(pck.eventData.token.valid);
    }
  else
    {
      nope::log::Log(Error)
          << "Received Event: " << static_cast<std::int32_t>(pck.eventType);
      throw std::runtime_error("Invalid packet received");
    }
}
