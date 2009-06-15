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

#ifndef NEL_TOOLS_MESH_LIB_H
#define NEL_TOOLS_MESH_LIB_H

//
// Includes
//



//
//
//

typedef struct _singleWeight
{
	long	index;
	double	weight;
} single_weight;

//
// class CNeLMesh in xsi_nel_mesh_lib.h
//

class CNeLMesh
{
public:

	CNeLMesh();
	~CNeLMesh();

	enum TType { eNormal, eCluster, ePortal };
	enum TCameraCollision { eAuto, eDisabled, eForced };

	// Core - Member Functions
	XSI::X3DObject&								getObject() { return object; };
	void										setObject( XSI::X3DObject o ) { object = o; };
	bool										isDeformer( XSI::X3DObject potential_bone );

	// Time - Member Functions
	float										getTime() { return time; };
	void										setTime( float t ) { time = t; };

	// Distance - Member Functions
	float										getMaxDistance() { return time; };
	void										setMaxDistance( float md ) { max_distance = md; };

	// Normals - Member Functions
	NLMISC::CVector								getLocalNormal( uint32 index );

	// Shadows - Member Functions
	float										getShadowNumberFaces() { return shadow_number_faces; };
	void										setShadowNumberFaces( float snf ) { shadow_number_faces = snf; };
	float										getShadowPercentFaces() { return shadow_percent_faces; };
	void										setShadowPercentFaces( float spf ) { shadow_percent_faces = spf; };

	// Type - Member Functions
	TType										getType() { return mesh_type; };
	void										setType( TType tt ) { mesh_type = tt; };

	// Collision - Member Functions
	TCameraCollision							getCameraCollision() { return camera_collision; };
	void										setCameraCollision( TCameraCollision tcc ) { camera_collision = tcc; };

	// Mesh Base Build - Member Functions
	NL3D::CMeshBase::CMeshBaseBuild*			getMeshBaseBuild() { return &mesh_base_build; };
	NL3D::CMeshBase::CMeshBaseBuild				getMeshBaseBuildObject() { return mesh_base_build; };
	void										setMeshBaseBuild( NL3D::CMeshBase::CMeshBaseBuild& mbb ) { mesh_base_build = mbb; };
	void										createMeshBaseBuild();

	// Mesh Build - Member Functions
	NL3D::CMesh::CMeshBuild*					getMeshBuild() { return &mesh_build; };
	NL3D::CMesh::CMeshBuild						getMeshBuildObject() { return mesh_build; };
	void										setMeshBuild( NL3D::CMesh::CMeshBuild& mb ) { mesh_build = mb; };
	void										createMeshBuild();

	// XSI Import - Member Functions
	void										getXSIProperties();
	void										getXSIPoints();
	void										getXSIFacets();
	void										getXSITriangles();
	void										getXSIMaterials();
	void										getXSITransforms();
	void										getXSISkin();
	void										getXSIMRMParameters();
	void										getXSIMRMInterfaces();

	void										getXSIBone( long parent, XSI::X3DObject xsi_bone );
	void										getXSIBoneChildren( long parent, XSI::X3DObject xsi_bone );

	// Spatial - Member Functions
	XSI::MATH::CVector3							getLocalScale() { return local_scale; };
	XSI::MATH::CVector3							getLocalRotEuler() { return local_rotation; };
	XSI::MATH::CVector3							getLocalPos() { return local_translation; };
	XSI::MATH::CVector3							getLocalPivot() { return local_pivot; };
	XSI::MATH::CQuaternion						getLocalRotQuat() { return local_rotation_quaternion; };

	XSI::MATH::CVector3							getGlobalScale() { return global_scale; };
	XSI::MATH::CVector3							getGlobalRotEuler() { return global_rotation; };
	XSI::MATH::CVector3							getGlobalPos() { return global_translation; };
	XSI::MATH::CVector3							getGlobalPivot() { return global_pivot; };
	XSI::MATH::CQuaternion						getGlobalRotQuat() { return global_rotation_quaternion; };

	void										setLocalScale( XSI::MATH::CVector3& s ) { local_scale = s; };
	void										setLocalRotEuler( XSI::MATH::CVector3& r ) { local_rotation = r; };
	void										setLocalPos( XSI::MATH::CVector3& t ) { local_translation = t; };
	void										setLocalPivot( XSI::MATH::CVector3& p ) { local_pivot = p; };
	void										setLocalRotQuat( XSI::MATH::CQuaternion& rq ) { local_rotation_quaternion = rq; };

