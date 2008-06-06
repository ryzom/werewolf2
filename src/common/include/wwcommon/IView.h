/**
 * \file IView.h
 * \date February 2006
 * \author Henri Kuuste
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

#ifndef __IVIEW_H__
#define __IVIEW_H__

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//

//
// Namespaces
//

namespace WWCOMMON {

//
// Class
//
class IView {
public:
	/// Update view data from the model.
	virtual void update() = 0;
	/// Render the view.
	virtual void render() = 0;

	/** Returns the name of this view. 
	 If this is the same for all views of the given type,
	 then only one of those can be added to the sob.
	 */
	virtual const char* getViewName() = 0;

	/// Show the view.
	virtual void show() { m_visible = true; }
	/// Hide the view
	virtual void hide() { m_visible = false; }
	/// Check if the view is visible.
	virtual bool isVisible() { return m_visible; }

	IView() { m_visible = false; }
	virtual ~IView() {}

private:
	bool m_visible;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __IVIEW_H__

