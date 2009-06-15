/* Copyright, 2003 Neverborn Entertainment.

 * This file is part of our XSI Plugins.

 * The XSI Plugins are free software, you can redistribute them and/or modify
 * them under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2, or (at your option)
 * any later version.

 * XSI Plugins are distributed WITHOUT ANY WARRANTY or implied warranty of
 * MERCHANTABILITY. See the GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with the XSI Plugins; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.

 */


#ifndef NEL_TOOLS_SINGLETON_H
#define NEL_TOOLS_SINGLETON_H

//
// Includes
//

#include <nel/misc/debug.h>

//
// Classes
//

template<class T>
class CSingleton
{
public:
	
	static T &instance()
	{
		if(!Instance)
		{
			Instance = new T;
			nlassert(Instance);
		}
		return *Instance;
	}
	
protected:
	
	CSingleton()
	{
	}
	
	static T *Instance;
};

template <class T>
T* CSingleton<T>::Instance = 0;

#endif
