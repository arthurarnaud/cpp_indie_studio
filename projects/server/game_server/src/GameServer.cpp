#include "game_server_stdafx.hpp"

GameServer::GameServer(std::string const & connectManagerIp,
                       std::uint16_t const cmPort, std::uint16_t const gsPort,
                       std::int32_t const maxClients)
    : m_connectManagerPort(cmPort), m_gameServerPort(gsPort),
      m_maxClients(maxClients), m_curClients(0), m_licence(),
      m_connectManagerSock(m_connectManagerPort, connectManagerIp, true,
                           network::ASocket::SocketType::BLOCKING),
      m_gameSock(
          m_gameServerPort, static_cast<std::uint32_t>(m_maxClients),
          network::ASocket::SocketType::BLOCKING), // TODO: Non-blocking ?
      m_connectSrv(), m_gameSrvTCP(), m_gameSrvUDP(), m_clientList(),
      m_tokenList()
{
  std::ifstream licenceFile(".license");

  if (!licenceFile.is_open())
    {
      nope::log::Log(Error) << "Cannot open file : .license";
      throw std::exception(); // TODO: Real exception
    }
  std::getline(licenceFile, m_licence);
  nope::log::Log(Debug) << "Loaded licence " << m_licence;
  licenceFile.close();

  // Various pre-allocations
  m_clientList.reserve(static_cast<std::size_t>(m_maxClients));
  m_tokenList.reserve(static_cast<std::size_t>(m_maxClients));
}

GameServer::~GameServer()
{
}

bool GameServer::run()
{
  nope::log::Log(Debug) << "Running Game Server";
  if (m_connectManagerSock.openConnection() && authenticateToConnectManager())
    {
      // Authenticated to connectManager, shall start gameClient server
      if (m_gameSock.openConnection())
	{
	  nope::log::Log(Info) << "Server started";
	  m_connectSrv = std::thread(&GameServer::connectManagerCom, this);
	  m_gameSrvTCP = std::thread(&GameServer::gameServerTCP, this);
	  m_gameSrvUDP = std::thread(&GameServer::gameServerUDP, this);
	  return (true);
	}
    }
  nope::log::Log(Error) << "Cannot start game server";
  return (false);
}

void GameServer::stop()
{
  nope::log::Log(Info) << "Stopping game servers";
  if (m_connectSrv.joinable())
    {
      m_connectSrv.join();
    }
  if (m_gameSrvTCP.joinable())
    {
      m_gameSrvTCP.join();
    }
  if (m_gameSrvUDP.joinable())
    {
      m_gameSrvUDP.join();
    }
  nope::log::Log(Info) << "Game servers stopped.";
}
