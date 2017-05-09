#ifndef GAMESERVER_CM_PACKET_HPP_
#define GAMESERVER_CM_PACKET_HPP_

#include <cstdint>
#include <cstddef>
#include <array>
#include "ISerializable.hpp"
#include "PacketHeader.hpp"

enum GameServerToCMEvent : std::uint16_t
{
  STRINGIFIED_EVENT = 0,
  LICENCE_EVENT,
  NB_CLIENTS
};

struct GameServerToCMPacketSimple
{
  constexpr static std::size_t buffSize = 256;
  std::array<char, buffSize> data;
};

struct GameServerToCMPacketLicence
{
  GameServerToCMPacketSimple license;
  std::uint16_t              port;
};

struct GameServerToCMPacketRaw
{
  GameServerToCMEvent eventType;
  union
  {
    GameServerToCMPacketSimple  string;
    GameServerToCMPacketLicence licence;
    uint16_t                    nbClients;
  } eventData;
};

struct GameServerToCMPacket : public ISerializable
{
  GameServerToCMPacketRaw pck;

  virtual std::unique_ptr<std::uint8_t[]>
      serialize(std::size_t &sizeToFill) const;

  virtual void deserialize(std::size_t size, std::uint8_t *data);

private:
  std::array<std::uint8_t, 4> __padding;
};

#endif // !GAMESERVER_CM_PACKET_HPP_