	void										setGlobalScale( XSI::MATH::CVector3& s ) { global_scale = s; };
	void										setGlobalRotEuler( XSI::MATH::CVector3& r ) { global_rotation = r; };
	void										setGlobalPos( XSI::MATH::CVector3& t ) { global_translation = t; };
	void										setGlobalPivot( XSI::MATH::CVector3& p ) { global_pivot = p; };
	void										setGlobalRotQuat( XSI::MATH::CQuaternion& rq ) { global_rotation_quaternion = rq; };

	// File IO - Member Functions
	void										writeFile();
	void										writeSkeletonFile();
	XSI::CString&								getFilePath() { return file_path; };
	void										getFilePath( XSI::CString p ) { file_path = p; };
	XSI::CString&								getFileName() { return file_name; };
	void										getFileName( XSI::CString n ) { file_name = n; };

	// Geometry - Member Functions
	void										setPointVertices( XSI::MATH::CVector3Array& v ) { point_geometry_vertices = v; };
	void										setPointNormals( XSI::MATH::CVector3Array& n ) { point_geometry_normals = n; };
	void										setPointIndices( XSI::CLongArray& i ) { point_geometry_index = i; };

	void										setFacetNormals( XSI::MATH::CVector3Array& n ) { facet_geometry_normals = n; };
	void										setFacetIndices( XSI::CLongArray& i ) { facet_geometry_index = i; };

	void										setTriangleVertices( XSI::MATH::CVector3Array& v ) { triangle_geometry_vertices = v; };
	void										setTriangleNormals( XSI::MATH::CVector3Array& n ) { triangle_geometry_normals = n; };
	void										setTriangleIndices( XSI::CLongArray& i ) { triangle_geometry_index = i; };
	void										setTriangleUVWs( XSI::CUVArray& uvw ) { triangle_geometry_uvws = uvw; };
	void										setTriangleVerticeColors( XSI::CVertexColorArray& c ) { triangle_geometry_vertice_colors = c; };

	XSI::MATH::CVector3Array					getPointVertices() { return point_geometry_vertices; };
	XSI::MATH::CVector3Array					getPointNormals() { return point_geometry_normals; };
	XSI::CLongArray								getPointIndices() { return point_geometry_index; };

	XSI::MATH::CVector3Array					getFacetNormals() { return facet_geometry_normals; };
	XSI::CLongArray								getFacetIndices() { return facet_geometry_index; };

	XSI::MATH::CVector3Array					getTriangleVertices() { return triangle_geometry_vertices; };
	XSI::MATH::CVector3Array					getTriangleNormals() { return triangle_geometry_normals; };
	XSI::CLongArray								getTriangleIndices() { return triangle_geometry_index; };
	XSI::CUVArray 								getTriangleUVWs() { return triangle_geometry_uvws; };
	XSI::CVertexColorArray						getTriangleVerticeColors() { return triangle_geometry_vertice_colors; };

	// Creation - Member Functions
	void										createMesh();
	void										createMRMMesh();
	void										createSkeleton();
	void										createVegetMesh();
	void										createMultiLODMesh();

	// Shadows - Member Functions
	void										createMRMShadows( NL3D::CMeshMRMGeom* mrm_mesh_geometry );
	void										createMRMShadows( NL3D::CMeshMRMSkinnedGeom* mrm_mesh_geometry );

	// Materials - Member Functions
	void										animateMaterials();

	// Mode - Member Functions
	bool										isVerbose() { return is_verbose; };
	void										isVerbose( bool b ) { is_verbose = b; };
	bool										isVeryVerbose() { return is_very_verbose; };
	void										isVeryVerbose( bool b ) { is_very_verbose = b; };
	bool										isInsaneVerbosity() { return is_insane_verbosity; };
	void										isInsaneVerbosity( bool b ) { is_insane_verbosity = b; };
	bool										isMRM() { return is_mrm; };
	void										isMRM( bool b ) { is_mrm = b; };
	bool										isSkinned() { return is_skinned; };
	void										isSkinned( bool b ) { is_skinned = b; };
	bool										isBlendShaped() { return is_blend_shaped; };
	void										isBlendShaped( bool b ) { is_blend_shaped = b; };
	bool										isMultiLOD() { return is_multi_lod; };
	void										isMultiLOD( bool b ) { is_multi_lod = b; };
	bool										isShadowCaster() { return is_shadow_caster; };
	void										isShadowCaster( bool b ) { is_shadow_caster = b; };
	bool										isShadowReciever() { return is_shadow_reciever; };
	void										isShadowReciever( bool b ) { is_shadow_reciever = b; };
	bool										isVPMesh() { return is_vp_mesh; };
	void										isVPMesh( bool b ) { is_vp_mesh = b; };
	bool										isLocalLightAttenuated() { return is_local_light_attenuated; };
	void										isLocalLightAttenuated( bool b ) { is_local_light_attenuated = b; };
	bool										isExportingAnimatedMaterials() { return is_exporting_animated_materials; };
	void										isExportingAnimatedMaterials( bool b ) { is_exporting_animated_materials = b; };
	bool										isShape() { return is_shape; };
	void										isShape( bool b ) { is_shape = b; };
	bool										isVegetable() { return is_vegetable; };
	void										isVegetable( bool b ) { is_vegetable = b; };
	bool										isCharacterLod() { return is_character_lod; };
	void										isCharacterLod( bool b ) { is_character_lod = b; };
	bool										isObjectNameFileName() { return is_object_name_file_name; };
	void										isObjectNameFileName( bool b ) { is_object_name_file_name = b; };
	bool										isExportingSkeleton() { return is_exporting_skeleton; };
	void										isExportingSkeleton( bool b ) { is_exporting_skeleton = b; };

private:

