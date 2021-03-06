#include "pakpak_stdafx.hpp"

#if defined __GNUC__ && !defined __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

namespace core
{
  NetworkAuth::NetworkAuth(std::string const &ip, std::uint16_t const port)
      : m_ip(ip), m_port(port), m_auth(false),
        m_sock(m_port, m_ip, true, network::ASocket::SocketType::BLOCKING),
        m_token()
  {
    nope::log::Log(Debug) << "Creating connection [NetworkAuth]";
    nope::log::Log(Info) << "Opening connection";
    if (!m_sock.openConnection())
      {
	nope::log::Log(Error) << "Cannot connect to " << ip << ":" << port;
	throw NetworkConnectionError("Cannot connect to " + ip + ":" +
	                             std::to_string(port));
      }
  }

  NetworkAuth::~NetworkAuth()
  {
    nope::log::Log(Debug) << "Destroying connection [NetworkAuth]";
  }

  bool NetworkAuth::disconnect()
  {
    // TODO
    return (false);
  }

  network::IClient::ClientAction NetworkAuth::write(IPacket const &pck)
  {
    network::IClient::ClientAction ret =
        network::IClient::ClientAction::SUCCESS;
    std::size_t const   sizeToWrite = pck.getSize();
    std::uint8_t const *data = pck.getData();

    if (m_sock.send(data, sizeToWrite) == false)
      {
	nope::log::Log(Debug) << "Failed to write data [NetworkAuth]";
	ret = network::IClient::ClientAction::FAILURE;
      }
    return (ret);
  }

  network::IClient::ClientAction NetworkAuth::read(IPacket &pck)
  {
    network::IClient::ClientAction ret =
        network::IClient::ClientAction::FAILURE;

    // Allocate buffer
    std::unique_ptr<std::uint8_t[]> buff =
        std::make_unique<std::uint8_t[]>(packetSize::GameClientToCMPacketSize);
    ssize_t buffLen = 0;
    ssize_t headerLen = 0;

    // Read header first
    if (m_sock.rec(buff.get(), sizeof(PacketHeader), &headerLen))
      {
	assert(headerLen >= 0);
	if (headerLen == 0)
	  {
	    nope::log::Log(Debug)
	        << "Read failed, shall disconnect [NetworkAuth]";
	    ret = network::IClient::ClientAction::DISCONNECT;
	  }
	else
	  {
	    nope::log::Log(Debug)
	        << "Received header, checking it [NetworkAuth]";
	    // Check header
	    PacketHeader *header =
	        reinterpret_cast<PacketHeader *>(buff.get());

	    header->magic.magic = ntohs(header->magic.magic);
	    std::uint8_t vers = header->getVersion();
	    std::uint8_t magic = header->getMagic();
	    header->magic.magic = htons(header->magic.magic);
	    if (vers == PacketHeader::Version && magic == PacketHeader::Magic)
	      {
		// Get size to read
		std::uint16_t sizeToRead = ntohs(header->size);

		nope::log::Log(Debug)
		    << "Should read " << sizeToRead << " [NetworkAuth]";
		// Read rest of the packet
		if (m_sock.rec(buff.get() + headerLen, sizeToRead, &buffLen))
		  {
		    assert(buffLen >= 0);
		    if (buffLen == 0)
		      {
			nope::log::Log(Debug)
			    << "Read failed, shall disconnect [NetworkAuth]";
			ret = network::IClient::ClientAction::DISCONNECT;
		      }
		    else
		      {
			ret = network::IClient::ClientAction::SUCCESS;
			pck.setData(
			    static_cast<std::size_t>(buffLen + headerLen),
			    std::move(buff));
		      }
		  }
	      }
	  }
      }
    return (ret);
  }

  bool NetworkAuth::hasTimedOut() const
  {
    // TODO
    return (false);
  }

  bool NetworkAuth::isAuthenticated() const
  {
    return (m_auth);
  }

