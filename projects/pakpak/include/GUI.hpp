//
// Created by duhieu_b on 23/05/17.
//

#ifndef CPP_INDIE_STUDIO_GUI_HPP_
#define CPP_INDIE_STUDIO_GUI_HPP_

#include <OGRE/Ogre.h>
#include <iostream>
#include <glm/glm.hpp>
#include <CEGUI/Cegui.h>

namespace core
{
  class GUI
  {
  public:
    ~GUI();
    GUI();
    void init(std::string const &);

    void destroy();

    void draw();

    void setFont(std::string const &);

    CEGUI::Window *createButton(CEGUI::Window *, std::string const &,
                                glm::vec4 const &, glm::vec4 const &,
                                std::string const &);

    CEGUI::Window *createButton(std::string const &, glm::vec4 const &,
                                glm::vec4 const &, std::string const &);

    void setPos(CEGUI::Window *, glm::vec4 const &, glm::vec4 const &);

    void loadSheme(std::string const &);

    CEGUI::OgreRenderer *getRender() const;

    CEGUI::GUIContext *getContext() const;

  private:
    CEGUI::OgreRenderer *m_renderer;
    CEGUI::Window *      m_root;
    CEGUI::GUIContext *  m_context;
  };
}