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
#include "xsi_nel.h"
#include "xsi_nel_export.h"
#include "xsi_nel_mesh_lib.h"
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

CStatus InspectObj( const CString& in_inputobjs, const CString& in_keywords, const CString& in_title, const CValue& /*number*/ in_mode, bool in_throw )
{

	CValueArray args(5);
	CValue return_value;
	long i(0);

	args[i++] = in_inputobjs;
	args[i++] = in_keywords;
	args[i++] = in_title;
	args[i++] = in_mode;
	args[i++] = in_throw;

	// Execute XSI Command
	CStatus status = CExport::instance().getApplication().ExecuteCommand( L"InspectObj", args, return_value );

	return status;

}

int GetFolder ( char *out_szPath )
{
	BROWSEINFO bi;
	LPMALLOC pMalloc = NULL;    
	LPITEMIDLIST pidl;
	char acSrc[ MAX_PATH ];
	int retval = 0;
 
	if ( SHGetMalloc( &pMalloc ) != NOERROR )
	{
		return 0;
	}
 
	ZeroMemory( &bi, sizeof( BROWSEINFO ) );
 
	bi.hwndOwner = GetActiveWindow();
	bi.pszDisplayName = acSrc;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpszTitle  = "Export To Folder Location";
 
	if ( pidl = SHBrowseForFolder( &bi ) )
	{
		if  ( !SHGetPathFromIDList( pidl, acSrc ) )
		{
			return 0;
		}

		pMalloc->Free( pidl );
 
		retval = 1;
	}
	else
	{
		retval = 0;
	}

	if ( pMalloc )   
	{
		pMalloc->Release();
	}
 
	strcpy ( out_szPath, acSrc );
 
	return retval;	
 
}

CStatus XSILoadPlugin( PluginRegistrar& in_reg )
{

	// Set The Plugin Related Information
	in_reg.PutName( L"NeL Tools" );
	in_reg.PutVersion( 0, 1 );

	// Set The Author Related Information
	in_reg.PutAuthor( L"Debug" );
	in_reg.PutEmail( L"debug@neverborn.net" );
	in_reg.PutURL( L"www.neverborn.net" );

	// Register Property Page
	in_reg.RegisterProperty( L"NeLProperties" );

	// Create Menus
	in_reg.RegisterMenu( siMenuMainApplicationID, L"NeL Tools", true, false );

	// Log Status Message
	CExport::instance().getApplication().LogMessage( in_reg.GetName() + L" has been loaded." );
	// TODO : log to nel

	return CStatus::OK;

}

CStatus XSIUnloadPlugin( PluginRegistrar& in_reg )
{

	// Log Status Message
	CExport::instance().getApplication().LogMessage( in_reg.GetName() + L" has been unloaded." );

	return CStatus::OK;

}

//
// Property Pages
//

