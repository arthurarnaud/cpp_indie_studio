#ifndef LICENSE_SERVER_HPP_
#define LICENSE_SERVER_HPP_

#include <memory>
#include "IServer.hpp"
#include "TCPSocket.hpp"

class LicenseServer : public network::IServer
{
public:
  explicit LicenseServer(std::uint16_t const port,
                         std::uint16_t const gameServerPort);
  virtual ~LicenseServer();

  virtual bool run();
  virtual void stop();
  virtual bool addClient();
  virtual bool removeClient(network::IClient &);

  void waitSignal();

private:
  void         _loop();
  bool         loadLicenses();
  std::int32_t checkActivity(fd_set &readfds, fd_set &writefds,
                             fd_set &   exceptfds,
                             bool const monitorLicenseServer);

  constexpr static std::uint32_t maxGameServer = 64;

  network::TCPSocket m_license;
  network::TCPSocket m_gameServer;

  class License
  {
  public:
    explicit License(std::string const &nb);

  private:
  };

  class GameServer : public network::IClient
  {
  public:
    explicit GameServer(sock_t socket, sockaddr_in_t const &in);

    GameServer(GameServer &&);

    sock_t getSocket() const;
    bool   canWrite() const;

    void toggleWrite();

    virtual bool                           disconnect();
    virtual network::IClient::ClientAction write();
    virtual network::IClient::ClientAction read();
    virtual bool                           hasTimedOut() const;

    bool operator==(GameServer const &other) const;

  private:
    network::TCPSocket m_sock;
    sockaddr_in_t      m_in;
    bool               m_write;

    // Explicit padding
    std::array<std::uint8_t, 7> __padding;
  };

  std::vector<std::string> m_licenseList; // TODO: Licenses ?
  std::vector<std::unique_ptr<GameServer>>
                          m_gameServerList; // TODO: Use Memory lib
  std::thread             m_thread;
  std::condition_variable m_cond;
  std::mutex              m_mut;
};

#endif // !LICENSE_SERVER_HPP_