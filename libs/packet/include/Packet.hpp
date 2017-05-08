#ifndef PACKET_HPP_
#define PACKET_HPP_

#include <memory>
#include <type_traits>
#include <cstring>
#include "IPacket.hpp"
#include "ISerializable.hpp"

template <typename T>
class Packet : public IPacket
{
  // Small check
  static_assert(std::is_base_of<ISerializable, T>(),
                "Type is not an ISerializable");

public:
  Packet() : m_size(0), m_data()
  {
  }

  Packet(Packet const &other)
      : m_size(other.m_size), m_data(new std::uint8_t[m_size])
  {
    std::memcpy(m_data.get(), other.m_data.get(), m_size);
  }

  Packet(Packet &&other)
      : m_size(other.m_size), m_data(std::move(other.m_data))
  {
    other.m_size = 0;
  }

  Packet(T const &obj)
  {
    this->fillData(obj);
  }

  virtual ~Packet()
  {
  }

  Packet &operator=(Packet const &other)
  {
    if (this == &other)
      return (*this);
    m_size = other.m_size;
    m_data = std::make_unique<std::uint8_t[]>(m_size);
    std::memcpy(m_data.get(), other.m_data.get(), m_size);
    return (*this);
  }

  Packet &operator=(Packet &&other)
  {
    if (this == &other)
      return (*this);
    m_size = other.m_size;
    m_data = std::move(m_data);
    other.m_size = 0;
    return (*this);
  }

  Packet &operator=(T const &obj)
  {
    this->fillData(obj);
  }

  void fillPacket(T const &obj)
  {
    m_size = sizeof(m_header);
    m_data = obj.serialize(m_size);

    PacketHeader *header = reinterpret_cast<PacketHeader *>(m_data.get());

    header->magic.__data._magic = 0x1D;
    header->magic.__data.vers = 0; // TODO: Change version here
    header->magic.magic = htonl(header->magic.magic);

    header->size = htonl(m_size - sizeof(m_header));

    // TODO: calculate checksum
    header->checkSum = 0;
    header->checkSum = htonl(header->checkSum);
  }

  void getPacket(T &obj)
  {
    // TODO: Fill header here
    obj.deserialize(m_size, m_data.get());
  }

  Packet &operator<<(T const &obj)
  {
    this->fillPacket(obj);
    return (*this);
  }

  Packet &operator>>(T &obj)
  {
    this->getPacket(obj);
    return (*this);
  }

  virtual std::size_t getSize() const
  {
    return (m_size);
  }

  virtual std::uint8_t *getData() const
  {
    return (m_data.get());
  }

  virtual void setData(std::size_t size, std::unique_ptr<std::uint8_t[]> data)
  {
    m_size = size;
    m_data = std::move(data);
  }

private:
  std::size_t                     m_size;
  std::unique_ptr<std::uint8_t[]> m_data;
  PacketHeader                    m_header;
};

#endif // !PACKET_HPP_