CStatus NeLProperties_Define( const CRef & in_context )
{

	CustomProperty	custom_property = Context(in_context).GetSource();
	Parameter		parameter;
	CValue			dummy;	// Used for arguments we don't want to set

	int capabilities = siPersistable | siAnimatable;

	// Controls For IO

	custom_property.AddParameter( L"IO_Use_Object_Name_For_Export_CheckBox", CValue::siBool, capabilities, L"", L"", true, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"IO_Export_As_Shape_CheckBox", CValue::siBool, capabilities, L"", L"", true, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"IO_Export_As_Vegetable_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"IO_Export_As_Character_Lod_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"IO_Export_Associated_Skeleton_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"IO_Never_Export_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"IO_Verbose_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"IO_Very_Verbose_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"IO_Insane_Verbosity_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );

	// Controls For Animation

	custom_property.AddParameter( L"ANIMATION_Prefix_Track_Names_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"ANIMATION_Export_Node_Animation_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"ANIMATION_Export_Animated_Materials_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"ANIMATION_Export_Note_Track_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"ANIMATION_Export_SSS_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"ANIMATION_Auto_Animation_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );

	// Controls For MRM

	custom_property.AddParameter( L"MRM_Active_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"MRM_Skinning_RadioGroup", CValue::siInt4, capabilities, L"", L"", 2l, 0l, 2l, dummy, dummy, parameter );
	custom_property.AddParameter( L"MRM_Steps_Slider", CValue::siInt4, capabilities, L"", L"", 16l, 1l, 256l, dummy, dummy, parameter );
	custom_property.AddParameter( L"MRM_Poly_Slider", CValue::siInt4, capabilities, L"", L"", 8l, 1l, 256l, dummy, dummy, parameter );
	custom_property.AddParameter( L"MRM_Distance_Finest_Slider", CValue::siInt4, capabilities, L"", L"", 1l, 1l, 4096l, dummy, dummy, parameter );
	custom_property.AddParameter( L"MRM_Distance_Middle_Slider", CValue::siInt4, capabilities, L"", L"", 127l, 1l, 4096l, dummy, dummy, parameter );
	custom_property.AddParameter( L"MRM_Distance_Coarse_Slider", CValue::siInt4, capabilities, L"", L"", 768l, 1l, 4096l, dummy, dummy, parameter );
	custom_property.AddParameter( L"MRM_Distance_Max_Slider", CValue::siInt4, capabilities, L"", L"", 1024l, 1l, 4096l, dummy, dummy, parameter );

	// Controls For LOD

	custom_property.AddParameter( L"LOD_Ungrowing_List_MultiLine", CValue::siString, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );	
	custom_property.AddParameter( L"LOD_Add_Button", CValue::siString, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LOD_Remove_Button", CValue::siString, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LOD_Up_Button", CValue::siString, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LOD_Down_Button", CValue::siString, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LOD_Blend_In_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LOD_Blend_Out_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LOD_Dynamic Mesh_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LOD_Course_Mesh_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LOD_Blend_Length_Slider", CValue::siInt4, capabilities, L"", L"", 8l, 1l, 4096l, dummy, dummy, parameter );
	custom_property.AddParameter( L"LOD_Bone_Distance_Max_Slider", CValue::siFloat, capabilities, L"", L"", 0l, 0l, 4096l, dummy, dummy, parameter );

	// Controls For Lighting

	custom_property.AddParameter( L"LIGHTING_Local_Lighting_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LIGHTING_Shadow_Caster_CheckBox", CValue::siBool, capabilities, L"", L"", true, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LIGHTING_Shadow_Reciever_CheckBox", CValue::siBool, capabilities, L"", L"", true, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LIGHTING_Time_RadioGroup", CValue::siInt4, capabilities, L"", L"", 0l, 0l, 2l, dummy, dummy, parameter );
	custom_property.AddParameter( L"LIGHTING_Do_Not_Cast_Exterior_Shadow", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LIGHTING_Do_Not_Cast_Interior_Shadow", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"LIGHTING_Time_RadioGroup", CValue::siInt4, capabilities, L"", L"", 1l, 0l, 2l, dummy, dummy, parameter );
	custom_property.AddParameter( L"LIGHTING_Shadow_Mesh_Percent_Slider", CValue::siInt4, capabilities, L"", L"", 50l, 1l, 100l, dummy, dummy, parameter );
	custom_property.AddParameter( L"LIGHTING_Shadow_Mesh_Faces_Slider", CValue::siInt4, capabilities, L"", L"", 256l, 1l, 2096l, dummy, dummy, parameter );

	// Controls For Collision

	custom_property.AddParameter( L"COLLISION_Camera_RadioGroup", CValue::siInt4, capabilities, L"", L"", 1l, 0l, 2l, dummy, dummy, parameter );
	custom_property.AddParameter( L"COLLISION_Mesh_Interior_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"COLLISION_Mesh_Exterior_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );

	// Controls For Vegetation

	custom_property.AddParameter( L"VEGETATION_Alpha_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"VEGETATION_Alpha_Sub_Radio_Group", CValue::siInt4, capabilities, L"", L"", 0l, 0l, 2l, dummy, dummy, parameter );
	custom_property.AddParameter( L"VEGETATION_Bend_Radio_Group", CValue::siInt4, capabilities, L"", L"", 0l, 0l, 1l, dummy, dummy, parameter );
	custom_property.AddParameter( L"VEGETATION_Bend_Factor_Slider", CValue::siFloat, capabilities, L"", L"", 1.0f, 0.0f, 1.0f, dummy, dummy, parameter );
	custom_property.AddParameter( L"VEGETATION_Best_Sided_Lighting_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );
	custom_property.AddParameter( L"VEGETATION_Double_Sided_CheckBox", CValue::siBool, capabilities, L"", L"", dummy, dummy, dummy, dummy, dummy, parameter );

	return CStatus::OK;

}

CStatus NeLProperties_DefineLayout( const CRef & in_context )
{

	PPGLayout	custom_layout = Context( in_context ).GetSource();
	PPGItem		item;

	custom_layout.Clear() ;

	// IO Properties

	custom_layout.AddTab( L"IO" );
		custom_layout.AddGroup( L"Export" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"IO_Use_Object_Name_For_Export_CheckBox", L"Export Object Name As File Name" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"IO_Export_As_Shape_CheckBox", L"Export As Shape" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"IO_Export_As_Vegetable_CheckBox", L"Export As Vegetable" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"IO_Export_As_Character_Lod_CheckBox", L"Export As Character LOD" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"IO_Export_Associated_Skeleton_CheckBox", L"Export This Objects Skeleton" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"IO_Never_Export_CheckBox", L"Disable Export Of This Object" );

			custom_layout.EndRow();
		custom_layout.EndGroup();
		custom_layout.AddGroup( L"Verbosity" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"IO_Verbose_CheckBox", L"Verbose Output" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"IO_Very_Verbose_CheckBox", L"Very Verbose Output" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"IO_Insane_Verbosity_CheckBox", L"Very Very Verbose Output" );

			custom_layout.EndRow();
		custom_layout.EndGroup();

	// Animation Properties

	custom_layout.AddTab( L"Animation" );
		custom_layout.AddGroup( L"Options" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"ANIMATION_Prefix_Track_Names_CheckBox", L"Prefix Track Names With Node Name" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"ANIMATION_Export_Node_Animation_CheckBox", L"Export Node Animation Track" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"ANIMATION_Export_Animated_Materials_CheckBox", L"Export Animated Materials" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"ANIMATION_Export_Note_Track_CheckBox", L"Export Note Track" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"ANIMATION_Export_SSS_CheckBox", L"Export Skeleton Spawn Script Track" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"ANIMATION_Auto_Animation_CheckBox", L"Automatically Bind Animation Based On Object Name" );

			custom_layout.EndRow();
		custom_layout.EndGroup();

	// MRM Properties

	CValueArray skin_redution_radio_group_options( 6 );
	skin_redution_radio_group_options[0] = L"Minimum";
	skin_redution_radio_group_options[1] = 0l;
	skin_redution_radio_group_options[2] = L"Best";	// (Default)
	skin_redution_radio_group_options[3] = 2l ;		// (Default)
	skin_redution_radio_group_options[4] = L"Maximum";
	skin_redution_radio_group_options[5] = 1l;

	custom_layout.AddTab( L"MRM" );

		custom_layout.AddGroup( L"Options" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"MRM_Active_CheckBox", L"Activate Multi Resolution Mesh" );

			custom_layout.EndRow();
		custom_layout.EndGroup();

		custom_layout.AddGroup( L"Skin Reduction" );
			custom_layout.AddRow();

				item = custom_layout.AddEnumControl( L"MRM_Skinning_RadioGroup", skin_redution_radio_group_options, L"", siControlRadio );
				item.PutAttribute( siUINoLabel, true );

			custom_layout.EndRow();
		custom_layout.EndGroup();

		custom_layout.AddGroup( L"Values" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"MRM_Steps_Slider", L"Steps" );
				item.PutLabelMinPixels( 128l );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"MRM_Poly_Slider", L"Poly Divsor" );
				item.PutLabelMinPixels( 128l );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"MRM_Distance_Finest_Slider", L"Distance Finest" );
				item.PutLabelMinPixels( 128l );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"MRM_Distance_Middle_Slider", L"Distance Middle" );
				item.PutLabelMinPixels( 128l );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"MRM_Distance_Coarse_Slider", L"Distance Coarsest" );
				item.PutLabelMinPixels( 128l );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"MRM_Distance_Max_Slider", L"Distance Maximum" );
				item.PutLabelMinPixels( 128l );

			custom_layout.EndRow();
		custom_layout.EndGroup();

	// LOD Properties

	custom_layout.AddTab( L"LOD" );


		custom_layout.AddGroup( L"Options" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"MRM_Distance_Max_Slider", L"Distance Maximum" );
				item.PutLabelMinPixels( 128l );

			custom_layout.EndRow();
		custom_layout.EndGroup();
		custom_layout.AddGroup( L"Ungrowing Order" );
			custom_layout.AddRow();

				item = custom_layout.AddEnumControl( L"LOD_Ungrowing_List_MultiLine", NULL, L"", siControlListBox );
				item.PutAttribute( siUINoLabel, true );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddButton( L"LOD_Add_Button", L"Add" );
				item.PutWidthPercentage( 25l );
				item = custom_layout.AddButton( L"LOD_Remove_Button", L"Remove" );
				item.PutWidthPercentage( 25l );
				item = custom_layout.AddButton( L"LOD_Up_Button", L"Up" );
				item.PutWidthPercentage( 25l );
				item = custom_layout.AddButton( L"LOD_Down_Button", L"Down" );
				item.PutWidthPercentage( 25l );

			custom_layout.EndRow();
		custom_layout.EndGroup();
		custom_layout.AddGroup( L"Blending" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LOD_Blend_In_CheckBox", L"Blend In" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LOD_Blend_Out_CheckBox", L"Blend Out" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LOD_Blend_Length_Slider", L"Blend Length" );
				item.PutLabelMinPixels( 128l );

			custom_layout.EndRow();
		custom_layout.EndGroup();
		custom_layout.AddGroup( L"Bones" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LOD_Bone_Distance_Max_Slider", L"Bone LOD Disable Distance" );
				item.PutLabelMinPixels( 128l );

			custom_layout.EndRow();
		custom_layout.EndGroup();

	// Lighting Properties

	CValueArray lighting_time_radio_group_options( 6 );
	lighting_time_radio_group_options[0] = L"Always";	// (Default)
	lighting_time_radio_group_options[1] = 0l;			// (Default)
	lighting_time_radio_group_options[2] = L"Day";
	lighting_time_radio_group_options[3] = 1l ;
	lighting_time_radio_group_options[4] = L"Night";
	lighting_time_radio_group_options[5] = 2l;

	custom_layout.AddTab( L"Lighting" );

		custom_layout.AddGroup( L"Lighting Group" );
			custom_layout.AddRow();

				item = custom_layout.AddEnumControl( L"LIGHTING_Time_RadioGroup", lighting_time_radio_group_options, L"", siControlRadio );
				item.PutAttribute( siUINoLabel, true );

			custom_layout.EndRow();
		custom_layout.EndGroup();
		custom_layout.AddGroup( L"Options" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LIGHTING_Local_Lighting_CheckBox", L"Use Local Lighting" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LIGHTING_Shadow_Caster_CheckBox", L"Casts Shadows" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LIGHTING_Shadow_Reciever_CheckBox", L"Recieves Shadows" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LIGHTING_Do_Not_Cast_Exterior_Shadow", L"Do Not Cast Shadows Outside" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LIGHTING_Do_Not_Cast_Interior_Shadow", L"Do Not Cast Shadows Inside" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LIGHTING_Shadow_Mesh_Percent_Slider", L"Max Percent Shadow Faces" );
				item.PutLabelMinPixels( 160l );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"LIGHTING_Shadow_Mesh_Faces_Slider", L"Max Number Shadow Faces" );
				item.PutLabelMinPixels( 160l );

			custom_layout.EndRow();
		custom_layout.EndGroup();

	// LMC Properties
		
	custom_layout.AddTab( L"LMC" );

		custom_layout.AddGroup( L"Options" );
			custom_layout.AddRow();

			custom_layout.EndRow();
		custom_layout.EndGroup();

	// Collision Properties

	CValueArray collision_camera_radio_group_options( 6 );
	collision_camera_radio_group_options[0] = L"Automatic ( Collision if mesh is lightmapped )";	// (Default)
	collision_camera_radio_group_options[1] = 0l;			// (Default)
	collision_camera_radio_group_options[2] = L"Disabled ( No Camera Collision )";
	collision_camera_radio_group_options[3] = 1l ;
	collision_camera_radio_group_options[4] = L"Always ( Collision with camera always )";
	collision_camera_radio_group_options[5] = 2l;

	custom_layout.AddTab( L"Collision" );

		custom_layout.AddGroup( L"Camera" );
			custom_layout.AddRow();

				item = custom_layout.AddEnumControl( L"COLLISION_Camera_RadioGroup", collision_camera_radio_group_options, L"", siControlRadio );
				item.PutAttribute( siUINoLabel, true );

			custom_layout.EndRow();
		custom_layout.EndGroup();
		custom_layout.AddGroup( L"Mesh" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"COLLISION_Mesh_Interior_CheckBox", L"Collision Mesh Interior" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"COLLISION_Mesh_Exterior_CheckBox", L"Collision Mesh Exterior" );

			custom_layout.EndRow();
		custom_layout.EndGroup();

	// Vegetable Properties
		
	CValueArray vegetation_alpha_sub_radio_group_options( 6 );
	vegetation_alpha_sub_radio_group_options[0] = L"Precomputed Lighting";	// (Default)
	vegetation_alpha_sub_radio_group_options[1] = 0l;						// (Default)
	vegetation_alpha_sub_radio_group_options[2] = L"Dynamic Lighting";
	vegetation_alpha_sub_radio_group_options[3] = 1l ;
	vegetation_alpha_sub_radio_group_options[4] = L"No Lighting";
	vegetation_alpha_sub_radio_group_options[5] = 2l ;

	CValueArray vegetation_bend_radio_group_options( 4 );
	vegetation_bend_radio_group_options[0] = L"Center Z";	// (Default)
	vegetation_bend_radio_group_options[1] = 0l;			// (Default)
	vegetation_bend_radio_group_options[2] = L"Center Null";
	vegetation_bend_radio_group_options[3] = 1l ;

	custom_layout.AddTab( L"Vegetables" );

		custom_layout.AddGroup( L"Alpha" );
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"VEGETATION_Alpha_CheckBox", L"Alpha Blending" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddEnumControl( L"VEGETATION_Alpha_Sub_Radio_Group", vegetation_alpha_sub_radio_group_options, L"", siControlRadio );
				item.PutAttribute( siUINoLabel, true );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"VEGETATION_Best_Sided_Lighting_CheckBox", L"Best Sided Lighting" );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"VEGETATION_Double_Sided_CheckBox", L"Double Sided" );

			custom_layout.EndRow();
		custom_layout.EndGroup();
		custom_layout.AddGroup( L"Bend" );
			custom_layout.AddRow();

				item = custom_layout.AddEnumControl( L"VEGETATION_Bend_Radio_Group", vegetation_bend_radio_group_options, L"", siControlRadio );
				item.PutAttribute( siUINoLabel, true );

			custom_layout.EndRow();
			custom_layout.AddRow();

				item = custom_layout.AddItem( L"VEGETATION_Bend_Factor_Slider", L"Bend Factor" );
				item.PutLabelMinPixels( 128l );

			custom_layout.EndRow();


	return CStatus::OK;

}

