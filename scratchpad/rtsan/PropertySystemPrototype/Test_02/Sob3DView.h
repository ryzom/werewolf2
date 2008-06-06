/**
 * \file Sob3DView.h
 * \date June 2006
 * \author Henri Kuuste
 */

/* Copyright, 2006 Werewolf
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

#ifndef __SOB3DVIEW_H__
#define __SOB3DVIEW_H__

//
// Standard Includes
//

//
// System Includes
//
#include <boost/python.hpp>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "IView.h"
#include "SOB.h"
#include "EMD.h"

//
// Namespaces
//

class Sob3DView : public IView {
public:
	Sob3DView(SOB* sob, EMD* emd) : m_Owner(sob), m_EMD(emd) { ; }
	virtual ~Sob3DView() { delete m_EMD; }

	void update();
	void render();

	const char* getViewName() { return m_Name; }

	SOB* getOwner() { return m_Owner; }
	EMD* getEMD() { return m_EMD; }

	static void init_script_class() {
		using namespace boost::python::api;
		using namespace boost::python;
		class_<Sob3DView, bases<IView> > w("Sob3DView", no_init);
		w.def("getOwner", &Sob3DView::getOwner, return_value_policy<reference_existing_object>());
		w.def("getEMD", &Sob3DView::getEMD, return_value_policy<reference_existing_object>());
		w.add_property("owner", w.attr("getOwner"));
		w.add_property("emd", w.attr("getEMD"));
	}

private:
	static const char* m_Name;
	SOB* m_Owner;
	EMD* m_EMD;
};

#endif // __SOB3DVIEW_H__
