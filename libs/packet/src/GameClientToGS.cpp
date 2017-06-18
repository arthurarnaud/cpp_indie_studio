#include "packet_stdafx.hpp"

constexpr std::size_t GameClientToGSPacketToken::tokenLength;

GameClientToGSPacket::GameClientToGSPacket() : pck()
{
}

std::unique_ptr<std::uint8_t[]>
    GameClientToGSPacket::serialize(std::size_t &sizeToFill) const
{
  // Set up cursor
  std::size_t cursor = sizeToFill;
  sizeToFill += sizeof(GameClientToGSPacket);

  // Allocate buffer
  std::unique_ptr<std::uint8_t[]> serial =
      std::make_unique<std::uint8_t[]>(sizeToFill);

  // Fill buffer
  std::memcpy(&serial[cursor], &pck, sizeof(GameClientToGSPacketRaw));

  GameClientToGSPacketRaw *data =
      reinterpret_cast<GameClientToGSPacketRaw *>(&serial[cursor]);
  data->eventType = static_cast<GameClientToGSEvent>(
      htons(static_cast<std::uint16_t>(data->eventType)));

  if (pck.eventType == GameClientToGSEvent::VALIDATION_EVENT)
    {
      data->eventData.valid = htons(pck.eventData.valid);
    }
  else if (pck.eventType == GameClientToGSEvent::FILE_EVENT)
    {
      data->eventData.file.len = htonl(pck.eventData.file.len);
    }

  return (serial);
}

void GameClientToGSPacket::deserialize(std::size_t, std::uint8_t *data)
{
  std::memcpy(&pck, data, sizeof(pck));

  pck.eventType = static_cast<GameClientToGSEvent>(
      ntohs(static_cast<std::uint16_t>(pck.eventType)));
  if (pck.eventType == GameClientToGSEvent::TOKEN_EVENT)
    {
    }
  else if (pck.eventType == GameClientToGSEvent::VALIDATION_EVENT)
    {
      pck.eventData.valid = ntohs(pck.eventData.valid);
    }
  else if (pck.eventType == GameClientToGSEvent::MD5_REQUEST ||
           pck.eventType == GameClientToGSEvent::MD5_RESPONSE)
    {
    }
  else if (pck.eventType == GameClientToGSEvent::FILE_EVENT)
    {
      pck.eventData.file.len = ntohl(pck.eventData.file.len);
    }
  else
    {
      throw std::runtime_error("Invalid packet received");
    }
}