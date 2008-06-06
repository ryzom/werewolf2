#include "stdafx.h"
#include "SOB.h"

SOB::SOB() {
	;
}

SOB::~SOB() {
	// delete views
	SOB::viewMap::iterator viewIter = m_Views.begin();
	for(;viewIter != m_Views.end();++viewIter) {
		delete viewIter->second;
	}
	m_Views.clear();

	// delete data
	SOB::dataMap::iterator iter = m_Data.begin();
	for( ;iter != m_Data.end(); ++iter) {
		delete iter->second;
	}
	m_Data.clear();
}

void SOB::addDataContainer(IDataContainer* data) {
	m_Data[data->getContainerName()] = data;
}

void SOB::removeDataContainer(const char* name) {
	SOB::dataMap::iterator iter = m_Data.find(name);
	if(iter != m_Data.end()) {
		delete iter->second;
		m_Data.erase(iter);
	}
}

IDataContainer* SOB::getDataContainer(const char* name) {
	SOB::dataMap::iterator iter = m_Data.find(name);
	if(iter == m_Data.end())
		return NULL;
	return iter->second;
}

void SOB::addView(IView *view) {
	m_Views[view->getViewName()] = view;
}

void SOB::removeView(const char* name) {
	SOB::viewMap::iterator iter = m_Views.find(name);
	if(iter != m_Views.end()) {
		delete iter->second;
		m_Views.erase(iter);
	}
}

IView* SOB::getView(const char* name) {
	SOB::viewMap::iterator iter = m_Views.find(name);
	if(iter == m_Views.end())
		return NULL;
	return iter->second;
}

void SOB::update() {
	updateViews();
}

void SOB::updateViews() {
	SOB::viewMap::iterator iter = m_Views.begin();
	for( ; iter != m_Views.end(); ++iter ) {
		iter->second->update();
	}
}

