/**
 * \file ScriptVariable.h
 * \date February 2006
 * \author Henri Kuuste
 * \author Matt Raykowski
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
#ifndef __SCRIPTVARIABLE_H__
#define __SCRIPTVARIABLE_H__

//
// Standard Includes
//
#include <string>

//
// System Includes
//
#include <angelscript.h>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "ScriptEngineDefs.h"
#include "ScriptArg.h"

//
// Namespaces
//

namespace WWSCRIPT {

class WWSCRIPT_API ScriptVariable {
public:
	ScriptVariable(ScriptArg* arg);
	ScriptVariable(ScriptArg::eType type, const char* name, ScriptArg::Binding* binding = NULL);

	~ScriptVariable();

	bool setScriptArg(asIScriptContext* context) const;

	bool setValue(int value);
	bool setValue(unsigned int value);
	bool setValue(long value);
	bool setValue(bool value);
	bool setValue(float value);
	bool setValue(double value);
	bool setValue(void* value);

	bool setValueFromReturn(asIScriptContext* context);
	bool setValueFromBinding();
	bool isBound() const;

	void Bind(const ScriptArg::Binding* binding);
	const ScriptArg::Binding* getBinding() const;

	bool getValue(int& value) const;
	bool getValue(unsigned int& value) const;
	bool getValue(long& value) const;
	bool getValue(bool& value) const;
	bool getValue(float& value) const;
	bool getValue(double& value) const;
	bool getValue(void** value) const;

	ScriptArg::eType getType() const;
	const std::string& getName() const;

	void reset();
	bool isSet() const;
	bool isArg() const;

	int getId() const;

private:
	typedef union {
		int int_val;
		unsigned int uint_val;
		long long_val;
		bool bool_val;
		float float_val;
		double double_val;
		void* object_val;
	} valueType;

	valueType m_value;
	const ScriptArg::Binding* m_binding;
	ScriptArg::eType m_type;
	std::string m_name;
	bool m_set;
	int m_id;
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTVARIABLE_H__