  std::vector<GameServer> NetworkAuth::getServerList()
  {
    std::vector<GameServer>        serverList = {};
    Packet<GameClientToCMPacket>   pck = {};
    network::IClient::ClientAction ret;
    GameClientToCMPacket           pckContent = {};

    // Send a request event
    pckContent.pck.eventType = GameClientToCMEvent::REQUEST_EVENT;
    pckContent.pck.eventData.intEvent.event = static_cast<std::uint16_t>(
        GameClientToCMPacketSimpleEvent::LIST_SERVERS);
    pck << pckContent;
    ret = write(pck);
    if (ret != network::IClient::ClientAction::SUCCESS)
      {
	nope::log::Log(Error) << "Cannot write packet to server.";
	throw NetworkWritePacketError("Cannot write packet to server.");
      }

    // Read response
    nope::log::Log(Debug) << "Reading packet";
    ret = read(pck);
    if (ret != network::IClient::ClientAction::SUCCESS)
      {
	nope::log::Log(Error) << "Cannot read packet from server.";
	throw NetworkReadPacketError("Cannot read packet from server.");
      }
    pck >> pckContent;
    nope::log::Log(Debug) << "Read successful !";

    // Check response
    if (pckContent.pck.eventType != GameClientToCMEvent::LIST_SERVERS_EVENT)
      {
	nope::log::Log(Error) << "Received invalid packet from server.";
	nope::log::Log(Debug) << "Invalid Packet: invalid event";
	throw NetworkInvalidPacketError(
	    "Received invalid packet from server.");
      }

    // Process response
    nope::log::Log(Info) << "Servers: "
                         << pckContent.pck.eventData.serverList.nbServers;
    for (std::int32_t i = 0; i < pckContent.pck.eventData.serverList.nbServers;
         ++i)
      {
	GameClientToCMPacketStatus const &cur =
	    pckContent.pck.eventData.serverList
	        .servers[static_cast<std::size_t>(i)];

	serverList.push_back(GameServer(std::string(cur.ip.data.data()),
	                                cur.port, cur.currentClients,
	                                cur.maxClients));
	nope::log::Log(Debug)
	    << "Server #" << i << ": " << std::string(cur.ip.data.data())
	    << ":" << cur.port << " [ " << cur.currentClients << " / "
	    << cur.maxClients << " ]";
      }

    // Return data
    nope::log::Log(Info) << "Found " << serverList.size() << " servers.";
    return (serverList);
  }

  std::string const &NetworkAuth::getToken(GameServer const &srv)
  {
    network::IClient::ClientAction ret;
    Packet<GameClientToCMPacket>   pck = {};
    GameClientToCMPacket           pckContent = {};

    m_token = "";
    nope::log::Log(Debug) << "Requesting Token to server";
    pckContent.pck.eventType = GameClientToCMEvent::REQUEST_EVENT;
    pckContent.pck.eventData.intEvent.event =
        static_cast<std::uint16_t>(GameClientToCMPacketSimpleEvent::GET_TOKEN);
    assert(pckContent.pck.eventData.intEvent.event ==
           pckContent.pck.eventData.tokenRequ.ev.event);
    if (srv.address.length() >= INET6_ADDRSTRLEN_INDIE)
      {
	nope::log::Log(Error) << "Invalid address length";
	throw NetworkInvalidAddressError("Invalid address length");
      }
    std::memcpy(pckContent.pck.eventData.tokenRequ.ip.data.data(),
                srv.address.c_str(), srv.address.length() + 1);
    pckContent.pck.eventData.tokenRequ.port = srv.port;
    nope::log::Log(Debug) << "Built packet";
    pck << pckContent;
    ret = write(pck);
    if (ret != network::IClient::ClientAction::SUCCESS)
      {
	nope::log::Log(Error) << "Cannot send packet to server.";
	throw NetworkConnectionError("Cannot send packet to server.");
      }
    nope::log::Log(Debug) << "Packet sent";

    // Getting token
    nope::log::Log(Debug) << "Reading packet";
    ret = read(pck);
    if (ret != network::IClient::ClientAction::SUCCESS)
      {
	nope::log::Log(Error) << "Cannot read packet from server.";
	throw NetworkReadPacketError("Cannot read packet from server.");
      }
    pck >> pckContent;
    nope::log::Log(Debug) << "Read successful !";
    if (pckContent.pck.eventType != GameClientToCMEvent::GET_TOKEN_EVENT)
      {
	nope::log::Log(Error) << "Received invalid packet from server.";
	nope::log::Log(Debug) << "Invalid Packet: invalid event";
	throw NetworkInvalidPacketError(
	    "Received invalid packet from server.");
      }

    nope::log::Log(Debug) << "TokenValid: "
                          << pckContent.pck.eventData.token.valid;
    m_token = std::string(pckContent.pck.eventData.token.data.data());
    nope::log::Log(Debug) << "TokenData: " << std::string();
    if (pckContent.pck.eventData.token.valid != 1)
      {
	nope::log::Log(Error) << "Received invalid packet from server.";
	nope::log::Log(Debug) << "Invalid Packet: invalid token";
	m_token = "";
	throw NetworkInvalidPacketError(
	    "Received invalid packet from server.");
      }
    nope::log::Log(Debug) << "Successfuly got token.";
    return (m_token);
  }
}

#if defined __GNUC__ && !defined __clang__
#pragma GCC diagnostic pop
#endif
