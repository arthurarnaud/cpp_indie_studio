//
// Created by duhieu_b on 31/05/17.
//

#include "pakpak_stdafx.hpp"

namespace menu
{
  bool MenuManager::keyPressed(OIS::KeyEvent const &ke)
  {
    for (std::size_t l_i = m_layers.size(); l_i > 0; --l_i)
      {
	if (m_layers[l_i - 1]->keyPressed(ke))
	  return true;
      }
    return false;
  }

  bool MenuManager::keyReleased(OIS::KeyEvent const &ke)
  {
    for (std::size_t l_i = m_layers.size(); l_i > 0; --l_i)
      {
	if (m_layers[l_i - 1]->keyReleased(ke))
	  return true;
      }
    return false;
  }

  bool MenuManager::mouseMoved(OIS::MouseEvent const &me)
  {
    for (std::size_t l_i = m_layers.size(); l_i > 0; --l_i)
      {
	if (m_layers[l_i - 1]->mouseMoved(me))
	  return true;
      }
    return false;
  }

  bool MenuManager::mousePressed(OIS::MouseEvent const &me,
                                 OIS::MouseButtonID     id)
  {
    for (std::size_t l_i = m_layers.size(); l_i > 0; --l_i)
      {
	if (m_layers[l_i - 1]->mousePressed(me, id))
	  return true;
      }
    return false;
  }

  bool MenuManager::mouseReleased(OIS::MouseEvent const &me,
                                  OIS::MouseButtonID     id)
  {
    for (std::size_t l_i = m_layers.size(); l_i > 0; --l_i)
      {
	if (m_layers[l_i - 1]->mouseReleased(me, id))
	  return true;
      }
    return false;
  }

  void MenuManager::push(core::MenuState layer)
  {
    m_layers.push(m_menuLayer[static_cast<std::size_t>(layer)].get());
  }

  void MenuManager::popLayer()
  {
    m_layers.pop();
  }

  MenuManager::MenuManager(Ogre::RenderWindow *  win,
                           core::SettingsPlayer &settings)
      : m_sceneMan(Ogre::Root::getSingleton().createSceneManager(
            "DefaultSceneManager", "Menu scene manager")),
        m_camera(m_sceneMan->createCamera("MenuCamera")), m_viewport(nullptr),
        m_gui(win), m_menuLayer({}), m_layers({})
  {
    m_menuLayer[static_cast<size_t>(core::MenuState::MainMenu)] =
        std::make_unique<core::MainMenu>(*this, m_gui);
    m_menuLayer[static_cast<size_t>(core::MenuState::Option)] =
        std::make_unique<core::MenuOptions>(*this, m_gui);
    m_menuLayer[static_cast<size_t>(core::MenuState::Keymap)] =
        std::make_unique<core::MenuKeymap>(*this, m_gui, settings);
    m_menuLayer[static_cast<size_t>(core::MenuState::SoloPlayerGame)] =
        std::make_unique<core::MenuSolo>(*this, m_gui);
    m_menuLayer[static_cast<size_t>(core::MenuState::MultiPlayerGame)] =
        std::make_unique<core::MenuMultiplayer>(*this, m_gui);
    m_menuLayer[static_cast<size_t>(core::MenuState::Score)] =
        std::make_unique<core::MenuScores>(*this, m_gui);

    m_layers.push(
        m_menuLayer[static_cast<size_t>(core::MenuState::MainMenu)].get());
    (void)win;
  }

  void MenuManager::begin()
  {
    if (!m_layers.size())
      {
	m_gui.setCursorArrow("TaharezLook/MouseArrow");
	m_gui.hideCursor(false);
	m_layers.push(
	    m_menuLayer[static_cast<size_t>(core::MenuState::MainMenu)].get());
      }
    m_layers[m_layers.size() - 1]->entry();
  }

  void MenuManager::end()
  {
    int size = static_cast<int>(m_layers.size());

    while (size > 0)
      {
	m_layers[size - 1]->destroy();
	m_layers.pop();
	--size;
      }
  }

  core::IMenuLayer *MenuManager::getMenuLayer()
  {
    return m_layers[m_layers.size() - 1];
  }

  void MenuManager::update()
  {
    m_gui.update();
  }

  MenuManager::~MenuManager()
  {
  }
}