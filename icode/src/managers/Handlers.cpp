/*
 * Handlers.cpp
 *
 *  Created on: 13 oct. 2021
 *      Author: azeddine
 */
#include "../icode.h"
IObject* QuitCommand();
IObject* RestartCommand();
IObject* FullScreenCommand();
IObject* ProjectExplorerHandler();
IObject* SplashHandler();
IObject* ManagerUIHandler();
HandlerName ICodePlugin::handlers[] = { //
		/* commands */
		{ "Quit", QuitCommand }, //
				{ "Restart", RestartCommand }, //
				{ "FullScreen", FullScreenCommand }, //
				/* views */
				{ "ProjectExplorer", ProjectExplorerHandler }, //
				/*  */
				{ "Splash", SplashHandler }, //
				//{ "ManagerUI", ManagerUIHandler }, //
				/* end */
		};
const int ICodePlugin::handlersCount = sizeof(handlers) / sizeof(handlers[0]);
