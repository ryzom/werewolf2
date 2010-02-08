#ifndef WW_PROPERTY_H
#define WW_PROPERTY_H

#include <string>

#include <nel/misc/smart_ptr.h>

#include "iproperty.h"

template<class T>
class PropertyData : public NLMISC::CRefCount {
public:
	T value;
	std::string name;
	bool dirty;
	//CL_Signal_v2<const T&, const T&> valueChanged;
};

template<class T>
class Property : public IProperty {
public:
	Property() { }

	Property(const Property &copy) : data(copy.data) { }

	Property(const std::string &name) : data(new PropertyData<T>()) {	
		data->name = name; 
		data->dirty = false;
	}

	virtual ~Property() { }

	void Set(const T& value) { 
		if(data->value != value) {
			T oldValue = data->value;
			data->value = value; 
			data->dirty = true;

			//data->valueChanged.invoke(oldValue, value);
		}
	}

	const T& Get() const { return data->value; }

	virtual const std::string &GetName() const { return data->name; }

	virtual bool IsNull() const { return data == NULL; }

	virtual bool IsDirty() const { return data->dirty; }
	virtual void ClearDirty() { data->dirty = false; }

	//virtual std::string ToString() const { return TypeSerializer::ToString(data->value); }
	//virtual void SetFromString(const T_String &value) { TypeSerializer::FromString(value, data->value); }
	//virtual int GetTypeId() const { return TypeSerializer::GetTypeId(data->value); }

	//CL_Signal_v2<const T&, const T&> &ValueChanged() { return data->valueChanged; }

	Property<T> operator= (const Property<T>& rhs);
	Property<T> operator= (const T& rhs);

	Property<T> operator+= (const Property<T>& rhs);
	Property<T> operator+= (const T& rhs);

	Property<T> operator-= (const Property<T>& rhs);
	Property<T> operator-= (const T& rhs);

	bool operator== (const Property<T>& rhs);
	bool operator== (const T& rhs);

	bool operator!= (const Property<T>& rhs);
	bool operator!= (const T& rhs);

	bool operator> (const Property<T>& rhs);
	bool operator> (const T& rhs);

	bool operator< (const Property<T>& rhs);
	bool operator< (const T& rhs);

	operator T() const { return data->value; }

private:
	NLMISC::CSmartPtr< PropertyData<T> > data;
};

template<class T>
inline Property<T> Property<T>::operator =(const Property<T> &rhs) {
	data = rhs.data;
	return *this;
}

template<class T>
inline Property<T> Property<T>::operator =(const T &rhs) {
	Set(rhs);
	return *this;
}

template<class T>
inline Property<T> Property<T>::operator +=(const Property<T> &rhs) {
	Set(data->value + rhs.data->value);
	return *this;
}

template<class T>
inline Property<T> Property<T>::operator +=(const T &rhs) {
	Set(data->value + rhs);
	return *this;
}

template<class T>
inline Property<T> Property<T>::operator -=(const Property<T> &rhs) {
	Set(data->value - rhs.data->value);
	return *this;
}

template<class T>
inline Property<T> Property<T>::operator -=(const T &rhs) {
	Set(data->value - rhs);
	return *this;
}

template<class T>
inline bool Property<T>::operator ==(const Property<T> &rhs) {
	return data == rhs.data;
}

template<class T>
inline bool Property<T>::operator ==(const T &rhs) {
	return (data->value == rhs);
}

template<class T>
inline bool Property<T>::operator !=(const Property<T> &rhs) {
	return data != rhs.data;
}

template<class T>
inline bool Property<T>::operator !=(const T &rhs) {
	return (data->value != rhs);
}

template<class T>
inline bool Property<T>::operator >(const Property<T> &rhs) {
	return (data->value > rhs.data->value);
}

template<class T>
inline bool Property<T>::operator >(const T &rhs) {
	return (data->value > rhs);
}

template<class T>
inline bool Property<T>::operator <(const Property<T> &rhs) {
	return (data->value < rhs.data->value);
}

template<class T>
inline bool Property<T>::operator <(const T &rhs) {
	return (data->value < rhs);
}


#endif // WW_PROPERTY_H