CStatus NeLProperties_PPGEvent( const CRef& io_context )
{

	// Get Context
	PPGEventContext context( io_context );
	PPGEventContext::PPGEvent event_id = context.GetEventID();
	CustomProperty custom_property = context.GetSource();

	// Event Handlers
	if ( event_id == PPGEventContext::siOnInit )
	{
		CExport::instance().getApplication().LogMessage( L"NeL properties editor has been displayed." );				
	}
	else if ( event_id == PPGEventContext::siButtonClicked )
	{
		CValue button = context.GetAttribute( L"Button" );
		CExport::instance().getApplication().LogMessage( L"Button '" + CString( button ) + L"' was pressed." );				
	}
	else if ( event_id == PPGEventContext::siParameterChange )
	{
		Parameter parameter = context.GetSource();
		CustomProperty changed_property = parameter.GetParent();

		if ( changed_property.GetFullName() == L"" )
		{

		}

		CExport::instance().getApplication().LogMessage( L"Parameter changed '" + changed_property.GetFullName() + L"' was changed to '" + parameter.GetValue().GetAsText() + L"'" );				
	}

	return CStatus::OK;

}

//
// Menus
//

CStatus NeLTools_Init( CRef& in_ref )
{

	// Set Up Variables
	Context		context;
	CStatus		status;
	Menu		menu;
	MenuItem	item;

	// Populate Context
	context = in_ref;
	menu = context.GetSource();


	// Create Clear NeL Properties Menu Item
	status = menu.AddCallbackItem( L"Remove Properties", L"onClearNeLProperties", item );

	// Did Create Menu Work
	if ( status != CStatus::OK ) { return status; }

	// Create Unflag Vertice(s) As Menu Item
	status = menu.AddCallbackItem( L"Remove MRM Interfaces", L"onClearMRMInterface", item );

	// Did Create Menu Work
	if ( status != CStatus::OK ) { return status; }

	// Create Flag Vertice(s) As Menu Item
	status = menu.AddCallbackItem( L"Build MRM Interfaces", L"onBuildMRMInterface", item );

	// Did Create Menu Work
	if ( status != CStatus::OK ) { return status; }

	// Create Clear NeL Properties Menu Item
	status = menu.AddCallbackItem( L"Build Material", L"onAddNeLMaterial", item );

	// Did Create Menu Work
	if ( status != CStatus::OK ) { return status; }

	// Create Export NeL Object Menu Item
	status = menu.AddCallbackItem( L"Export Object", L"onExportObjectToNeL", item );

	// Did Create Menu Work
	if ( status != CStatus::OK ) { return status; }

	// Create Export NeL Object Menu Item
	status = menu.AddCallbackItem( L"Export Skeleton", L"onExportSkeletonToNeL", item );

	// Did Create Menu Work
	if ( status != CStatus::OK ) { return status; }

	// Create Export NeL Object Menu Item
	status = menu.AddCallbackItem( L"Export Animation", L"onExportAnimationToNeL", item );

	// Did Create Menu Work
	if ( status != CStatus::OK ) { return status; }

	// Create NeL Properties Menu Item
	status = menu.AddCallbackItem( L"Object Properties", L"onEditNeLProperties", item );

	// Did Create Menu Work
	if ( status != CStatus::OK ) { return status; }

	return CStatus::OK; 

}

