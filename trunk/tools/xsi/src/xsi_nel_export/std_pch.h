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


#ifndef NEL_TOOLS_PCH_H
#define NEL_TOOLS_PCH_H

//
// Windows Includes
//

#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <cerrno>

#include <malloc.h>
#include <winsock2.h>
#include <atlbase.h>
#include <shlobj.h>

//
// STLPort Includes
//

#include <algorithm>
#include <bitset>
#include <deque>
#include <exception>
#include <hash_map>
#include <hash_set>
#include <iterator>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

//
// XML Includes
//

#include <libxml/parser.h>

//
// XSI Includes
//

#include <xsi_actionsource.h>
#include <xsi_animationsourceitem.h>
#include <xsi_animsource.h>
#include <xsi_application.h>
#include <xsi_argument.h>
#include <xsi_argumenthandler.h>
#include <xsi_arrayparameter.h>
#include <xsi_base.h>
#include <xsi_boolarray.h>
#include <xsi_camera.h>
#include <xsi_camerarig.h>
#include <xsi_chainbone.h>
#include <xsi_chaineffector.h>
#include <xsi_chainelement.h>
#include <xsi_chainroot.h>
#include <xsi_clip.h>
#include <xsi_clipcontainer.h>
#include <xsi_clipeffect.h>
#include <xsi_clipeffectitem.h>
#include <xsi_cliprelation.h>
#include <xsi_cluster.h>
#include <xsi_clusterproperty.h>
#include <xsi_color.h>
#include <xsi_comapihandler.h>
#include <xsi_command.h>
#include <xsi_consthistory.h>
#include <xsi_constraint.h>
#include <xsi_context.h>
#include <xsi_controlpoint.h>
#include <xsi_customoperator.h>
#include <xsi_customproperty.h>
#include <xsi_decl.h>
#include <xsi_desktop.h>
#include <xsi_dictionary.h>
#include <xsi_directed.h>
#include <xsi_doublearray.h>
#include <xsi_edge.h>
#include <xsi_envelope.h>
#include <xsi_expression.h>
#include <xsi_facet.h>
#include <xsi_factory.h>
#include <xsi_fcurve.h>
#include <xsi_fcurvekey.h>
#include <xsi_filter.h>
#include <xsi_geometry.h>
#include <xsi_graphicsequencer.h>
#include <xsi_graphicsequencercontext.h>
#include <xsi_griddata.h>
#include <xsi_group.h>
#include <xsi_image.h>
#include <xsi_imageclip2.h>
#include <xsi_imageclip.h>
#include <xsi_inputport.h>
#include <xsi_joint.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>
#include <xsi_knot.h>
#include <xsi_layer.h>
#include <xsi_layout.h>
#include <xsi_library.h>
#include <xsi_light.h>
#include <xsi_lightrig.h>
#include <xsi_longarray.h>
#include <xsi_mappeditem.h>
#include <xsi_material.h>
#include <xsi_math.h>
#include <xsi_matrix3.h>
#include <xsi_matrix4.h>
#include <xsi_menu.h>
#include <xsi_menuitem.h>
#include <xsi_miuserdata_defs.h>
#include <xsi_mixer.h>
#include <xsi_model.h>
#include <xsi_null.h>
#include <xsi_nurbscurve.h>
#include <xsi_nurbscurvelist.h>
#include <xsi_nurbsdata.h>
#include <xsi_nurbssample.h>
#include <xsi_nurbssurface.h>
#include <xsi_nurbssurfacemesh.h>
#include <xsi_ogllight.h>
#include <xsi_oglmaterial.h>
#include <xsi_ogltexture.h>
#include <xsi_operator.h>
#include <xsi_outputport.h>
#include <xsi_parameter.h>
#include <xsi_particle.h>
#include <xsi_particleattribute.h>
#include <xsi_particlecloud.h>
#include <xsi_particlecloudprimitive.h>
#include <xsi_particletype.h>
#include <xsi_pass.h>
#include <xsi_plugin.h>
#include <xsi_pluginitem.h>
#include <xsi_pluginregistrar.h>
#include <xsi_point.h>
#include <xsi_polygonface.h>
#include <xsi_polygonmesh.h>
#include <xsi_polygonnode.h>
#include <xsi_port.h>
#include <xsi_portgroup.h>
#include <xsi_ppgeventcontext.h>
#include <xsi_ppgitem.h>
#include <xsi_ppglayout.h>
#include <xsi_preferences.h>
#include <xsi_primitive.h>
#include <xsi_progressbar.h>
#include <xsi_project.h>
#include <xsi_projectitem.h>
#include <xsi_property.h>
#include <xsi_proxyparameter.h>
#include <xsi_quaternion.h>
#include <xsi_ref.h>
#include <xsi_rig.h>
#include <xsi_rotation.h>
#include <xsi_rtshaders.h>
#include <xsi_sample.h>
#include <xsi_scene.h>
#include <xsi_sceneitem.h>
#include <xsi_segment.h>
#include <xsi_selection.h>
#include <xsi_shader.h>
#include <xsi_shapeclip.h>
#include <xsi_shapekey.h>
#include <xsi_siobject.h>
#include <xsi_source.h>
#include <xsi_statickinematicstate.h>
#include <xsi_staticsource.h>
#include <xsi_status.h>
#include <xsi_string.h>
#include <xsi_subcomponent.h>
#include <xsi_texture.h>
#include <xsi_texturelayer.h>
#include <xsi_texturelayerport.h>
#include <xsi_time.h>
#include <xsi_timecontrol.h>
#include <xsi_track.h>
#include <xsi_transformation.h>
#include <xsi_transition.h>
#include <xsi_triangle.h>
#include <xsi_trianglevertex.h>
#include <xsi_uiobject.h>
#include <xsi_uipersistable.h>
#include <xsi_uitoolkit.h>
#include <xsi_updatecontext.h>
#include <xsi_userdatablob.h>
#include <xsi_userdatamap.h>
#include <xsi_uv.h>
#include <xsi_value.h>
#include <xsi_vector3.h>
#include <xsi_vector4.h>
#include <xsi_vertex.h>
#include <xsi_vertexcolor.h>
#include <xsi_view.h>
#include <xsi_viewcontext.h>
#include <xsi_viewnotification.h>
#include <xsi_x3dobject.h>

