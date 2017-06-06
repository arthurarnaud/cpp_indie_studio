#include "GameClient.hpp"

GameClient::GameClient(sock_t const fd, std::vector<Token> &tokenList,
                       std::size_t const ndx)
    : m_sock(fd), m_canWrite(false), m_state(GameClient::State::CONNECTED),
      m_packet(), m_tokenList(tokenList), m_id(static_cast<std::int32_t>(ndx))
{
}

GameClient::~GameClient()
{
}

bool GameClient::disconnect()
{
  nope::log::Log(Debug) << "GameClient disconnected #" << getSocket();
  m_sock.closeConnection();
  return (true);
}

network::IClient::ClientAction GameClient::write(IPacket const &pck)
{
  network::IClient::ClientAction ret = network::IClient::ClientAction::SUCCESS;
  std::size_t const              sizeToWrite = pck.getSize();
  std::uint8_t const *           data = pck.getData();

  if (m_sock.send(data, sizeToWrite) == false)
    {
      nope::log::Log(Debug) << "Failed to write data [GameClient]";
      ret = network::IClient::ClientAction::FAILURE;
    }
  return (ret);
}

network::IClient::ClientAction GameClient::read(IPacket &pck)
{
  network::IClient::ClientAction ret = network::IClient::ClientAction::FAILURE;
  std::size_t const buffSize = packetSize::GameClientToGSPacketSize;

  // Allocate buffer
  std::unique_ptr<uint8_t[]> buff = std::make_unique<std::uint8_t[]>(buffSize);
  ssize_t                    buffLen = 0;

  if (m_sock.rec(buff.get(), buffSize, &buffLen))
    {
      assert(buffLen >= 0);
      if (buffLen == 0)
	{
	  nope::log::Log(Debug)
	      << "Read failed, shall disconnect [GameClient]";
	  ret = network::IClient::ClientAction::DISCONNECT;
	}
      else
	{
	  ret = network::IClient::ClientAction::SUCCESS;
	  pck.setData(static_cast<std::size_t>(buffLen), std::move(buff));
	}
    }
  return (ret);
}

bool GameClient::hasTimedOut() const
{
  // TODO
  return (false);
}

sock_t GameClient::getSocket() const
{
  return (m_sock.getSocket());
}

bool GameClient::canWrite() const
{
  return (m_canWrite);
}

void GameClient::toggleWrite()
{
  m_canWrite = !m_canWrite;
}

std::int32_t GameClient::getId() const
{
  return (m_id);
}

network::IClient::ClientAction GameClient::treatIncomingData()
{
  network::IClient::ClientAction ret = network::IClient::ClientAction::FAILURE;
  GameClientToGSPacket           rep;

  switch (m_state)
    {
    case State::CONNECTED:
      nope::log::Log(Debug) << "Reading in state CONNECTED [GameClient]";
      ret = read(m_packet);
      if (ret == network::IClient::ClientAction::SUCCESS)
	{
	  m_packet >> rep;

	  // The only event allowed here is TOKEN_EVENT
	  if (rep.pck.eventType == GameClientToGSEvent::TOKEN_EVENT)
	    {
	      std::string tokenStr =
	          std::string(rep.pck.eventData.token.data.data(), 40);
	      nope::log::Log(Debug) << "Received Token: " << tokenStr;
	      ret = network::IClient::ClientAction::FAILURE;
	      for (Token const &tok : m_tokenList)
		{
		  nope::log::Log(Debug) << "Is it ? -> " << tok.getToken();
		  if (tok == tokenStr)
		    {
		      nope::log::Log(Debug) << "Found token in token list";
		      if (tok.isValid())
			{
			  nope::log::Log(Debug) << "Token is valid";
			  m_state = State::AUTHENTICATING;
			  ret = network::IClient::ClientAction::SUCCESS;
			}
		      // Found it, now remove it from the list
		      m_tokenList.erase(std::remove(m_tokenList.begin(),
		                                    m_tokenList.end(), tok),
		                        m_tokenList.end());
		      break;
		    }
		}
	    }
	}
      break;
    case State::AUTHENTICATING:
      break;
    case State::AUTHENTICATED:
      break;
    }
  if (ret == network::IClient::ClientAction::SUCCESS)
    {
      toggleWrite();
    }
  return (ret);
}

network::IClient::ClientAction GameClient::treatOutgoingData()
{
  network::IClient::ClientAction ret = network::IClient::ClientAction::FAILURE;
  GameClientToGSPacket           rep;

  switch (m_state)
    {
    case State::CONNECTED:
      break;
    case State::AUTHENTICATING:
      // Notify the client that it is authenticated
      rep.pck.eventType = GameClientToGSEvent::VALIDATION_EVENT;
      rep.pck.eventData.valid = 1;

      m_packet << rep;
      ret = write(m_packet);
      m_state = State::AUTHENTICATED;
      break;
    case State::AUTHENTICATED:
      break;
    }
  if (ret == network::IClient::ClientAction::SUCCESS)
    {
      toggleWrite();
    }
  return (ret);
}

bool GameClient::operator==(GameClient const &other) const
{
  if (this != &other)
    {
      return (m_sock == other.m_sock);
    }
  return (true);
}