CStatus NeLTools_Term( CRef& in_ref )
{

	// Log Status Message
	CExport::instance().getApplication().LogMessage( L" NeL Tools menus removed successfully." );

	return CStatus::OK; 

}

//
// Menu Callbacks
//

CStatus onBuildMRMInterface( CRef& in_ref )
{

	// Configure Viewports
	CExport::instance().configureCameras();

	// Get All Selected Objects
	CRefArray objects = CExport::instance().getApplication().GetSelection().GetArray();

	if ( objects.GetCount() < 1 )
	{
		// Log Error Message
		CExport::instance().getApplication().LogMessage( L"Cannot build interfaces need at least one object.", siWarningMsg );
		nlinfo( "Cannot build interfaces need at least one object." );
		return CStatus::Fail;
	}

	// Check Object
	for( long i = 0; i < objects.GetCount(); i++ )
	{
		// Get Object To Test
		X3DObject test_object( objects[ i ] );

		if ( objects[ i ].GetClassID() != siX3DObjectID )
		{
			// Log Error Message
			CExport::instance().getApplication().LogMessage( L"Object '" + objects[ i ].GetAsText() + L"' cannot have an interface.", siWarningMsg  );
			nlinfo("Object '%s' cannot have an interface.", CExport::instance().convertString( objects[ i ].GetAsText() ).c_str() );
		}
		else
		{
			if ( CExport::instance().buildMRMInterfaces( test_object ) != CStatus::OK )
			{
				// Log Error Message
				CExport::instance().getApplication().LogMessage( L"Failed to build interfaces for '" + test_object.GetName() + L"'.", siWarningMsg  );
				nlinfo( "Failed to build interfaces for '%s'.", CExport::instance().convertString( test_object.GetName() ).c_str() );
			}
			else
			{
				// Log Status Message
				CExport::instance().getApplication().LogMessage( L"MRM interfaces built for '" + test_object.GetName() + L"'." );
				nlinfo( "MRM interfaces built for '%s'.", CExport::instance().convertString( test_object.GetName() ).c_str() );
			}
		}
	}

	return CStatus::OK;

}

