#include "pakpak_stdafx.hpp"

namespace game
{
  Menu::Menu(GameData &data, ILayerStack &layer, core::HUD *hud,
             std::vector<std::unique_ptr<LocalPlayer>> &players)
      : ALayer(data, layer, hud, players)
  {
  }

  Menu::~Menu()
  {
  }

  void Menu::enable()
  {
    if (m_gui)
      {
	nope::log::Log(Debug) << "Enabling pause menu";
	m_gui->addLayout("pause.layout");
	m_gui->showCursor(true);
	m_gui->grabMouse();
	Pauser::pause();

	CEGUI::Window *button =
	    m_gui->getRoot()->getChildRecursive("resume_button");
	if (button)
	  {
	    button->subscribeEvent(
	        CEGUI::PushButton::EventClicked,
	        CEGUI::Event::Subscriber(&Menu::onResumeClick, this));
	  }
	else
	  {
	    throw GUIError("Missing asset resume_button");
	  }
	button = m_gui->getRoot()->getChildRecursive("quit_button");
	if (button)
	  {
	    button->subscribeEvent(
	        CEGUI::PushButton::EventClicked,
	        CEGUI::Event::Subscriber(&Menu::onQuitClick, this));
	  }
	else
	  {
	    throw GUIError("Missing asset quit_button");
	  }
      }
  }

  void Menu::disable()
  {
    if (m_gui)
      {
	nope::log::Log(Debug) << "disabling pause menu";
	m_gui->removeLayout("pause.layout");
	m_gui->hideCursor(true);
      }
  }

  void Menu::update()
  {
  }

  void Menu::display()
  {
  }

  bool Menu::keyPressed(OIS::KeyEvent const &arg)
  {
    if (arg.key == OIS::KC_ESCAPE)
    {
	nope::log::Log(Debug) << "Closing pause menu";
        Pauser::unpause();
	m_layerStack.popLayer();
      }
    CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(
        static_cast<CEGUI::Key::Scan>(arg.key));
    return (false);
  }

  bool Menu::keyReleased(OIS::KeyEvent const &arg)
  {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(
        static_cast<CEGUI::Key::Scan>(arg.key));
    return (true);
  }

  bool Menu::mouseMoved(OIS::MouseEvent const &arg)
  {
    CEGUI::GUIContext &context =
        CEGUI::System::getSingleton().getDefaultGUIContext();
    context.injectMouseMove(static_cast<float>(arg.state.X.rel),
                            static_cast<float>(arg.state.Y.rel));
    return (false);
  }

  bool Menu::mousePressed(OIS::MouseEvent const &, OIS::MouseButtonID button)
  {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(
        convertButton(button));
    return (false);
  }

  bool Menu::mouseReleased(OIS::MouseEvent const &, OIS::MouseButtonID button)
  {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(
        convertButton(button));
    return (true);
  }

  bool Menu::onResumeClick(CEGUI::EventArgs const &)
  {
    nope::log::Log(Debug) << "Resume button clicked.";
    Pauser::unpause();
    m_layerStack.popLayer();
    return true;
  }

  CEGUI::MouseButton Menu::convertButton(OIS::MouseButtonID buttonID)
  {
    switch (buttonID)
      {
      case OIS::MB_Left:
	return CEGUI::LeftButton;

      case OIS::MB_Right:
	return CEGUI::RightButton;

      case OIS::MB_Middle:
	return CEGUI::MiddleButton;

      default:
	return CEGUI::LeftButton;
      }
  }

  bool Menu::onQuitClick(CEGUI::EventArgs const &)
  {
    m_gui->setQuit(true);
    return true;
  }
}
