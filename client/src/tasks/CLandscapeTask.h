/**
 * \file CLandscapeTask.h
 * \date May 2004
 * \author Matt Raykowski
 * \author Some code adapted from Snowballs, Nevrax France.
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

#ifndef __CLANDSCAPETASK_H__
#define __CLANDSCAPETASK_H__

//
// System Includes
//
#include <vector>
#include "CEGUI.h"

//
// NeL Includes
//
#include <nel/3d/u_landscape.h>
#include <nel/misc/time_nl.h>
#include <nel/misc/vector.h>
#include <nel/3d/u_light.h>
#include <nel/misc/smart_ptr.h>
#include <nel/misc/progress_callback.h>
//
// Werewolf Includes
//
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Namespaces
//
using namespace NL3D;
using namespace NLMISC;

namespace WWCLIENT {

//
// Typdefs
//
typedef std::vector<UInstanceGroup*> InstanceGroupVector;

//
// Class
//

/**
 * \brief The task for management of landscape.
 */
class CLandscapeTask : public WWCOMMON::ISingleton<CLandscapeTask>, public WWCOMMON::ITask {
public:
	/**
	 * \brief Initializes the landscape system.
	 *
	 * This method initializes the landscape system by creating the landscape
	 * driver and loading the appropriate landscape banks.
	 */
	virtual void init();

	/**
	 * \brief Refreshes the landscape in view.
	 */
	virtual void update();
	virtual void render();
	virtual void release(); /**< From ITask, contains no logic. */

	void reset();
	void error(std::string reason);
	
	virtual std::string name(); /**< Returns the name of the task, CLandscapeTask. */

	NL3D::ULandscape &getLandscape();
	InstanceGroupVector &getInstanceGroups();
	NLMISC::CVector getSunDirection();

	float getZoneRadius() { return m_ZoneRadius; }
	void setZoneRadius(float radius) { m_ZoneRadius=radius; }
	void updateProgressBar(float prog1, float prog2, std::string message);
private:
	//CEGUI::MultiLineEditbox				*m_DebugZonesList;

	NL3D::ULandscape					*m_Landscape;
	std::vector<NL3D::UInstanceGroup*>	 m_InstanceGroups;
	NLMISC::CVector						 m_SunDirection;
	NLMISC::CVector						 m_CurPos;
	float								 m_ZoneRadius;

	// zone refresher data.
	std::vector<std::string>			m_ZonesAdded;
	std::vector<std::string>			m_ZonesRemoved;
};

/**
 * \brief A general progress bar callback.
 */
class CLandCallback : public IProgressCallback {
public:
	CLandCallback() { m_BaseProgress=0.0f; }
	virtual ~CLandCallback() { ; }

	void setProgressBar(CEGUI::ProgressBar *progbar) { m_ProgBar=progbar; m_BaseProgress=m_ProgBar->getProgress(); }
	virtual void progress(float progressValue) {
		float val=(getCropedValue(progressValue)*0.2f)+m_BaseProgress;
		CLandscapeTask::instance().updateProgressBar( val, val, "Loading landscape zones..." );
	}

protected:
	CEGUI::ProgressBar *m_ProgBar;
	float m_BaseProgress;
};

}; // END NAMESPACE WWCLIENT

#endif // __CLANDSCAPETASK_H__
