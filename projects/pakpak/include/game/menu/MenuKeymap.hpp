//
// Created by brout_m on 09/06/17.
//

#ifndef MENUKEYMAP_HPP
#define MENUKEYMAP_HPP

#include "GUI.hpp"
#include "MenuManager.hpp"
#include "IMenuLayer.hpp"
#include "SettingsPlayer.hpp"

namespace core
{
  class MenuKeymap : public IMenuLayer
  {
  public:
    MenuKeymap(menu::MenuManager &menuManager, GUI &gui,
               SettingsPlayer &settings, SoundManager &sound);
    virtual ~MenuKeymap()
    {
    }

    void      draw();
    void      entry();
    void      exit();
    void      destroy();
    GameState update() const;
    void      build();
    bool keyPressed(const OIS::KeyEvent &arg);
    bool mouseMoved(const OIS::MouseEvent &arg);
    bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    bool keyReleased(const OIS::KeyEvent &arg);

  private:
    MenuKeymap(MenuKeymap const &) = delete;
    MenuKeymap(MenuKeymap &&) = delete;
    MenuKeymap &       operator=(MenuKeymap const &) = delete;
    MenuKeymap &       operator=(MenuKeymap &&) = delete;
    CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);
    void checkPlayerButton();

    bool onPlayer1Click(CEGUI::EventArgs const &);
    bool onPlayer2Click(CEGUI::EventArgs const &);
    bool onPlayer3Click(CEGUI::EventArgs const &);
    bool onPlayer4Click(CEGUI::EventArgs const &);

    bool onPlayer1Area(CEGUI::EventArgs const &);
    bool onPlayer2Area(CEGUI::EventArgs const &);
    bool onPlayer3Area(CEGUI::EventArgs const &);
    bool onPlayer4Area(CEGUI::EventArgs const &);

    bool onBackClick(CEGUI::EventArgs const &);
    bool onUpClick(CEGUI::EventArgs const &);
    bool onDownClick(CEGUI::EventArgs const &);
    bool onLeftClick(CEGUI::EventArgs const &);
    bool onRightClick(CEGUI::EventArgs const &);
    bool onUseClick(CEGUI::EventArgs const &);
    bool onPauseClick(CEGUI::EventArgs const &);

    bool onBackArea(CEGUI::EventArgs const &);
    bool onUpArea(CEGUI::EventArgs const &);
    bool onDownArea(CEGUI::EventArgs const &);
    bool onLeftArea(CEGUI::EventArgs const &);
    bool onRightArea(CEGUI::EventArgs const &);
    bool onUseArea(CEGUI::EventArgs const &);
    bool onPauseArea(CEGUI::EventArgs const &);

    void soundPass();
    void soundClick();
    void swapButtons(std::size_t newSelected);
    void loadLayout(std::size_t playerIndex);

    GUI &              m_gui;
    GameState          m_curState;
    menu::MenuManager &m_menuManager;
    size_t             m_playerSelected;
    CEGUI::Window *    m_buttons[4];
    SettingsPlayer &   m_settings;
    bool               m_listening;
    OIS::KeyCode *     m_curKey;
    std::string        m_curButton;
    SoundManager &     m_sound;
  };
}

#endif // MENUKEYMAP_HPP
