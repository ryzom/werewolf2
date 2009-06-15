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

//
// Includes
//

#include "std_pch.h"
#include "xsi_nel_mesh_lib.h"
#include "xsi_nel_export.h"
#include "xsi_nel_material.h"

//
// Namespaces
//

using namespace XSI;
using namespace NLMISC;
using namespace NL3D;

//
// Functions
//

CStatus CMaterials::convertMaterial( Shader& xsi_shader, CMaterial& nel_material )
{
	
	Parameter parameter;
	CParameterRefArray parameters;


	parameter = xsi_shader.GetParameters().GetItem( L"unlighted" );
	bool unlighted = bool( parameter.GetValue() );

	parameter = xsi_shader.GetParameters().GetItem( L"opacity" );
	float opacity = float( parameter.GetValue() );

	parameter = xsi_shader.GetParameters().GetItem(L"alpa_test" );
	bool alpha_test = bool( parameter.GetValue() );

	parameter = xsi_shader.GetParameters().GetItem(L"no_z_write" );
	bool no_z_write = bool( parameter.GetValue() );

	parameter = xsi_shader.GetParameters().GetItem(L"force_z_write" );
	bool force_z_write = bool( parameter.GetValue() );


	CRGBAF diffuse_light_color( 0.0f, 0.0f, 0.0f, 1.0f );

	parameter = xsi_shader.GetParameters().GetItem( L"diffuse_light_color" );
	parameters = parameter.GetParameters();

	parameter = parameters.GetItem( L"red" );
	diffuse_light_color.R = float( parameter.GetValue() );
	parameter = parameters.GetItem( L"green" );
	diffuse_light_color.G = float( parameter.GetValue() );
	parameter = parameters.GetItem( L"blue" );
	diffuse_light_color.B = float( parameter.GetValue() );
	diffuse_light_color.A = opacity;


	CRGBAF ambient_light_color( 0.0f, 0.0f, 0.0f, 1.0f );

	parameter = xsi_shader.GetParameters().GetItem( L"ambient_light_color" );
	parameters = parameter.GetParameters();

	parameter = parameters.GetItem( L"red" );
	ambient_light_color.R = float( parameter.GetValue() );
	parameter = parameters.GetItem( L"green" );
	ambient_light_color.G = float( parameter.GetValue() );
	parameter = parameters.GetItem( L"blue" );
	ambient_light_color.B = float( parameter.GetValue() );


	CRGBAF emissive_light_color( 0.0f, 0.0f, 0.0f, 1.0f );

	parameter = xsi_shader.GetParameters().GetItem( L"emissive_light_color" );
	parameters = parameter.GetParameters();

	parameter = parameters.GetItem( L"red" );
	emissive_light_color.R = float( parameter.GetValue() );
	parameter = parameters.GetItem( L"green" );
	emissive_light_color.G = float( parameter.GetValue() );
	parameter = parameters.GetItem( L"blue" );
	emissive_light_color.B = float( parameter.GetValue() );


	CRGBAF specular_light_color( 0.0f, 0.0f, 0.0f, 1.0f );

	parameter = xsi_shader.GetParameters().GetItem( L"specular_light_color" );
	parameters = parameter.GetParameters();

	parameter = parameters.GetItem( L"red" );
	specular_light_color.R = float( parameter.GetValue() );
	parameter = parameters.GetItem( L"green" );
	specular_light_color.G = float( parameter.GetValue() );
	parameter = parameters.GetItem( L"blue" );
	specular_light_color.B = float( parameter.GetValue() );


	parameter = xsi_shader.GetParameters().GetItem( L"specular_glossiness" );		
	float specular_glossiness = float( parameter.GetValue() );


	if ( unlighted )
	{
		nel_material.initUnlit();
	}
	else
	{
		nel_material.initLighted();
		nel_material.setLighting( true, emissive_light_color, ambient_light_color, diffuse_light_color, specular_light_color, specular_glossiness );
	}

	if( opacity < 1.0f )
	{

		//nel_material.setOpacity( opacity );

		if( alpha_test )
		{
			nel_material.setAlphaTest( true );
			if( no_z_write )
			{
				nel_material.setZWrite( false );
			}
			else
			{
				nel_material.setZWrite( true );
			}
		}
		else
		{
			nel_material.setBlend( true );
			if( force_z_write )
			{
				nel_material.setZWrite( true );
			}
			else
			{
				nel_material.setZWrite( false );
			}
		}

		nel_material.setBlendFunc( CMaterial::srcalpha, CMaterial::invsrcalpha );

	}

	CRefArray image_clips = xsi_shader.GetImageClips();

	for( long i = 0; i < image_clips.GetCount(); i++ )
	{

		ImageClip2 clip( image_clips[ i ] );
		Source source( clip.GetSource() );
		CValue file_name = source.GetParameterValue( L"path" );
		_STL::string nel_file = CFile::getFilename( CExport::instance().convertString( file_name.GetAsText() ) );

		ITexture* src_texture = new CTextureFile;
		static_cast< CTextureFile * >( src_texture )->setFileName( nel_file );

		src_texture->setWrapS( ITexture::Clamp );
		src_texture->setWrapT( ITexture::Clamp );

		nel_material.setTexture( uint8( i ), src_texture );
		nel_material.setTexCoordGen( i, false );
	
	}

	return CStatus::OK;

}

CRGBAF convertColor( CColor xsi_color )
{
	CRGBAF nel_color;
	nel_color.R = float( xsi_color.r );
	nel_color.G = float( xsi_color.g );
	nel_color.B = float( xsi_color.b );
	nel_color.A = float( xsi_color.a );
	return nel_color;
}

CColor convertColor( CRGBAF nel_color )
{
	CColor xsi_color;
	xsi_color.r = nel_color.R;
	xsi_color.g = nel_color.G;
	xsi_color.b = nel_color.B;
	xsi_color.a = nel_color.A;
	return xsi_color;
}