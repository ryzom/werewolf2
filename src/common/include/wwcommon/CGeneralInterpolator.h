/**
 * \file CGeneralInterpolato.h
 * \date May 2005
 * \author Matt Raykowski
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

#ifndef __CGENERALINTERPOLATOR_H__
#define __CGENERALINTERPOLATOR_H__

//
// Standard Includes
//
#include <deque>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/value_smoother.h>

//
// Werewolf Includes
//

//
// Namespaces
//

namespace WWCOMMON {

template <class TKEY>
class CGeneralInterpolator {
public:
	/**
	 * \brief Constructor.
	 *
	 * This constructor sets the history length - the length of time to
	 * retain keys. It also adds a default key to the queue to make sure
	 * the interpolator is functional.
	 *
	 * \param historyLength The time to keep keys before they expire.
	 */
	CGeneralInterpolator(double historyLength, uint smootherSamples) {
		m_HistoryLength = historyLength;
		m_Smoother.init(smootherSamples);
		addKey(TKEY());
	};

	/**
	 * \brief Default destructor.
	 */
	virtual ~CGeneralInterpolator() {
		// does nothing
	};

	/**
	 * \brief Adds a new key to the interpolator.
	 *
	 * \param key The key to be added to the queue.
	 */
	virtual void addKey(const TKEY &key) {
		// assume added keys are newer than anything we got so far
		m_Keys.push_front(key);
		m_Smoother.addValue(key);
	};

	/**
	 * \brief Cleans up garbage keys and expired keys.
	 */
	virtual void update() {
		// TODO henri:henri maybe leave just one key to be able to give a value if asked.
		double minTime = m_Keys.front().getTime() - m_HistoryLength;
		while((m_Keys.back().getTime() < minTime) && (m_Keys.size() > 1)) {
			m_Keys.pop_back();
		}
	};

	/**
	 * \brief Get the a key in time.
	 *
	 * This returns keys based on the following tests:
	 * * It throws a warning to the log if the list is empty.
	 * * If there is only one key in the list, it returns that key.
	 * * If the key is older than the requested time and the beginning, return the beginning.
	 * * If the key is older than the requested time, interpolate between the previous key and return the result.
	 * * Return the last key if the list is exhausted.
	 *
	 * \param time The time for which you want a key.
	 * \return TKEY key representing the interpolated key.
	 */
	virtual TKEY getKey(double time) {
		typename std::deque< TKEY >::iterator it;
		it = m_Keys.begin();

		// if there are no keys just log a warning.
		if(m_Keys.empty())
			nlwarning("Interpolator accessed while empty!");

		// if there is only one key, log and continue.
		if(m_Keys.size()==1) {
			nlwarning("Only one key available, skipping interpolation.");
			return m_Keys.front();
		}

		// otherwise go through the list and interpolate.
		TKEY *previous;
		while(it != m_Keys.end()) {
			if((*it).getTime() <= time) {
				if(it == m_Keys.begin())
					return *it;

				return (*it).interpolate(time, *previous);
			}
			previous = &(*it);
			it++;
		}
//		nlwarning("Hit end of list.");
		return getLastKey();
	};

	virtual TKEY getNearestKey(double time) {
		typename std::deque< TKEY >::iterator it;
		it = m_Keys.begin();

		// if there are no keys just log a warning.
		if(m_Keys.empty())
			nlwarning("Interpolator accessed while empty!");

		// if there is only one key, log and continue.
		if(m_Keys.size()==1) {
			return m_Keys.front();
		}

		// otherwise go through the list and interpolate.
		TKEY prev = m_Keys.front();
		while(it != m_Keys.end()) {
			if((*it).getTime() <= time) {
				double dtP = prev.getTime() - time;
				double dtC = time - (*it).getTime();
				if(dtC < dtP)
					return *it;
				else
					return prev;
			}
			prev = (*it);
			++it;
		}
//		nlwarning("Hit end of list.");
		return getLastKey();
	};


	/**
	 * \brief Get the smoothed key in time given a smoothing count.
	 *
	 * This returns keys based on the following tests:
	 * * It throws a warning to the log if the list is empty.
	 * * If there is only one key in the list, it returns that key.
	 * * If the key is older than the requested time and the beginning, return the beginning.
	 * * If the key is older than the requested time, interpolate between the previous key and return the result.
	 * * Return the last key if the list is exhausted.
	 *
	 * \param time The time for which you want a key.
	 * \param samples The number of samples to take.
	 * \return TKEY key representing the interpolated key.
	 */
	virtual TKEY getSmoothKey(double time, uint samples) {
		typename std::deque< TKEY >::iterator it;
		it = m_Keys.begin();

		// if there are no keys just log a warning.
		if(m_Keys.empty())
			nlwarning("Interpolator accessed while empty!");

		// if there is only one key, log and continue.
		if(m_Keys.size()==1) {
			nlwarning("Only one key available, skipping interpolation.");
			return m_Keys.front();
		}

		NLMISC::CValueSmootherTemplate<TKEY> smoother;
		smoother.init(samples);

		while(it != m_Keys.end() && samples > 0) {
			if((*it).getTime() <= time) {
				smoother.addValue(*it);
				--samples;
			}
			++it;
		}
		return smoother.getSmoothValue();
	};

	virtual TKEY getSmoothKey() {
		return m_Smoother.getSmoothValue();
	};

	/**
	 * \brief Provides the key at the front of the queue.
	 *
	 * \return TKEY The key at the front of the queue.
	 */
	virtual TKEY &getLastKey() {	return m_Keys.front(); };

	/**
	 * \brief Provies the key at the index position.
	 *
	 * \param index The index in the queue to return.
	 * \return TKEY key value at that index.
	 */
	virtual TKEY &getIndexKey(uint16 index) {
		return m_Keys[index];
	}

	uint getNumKeys() {
		return m_Keys.size();
	}

protected:
	/**
	 * \brief The time period to retain keys before expiring them.
	 */
	double m_HistoryLength;
	
	/**
	 * \brief The key queue.
	 */
	std::deque<TKEY> m_Keys;
	NLMISC::CValueSmootherTemplate<TKEY> m_Smoother;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CGENERALINTERPOLATOR_H__

