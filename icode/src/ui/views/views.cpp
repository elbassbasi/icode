/*
 * views.cpp
 *
 *  Created on: 19 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "views.h"
void RegistreViews(){
	ICodePlugin::Get()->RegistreView("ProjectExplorer",ProjectExplorerHandler);
}



