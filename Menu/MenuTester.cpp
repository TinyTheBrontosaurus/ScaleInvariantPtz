#include "Menu.h"

/**************************************************************
 *
 * Main program
 * Description: Create the Menu and use standard input
 *              as events for the Menus.
 *
 *
 **************************************************************
 */
int main()
{

  //Get a pointer to the menu and display the banner
  Menu *menu = Menu::displayBanner();
  //ControllerOptions options;
  //menu->setOptions(&options);
  Menu::execute = true;

  //Process actions from standard input for the menu
  while( Menu::execute )
  {
    char action;
    action = getchar();
    menu = menu->performAction(action);
  }

  Menu::print( "Goodbye!\n" );
  return 0;

}

// File: $Id: MenuTester.cpp,v 1.4 2005/05/26 14:39:47 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: MenuTester.cpp,v $
// Revision 1.4  2005/05/26 14:39:47  edn2065
// Made execute from Menu a static class variable.
//
// Revision 1.3  2005/05/24 15:36:51  edn2065
// Implemented I/O display in menu.
// Bugs appeared:
//   Horizontal line on video (maybe a timing issue?)
//   OpenCV experiences NULL pointer because join may not be working
//
// Revision 1.2  2005/04/27 22:58:30  edn2065
// Added pure static controller to menu
//
// Revision 1.1  2005/04/21 13:47:39  edn2065
// Added Menus and fully commented. commented ControllerOptions.
//