//
// XSI Utilities
//

#include <SIBCArray.h>

//
// Remove Default MIN & MAX
//

#undef min
#undef max

//
// NeL Misc Includes
//

#include <nel/misc/aabbox.h>
#include <nel/misc/algo.h>
#include <nel/misc/bit_mem_stream.h>
#include <nel/misc/bit_set.h>
#include <nel/misc/config_file.h>
#include <nel/misc/command.h>
#include <nel/misc/common.h>
#include <nel/misc/displayer.h>
#include <nel/misc/events.h>
#include <nel/misc/event_server.h>
#include <nel/misc/event_listener.h>
#include <nel/misc/fast_mem.h>
#include <nel/misc/file.h>
#include <nel/misc/hierarchical_timer.h>
#include <nel/misc/i_xml.h>
#include <nel/misc/line.h>
#include <nel/misc/matrix.h>
#include <nel/misc/mem_displayer.h>
#include <nel/misc/mem_stream.h>
#include <nel/misc/o_xml.h>
#include <nel/misc/path.h>
#include <nel/misc/plane.h>
#include <nel/misc/polygon.h>
#include <nel/misc/progress_callback.h>
#include <nel/misc/quat.h>
#include <nel/misc/report.h>
#include <nel/misc/rgba.h>
#include <nel/misc/sha1.h>
#include <nel/misc/smart_ptr.h>
#include <nel/misc/stream.h>
#include <nel/misc/stream_inline.h>
#include <nel/misc/string_common.h>
#include <nel/misc/string_mapper.h>
#include <nel/misc/system_info.h>
#include <nel/misc/thread.h>
#include <nel/misc/types_nl.h>
#include <nel/misc/time_nl.h>
#include <nel/misc/uv.h>
#include <nel/misc/value_smoother.h>
#include <nel/misc/vector.h>
#include <nel/misc/vectord.h>
#include <nel/misc/vector_2d.h>

//
// NeL 3D Includes
//

#include <3d/animation.h>
#include <3d/animated_material.h>
#include <3d/coarse_mesh_manager.h>
#include <3d/index_buffer.h>
#include <3d/lod_character_shape.h>
#include <3d/material.h>
#include <3d/mesh_base.h>
#include <3d/mesh_base_instance.h>
#include <3d/mesh_blender.h>
#include <3d/mesh_block_manager.h>
#include <3d/mesh_geom.h>
#include <3d/mesh.h>
#include <3d/mesh_instance.h>
#include <3d/mesh_mrm.h>
#include <3d/mesh_mrm_instance.h>
#include <3d/mesh_mrm_skinned.h>
#include <3d/mesh_mrm_skinned_instance.h>
#include <3d/mesh_multi_lod.h>
#include <3d/mesh_multi_lod_instance.h>
#include <3d/mesh_morpher.h>
#include <3d/mesh_vertex_program.h>
#include <3d/meshvp_per_pixel_light.h>
#include <3d/meshvp_wind_tree.h>
#include <3d/mrm_builder.h>
#include <3d/mrm_internal.h>
#include <3d/mrm_level_detail.h>
#include <3d/mrm_mesh.h>
#include <3d/mrm_parameters.h>
#include <3d/patch.h>
#include <3d/quad_grid.h>
#include <3d/register_3d.h>
#include <3d/scene.h>
#include <3d/scene_user.h>
#include <3d/shape.h>
#include <3d/shape_bank.h>
#include <3d/skeleton_model.h>
#include <3d/skeleton_shape.h>
#include <3d/stripifier.h>
#include <3d/tangent_space_build.h>
#include <3d/texture_cube.h>
#include <3d/texture_file.h>
#include <3d/transform_shape.h>
#include <3d/vegetable_shape.h>
#include <3d/vertex_buffer.h>
#include <3d/water_model.h>
#include <3d/water_shape.h>
#include <3d/water_height_map.h>
#include <3d/water_pool_manager.h>

#include <nel/3d/animation_time.h>

#include <nel/3d/u_animation.h>
#include <nel/3d/u_animation_set.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_cloud_scape.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_instance_group.h>
#include <nel/3d/u_landscape.h>
#include <nel/3d/u_light.h>
#include <nel/3d/u_material.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_shape.h>
#include <nel/3d/u_skeleton.h>
#include <nel/3d/u_texture.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_track.h>
#include <nel/3d/u_particle_system_instance.h>
#include <nel/3d/u_play_list.h>
#include <nel/3d/u_play_list_manager.h>
#include <nel/3d/u_point_light.h>

//
// NeL Sound Includes
//

#include <nel/sound/u_audio_mixer.h>
#include <nel/sound/u_listener.h>
#include <nel/sound/u_source.h>

//
// MySQL Includes
//

#include <mysql.h>

//
// Angel Script Includes
//

#include <angelscript.h>

//
// Our Includes
//

#include "singleton.h"

#endif
