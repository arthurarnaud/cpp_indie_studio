#include "pakpak_stdafx.hpp"

namespace game
{
  Spectator::Spectator(GameData &data, ILayerStack &layer, core::HUD *hud,
                       std::vector<std::unique_ptr<LocalPlayer>> &players)
      : ALayer(data, layer, hud, players)
  {
  }

  Spectator::~Spectator()
  {
  }

  void Spectator::enable()
  {
  }

  void Spectator::disable()
  {
  }

  void Spectator::update()
  {
  }

  void Spectator::display()
  {
  }

  bool Spectator::keyPressed(OIS::KeyEvent const &)
  {
    return (false);
  }

  bool Spectator::keyReleased(OIS::KeyEvent const &)
  {
    return (false);
  }

  bool Spectator::mouseMoved(OIS::MouseEvent const &)
  {
    return (false);
  }

  bool Spectator::mousePressed(OIS::MouseEvent const &, OIS::MouseButtonID)
  {
    return (false);
  }

  bool Spectator::mouseReleased(OIS::MouseEvent const &, OIS::MouseButtonID)
  {
    return (false);
  }
}
