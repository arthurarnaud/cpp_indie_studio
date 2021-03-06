#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <vector>
#include <cstdint>
#include <chrono>
#include <OGRE/OgreSceneManager.h>

// Disable clang warning for templated class padding
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang            system_header
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

#if defined(_WIN32)
#include <OgreBulletDynamicsWorld.h>
#include <Debug/OgreBulletCollisionsDebugDrawer.h>
#include <OgreBulletDynamicsRigidBody.h>
#include <OgreBulletCollisionsShape.h>
#else
#include <OgreBullet/Dynamics/OgreBulletDynamicsWorld.h>
#include <OgreBullet/Collisions/Debug/OgreBulletCollisionsDebugDrawer.h>
#include <OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h>
#include <OgreBullet/Collisions/OgreBulletCollisionsShape.h>
#endif // !_WIN32
#include <game/map/Map.hpp>

// Disable clang warning for templated class padding
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include "Map.hpp"

namespace game
{
  class PlayerData;
  class GameData
  {
  public:
    GameData();
    GameData(GameData const &) = delete;
    GameData(GameData &&) = delete;
    ~GameData();

    GameData &operator=(GameData const &) = delete;
    GameData &operator=(GameData &&) = delete;

    PlayerData &      operator[](std::size_t);
    PlayerData const &operator[](std::size_t) const;

    void        setPlayerNb(std::size_t);
    std::size_t getPlayerNb() const;

    void        setLocalPlayerNb(std::size_t);
    std::size_t getLocalPlayerNb() const;

    void startTime();

    void update();

    std::vector<PlayerData> &getPlayers();

    Ogre::Camera *   createCamera(std::string const &name);
    Ogre::Entity *   createEntity(std::string const &name);
    Ogre::SceneNode *createSceneNode();

    Ogre::SceneManager *               sceneMgr();
    OgreBulletDynamics::DynamicsWorld *physicWorld();

    void          addFinalPlayer(std::uint16_t id);
    std::uint32_t getFinalPlayerPosition(std::uint16_t id);

    void setMap(std::string const &mapName);

#ifdef DEBUG
    OgreBulletCollisions::DebugDrawer *debugDrawer();
#endif // !DEBUG

    OgreBulletDynamics::RigidBody *
               addPhysicEntity(std::unique_ptr<OgreBulletCollisions::CollisionShape>,
                               std::string const &);
    Map &      map();
    Map const &map() const;

    std::int32_t getLaps() const;
    void         setLaps(std::int32_t);

    void resetPhysicWorld();
    void clearPhysicWorld();

  private:
    Ogre::SceneManager *                               m_sceneMgr;
    std::vector<PlayerData>                            m_players;
    std::unique_ptr<OgreBulletDynamics::DynamicsWorld> m_world;
#ifdef DEBUG
    std::unique_ptr<OgreBulletCollisions::DebugDrawer> m_debugDrawer;
#endif
    std::vector<std::unique_ptr<OgreBulletDynamics::RigidBody>> m_bodies;
    std::vector<std::unique_ptr<OgreBulletCollisions::CollisionShape>>
                                                                m_shapes;
    std::unique_ptr<Map>                                        m_map;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
    std::int32_t                                                m_laps;
    std::vector<std::uint32_t>                                  m_finalRanking;
    std::size_t m_localPlayerNb;
  };
}

#endif // !GAMEDATA_HPP_
