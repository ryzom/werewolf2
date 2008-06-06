/**
 * \file IDirtyPropagator.h
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

#ifndef __IDIRTYPROPAGATOR_H__
#define __IDIRTYPROPAGATOR_H__

//
// Standard Includes
//

//
// System Includes
//
#include <list>
#include <boost/python.hpp>

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

class IDirtyPropagator {
public:
	IDirtyPropagator();
	virtual ~IDirtyPropagator();

	typedef	IDirtyPropagator*		TConnection;
	typedef std::list<TConnection>	TConList;

	bool			isDirty();

	void			setDirty();
	void			setClean();
	
	virtual bool	makeInputConnection(IDirtyPropagator *input);
	bool			breakInputConnection(IDirtyPropagator *input);
	bool			breakOutputConnection(IDirtyPropagator *output);	
	
protected:
	friend class IDirtyPropagator;

	void			killInputConnection(TConnection con);
	void			killOutputConnection(TConnection con);
	void			breakAllConnections();
	bool			makeOutputConnection(IDirtyPropagator *output);

	void			propagateDirtyState();

private:
	bool			m_Dirty;

	TConList*		m_Inputs;
	TConList*		m_Outputs;
};

class IDirtyPropagatorWrap : public IDirtyPropagator, public boost::python::wrapper<IDirtyPropagator> {
public:
	bool makeInputConnection(IDirtyPropagator* input) {
		if(boost::python::override f = this->get_override("makeInputConnection"))
			return boost::python::call<bool>(f.ptr(), input);
		return IDirtyPropagator::makeInputConnection(input);
	}

	bool default_makeInputConnection(IDirtyPropagator* input) {
		return this->IDirtyPropagator::makeInputConnection(input);
	}

	static void init_script_class() {
		using namespace boost::python;
		using namespace boost::python::api;
		class_<IDirtyPropagatorWrap, boost::noncopyable> idpw("IDirtyPropagator", no_init);
		idpw.def("makeInputConnection", &IDirtyPropagator::makeInputConnection, &IDirtyPropagatorWrap::default_makeInputConnection);
		idpw.def("breakInputConnection", &IDirtyPropagator::breakInputConnection);
		idpw.def("breakOutputConnection", &IDirtyPropagator::breakOutputConnection);
		idpw.def("setDirty", &IDirtyPropagator::setDirty);
		idpw.def("setClean", &IDirtyPropagator::setClean);
		idpw.add_property("dirty", &IDirtyPropagator::isDirty);
	}
};

}; // END OF NAMESPACE WWCOMMON


#endif // __IDIRTYPROPAGATOR_H__
