#ifndef GAMESERVER_HPP_
#define GAMESERVER_HPP_

#include <string>
#include <cstdint>
#include <thread>
#include "TCPSocket.hpp"
#include "IServer.hpp"
#include "IClient.hpp"
#include "Token.hpp"
#include "GameClient.hpp"

// Disable clang warning for implicit padding
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

class GameServer : public network::IServer, public network::IClient
{
public:
  explicit GameServer(std::string const & connectManagerIp,
                      std::uint16_t const cmPort, std::uint16_t const gsPort,
                      std::int32_t const maxClients);
  virtual ~GameServer();

  // IServer
  virtual bool run();
  virtual void stop();

  // TCP
  virtual bool addClient();
  virtual bool removeClient(network::IClient &);

  // IClient
  virtual bool                           disconnect();
  virtual network::IClient::ClientAction write(IPacket const &);
  virtual network::IClient::ClientAction read(IPacket &);
  virtual bool                           hasTimedOut() const;

private:
  // ConnectManager methods
  bool         authenticateToConnectManager();
  void         connectManagerCom();
  std::int32_t connectManagerComActivity(std::int32_t const sock,
                                         fd_set &readfds, fd_set &writefds,
                                         fd_set &exceptfds, bool canWrite);
  network::IClient::ClientAction connectManagerComTreatInput(bool &canWrite);
  network::IClient::ClientAction connectManagerComTreatOutput(bool &canWrite);

  // GameServerTCP methods
  void         gameServerTCP();
  std::int32_t gameServerTCPActivity(std::int32_t const sock, fd_set &readfds,
                                     fd_set &writefds, fd_set &exceptfds,
                                     bool canWrite);
  std::int32_t gameServerTCPIO(std::int32_t const sock, fd_set &readfds,
                               fd_set &writefds, fd_set &exceptfds,
                               bool canWrite);

  // GameServerUDP methods
  void gameServerUDP();

  // Basic datas
  std::uint16_t      m_connectManagerPort;
  std::uint16_t      m_gameServerPort;
  std::int32_t const m_maxClients;
  std::int32_t       m_curClients;
  std::string        m_licence;

  // Sockets and servers
  network::TCPSocket m_connectManagerSock;
  network::TCPSocket m_gameSock;
  std::thread        m_connectSrv;
  std::thread        m_gameSrvTCP;
  std::thread        m_gameSrvUDP;

  // Tokens
  std::vector<GameClient> m_clientList;
  std::vector<Token>      m_tokenList;
};

// Disable clang warning for implicit padding
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif // !GAMESERVER_HPP_
