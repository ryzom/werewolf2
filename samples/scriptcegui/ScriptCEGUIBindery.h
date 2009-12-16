/**
 * \file ScriptCEGUIBindery.h
 * \date December 2009
 * \author Matt Raykowski
 */

/* Copyright, 2009 Werewolf
 *
 * This file is part of Werewolf.
 * Werewolf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * Werewolf is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Werewolf; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef __SCRIPTCEGUIBINDERY_H__
#define __SCRIPTCEGUIBINDERY_H__

//
// System Includes
//
#include <string>

#include <angelscript.h>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>

//
// CEGUI Includes
//
#include <CEGUIUDim.h>
#include <CEGUIWindow.h>

//
// Werewolf Includes
//
#include <wwscript/ScriptBindings/ScriptBinding.h>
#include <wwscript/ScriptEngine/ScriptManager.h>

template<class T>
T &getInstanceTmpl() {
	return T::getInstance();
}

template<class T>
T &getSingletonTmpl() {
	return T::getSingleton();
}

void CEGUILoggerLogEvent(std::string &str) {
	CEGUI::Logger::getSingleton().logEvent(str.c_str());
}

void CEGUILoggerLogEventLvl(std::string &str, CEGUI::LoggingLevel lvl) {
	CEGUI::Logger::getSingleton().logEvent(str.c_str(), lvl);
}

CEGUI::Scheme *CEGUISchemeManagerLoadScheme(std::string &schemeName, CEGUI::SchemeManager *schemeManager) {
	return schemeManager->loadScheme(schemeName.c_str());
}

std::string *getCEGUISchemeName(CEGUI::Scheme *scheme) {
	return new std::string(scheme->getName().c_str());
}

CEGUI::Font *CEGUIFontManagerCreateFont(std::string &fontName, CEGUI::FontManager *fontManager) {
	return fontManager->createFont(fontName.c_str());
}

void CEGUISystemSetDefaultFont(std::string &fontName, CEGUI::System *system) {
	system->setDefaultFont(fontName.c_str());
}
void CEGUISystemSetDefaultMouseCursor(std::string &imageset, std::string &image_name, CEGUI::System *system) {
	system->setDefaultMouseCursor(imageset.c_str(), image_name.c_str());
}

template<class T>
void CEGUIPropertySetRemoveProperty(std::string &propertyName, T *propSet) {
	propSet->removeProperty(propertyName.c_str());
}

template<class T>
bool CEGUIPropertySetIsPropertyPresent(std::string &propertyName, T *propSet) {
	return propSet->isPropertyPresent(propertyName.c_str());
}

// TODO fix mem leak?
template<class T>
std::string &CEGUIPropertySetGetPropertyHelp(std::string &propertyName, T *propSet) {
	std::string *str=new std::string(propSet->getPropertyHelp(propertyName.c_str()).c_str());
	return *str;
}

template<class T>
void CEGUIPropertySetSetProperty(std::string &prop_name, std::string &prop_val, T *propSet) {
	propSet->setProperty(prop_name.c_str(), prop_val.c_str());
}

template<class T>
bool CEGUIPropertySetIsPropertyDefault(std::string &prop_name, T *propSet) {
	return propSet->isPropertyDefault(prop_name.c_str());
}

// TODO fix mem leak?
template<class T>
std::string &CEGUIPropertySetGetPropertyDefault(std::string &prop_name, T *propSet) {
	std::string *str = new std::string(propSet->getPropertyDefault(prop_name.c_str()).c_str());
	return *str;
}

CEGUI::Window *CEGUIWindowManagerCreateWindow(std::string &type, std::string &name, CEGUI::WindowManager *wndmgr) {
	return wndmgr->createWindow(type.c_str(), name.c_str());
}

CEGUI::Imageset *CEGUIImagesetManagerCreateImagesetFromImageFile(std::string &name, std::string &file, CEGUI::ImagesetManager *isMgr) {
	return isMgr->createImagesetFromImageFile(name.c_str(), file.c_str());
}

CEGUI::Window *CEGUIWindowManagerLoadWindowLayout(std::string &wnd_name, CEGUI::WindowManager *wndMgr) {
	return wndMgr->loadWindowLayout(wnd_name.c_str());
}

class BindCEGUIObjects : public WWSCRIPT::ScriptBinding {
public:
	bool bindObjects() {
		// Types
		bindCEGUIUDim();
		bindCEGUIUVector2();
		bindCEGUIScheme();
		bindCEGUIFont();
		bindCEGUIWindow();
		bindCEGUIImageset();
		

		// Singletons.
		bindCEGUISystem();
		bindCEGUILogger();
		bindCEGUISchemeManager();
		bindCEGUIFontManager();
		bindCEGUIWindowManager();
		bindCEGUIImagesetManager();

		// Done
		return true;
	}

	// TODO
	// * addProperty
	// * getProperty
	template<class T>
	void registerCEGUIPropertySet(std::string typeName) {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;
		r = engine->RegisterObjectMethod(typeName.c_str(), "void removeProperty(string&)", asFUNCTION((CEGUIPropertySetRemoveProperty<T>)), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod(typeName.c_str(), "bool isPropertyPresent(string&)", asFUNCTION((CEGUIPropertySetIsPropertyPresent<T>)), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod(typeName.c_str(), "bool isPropertyDefault(string&)", asFUNCTION((CEGUIPropertySetIsPropertyDefault<T>)), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod(typeName.c_str(), "string &getPropertyDefault(string&)", asFUNCTION((CEGUIPropertySetGetPropertyDefault<T>)), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod(typeName.c_str(), "string &getPropertyHelp(string&)", asFUNCTION((CEGUIPropertySetGetPropertyHelp<T>)), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		
		r = engine->RegisterObjectMethod(typeName.c_str(), "void clearProperties()", asMETHODPR(T, clearProperties, (void), void), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "void setProperty(string&,string&)", asFUNCTION((CEGUIPropertySetSetProperty<T>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);		
	}

	void bindCEGUISystem() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUISystem", sizeof(CEGUI::System), asOBJ_REF); nlassert(r>=0);
		r = engine->RegisterGlobalFunction("CEGUISystem @getCEGUISystem()", asFUNCTION((getSingletonTmpl<CEGUI::System>)), asCALL_CDECL); nlassert(r>=0);

		// Register object methods.
		r = engine->RegisterObjectMethod("CEGUISystem", "void setDefaultFont(string&)", asFUNCTION(CEGUISystemSetDefaultFont), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUISystem", "void setDefaultMouseCursor(string&,string&)", asFUNCTION(CEGUISystemSetDefaultMouseCursor), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUISystem", "CEGUIWindow@ setGUISheet(CEGUIWindow@)", asMETHODPR(CEGUI::System, setGUISheet, (CEGUI::Window *), CEGUI::Window *), asCALL_THISCALL); nlassert( r >= 0 );

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUISystem", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUISystem", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCEGUILogger() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUILogger", sizeof(CEGUI::Logger), asOBJ_REF); nlassert(r>=0);
		r = engine->RegisterGlobalFunction("CEGUILogger @getCEGUILogger()", asFUNCTION((getSingletonTmpl<CEGUI::Logger>)), asCALL_CDECL); nlassert(r>=0);

		// Register enums
		r = engine->RegisterEnum("CEGUILoggingLevel"); nlassert(r>=0);
		r = engine->RegisterEnumValue("CEGUILoggingLevel", "Errors", CEGUI::LoggingLevel::Errors); nlassert(r>=0);
		r = engine->RegisterEnumValue("CEGUILoggingLevel", "Warnings", CEGUI::LoggingLevel::Warnings); nlassert(r>=0);
		r = engine->RegisterEnumValue("CEGUILoggingLevel", "Standard", CEGUI::LoggingLevel::Standard); nlassert(r>=0);
		r = engine->RegisterEnumValue("CEGUILoggingLevel", "Informative", CEGUI::LoggingLevel::Informative); nlassert(r>=0);
		r = engine->RegisterEnumValue("CEGUILoggingLevel", "Insane", CEGUI::LoggingLevel::Insane); nlassert(r>=0);

		// Register object methods.
		r = engine->RegisterObjectMethod("CEGUILogger", "void logEvent(string&)", asFUNCTION(CEGUILoggerLogEvent), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUILogger", "void setLoggingLevel(CEGUILoggingLevel)", asMETHODPR(CEGUI::Logger,setLoggingLevel,(CEGUI::LoggingLevel),void), asCALL_THISCALL); assert( r >= 0 );

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUILogger", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUILogger", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCEGUISchemeManager() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUISchemeManager", sizeof(CEGUI::SchemeManager), asOBJ_REF); nlassert(r>=0);
		r = engine->RegisterGlobalFunction("CEGUISchemeManager @getCEGUISchemeManager()", asFUNCTION((getSingletonTmpl<CEGUI::SchemeManager>)), asCALL_CDECL); nlassert(r>=0);

		// Register object methods.
		r = engine->RegisterObjectMethod("CEGUISchemeManager", "CEGUIScheme @loadScheme(string&)", asFUNCTION(CEGUISchemeManagerLoadScheme), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUISchemeManager", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUISchemeManager", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCEGUIFontManager() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUIFontManager", sizeof(CEGUI::FontManager), asOBJ_REF); nlassert(r>=0);
		r = engine->RegisterGlobalFunction("CEGUIFontManager @getCEGUIFontManager()", asFUNCTION((getSingletonTmpl<CEGUI::FontManager>)), asCALL_CDECL); nlassert(r>=0);

		// Register object methods.
		r = engine->RegisterObjectMethod("CEGUIFontManager", "CEGUIFont @createFont(string&)", asFUNCTION(CEGUIFontManagerCreateFont), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUIFontManager", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIFontManager", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCEGUIWindowManager() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUIWindowManager", sizeof(CEGUI::WindowManager), asOBJ_REF); nlassert(r>=0);
		r = engine->RegisterGlobalFunction("CEGUIWindowManager @getCEGUIWindowManager()", asFUNCTION((getSingletonTmpl<CEGUI::WindowManager>)), asCALL_CDECL); nlassert(r>=0);

		// Register object methods.
		r = engine->RegisterObjectMethod("CEGUIWindowManager", "CEGUIWindow @createWindow(string&,string&)", asFUNCTION(CEGUIWindowManagerCreateWindow), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindowManager", "CEGUIWindow @loadWindowLayout(string&)", asFUNCTION(CEGUIWindowManagerLoadWindowLayout), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUIWindowManager", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIWindowManager", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCEGUIImagesetManager() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUIImagesetManager", sizeof(CEGUI::ImagesetManager), asOBJ_REF); nlassert(r>=0);
		r = engine->RegisterGlobalFunction("CEGUIImagesetManager @getCEGUIImagesetManager()", asFUNCTION((getSingletonTmpl<CEGUI::ImagesetManager>)), asCALL_CDECL); nlassert(r>=0);

		// Register object methods.
		r = engine->RegisterObjectMethod("CEGUIImagesetManager", "CEGUIWindow @createImagesetFromImageFile(string&,string&)", asFUNCTION(CEGUIWindowManagerCreateWindow), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUIImagesetManager", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIImagesetManager", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCEGUIScheme() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUIScheme", sizeof(CEGUI::Scheme), asOBJ_REF); nlassert(r>=0);

		// Register object methods.
		r = engine->RegisterObjectMethod("CEGUIScheme", "string &getName()", asFUNCTION(getCEGUISchemeName), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUIScheme", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIScheme", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCEGUIFont() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUIFont", sizeof(CEGUI::Font), asOBJ_REF); nlassert(r>=0);

		// Register object methods.
		//r = engine->RegisterObjectMethod("CEGUIScheme", "string &getName()", asFUNCTION(getCEGUISchemeName), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		registerCEGUIPropertySet<CEGUI::Font>("CEGUIFont");

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUIFont", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIFont", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCEGUIWindow() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUIWindow", sizeof(CEGUI::Window), asOBJ_REF); nlassert(r>=0);

		// Register object methods.
		//r = engine->RegisterObjectMethod("CEGUIScheme", "string &getName()", asFUNCTION(getCEGUISchemeName), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		registerCEGUIPropertySet<CEGUI::Window>("CEGUIWindow");
		r = engine->RegisterObjectMethod("CEGUIWindow", "void addChildWindow(CEGUIWindow@)", asMETHODPR(CEGUI::Window, addChildWindow, (CEGUI::Window *), void), asCALL_THISCALL); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "void setXPosition(CEGUIUDim@)", asMETHODPR(CEGUI::Window, setXPosition, (const CEGUI::UDim &), void), asCALL_THISCALL); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "void setYPosition(CEGUIUDim@)", asMETHODPR(CEGUI::Window, setYPosition, (const CEGUI::UDim &), void), asCALL_THISCALL); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "void setPosition(CEGUIUVector2@)", asMETHODPR(CEGUI::Window, setPosition, (const CEGUI::UVector2 &), void), asCALL_THISCALL); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "void setSize(CEGUIUVector2@)", asMETHODPR(CEGUI::Window, setSize, (const CEGUI::UVector2 &), void), asCALL_THISCALL); nlassert( r >= 0 );
		//r = engine->RegisterObjectMethod("CEGUIWindow", "CEGUIUVector2@ getSize()", asMETHODPR(CEGUI::Window, getSize, (void) const, CEGUI::UVector2), asCALL_THISCALL); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "void setMaxSize(CEGUIUVector2@)", asMETHODPR(CEGUI::Window, setMaxSize, (const CEGUI::UVector2 &), void), asCALL_THISCALL); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "void setMinSize(CEGUIUVector2@)", asMETHODPR(CEGUI::Window, setMaxSize, (const CEGUI::UVector2 &), void), asCALL_THISCALL); nlassert( r >= 0 );
		//r = engine->RegisterObjectMethod("CEGUIWindow", "CEGUIUVector2@ getMaxSize()", asMETHODPR(CEGUI::Window, getMaxSize, (void) const, const CEGUI::UVector2 &), asCALL_THISCALL); nlassert( r >= 0 );
		//r = engine->RegisterObjectMethod("CEGUIWindow", "CEGUIUVector2@ getMinSize()", asMETHODPR(CEGUI::Window, getMinSize, (void) const, const CEGUI::UVector2 &), asCALL_THISCALL); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "void setWidth(CEGUIUDim@)", asMETHODPR(CEGUI::Window, setWidth, (const CEGUI::UDim &), void), asCALL_THISCALL); nlassert( r >= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "void setHeight(CEGUIUDim@)", asMETHODPR(CEGUI::Window, setHeight, (const CEGUI::UDim &), void), asCALL_THISCALL); nlassert( r >= 0 );

		r = engine->RegisterObjectMethod("CEGUIWindow", "bool isDestroyedByParent()", asMETHODPR(CEGUI::Window, isDestroyedByParent, (void) const, bool), asCALL_THISCALL); nlassert( r>= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "bool isAlwaysOnTop()", asMETHODPR(CEGUI::Window, isAlwaysOnTop, (void) const, bool), asCALL_THISCALL); nlassert( r>= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "bool isClippedByParent()", asMETHODPR(CEGUI::Window, isClippedByParent, (void) const, bool), asCALL_THISCALL); nlassert( r>= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "bool isActive()", asMETHODPR(CEGUI::Window, isActive, (void) const, bool), asCALL_THISCALL); nlassert( r>= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "bool isDisabled(bool)", asMETHODPR(CEGUI::Window, isDisabled, (bool) const, bool), asCALL_THISCALL); nlassert( r>= 0 );
		r = engine->RegisterObjectMethod("CEGUIWindow", "bool isVisible(bool)", asMETHODPR(CEGUI::Window, isVisible, (bool) const, bool), asCALL_THISCALL); nlassert( r>= 0 );
		
		//const UVector2& getPosition() const; plus get x/y

		//const String& getName(void) const  {return d_name;}

		
		// bool isChild(const String& name)
		//const String& getText(void) const


		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUIWindow", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIWindow", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCEGUIImageset() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUIImageset", sizeof(CEGUI::Imageset), asOBJ_REF); nlassert(r>=0);

		// Register object methods.
		//r = engine->RegisterObjectMethod("CEGUIScheme", "string &getName()", asFUNCTION(getCEGUISchemeName), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUIImageset", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIImageset", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCEGUIUVector2() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUIUVector2", sizeof(CEGUI::UVector2), asOBJ_REF); nlassert(r>=0);

		// Register object methods.
		//r = engine->RegisterObjectMethod("CEGUIScheme", "string &getName()", asFUNCTION(getCEGUISchemeName), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		
		// Register object properties.
	//	r = engine->RegisterObjectProperty("CEGUIUVector2", "CEGUIUDim @d_offset", offsetof(CEGUI::UVector2,d_x)); nlassert(r >= 0);
	//	r = engine->RegisterObjectProperty("CEGUIUVector2", "CEGUIUDim @d_scale", offsetof(CEGUI::UVector2,d_y)); nlassert(r >= 0);


		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUIUVector2", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIUVector2", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIUVector2", asBEHAVE_FACTORY, "CEGUIUVector2 @f()", asFUNCTION((WWSCRIPT::asCreateFactory0<CEGUI::UVector2>)), asCALL_CDECL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIUVector2", asBEHAVE_FACTORY, "CEGUIUVector2 @f(CEGUIUDim@,CEGUIUDim@)", asFUNCTION((WWSCRIPT::asCreateFactory2<CEGUI::UVector2,CEGUI::UDim,CEGUI::UDim>)), asCALL_CDECL); nlassert(r>=0);
	}

	void bindCEGUIUDim() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;

		r = engine->RegisterObjectType("CEGUIUDim", sizeof(CEGUI::UDim), asOBJ_REF); nlassert(r>=0);

		// Register object methods.
		//r = engine->RegisterObjectMethod("CEGUIScheme", "string &getName()", asFUNCTION(getCEGUISchemeName), asCALL_CDECL_OBJLAST); nlassert( r >= 0 );
		
		// Register object properties.
		r = engine->RegisterObjectProperty("CEGUIUDim", "float d_offset", offsetof(CEGUI::UDim,d_offset)); nlassert(r >= 0);
		r = engine->RegisterObjectProperty("CEGUIUDim", "float d_scale", offsetof(CEGUI::UDim,d_scale)); nlassert(r >= 0);


		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CEGUIUDim", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIUDim", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIUDim", asBEHAVE_FACTORY, "CEGUIUDim @f()", asFUNCTION((WWSCRIPT::asCreateFactory0<CEGUI::UDim>)), asCALL_CDECL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CEGUIUDim", asBEHAVE_FACTORY, "CEGUIUDim @f(float,float)", asFUNCTION((WWSCRIPT::asCreateFactory2<CEGUI::UDim,float,float>)), asCALL_CDECL); nlassert(r>=0);
	}

};

#endif // __SCRIPTCEGUIBINDERY_H__

