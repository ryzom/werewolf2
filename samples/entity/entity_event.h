#ifndef WW_ENTITY_EVENT_H
#define WW_ENTITY_EVENT_H

#include <string>

#define WW_REGISTER_EVENT(EVENT) \
	NLMISC::CClassRegistry::registerClass(#EVENT, EVENT::creator, typeid(EVENT).name());

class EntityEvent : public NLMISC::IStreamable, public NLMISC::CRefCount {
public:
	static const uint16 EntityEventID = 0;
	NLMISC_DECLARE_CLASS(EntityEvent);

	virtual ~EntityEvent() { }
	uint16 getId() const { return m_ID; }

	virtual void serial(NLMISC::IStream &f) {
		f.serial(m_ID);

		if(hasTimestamp())
			f.serial(Timestamp);
	}

	double					Timestamp;

	/** Implement these if the event needs to know the player ID on the server */
	virtual uint32 getPlayerID() const { return 0; }
	virtual void setPlayerID(uint32) { }

	EntityEvent() : m_ID(EntityEvent::EntityEventID), Timestamp(0) { }
protected:
	

	/// implement this to indicate if a timestamp would be needed
	virtual bool hasTimestamp() const { return false; }

//~ Instance fields ****************************************
	uint16					m_ID;
};

#endif // WW_ENTITY_EVENT_H