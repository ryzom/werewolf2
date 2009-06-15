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

#ifndef NEL_TOOLS_MATERIAL_H
#define NEL_TOOLS_MATERIAL_H

//
// Includes
//



//
// Class CMaterials in xsi_nel_material.h
//

class CMaterials : public CSingleton<CMaterials>
{
public:

	XSI::CStatus			convertMaterial( XSI::Shader& xsi_shader, NL3D::CMaterial& nel_material  );

	NLMISC::CRGBA			convertColor( XSI::CColor xsi_color );
	XSI::CColor				convertColor( NLMISC::CRGBA nel_color );

private:

};

#endif