CStatus onClearMRMInterface( CRef& in_ref )
{ 

	// Get All Selected Objects
	CRefArray objects = CExport::instance().getApplication().GetSelection().GetArray();

	if ( objects.GetCount() < 1 )
	{
		// Log Error Message
		CExport::instance().getApplication().LogMessage( L"Cannot clear interfaces need at least one object.", siWarningMsg );
		nlinfo( "Cannot clear interfaces need at least one object." );
		return CStatus::Fail;
	}

	for( long i = 0; i < objects.GetCount(); i++ )
	{
		// Get Object To Test
		X3DObject test_object( objects[ i ] );

		if ( objects[ i ].GetClassID() != siX3DObjectID )
		{
			// Log Error Message
			CExport::instance().getApplication().LogMessage( L"Object '" + objects[ i ].GetAsText() + L"' cannot have an interface." );
			nlinfo("Object '%s' cannot have an interface.", CExport::instance().convertString( objects[ i ].GetAsText() ).c_str() );
		}
		else
		{
			if ( CExport::instance().clearMRMInterfaces( X3DObject( objects[ i ] ) ) != CStatus::OK )
			{
				// Log Error Message
				CExport::instance().getApplication().LogMessage( L"Cannot clear interfaces for '" + test_object.GetName() + L"'.", siWarningMsg  );
				nlinfo( "Cannot clear interfaces for '%s'.", CExport::instance().convertString( test_object.GetName() ).c_str() );
			}
			else
			{
				// Log Status Message
				CExport::instance().getApplication().LogMessage( L"Cleared interfaces for '" + test_object.GetName() + L"'." );
				nlinfo( "Cleared interfaces for '%s'.", CExport::instance().convertString( test_object.GetName() ).c_str() );
			}
		}
	}

	return CStatus::OK;

}

