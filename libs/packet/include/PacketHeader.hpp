#ifndef PACKET_HEADER_HPP
#define PACKET_HEADER_HPP

#include <cstdint>

struct PacketHeader
{
  union
  {
    std::uint16_t magic;
    struct
    {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      std::uint8_t vers;
      std::uint8_t _magic;
#else
      std::uint8_t _magic;
      std::uint8_t vers;
#endif
    } __data;
  } magic;

  std::uint16_t size;
  std::uint16_t checkSum;

  // Methods
  inline uint8_t getVersion() const
  {
    return (magic.__data.vers);
  }

  inline uint8_t getMagic() const
  {
    return (magic.__data._magic);
  }

  constexpr static std::uint8_t Magic = 0x1D;
  constexpr static std::uint8_t Version = 0;
};

static_assert(sizeof(PacketHeader) == sizeof(std::uint16_t) * 3,
              "PacketHeader is padded");

#endif // !PACKET_HEADER_HPP_