	// Core - Member Variables 
	XSI::X3DObject								object;

	// Distance - Member Variables 
	float										max_distance;

	// Core - Member Variables 
	float										time;

	// Core - Member Variables 
	float										shadow_percent_faces;
	float										shadow_number_faces;

	// Type - Member Variables
	TType										mesh_type;

	// Collision - Member Variables
	TCameraCollision							camera_collision;

	// Spatial - Member Variables
	NL3D::CMeshBase::CMeshBaseBuild				mesh_base_build;
	NL3D::CVegetableShapeBuild					vegetable_build;
	NL3D::CMesh::CMeshBuild						mesh_build;
	NL3D::CVegetableShape						vegetable;
	NL3D::CMRMParameters						mrm_parameters;
	NL3D::CSkeletonShape*						skeleton;
	NL3D::IShape*								i_shape;

	// STL -  Member Variables
	_STL::vector< NL3D::CMesh::CMeshBuild* >	mesh_build_list;
	_STL::vector< NL3D::CBoneBase>				skeleton_bones;
	_STL::multimap< float,long >				skinning_weights;
	_STL::multimap< long, float >				skinning_vertices;
	_STL::vector< _STL::multimap< long, float > >skinning_bones;

	// Mode -  Member Variables
	bool										is_verbose;
	bool										is_very_verbose;
	bool										is_insane_verbosity;
	bool										is_skinned;
	bool										is_blend_shaped;
	bool										is_mrm;
	bool										is_multi_lod;
	bool										is_shadow_caster;
	bool										is_shadow_reciever;
	bool										is_vp_mesh;
	bool										is_local_light_attenuated;
	bool										is_exporting_animated_materials;
	bool										is_shape;
	bool										is_vegetable;
	bool										is_character_lod;
	bool										is_object_name_file_name;
	bool										is_exporting_skeleton;

	// Spatial - Member Variables
	XSI::MATH::CVector3							local_scale;
	XSI::MATH::CVector3							local_rotation;
	XSI::MATH::CVector3							local_translation;
	XSI::MATH::CVector3							local_pivot;
	XSI::MATH::CQuaternion						local_rotation_quaternion;

	XSI::MATH::CVector3							global_scale;
	XSI::MATH::CVector3							global_rotation;
	XSI::MATH::CVector3							global_translation;
	XSI::MATH::CVector3							global_pivot;
	XSI::MATH::CQuaternion						global_rotation_quaternion;

	// Geometry - Member Variables
	XSI::MATH::CVector3Array					point_geometry_vertices;
	XSI::MATH::CVector3Array					point_geometry_normals;
	XSI::CLongArray								point_geometry_index;

	XSI::MATH::CVector3Array					facet_geometry_normals;
	XSI::CLongArray								facet_geometry_index;

	XSI::MATH::CVector3Array					triangle_geometry_vertices;
	XSI::MATH::CVector3Array					triangle_geometry_normals;
	XSI::CLongArray								triangle_geometry_index;

	XSI::CUVArray 								triangle_geometry_uvws;
	XSI::CVertexColorArray						triangle_geometry_vertice_colors;

	// Vegetable - Member Variables
	bool										vegetable_alpha_blend;
	bool										vegetable_pre_compute_lighting;
	bool										vegetable_double_sided;
	bool										vegetable_lighted;
	bool										vegetable_best_sided_pre_computed_lighting;
	long										vegetable_bend_center_mode;
	float										vegetable_bend_factor;

	// Bones - Member Variables
	float										bone_distance_max;
	long										current_bone;

	// File IO - Member Variables
	XSI::CString								file_path;
	XSI::CString								file_name;

};

#endif