CStatus onEditNeLProperties( CRef& in_ref )
{

	// Get All Selected Objects
	CRefArray objects = CExport::instance().getApplication().GetSelection().GetArray();

	if ( objects.GetCount() < 1 )
	{
		// Log Error Message
		CExport::instance().getApplication().LogMessage( L"Cannot add or edit custom data need at least one object.", siWarningMsg );
		nlinfo( "Cannot add or edit custom data need at least one object." );
		return CStatus::Fail;
	}

	for( long i = 0; i < objects.GetCount(); i++ )
	{
		// Get Object To Test
		X3DObject test_object( objects[ i ] );

		if ( objects[ i ].GetClassID() != siX3DObjectID )
		{
			// Log Error Message
			CExport::instance().getApplication().LogMessage( L"Object '" + objects[ i ].GetAsText() + L"' cannot have NeL properties." );
			nlinfo("Object '%s' cannot have NeL properties.", CExport::instance().convertString( objects[ i ].GetAsText() ).c_str() );
		}
		else
		{
			CustomProperty custom_property = test_object.GetProperties().GetItem( L"NeLProperties" );
	
			if ( !custom_property.IsValid() )
			{
				custom_property = test_object.AddProperty( L"NeLProperties" );
			}

			InspectObj( custom_property.GetFullName(), L"", L"NeL Mesh Properties", CValue( long( siModal ) ), false);
		}
	}

	return CStatus::OK;

}

