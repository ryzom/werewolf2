// Local includes
#include "ScriptLoader/ScriptVariable.h"
#include "PropertyManager/PropertyManager.h"

ScriptVariable::ScriptVariable(ScriptArg* arg) : m_set(false) {
	m_type = arg->getType();
	m_name = arg->getName();
	m_binding = arg->getBinding();
	m_id = arg->getId();
}

ScriptVariable::ScriptVariable(ScriptArg::eType type, const char* name, ScriptArg::Binding* binding) : m_set(false) {
	m_type = type;
	m_name = name;
	m_binding = binding;
	m_id = -1;
}

ScriptVariable::~ScriptVariable() {
	// do nothing
}

bool ScriptVariable::isBound() const {
	return m_binding != NULL;
}

bool ScriptVariable::isSet() const {
	return m_set;
}

bool ScriptVariable::isArg() const {
	return m_id >= 0;
}

int ScriptVariable::getId() const {
	return m_id;
}

const std::string& ScriptVariable::getName() const {
	return m_name;
}

ScriptArg::eType ScriptVariable::getType() const {
	return m_type;
}

bool ScriptVariable::setValue(int value) {
	if(m_type != ScriptArg::INT)
		return false;
	m_value.int_val = value;
	m_set = true;
	return true;
}

bool ScriptVariable::setValue(unsigned int value) {
	if(m_type != ScriptArg::UINT)
		return false;
	m_value.uint_val = value;
	m_set = true;
	return true;
}

bool ScriptVariable::setValue(long value) {
	if(m_type != ScriptArg::LONG)
		return false;
	m_value.long_val = value;
	m_set = true;
	return true;
}

bool ScriptVariable::setValue(bool value) {
	if(m_type != ScriptArg::BOOL)
		return false;
	m_value.bool_val = value;
	m_set = true;
	return true;
}

bool ScriptVariable::setValue(float value) {
	if(m_type != ScriptArg::FLOAT)
		return false;
	m_value.float_val = value;
	m_set = true;
	return true;
}

bool ScriptVariable::setValue(double value) {
	if(m_type != ScriptArg::DOUBLE)
		return false;
	m_value.double_val = value;
	m_set = true;
	return true;
}

bool ScriptVariable::setValue(void* value) {
	if(m_type < ScriptArg::OBJECT)
		return false;
	m_value.object_val = value;
	m_set = true;
	return true;
}

bool ScriptVariable::setValueFromReturn(asIScriptContext* context) {
	if(context->GetState() != asEXECUTION_FINISHED)
		return false;
	switch(m_type) {
		case ScriptArg::VOID_TYPE:
			return false;
		case ScriptArg::INT:
			return setValue((int)context->GetReturnDWord());
		case ScriptArg::UINT:
			return setValue((unsigned int)context->GetReturnDWord());
		case ScriptArg::LONG:
			return setValue((long)context->GetReturnDWord());
		case ScriptArg::BOOL:
			return setValue((bool)context->GetReturnDWord());
		case ScriptArg::FLOAT:
			return setValue(context->GetReturnFloat());
		case ScriptArg::DOUBLE:
			return setValue(context->GetReturnDouble());
		default: // >= object
			return setValue(context->GetReturnObject());
	}
}

bool ScriptVariable::setValueFromBinding() {
	if(!isBound())
		return false;
	IProperty *prop = PropertyManager::instance().getProperty(m_binding->map.c_str(), m_binding->property.c_str());
	if(prop == NULL)
		return false;
	prop->setScriptParameter(this);
	return m_set;
}

bool ScriptVariable::setScriptArg(asIScriptContext* context) const {
	if(!isSet() || !isArg())
		return false;
	switch(m_type) {
	case ScriptArg::VOID_TYPE:
		return false;
	case ScriptArg::INT:
		context->SetArgDWord(m_id, (asDWORD)m_value.int_val);
		break;
	case ScriptArg::UINT:
		context->SetArgDWord(m_id, (asDWORD)m_value.uint_val);
		break;
	case ScriptArg::LONG:
		context->SetArgDWord(m_id, (asDWORD)m_value.long_val);
		break;
	case ScriptArg::BOOL:
		context->SetArgDWord(m_id, (asDWORD)m_value.bool_val);
		break;
	case ScriptArg::FLOAT:
		context->SetArgFloat(m_id, m_value.float_val);
		break;
	case ScriptArg::DOUBLE:
		context->SetArgDouble(m_id, m_value.double_val);
		break;
	default:
		context->SetArgObject(m_id, m_value.object_val);
		break;
	}
	return true;
}

void ScriptVariable::Bind(const ScriptArg::Binding* binding) {
	m_binding = binding;
}

const ScriptArg::Binding* ScriptVariable::getBinding() const {
	return m_binding;
}

bool ScriptVariable::getValue(int& value) const {
	if(!isSet() || m_type != ScriptArg::INT)
		return false;
	value = m_value.int_val;
	return true;
}

bool ScriptVariable::getValue(unsigned int& value) const {
	if(!isSet() || m_type != ScriptArg::UINT)
		return false;
	value = m_value.uint_val;
	return true;
}

bool ScriptVariable::getValue(long& value) const {
	if(!isSet() || m_type != ScriptArg::LONG)
		return false;
	value = m_value.long_val;
	return true;
}

bool ScriptVariable::getValue(bool& value) const {
	if(!isSet() || m_type != ScriptArg::BOOL)
		return false;
	value = m_value.bool_val;
	return true;
}

bool ScriptVariable::getValue(float& value) const {
	if(!isSet() || m_type != ScriptArg::FLOAT)
		return false;
	value = m_value.float_val;
	return true;
}

bool ScriptVariable::getValue(double& value) const {
	if(!isSet() || m_type != ScriptArg::DOUBLE)
		return false;
	value = m_value.double_val;
	return true;
}

bool ScriptVariable::getValue(void** value) const {
	if(!isSet() || m_type < ScriptArg::OBJECT)
		return false;
	*value = m_value.object_val;
	return true;
}

void ScriptVariable::reset() {
	m_set = false;
}