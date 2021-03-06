#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include <cstddef>
#include "ConfigTools.hpp"

// Disable clang warning for implicit padding
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

class GameLogic
{
  // TODO: Currently with MapConfig, should be something else in the future
  using map_t = MapConfig;

  // TODO: Change player to player's type
  using player_t = char;

public:
  GameLogic();
  GameLogic(GameLogic const &) = delete;
  GameLogic(GameLogic &&) = default;

  GameLogic &operator=(GameLogic const &) = delete;
  GameLogic &operator=(GameLogic &&) = default;

  ~GameLogic();

  // General configuration
  void run(); // Thread

  void requestNextGame();

  // GamePlayer
  std::size_t getCurrentPlayers() const;
  std::size_t getCurrentHumanPlayers() const;
  std::size_t getCurrentAIs() const;
  std::size_t getCurrentSpectator() const;

  // Maps
  map_t const &getCurrentMap() const;

  void addHumanPlayer();
  void removeHumanPlayer();

private:
  enum class GameState : std::uint16_t
  {
    LOADING,
    PLAYING
  };

  void setUp();

  // Stop current game, and play the next scheduled map
  void nextMap();

  void addSpectator();
  void addAI();
  void removeAI();

  void loadMap();

  void setState(GameState const state);

  void loadingState();
  void stopGame();
  void syncClients();
  void playingState();
  void updatePlayersCount();

  // General configuration
  std::atomic<bool> m_running;
  std::atomic<bool> m_playing;
  std::size_t       m_maxPlayers;
  std::size_t       m_currentHumanPlayers;
  std::size_t       m_currentAIs;
  std::size_t       m_currentSpectators;

  // TODO: GameData ?
  std::vector<map_t>           m_maps;
  std::vector<map_t>::iterator m_currentMap;
  std::vector<map_t>::iterator m_nextMap;
  GameState                    m_prevState;
  GameState                    m_state;

  std::vector<player_t> m_humans;
  std::vector<player_t> m_spectators;
  std::vector<player_t> m_ai;
};

// Disable clang warning for implicit padding
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif // !GAME_LOGIC_HPP_