CStatus onAddNeLMaterial( CRef& in_ref )
{

	// Variables
	CValueArray arguments( 1 );
	CValue return_value;

	// Apply Shader
	arguments[0] = L"xsi_nel_shader.Preset";
	CExport::instance().getApplication().ExecuteCommand( L"ApplyShader", arguments, return_value );

	return CStatus::OK;

}

CStatus onClearNeLProperties( CRef& in_ref )
{ 

	// Get All Selected Objects
	CRefArray objects = CExport::instance().getApplication().GetSelection().GetArray();

	if ( objects.GetCount() < 1 )
	{
		// Log Error Message
		CExport::instance().getApplication().LogMessage( L"Cannot clear NeL properties need at least one object.", siWarningMsg );
		nlinfo( "Cannot clear NeL properties need at least one object." );
		return CStatus::Fail;
	}

	for( long i = 0; i < objects.GetCount(); i++ )
	{
		// Get Object To Test
		X3DObject test_object( objects[ i ] );

		if ( objects[ i ].GetClassID() != siX3DObjectID )
		{
			// Log Error Message
			CExport::instance().getApplication().LogMessage( L"Object '" + objects[ i ].GetAsText() + L"' cannot have NeL properties." );
			nlinfo("Object '%s' cannot have NeL properties.", CExport::instance().convertString( objects[ i ].GetAsText() ).c_str() );
		}
		else
		{
			if ( CExport::instance().clearObjectProperties( X3DObject( objects[ i ] ) ) != CStatus::OK )
			{
				// Log Error Message
				CExport::instance().getApplication().LogMessage( L"Cannot clear NeL properties for '" + test_object.GetName() + L"'.", siWarningMsg  );
				nlinfo( "Cannot clear NeL properties for '%s'.", CExport::instance().convertString( test_object.GetName() ).c_str() );
			}
			else
			{
				// Log Status Message
				CExport::instance().getApplication().LogMessage( L"Cleared NeL properties for '" + test_object.GetName() + L"'." );
				nlinfo( "Cleared NeL properties for '%s'.", CExport::instance().convertString( test_object.GetName() ).c_str() );
			}
		}
	}

	return CStatus::OK;

}

