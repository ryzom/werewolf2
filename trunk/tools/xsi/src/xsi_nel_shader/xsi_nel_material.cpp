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

#include <math.h>
#include <shader.h>

#include "xsi_nel_xsi_shader.h"

DLLEXPORT int nel_xsi_shader_version(void)
{

	return 1;

}

DLLEXPORT void nel_xsi_shader_init( miState * state, nel_xsi_shader_params * in_pParams, miBoolean *inst_req)
{

	if(!in_pParams)
	{
		*inst_req = miFALSE;
	}
	else
	{

	}

}

DLLEXPORT miBoolean nel_xsi_shader( miColor * out_pResult, miState * state, nel_xsi_shader_params * in_pParams )
{

	// read in the dialog box values
    miColor ambient = *mi_eval_color( &in_pParams->m_ambient_light_color );
    miColor diffuse = *mi_eval_color( &in_pParams->m_diffuse_light_color );
    miColor specular = *mi_eval_color( &in_pParams->m_specular_light_color );

    miInteger spec_size = *mi_eval_integer( &in_pParams->m_specular_glossiness );
    miInteger opacity = *mi_eval_integer( &in_pParams->m_opacity );

	miColor reflection = *mi_eval_color( &in_pParams->m_reflection_color );                  
	miColor transparency = *mi_eval_color( &in_pParams->m_transparency_color );                  
	miScalar ior = *mi_eval_scalar( &in_pParams->m_ior );


	miColor sample_colour;	// returned light colour sample
	miColor this_light;		// colour contribution by this light
	miScalar spec_strength; // result of specular calculation

	miScalar dot_nl;		// working variable - see text
	miVector dir;			// ditto

	miVector ref_dir;		// reflection direction vector
	miColor ref_colour;		// returned reflection ray colour
	miScalar trans_average; // average transparency of the colour channels

    // Start the running total with the ambient value
    // XSI scales its ambient values by a global multiplier. We don't.
    out_pResult->r = ambient.r;
    out_pResult->g = ambient.g;
    out_pResult->b = ambient.b;

	// get the number of scene lights
	int n_lights;
	mi_query( miQ_NUM_GLOBAL_LIGHTS, state, miNULLTAG, &n_lights );
	
	// if there are any, get an array of light tags
	miTag *light;
	if ( n_lights > 0 )
	{
        mi_query( miQ_GLOBAL_LIGHTS, state, miNULLTAG, &light );
	}

	// for each light
	for ( int i=0; i < n_lights; i++ ) {
 
		// zero the counters for this light
		this_light.r = this_light.g = this_light.b = 0;
		int samples = 0;
		
		// keep sampling the light shader until the function returns miFalse
        while ( mi_sample_light( &sample_colour, &dir, &dot_nl, state, light[ i ], &samples ) ) {

			// specular strength
			spec_strength = mi_phong_specular((float)spec_size, state, &dir);

			// add up the diffuse and specular components
			this_light.r += sample_colour.r * ( dot_nl * diffuse.r + spec_strength * specular.r );
			this_light.g += sample_colour.g * ( dot_nl * diffuse.g + spec_strength * specular.g );
			this_light.b += sample_colour.b * ( dot_nl * diffuse.b + spec_strength * specular.b );

        }

		 // if there was more than one sample, divide the results back down
        if ( samples > 1 )
		{
            this_light.r /= samples;
            this_light.g /= samples;
            this_light.b /= samples;
        }

	    // add the results for this light to the running total
		out_pResult->r += this_light.r;
		out_pResult->g += this_light.g;
		out_pResult->b += this_light.b;
    }

	// reflection - if any reflection colour set 
	if(reflection.r + reflection.g + reflection.b > 0) {

		// reduce the lighting to compensate for the reflectivity
		out_pResult->r *= 1.0f - reflection.r;
		out_pResult->g *= 1.0f - reflection.g;
		out_pResult->b *= 1.0f - reflection.b;

		// calculate the reflection direction
		mi_reflection_dir(&ref_dir, state);

		// trace a reflection ray - if nothing is hit, trace an environment ray
		if( mi_trace_reflection( &ref_colour, state, &ref_dir) || mi_trace_environment(&ref_colour, state, &ref_dir ) )
		{

			// add the reflection results into the running total
			out_pResult->r += reflection.r * ref_colour.r;
			out_pResult->g += reflection.g * ref_colour.g;
			out_pResult->b += reflection.b * ref_colour.b;
		}
	}

	// transparency - if any transparent colour set 
	if( transparency.r + transparency.g + transparency.b > 0 ) {

		// reduce the lighting to compensate for the transparency
		out_pResult->r *= 1.0f - transparency.r;
		out_pResult->g *= 1.0f - transparency.g;
		out_pResult->b *= 1.0f - transparency.b;

		// trace a refraction ray - if nothing is hit, trace an environment ray
		if( mi_refraction_dir( &ref_dir, state, 1.0f, ior ) && mi_trace_refraction( &ref_colour, state, &ref_dir ) || mi_trace_environment( &ref_colour, state, &ref_dir ) )
		{

			// add the results into the running total
			out_pResult->r += transparency.r * ref_colour.r;
			out_pResult->g += transparency.g * ref_colour.g;
			out_pResult->b += transparency.b * ref_colour.b;

			// set the alpha to something sensible
			trans_average = ( transparency.r + transparency.g + transparency.b ) / 3.0f;
			out_pResult->a = ( 1 - trans_average ) + (trans_average * ref_colour.a );
		}
	} 
	else
	{

		// if the object isn't transparent, it's opaque
		out_pResult->a = 1;
	}


	return miTRUE;

}

DLLEXPORT void nel_xsi_shader_exit( miState * state, nel_xsi_shader_params * in_pParams)
{

	if( !in_pParams )
	{

	}
	else
	{

	}

}
