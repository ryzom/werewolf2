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

#ifndef _NEL_XSI_SHADER_H_
#define _NEL_XSI_SHADER_H_

#include <math.h>
#include <shader.h>

typedef struct
{
	miColor m_ambient_light_color; /* Ambient Lighting Color */
	miColor m_diffuse_light_color; /* Diffuse Lighting Color */
	miColor m_emissive_light_color; /* Emissive Lighting Color */
	miColor m_specular_light_color; /* Specular Lighting Color */
	miInteger m_specular_glossiness; /* Specular Glossiness */
	miInteger m_opacity; /* Opacity */
	miColor m_transparency_color; /* Transparency Color */
	miColor m_reflection_color; /* Reflection Color */
	miScalar m_ior; /* IOR */
	miInteger m_shader; /* Shader Selection */
	miBoolean m_unlighted; /* Unlighted */
	miInteger m_lightmap_texture_channel; /* Lightmap Texture Channel */
	miBoolean m_lightmap_stained_glass_window; /* Stained Glass Window Lightmap */
	miBoolean m_color_vertex; /* Color Vertex */
	miBoolean m_alpha_blend; /* Alpha Blend */
	miInteger m_blend_source; /* Blend Source */
	miInteger m_blend_destination; /* Blend Source */
	miBoolean m_alpha_test; /* Alpha Test */
	miBoolean m_alpha_vertex; /* Alpha Test */
	miBoolean m_alpha_vertex_channel; /* Alpha Test */
	miInteger m_z_bias; /* Z Bias */
	miBoolean m_force_z_write; /* Force Z Write */
	miBoolean m_no_z_write; /* No Z Write */
	miTag m_texture_one; /* Texture 1 */
	miTag m_texture_two; /* Texture 2 */
	miTag m_texture_three; /* Texture 3 */
	miTag m_texture_four; /* Texture 4 */
} nel_xsi_shader_params;

extern "C" {
DLLEXPORT int nel_xsi_shader_version(void);
DLLEXPORT miBoolean nel_xsi_shader( miColor *out_pResult, miState *state, nel_xsi_shader_params *in_pParams );
DLLEXPORT void nel_xsi_shader_init( miState * state, nel_xsi_shader_params * in_pParams, miBoolean *inst_req );
DLLEXPORT void nel_xsi_shader_exit( miState * state, nel_xsi_shader_params * in_pParams);
}
#endif