CStatus onExportObjectToNeL( CRef& in_ref )
{ 

	// Get All Selected Objects
	CRefArray objects = CExport::instance().getApplication().GetSelection().GetArray();

	if ( objects.GetCount() < 1 )
	{
		// Log Error Message
		CExport::instance().getApplication().LogMessage( L"Cannot export data need at least one object.", siWarningMsg );
		nlinfo( "Cannot export data need at least one object." );
		return CStatus::Fail;
	}

	// Rest Multi File Export Path
	CExport::instance().file_path = L"";

	// Check Object
	for( long i = 0; i < objects.GetCount(); i++ )
	{
		// Get Object To Export
		X3DObject export_object( objects[ i ] );

		if ( objects[ i ].GetClassID() != siX3DObjectID )
		{
			// Log Error Message
			CExport::instance().getApplication().LogMessage( L"Cannot export mesh data from '" + objects[ i ].GetAsText() + L"'.", siWarningMsg );
			nlinfo( "Cannot export mesh data from '%s'.", CExport::instance().convertString( objects[ i ].GetAsText() ).c_str() );
		}
		else
		{
			// Get NeL Property Page
			CustomProperty custom_property = export_object.GetProperties().GetItem( L"NeLProperties" );

			// Check NeL Property Page Exists
			if ( !custom_property.IsValid() )
			{
				// Log Error Message
				CExport::instance().getApplication().LogMessage( L"Cannot obtain NeL properties page from object '" + export_object.GetFullName() + L"'.", siWarningMsg );
				nlinfo( "Cannot obtain NeL properties page from object %s.", CExport::instance().convertString( export_object.GetFullName() ).c_str() );
			}
			else
			{
				// Get Never Export Object Option ( IO Page ) 
				Parameter parameter = custom_property.GetParameters().GetItem( L"IO_Never_Export_CheckBox" );
				bool never_export = bool( parameter.GetValue() );

				if ( never_export )
				{
						// Log Error Message
						CExport::instance().getApplication().LogMessage( L"Object '" + export_object.GetFullName() + L"' is flagged as don't export." );
						nlinfo( "Object '%s' is flagged as don't export.", CExport::instance().convertString( export_object.GetFullName() ).c_str() );
				}
				else
				{
					// Get Export Object Name As File Name Option ( IO Page ) 
					Parameter parameter = custom_property.GetParameters().GetItem( L"IO_Use_Object_Name_For_Export_CheckBox" );
					bool get_filename = bool( parameter.GetValue() );

					if ( get_filename && CExport::instance().file_path == L"" )
					{

						char *char_path = (char *)malloc( MAX_PATH );

						if ( GetFolder( char_path ) != 1 )
						{
							// Log Error Message
							CExport::instance().getApplication().LogMessage( L"Export was aborted.", siWarningMsg );
							nlinfo( "Export was aborted." );
							return CStatus::Fail;
						}

						_STL::string std_path = char_path;
						free( char_path );

						nldebug( "Folder path : ['%s'] ", std_path.c_str() );

						CExport::instance().file_path = CExport::instance().convertString( std_path );
					}

					if ( CExport::instance().exportMesh( export_object ) != CStatus::OK )
					{
						// Log Error Message
						CExport::instance().getApplication().LogMessage( L"An error occured exporting object '" + export_object.GetFullName() + L"'.", siWarningMsg );
						nlinfo( "An error occured exporting object '%s'.", CExport::instance().convertString( export_object.GetFullName() ).c_str() );
					}
					else
					{
						// Log Status Message
						CExport::instance().getApplication().LogMessage( L"Object '" + export_object.GetFullName() + L"' was exported." );
						nlinfo( "Object '%s' was exported.", CExport::instance().convertString( export_object.GetFullName() ).c_str() );
					}
				}
			}
		}
	}

	return CStatus::OK;

}

CStatus onExportAnimationToNeL( CRef& in_ref )
{ 

	return CStatus::OK;

}


CStatus onExportSkeletonToNeL( CRef& in_ref )
{ 

	return CExport::instance().exportSkeleton();

}
