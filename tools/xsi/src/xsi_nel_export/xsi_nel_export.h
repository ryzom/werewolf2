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

#ifndef NEL_TOOLS_EXPORT_H
#define NEL_TOOLS_EXPORT_H

//
// Includes
//



//
// class CExport in xsi_nel_export.h
//

class CExport : public CSingleton<CExport>
{
public:

	NLMISC::CVector				convertVector( XSI::MATH::CVector3 xsi_vector );
	XSI::MATH::CVector3			convertVector( NLMISC::CVector nel_vector );
	NLMISC::CVector				convertVectorAndAxis( XSI::MATH::CVector3 xsi_vector );
	NLMISC::CVector				convertVectorAndAxisAndInvertX( XSI::MATH::CVector3 xsi_vector );
	NLMISC::CQuat				convertQuaternion( XSI::MATH::CQuaternion xsi_quaternion );
	XSI::MATH::CQuaternion		convertQuaternion( NLMISC::CQuat nel_quaternion );
	NLMISC::CQuat				convertQuaternionAndAxis( XSI::MATH::CQuaternion xsi_quaternion );
	NLMISC::CMatrix				convertMatrix( XSI::MATH::CMatrix4 xsi_matrix );
	_STL::string				convertString( XSI::CString xsi_string );
	XSI::CString				convertString( _STL::string nel_string );

	XSI::Application			getApplication() { return xsi_application; };

   	XSI::CStatus				exportMesh( XSI::X3DObject& object );
   	XSI::CStatus				exportSkeleton();
	XSI::CStatus				buildMRMInterfaces( XSI::X3DObject object );
   	XSI::CStatus				clearMRMInterfaces( XSI::X3DObject object );

   	XSI::CStatus				clearObjectProperties( XSI::X3DObject object );

	XSI::CStatus				configureCameras();

	XSI::CString				file_path;

private:

	XSI::Application			xsi_application;

};

#endif
