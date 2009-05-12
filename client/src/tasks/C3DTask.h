/**
 * \file C3DTask.h
 * \date May 2004
 * \author Matt Raykowski
 * \author Some code adapted from MTP-Target.
 * \todo Implement captureCurse strictly maybe in CGuiTask or CGuiObjectTask
 * \todo Retouch C3DTask::kbGetString()
 */

/* Copyright, 2004 Werewolf
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

#ifndef __C3DTASK_H__
#define __C3DTASK_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_texture.h>
#include <nel/misc/rgba.h>

//
// Werewolf Includes
//
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"
#include "wwcommon/CGenericObjectPool.h"

//
// Class
//

namespace WWCLIENT {

class C3DTask : public WWCOMMON::ISingleton<C3DTask>, public WWCOMMON::ITask {
public:
	/**
	 * \brief Initializes the whole 3D system.
	 *
	 * This should be called only once. It loads all of the 3D variables from
	 * the configuration file and sets up the UDriver, UScene, and UTextContext
	 * settings. Too many things to at-see.
	 */
	virtual void init();

	/**
	 * \brief Updates animation cycles and exits if necessary.
	 *
	 * First this method checks m_Driver->isActive() to make sure nothing has
	 * requested the engine to stop. m_Scene->animate() is called to process
	 * any necessary animation updates in the scene. Finally the Driver event
	 * server pipeline is pumped.
	 */
	virtual void update();

	/**
	 * \brief Called when the world needs to be rendered.
	 *
	 * This is called, presumably only by CTaskManager, when the m_Scene needs
	 * rendering. Here's the pipeline from this method's perspective:
	 *	-# Fog is enabled.
	 *	-# m_Scene is rendered.
	 *	-# Fog is disabled.
	 *	-# CEntityManager is called to render it's pieces.
	 *
	 */
	virtual void render();
	virtual void release();			/**< This method, from ITask, does nothing. */
	virtual std::string name();

	NL3D::UDriver		&driver() const;
	NL3D::UScene		&scene() const; 
	NL3D::UTextContext	&textContext() const;

	/**
	 * \brief Captures the cursor for the GUI
	 *
	 * This method captures the cursor for use with the Gui. It uses
	 * the CGuiTask to accomplish this.
	 * @param b boolean representing whether or not to capture the cursor.
	 */
	void captureCursor(bool b);

	/**
	 * \brief Clears the driver buffers.
	 *
	 * This clears the m_Driver buffers using m_ClearColor.
	 * @see m_ClearColor
	 */
	void clear();

	/**
	 * \brief Changes the clear color.
	 *
	 * This sets m_ClearColor to the color parameter.
	 * @param color The color to set m_ClearColor to.
	 * @see m_ClearColor
	 */
	void clearColor(NLMISC::CRGBA color);

	uint16 getScreenWidth();
	uint16 getScreenHeight();

private:
	/**
	 * \brief Contains the NeL Window Driver.
	 *
	 * This contains a copy of NeL's UDriver which is the game window
	 * interface driver. From UDriver you can creates Scenes and render
	 * basic (slowly) primitives.
	 */
	NL3D::UDriver				*m_Driver;

	/**
	 * \brief Contains the NeL interface for string rendering.
	 *
	 * This object is used for rendering different strings to the screen.
	 */
	NL3D::UTextContext	*m_TextContext;

	/**
	 * \brief Contains the NeL Scene Interface.
	 *
	 * This is used for creating Scene instances such as cameras, lights, etc.
	 */
	NL3D::UScene				*m_Scene;

	uint16							m_ScreenWidth;	/**< Contains the width of the screen in pixels. */
	uint16							m_ScreenHeight;	/**< Contains the height of the screen in pixels. */
	NLMISC::CRGBA				m_AmbientColor;
	NLMISC::CRGBA				m_ClearColor;		/**< Contains the color used to clear the driver buffers. */
};

}; // END NAMESPACE WWCLIENT

#endif // __C3DTASK_H__
