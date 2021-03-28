/*
 * ICodePlugin.h
 *
 *  Created on: 16 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_ICODE_ICODEPLUGIN_H_
#define ICODE_INCLUDE_ICODE_ICODEPLUGIN_H_
#include "MenuManager.h"
void CommandsRegistre();
class IPageLayout;
class IViewPart;
class MWindow;
typedef void (*PerspectiveInit)(IPageLayout *layout);
typedef IViewPart* (*ViewHandler)();
class ICODE_PUBLIC ICodePlugin: public Plugin {
public:
	static ICodePlugin *plugin;
	static ICodePlugin* Get();
	virtual MenuManager* GetMainMenuManager()=0;
	virtual int RegistrePerspective(const char *name, int image,
			PerspectiveInit init, int flags)=0;
	virtual void OpenPerspective(MWindow *parent, const char *name)=0;
	virtual int RegistreView(const char *id, ViewHandler handler)=0;
	virtual IViewPart* CreateView(const char *id)=0;
	virtual WImage* GetImage(int image)=0;
	virtual int GetImageIndex(int image)=0;
};

#endif /* ICODE_INCLUDE_ICODE_ICODEPLUGIN_H_ */
