#include "stdafx.h"
#include <nel/misc/types_nl.h>

#ifdef NL_OS_WINDOWS
#	include <windows.h>
#	undef min
#	undef max
#endif

#include <nel/misc/time_nl.h>
#include <nel/misc/path.h>
#include <nel/misc/config_file.h>
#include <nel/misc/vectord.h>
#include <3d/event_mouse_listener.h>
#include <3d/mesh_instance.h>
#include <nel/3d/u_point_light.h>

#include <nel/3d/u_driver.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_instance.h>
#include <nel/misc/value_smoother.h>
#include <nel/misc/file.h>
#include <3d/texture_file.h>
#include <3d/mesh.h>
#include <3d/scene_user.h>
#include <vector>
#include "hull.h"
#include <3d/ray_mesh.h>

#include <vector>

using namespace std;
using namespace NLMISC;
using namespace NL3D;

CConfigFile				ConfigFile;

// The 3d driver
UDriver					*Driver = NULL;

// This is the main scene
UScene					*Scene = NULL;

// The previous and current frame dates
double					LastTime, NewTime, DeltaTime;

// This variable is used to display text on the screen
UTextContext			*TextContext = NULL;

UCamera					Camera;
UPointLight				pl;

CVector gravForce(float, float mass, const CVector&, const CQuat&, const CVector&, const CVector&, const CMatrix&, const CVector&, const CVector&) {
	const float grav = 9.81f;
	const CVector gravDir(0.0f, 0.0f, -1.0f);
	return (mass*grav)*gravDir;
}

CVector resistiveForce(float, float mass, const CVector&, const CQuat&, const CVector& linMom, const CVector&, const CMatrix&, const CVector& linVel, const CVector&) {
    float resistance = -0.8f;
	return linMom*resistance;
}

CVector resistiveTorque(float, float mass, const CVector&, const CQuat&, const CVector&, const CVector& angMom, const CMatrix&, const CVector&, const CVector&) {
	float resistance = -4.0f;
	return angMom*resistance;
}

CVector pseudoTorque(float t, float mass, const CVector& pos, const CQuat& rot, const CVector& linMom, const CVector& angMom, const CMatrix& rotMat, const CVector& linVel, const CVector& angVel) {
	const float f = 0.1f;
	const CVector fDir(0.0f, 0.0f, 1.0f);
	return f*fDir; // + resistiveTorque(t, mass, pos, rot, linMom, angMom, rotMat, linVel, angVel);
}

class CRigidBody {
public:
	CRigidBody() {
		m_mass = 1000000;
		m_invMass = 0;
		m_inertia = CMatrix::Identity;
		m_invInertia = CMatrix::Identity;
		reset();
	}

	virtual ~CRigidBody() {
		destroy();
	}

	void init() {
		Instance = Scene->createInstance (ConfigFile.getVar("Model").asString());
		Instance.show();
		Instance.setTransformMode(UTransformable::RotQuat);
		CenterOfMassInstance = Scene->createInstance(ConfigFile.getVar("ComModel").asString());
		CenterOfMassInstance.hide();
		CenterOfMassInstance.setScale(0.2, 0.2, 0.2);
		createConvexHull();
		Force = &resistiveForce;
		Torque = &resistiveTorque;
	}

	void reset() {
		m_rot = CQuat::Identity;
		m_linMom = CVector::Null;
		m_angMom = CVector::Null;
		m_rotMat = CMatrix::Identity;
		m_linVel = CVector::Null;
		m_angVel = CVector::Null;
	}

	void destroy() {
		if(!Instance.empty())
			Scene->deleteInstance(Instance);
		if(!HullInstance.empty())
			Scene->deleteInstance(HullInstance);
		if(!CenterOfMassInstance.empty())
			Scene->deleteInstance(CenterOfMassInstance);
	}

	CVector& position();

	void setMass(float mass) {
		if(mass > 0) {
			m_mass = mass;
			m_invMass = 1.0f/mass;
		} else {
			m_mass = 1000000;
			m_invMass = 0;
		}
	}

	void setBodyInertia(const CMatrix& inertia) {
		m_inertia = inertia;
		m_invInertia = inertia.inverted();
	}

	void setPosition(const CVector& pos) {
		m_pos = pos;
		Instance.setPos(m_pos);
		HullInstance.setPos(m_pos);
		CenterOfMassInstance.setPos(m_pos);
	}

	void setRot(const CQuat& rot) {
		m_rot = rot;
		m_rotMat.setRot(rot);
		m_rotMatTransp = m_rotMat;
		m_rotMatTransp.transpose3x3();
		Instance.setRotQuat(m_rot);
		HullInstance.setRotQuat(m_rot);
	}

	void setLinearMomentum(const CVector& linMom) {
		m_linMom = linMom;
		m_linVel = m_invMass*m_linMom;
	}

	void setAngularMomentum(const CVector& angMom) {
		m_angMom = angMom;
		m_angVel = m_rotMat*m_invInertia*m_rotMatTransp*m_angMom;
	}

	void setRot(const CMatrix& rot) {
		m_rotMat = rot;
		m_rot = rot.getRot();
		m_rotMatTransp = m_rotMat;
		m_rotMatTransp.transpose3x3();
		Instance.setRotQuat(m_rot);
		HullInstance.setRotQuat(m_rot);
	}

	void setLinearVelocity(const CVector& linVel) {
		m_linVel = linVel;
		m_linMom = m_mass*m_linVel;
	}

	void setAngularVelocity(const CVector& angVel) {
		m_angVel = angVel;
		m_angMom = m_rotMat*m_inertia*m_rotMatTransp*m_angVel;
	}

	void setInternalForce (const CVector& force) {
		m_internalForce = force;
	}

    void setInternalTorque (const CVector& torque) {
		m_internalTorque = torque;
	}

    void setExternalForce (const CVector& force) {
		m_externalForce = force;
	}

    void setExternalTorque (const CVector& torque) {
		m_externalTorque = torque;
	}

    void appendInternalForce (const CVector& force) {
		m_internalForce += force;
	}

    void appendInternalTorque (const CVector& torque) {
		m_internalTorque += torque;
	}

	float getMass() const {	return m_mass; }
	float getInverseMass() const { return m_invMass; }
	const CMatrix& getBodyInertia() const { return m_inertia; }
	const CMatrix& getBodyInverseInertia() const { return m_invInertia; }
	CMatrix getWorldInertia() const { return m_rotMat*m_inertia*m_rotMatTransp; }
	CMatrix getWorldInverseInertia() const { return m_rotMat*m_invInertia*m_rotMatTransp; }
	const CVector& getPosition() const { return m_pos; }
	const CQuat& getRot() const { return m_rot; }
	const CVector& getLinearMomentum() const { return m_linMom; }
	const CVector& getAngularMomentum() const { return m_angMom; }
	const CMatrix& getRotMat() const { return m_rotMat; }
	const CVector& getLinearVelocity() const { return m_linVel; }
	const CVector& getAngularVelocity() const { return m_angVel; }
	const CVector& getCenterOfMass() const { return m_centerOfMass; }

	void update(float t, float dt) {
		float haldt = 0.5f*dt;
		float sixthDt = dt/6.0f;
		float tpHaldt = t + haldt;
		float tpDt = t + dt;

		CVector newPos, newLinMom, newAngMom, newLinVel, newAngVel;
		CQuat newRot;
		CMatrix newRotMat = CMatrix::Identity;

	    // A1 = G(T,S0), B1 = S0 + (DT/2)*A1
		CVector a1DxDt = m_linVel;
		CQuat w = CQuat(m_angVel.x,m_angVel.y,m_angVel.z,0.0f);
		CQuat a1DqDt = 0.5f*w*m_rot;
		CVector a1DpDt = m_externalForce + m_internalForce;
		CVector a1DlDt = m_externalTorque + m_internalTorque;
		m_internalForce = CVector::Null;
		m_internalTorque = CVector::Null;
		newPos = m_pos + haldt*a1DxDt;
		newRot = m_rot + haldt*a1DqDt;
		newLinMom = m_linMom + haldt*a1DpDt;
		newAngMom = m_angMom + haldt*a1DlDt;
		newRotMat.setRot(newRot);
		newLinVel = m_invMass*newLinMom;
		CMatrix newRotMatTransp = newRotMat;
		newRotMatTransp.transpose3x3();
		newAngVel = newRotMat*m_invInertia*newRotMatTransp*newAngMom;

		// A2 = G(T+DT/2,B1), B2 = S0 + (DT/2)*A2
		CVector a2DxDt = newLinVel;
		w = CQuat(newAngVel.x,newAngVel.y,newAngVel.z,0.0f);
		CQuat a2DqDt = 0.5f*w*newRot;
		CVector a2DpDt = Force(tpHaldt,m_mass,newPos,newRot,newLinMom,newAngMom,newRotMat,newLinVel,newAngVel);
		CVector a2DlDt = Torque(tpHaldt,m_mass,newPos,newRot,newLinMom,newAngMom,newRotMat,newLinVel,newAngVel);
		newPos = m_pos + haldt*a2DxDt;
		newRot = m_rot + haldt*a2DqDt;
		newLinMom = m_linMom + haldt*a2DpDt;
		newAngMom = m_angMom + haldt*a2DlDt;
		newRotMat.setRot(newRot);
		newRotMatTransp = newRotMat;
		newRotMatTransp.transpose3x3();
		newLinVel = m_invMass*newLinMom;
		newAngVel = newRotMat*m_invInertia*newRotMatTransp*newAngMom;

		// A3 = G(T+DT/2,B2), B3 = S0 + DT*A3
		CVector a3DxDt = newLinVel;
		w = CQuat(newAngVel.x,newAngVel.y,newAngVel.z,0.0f);
		CQuat a3DqDt = 0.5f*w*newRot;
		CVector a3DpDt = Force(tpHaldt,m_mass,newPos,newRot,newLinMom,newAngMom,newRotMat,newLinVel,newAngVel);
		CVector a3DlDt = Torque(tpHaldt,m_mass,newPos,newRot,newLinMom,newAngMom,newRotMat,newLinVel,newAngVel);
		newPos = m_pos + dt*a3DxDt;
		newRot = m_rot + dt*a3DqDt;
		newLinMom = m_linMom + dt*a3DpDt;
		newAngMom = m_angMom + dt*a3DlDt;
		newRotMat.setRot(newRot);
		newRotMatTransp = newRotMat;
		newRotMatTransp.transpose3x3();
		newLinVel = m_invMass*newLinMom;
		newAngVel = newRotMat*m_invInertia*newRotMatTransp*newAngMom;

		// A4 = G(T+DT,B3), S1 = S0 + (DT/6)*(A1+2*(A2+A3)+A4)
		CVector a4DxDt = newLinVel;
		w = CQuat(newAngVel.x,newAngVel.y,newAngVel.z,0.0f);
		CQuat a4DqDt = 0.5f*w*newRot;
		CVector a4DpDt = Force(tpDt,m_mass,newPos,newRot,newLinMom,newAngMom,newRotMat,newLinVel,newAngVel);
		CVector a4DlDt = Torque(tpDt,m_mass,newPos,newRot,newLinMom,newAngMom,newRotMat,newLinVel,newAngVel);
		m_pos = m_pos + sixthDt*(a1DxDt + 2.0f*(a2DxDt + a3DxDt) + a4DxDt);
		setPosition(m_pos);
		m_rot = m_rot + sixthDt*(a1DqDt + 2.0f*(a2DqDt + a3DqDt) + a4DqDt);
		m_linMom = m_linMom + sixthDt*(a1DpDt + 2.0f*(a2DpDt + a3DpDt) + a4DpDt);
		m_angMom = m_angMom + sixthDt*(a1DlDt + 2.0f*(a2DlDt + a3DlDt) + a4DlDt);
		setRot(m_rot);
		m_linVel = m_invMass*m_linMom;
		m_angVel = m_rotMat*m_invInertia*m_rotMatTransp*m_angMom;

		// make force and torque correspond to new time T+DT
		m_externalForce = Force(tpDt,m_mass,m_pos,m_rot,m_linMom,m_angMom,m_rotMat,m_linVel,m_angVel);
		m_externalTorque = Torque(tpDt,m_mass,m_pos,m_rot,m_linMom,m_angMom,m_rotMat,m_linVel,m_angVel);
	}

	bool& moved() {
		return m_bMoved;
	}

	typedef CVector (*Function)
	(
		float,				// time
		float,				// mass
		const CVector&,		// position
		const CQuat&,		// rotation
		const CVector&,		// linear momentum
		const CVector&,		// angular momentum
		const CMatrix&,		// rotation
		const CVector&,		// linear velocity
		const CVector&		// angular velocity
	);

	Function Force;
	Function Torque;

	UInstance				Instance;
	UInstance				HullInstance;
	UInstance				CenterOfMassInstance;
protected:
	void setCenterOfMass(const CVector& centerOfMass) {
		m_centerOfMass = centerOfMass;
		Instance.setPivot(m_centerOfMass);
		HullInstance.setPivot(m_centerOfMass);
		CenterOfMassInstance.setPos(m_centerOfMass);
	}
	void createConvexHull() {
		if(Instance.empty())
			return;

		NL3D::CMesh* mesh = dynamic_cast<NL3D::CMesh*>(Instance.getObjectPtr()->Shape.getPtr());
		std::vector<NLMISC::CVector> vertices;
		mesh->getMeshGeom().retrieveVertices(vertices);

		printf("Original mesh has %d vertices\n", vertices.size());

		HullDesc desc;
		desc.SetHullFlag(QF_TRIANGLES);
		desc.mVcount = vertices.size();
		desc.mVertexStride = sizeof(float)*3;
		desc.mSkinWidth = 0.01f;
		//	desc.SetHullFlag(QF_SKIN_WIDTH);
		float* verticeArray = new float[vertices.size()*3];
		float maxDist = 0;
		for(uint i = 0; i < vertices.size(); ++i) {
			verticeArray[i*3] = vertices[i].x;
			verticeArray[i*3+1] = vertices[i].y;
			verticeArray[i*3+2] = vertices[i].z;
			float dist = vertices[i].norm();
			if(dist > maxDist)
				maxDist = dist;
		}
		printf("Max distance from origin %f\n", maxDist);
		desc.mVertices = verticeArray;

		HullResult result;
		HullLibrary hl;

		HullError ret = hl.CreateConvexHull(desc, result);

		if( ret == QE_OK ) {
			printf("Successfully created convex hull.\r\n");

			if ( result.mPolygons )
				printf("Hull contains %d poygons.\r\n", result.mNumFaces );
			else
				printf("Hull contains %d triangles.\r\n", result.mNumFaces );

			printf("Output hull contains %d vertices.\r\n", result.mNumOutputVertices );

			createConvexHullInstance(result);
			calculateConvexHullMassProperties(result);
			printf("Mass %f\n", m_mass);
			printf("Center of mass (%.3f, %.3f, %.3f)\n", m_centerOfMass.x, m_centerOfMass.y, m_centerOfMass.z);
			hl.ReleaseResult(result);
		} else {
			printf("Failed to create convex hull.\r\n");
		}

		delete[] verticeArray;
	}

	void createConvexHullInstance(HullResult& result) {
		NL3D::CMeshBase::CMeshBaseBuild mBuildBaseMesh;
		NL3D::CMesh::CMeshBuild			mBuildMesh;

		mBuildMesh.VertexFlags = 0;
		mBuildBaseMesh.Materials.resize(1);
		NL3D::CMaterial* mat = &mBuildBaseMesh.Materials[0];
		mat->initLighted();
		mat->setLighting(true, NLMISC::CRGBA::Black, NLMISC::CRGBA::Black, NLMISC::CRGBA(255, 255, 255, 128), NLMISC::CRGBA::Black);
		mat->setBlend(true);
		mat->setBlendFunc(NL3D::CMaterial::invsrcalpha, NL3D::CMaterial::srcalpha);
		mat->setOpacity(128);
		mat->setShader(NL3D::CMaterial::Normal);


		mBuildBaseMesh.bCastShadows = false;
		mBuildBaseMesh.bRcvShadows = false;
		mBuildBaseMesh.UseLightingLocalAttenuation = false;
		NL3D::CMesh* mesh = dynamic_cast<NL3D::CMesh*>(Instance.getObjectPtr()->Shape.getPtr());
		mBuildBaseMesh.DefaultScale = mesh->getDefaultScale()->getDefaultValue();
		mBuildBaseMesh.DefaultRotQuat = mesh->getDefaultRotQuat()->getDefaultValue();
		mBuildBaseMesh.DefaultRotEuler = mesh->getDefaultRotEuler()->getDefaultValue();
		mBuildBaseMesh.DefaultPivot = mesh->getDefaultPivot()->getDefaultValue();
		mBuildBaseMesh.DefaultPos = mesh->getDefaultPos()->getDefaultValue();
		mBuildMesh.VertexFlags |= NL3D::CVertexBuffer::PositionFlag;

		mBuildMesh.Vertices.resize(result.mNumOutputVertices);
		for(uint i = 0; i < result.mNumOutputVertices; ++i) {
			const float* v = &result.mOutputVertices[i*3];
			mBuildMesh.Vertices[i] = NLMISC::CVector(v[0], v[1], v[2]);
		}

		mBuildMesh.Faces.resize(result.mNumFaces);
		for(uint i = 0; i < result.mNumFaces; ++i) {
			mBuildMesh.Faces[i].MaterialId = 0;
			mBuildMesh.Faces[i].SmoothGroup = 0;
			unsigned int *idx = &result.mIndices[i*3];
			mBuildMesh.Faces[i].Corner[0].Vertex = idx[0];
			mBuildMesh.Faces[i].Corner[1].Vertex = idx[1];
			mBuildMesh.Faces[i].Corner[2].Vertex = idx[2];
		}

		mBuildMesh.Interfaces.clear();
		mBuildMesh.InterfaceLinks.clear();

		NLMISC::CSmartPtr<NL3D::CMesh> outMesh = new NL3D::CMesh();
		outMesh->build(mBuildBaseMesh, mBuildMesh);

		CSceneUser* sceneUser = dynamic_cast<NL3D::CSceneUser*>(Scene);
		sceneUser->getScene().getShapeBank()->add("hull", outMesh.getPtr());
		HullInstance = Scene->createInstance("hull");
		HullInstance.hide();
		HullInstance.setTransformMode(UTransformable::RotQuat);
	}

	void calculateConvexHullMassProperties(HullResult& result) {
		const float fOneDiv6 = 1.0f/6.0f;
		const float fOneDiv24 = 1.0f/24.0f;
		const float fOneDiv60 = 1.0f/60.0f;
		const float fOneDiv120 = 1.0f/120.0f;

		// order: 1, x, y, z, x^2, y^2, z^2, xy, yz, zx
		float afIntegral[10] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

		std::vector<NLMISC::CVector> akVertex;
		akVertex.resize(result.mNumOutputVertices);
		for(uint i = 0; i < result.mNumOutputVertices; ++i) {
			const float* v = &result.mOutputVertices[i*3];
			akVertex[i] = NLMISC::CVector(v[0], v[1], v[2]);
		}

		const unsigned int* piConnect = result.mIndices;
		int i;
		for (i = 0; i < result.mNumFaces; i++)
		{
			// get vertices of triangle i
			NLMISC::CVector kV0 = akVertex[*piConnect++];
			NLMISC::CVector kV1 = akVertex[*piConnect++];
			NLMISC::CVector kV2 = akVertex[*piConnect++];

			// get cross product of edges
			NLMISC::CVector kV1mV0 = kV1 - kV0;
			NLMISC::CVector kV2mV0 = kV2 - kV0;
			NLMISC::CVector kN = kV1mV0^kV2mV0;

			// compute integral terms
			float fTmp0, fTmp1, fTmp2;
			float fF1x, fF2x, fF3x, fG0x, fG1x, fG2x;
			fTmp0 = kV0.x + kV1.x;
			fF1x = fTmp0 + kV2.x;
			fTmp1 = kV0.x*kV0.x;
			fTmp2 = fTmp1 + kV1.x*fTmp0;
			fF2x = fTmp2 + kV2.x*fF1x;
			fF3x = kV0.x*fTmp1 + kV1.x*fTmp2 + kV2.x*fF2x;
			fG0x = fF2x + kV0.x*(fF1x + kV0.x);
			fG1x = fF2x + kV1.x*(fF1x + kV1.x);
			fG2x = fF2x + kV2.x*(fF1x + kV2.x);

			float fF1y, fF2y, fF3y, fG0y, fG1y, fG2y;
			fTmp0 = kV0.y + kV1.y;
			fF1y = fTmp0 + kV2.y;
			fTmp1 = kV0.y*kV0.y;
			fTmp2 = fTmp1 + kV1.y*fTmp0;
			fF2y = fTmp2 + kV2.y*fF1y;
			fF3y = kV0.y*fTmp1 + kV1.y*fTmp2 + kV2.y*fF2y;
			fG0y = fF2y + kV0.y*(fF1y + kV0.y);
			fG1y = fF2y + kV1.y*(fF1y + kV1.y);
			fG2y = fF2y + kV2.y*(fF1y + kV2.y);

			float fF1z, fF2z, fF3z, fG0z, fG1z, fG2z;
			fTmp0 = kV0.z + kV1.z;
			fF1z = fTmp0 + kV2.z;
			fTmp1 = kV0.z*kV0.z;
			fTmp2 = fTmp1 + kV1.z*fTmp0;
			fF2z = fTmp2 + kV2.z*fF1z;
			fF3z = kV0.z*fTmp1 + kV1.z*fTmp2 + kV2.z*fF2z;
			fG0z = fF2z + kV0.z*(fF1z + kV0.z);
			fG1z = fF2z + kV1.z*(fF1z + kV1.z);
			fG2z = fF2z + kV2.z*(fF1z + kV2.z);

			// update integrals
			afIntegral[0] += kN.x*fF1x;
			afIntegral[1] += kN.x*fF2x;
			afIntegral[2] += kN.y*fF2y;
			afIntegral[3] += kN.z*fF2z;
			afIntegral[4] += kN.x*fF3x;
			afIntegral[5] += kN.y*fF3y;
			afIntegral[6] += kN.z*fF3z;
			afIntegral[7] += kN.x*(kV0.y*fG0x + kV1.y*fG1x + kV2.y*fG2x);
			afIntegral[8] += kN.y*(kV0.z*fG0y + kV1.z*fG1y + kV2.z*fG2y);
			afIntegral[9] += kN.z*(kV0.x*fG0z + kV1.x*fG1z + kV2.x*fG2z);
		}

		afIntegral[0] *= fOneDiv6;
		afIntegral[1] *= fOneDiv24;
		afIntegral[2] *= fOneDiv24;
		afIntegral[3] *= fOneDiv24;
		afIntegral[4] *= fOneDiv60;
		afIntegral[5] *= fOneDiv60;
		afIntegral[6] *= fOneDiv60;
		afIntegral[7] *= fOneDiv120;
		afIntegral[8] *= fOneDiv120;
		afIntegral[9] *= fOneDiv120;

		// mass
		setMass(afIntegral[0]);

		// center of mass
		NLMISC::CVector centerOfMass = NLMISC::CVector(afIntegral[1],afIntegral[2],afIntegral[3])*m_invMass;
		setCenterOfMass(centerOfMass);

		// inertia relative to world origin
		float rkInertia[9];
		rkInertia[0] = afIntegral[5] + afIntegral[6];
		rkInertia[3] = -afIntegral[7];
		rkInertia[6] = -afIntegral[9];
		rkInertia[1] = rkInertia[3];
		rkInertia[4] = afIntegral[4] + afIntegral[6];
		rkInertia[7] = -afIntegral[8];
		rkInertia[2] = rkInertia[5];
		rkInertia[5] = rkInertia[7];
		rkInertia[8] = afIntegral[4] + afIntegral[5];

		rkInertia[0] -= m_mass*(m_centerOfMass.y*m_centerOfMass.y + m_centerOfMass.z*m_centerOfMass.z);
		rkInertia[3] += m_mass*m_centerOfMass.x*m_centerOfMass.y;
		rkInertia[6] += m_mass*m_centerOfMass.z*m_centerOfMass.x;
		rkInertia[1] = rkInertia[3];
		rkInertia[4] -= m_mass*(m_centerOfMass.z*m_centerOfMass.z + m_centerOfMass.x*m_centerOfMass.x);
		rkInertia[7] += m_mass*m_centerOfMass.y*m_centerOfMass.z;
		rkInertia[2] = rkInertia[6];
		rkInertia[5] = rkInertia[7];
		rkInertia[8] -= m_mass*(m_centerOfMass.x*m_centerOfMass.x +	m_centerOfMass.y*m_centerOfMass.y);
		CMatrix inertiaTensor(CMatrix::Identity);
		inertiaTensor.setRot(rkInertia);
	}
	
	float m_mass, m_invMass;
	CMatrix m_inertia, m_invInertia;
	CVector m_centerOfMass;

	CVector m_pos;
	CQuat m_rot;
	CVector m_linMom;
	CVector m_angMom;

	CMatrix m_rotMat;
	CMatrix m_rotMatTransp;
	CVector m_linVel;
	CVector m_angVel;

	bool m_bMoved;

    CVector m_externalForce, m_externalTorque;
	CVector m_internalForce, m_internalTorque;

};
CRigidBody Body;

// true if you want to exit the main loop
static bool				NeedExit = false;

CEvent3dMouseListener	MouseListener;

NLMISC::CValueSmootherTemplate<double> m_DeltaTimeSmooth;

class CSelector : public NLMISC::IEventListener {
public:
	CSelector() : m_rmbDown(false) {
	}

	~CSelector() {
	}

	void addToServer(NLMISC::CEventServer& server) {
		m_cursor = Scene->createInstance(ConfigFile.getVar("CursorModel").asString());
		m_cursor.setScale(0.2,0.2,0.2);
		m_cursor.hide();
//		m_cursor.setTransformMode(UTransformable::DirectMatrix);
		NL3D::CMesh* mesh = dynamic_cast<NL3D::CMesh*>(Body.Instance.getObjectPtr()->Shape.getPtr());
		if(!mesh->getMeshGeom().retrieveVertices(m_vertices)) {
			printf("ERROR: failed to get vertices\n");
			return;
		}
		if(!mesh->getMeshGeom().retrieveTriangles(m_indices)) {
			printf("ERROR: failed to get triangles\n");
			return;
		}

		server.addListener(NLMISC::EventMouseMoveId, this);
		server.addListener(NLMISC::EventMouseDownId, this);
		server.addListener(NLMISC::EventMouseUpId, this);
	}

	void removeFromServer(NLMISC::CEventServer& server) {
		if(!m_cursor.empty())
			Scene->deleteInstance(m_cursor);

		server.removeListener(NLMISC::EventMouseMoveId, this);
		server.removeListener(NLMISC::EventMouseDownId, this);
		server.removeListener(NLMISC::EventMouseUpId, this);
	}

protected:
	virtual void operator ()(const NLMISC::CEvent& event) {
		if(event==NLMISC::EventMouseDownId||event==NLMISC::EventMouseUpId||event==NLMISC::EventMouseMoveId) {
			NLMISC::CEventMouse *mouseEvent=(NLMISC::CEventMouse *)&event;
			if(event == NLMISC::EventMouseDownId) {
				if (mouseEvent->Button & NLMISC::rightButton) {
					m_rmbDown = true;
					m_cursor.show();
					showSelection(mouseEvent);
				}
			} else if (event == NLMISC::EventMouseUpId) {
				if (mouseEvent->Button & NLMISC::rightButton) {
					m_rmbDown = false;
					m_cursor.hide();
				}
			} else if (event == NLMISC::EventMouseMoveId) {
				if(m_rmbDown) {
					showSelection(mouseEvent);
				}
			}
		}
	}

	void showSelection(CEventMouse* event) {
		if(Body.Instance.empty())
			return;
		if(m_vertices.size() == 0)
			return;
		float x = event->X;
		float y = event->Y;

		CViewport vp = Scene->getViewport();
		CMatrix camMat = Scene->getCam().getMatrix();
		CFrustum frustum = Scene->getCam().getFrustum();
		Driver->setFrustum(frustum);
		CVector rayStart;
		CVector rayDir;
		vp.getRayWithPoint(x, y, rayStart, rayDir, camMat, frustum);
		rayDir.normalize();
		CMatrix worldMat = Body.Instance.getMatrix();
		CMatrix invRayMat = CMatrix::Identity;
		invRayMat.setPos(rayStart);
		invRayMat.setArbitraryRotK(rayDir);
		invRayMat.invert();
		std::vector<NLMISC::CVector> vertices;
		vertices.resize(m_vertices.size());
		for (uint i = 0; i < m_vertices.size(); ++i) {
			vertices[i] = invRayMat * (worldMat * m_vertices[i]);
		}
		float dist2D, distZ;
		CRayMesh::getRayIntersection(vertices, m_indices, dist2D, distZ, true);
		CVector cursorPos = rayStart + (rayDir * distZ);
		m_cursor.setPos(cursorPos);

		CVector contact = cursorPos - Body.getPosition();
		CVector impulse = rayDir*0.8f;
		CVector angImpulse = contact^impulse;
		Body.setLinearMomentum(Body.getLinearMomentum() + impulse);
		Body.setAngularMomentum(Body.getAngularMomentum() + angImpulse);
	}

	bool m_rmbDown;
	UInstance m_cursor;
	std::vector<NLMISC::CVector> m_vertices;
	std::vector<uint32> m_indices;
};

CSelector Selector;

void init();
void mainLoop();
double getTime();
void toggleVisibility(UInstance& instance);
double getDeltaTime() { return DeltaTime; }

int main(int argc, char **argv) {
	init();
	
	mainLoop();

	Body.destroy();
	// Release the 3d driver
	MouseListener.removeFromServer(Driver->EventServer);
	delete Driver;

	// Exit
	return EXIT_SUCCESS;
}

double getTime() {
	return CTime::ticksToSecond(CTime::getPerformanceTime());
}

void init() {
	m_DeltaTimeSmooth.init(4000);

	ConfigFile.load ("basic.cfg");
	CPath::addSearchPath(ConfigFile.getVar("DataPath").asString (), true, false);
	Driver = UDriver::createDriver();

	// Create the window with config file values
	Driver->setDisplay (UDriver::CMode(ConfigFile.getVar("ScreenWidth").asInt(), 
		ConfigFile.getVar("ScreenHeight").asInt(),
		ConfigFile.getVar("ScreenDepth").asInt(),
		ConfigFile.getVar("ScreenFull").asInt()==0));
	Driver->setSwapVBLInterval(0);

	// Set the cache size for the font manager (in bytes)
	Driver->setFontManagerMaxMemory (2000000);

	MouseListener.addToServer(Driver->EventServer);

	// Create a Text context for later text rendering
	TextContext = Driver->createTextContext (CPath::lookup(ConfigFile.getVar("FontName").asString ()));

	// Create a scene
	Scene = Driver->createScene(false);

	nlinfo ("Testing basic physics");
	nlinfo ("");
	nlinfo ("Press ESC to exit...");

	Body.init();

	CConfigFile::CVar cv;
	cv=ConfigFile.getVar("SunAmbientColor");
	NLMISC::CRGBA landscapeSunAmbient(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunDiffuseColor");
	NLMISC::CRGBA landscapeSunDiffuse(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunSpecularColor");
	NLMISC::CRGBA landscapeSunSpecular(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunDirection");
	NLMISC::CVector sunDirection(cv.asFloat(0), cv.asFloat(1), cv.asFloat(2));

	Scene->setSunDirection(sunDirection);
	Scene->setSunAmbient(landscapeSunAmbient);
	Scene->setSunDiffuse(landscapeSunDiffuse);
	Scene->setSunSpecular(landscapeSunSpecular);
	Scene->enableLightingSystem(true);

	Camera = Scene->getCam();
	Camera.setTransformMode (UTransformable::DirectMatrix);
	Camera.setPerspective ((float)Pi/2.f, 1.33f, 0.1f, 1000);

	pl = Scene->createPointLight();
	cv = ConfigFile.getVar("LightPosition");
	pl.setPos(cv.asFloat(0), cv.asFloat(1), cv.asFloat(2));
	cv = ConfigFile.getVar("LightColor");
	pl.setDiffuse(CRGBA(cv.asInt(0), cv.asInt(1), cv.asInt(2)));

	CAABBox bbox;
	Body.Instance.getShapeAABBox(bbox);
	Camera.lookAt(CVector(0.7f, -0.7f, 0.7f) * bbox.getRadius(), CVector(0, 0, 0));

	Selector.addToServer(Driver->EventServer);
	MouseListener.setMatrix (Camera.getMatrix());
	MouseListener.setFrustrum (Camera.getFrustum());
	MouseListener.setViewport (Driver->getViewport());
	MouseListener.setHotSpot (CVector(0,0,0));
	MouseListener.setMouseMode (CEvent3dMouseListener::edit3d);
	MouseListener.enableModelMatrixEdition(false);
	MouseListener.enableTranslateXYInWorld(false);

	NewTime = getTime();
}

void mainLoop() {
	while ((!NeedExit) && Driver->isActive()) {
		// Clear all buffers
		Driver->clearBuffers (CRGBA (0, 0, 0));

		// Update the time counters
		LastTime = NewTime;
		NewTime = getTime();
		DeltaTime = NewTime - LastTime;

		// Set new animation date
		Scene->animate (float(NewTime));
		Body.update(getTime(), DeltaTime);

		// Render
		Scene->render ();
		float lineHeight = ConfigFile.getVar("LineHeight").asFloat();

		// Display the frame rate
		double dt = (double)(NewTime - LastTime);
		m_DeltaTimeSmooth.addValue(dt);
		float fps = m_DeltaTimeSmooth.getSmoothValue() ? 1.0 / m_DeltaTimeSmooth.getSmoothValue() : 0.0;
		TextContext->setHotSpot (UTextContext::BottomRight);
		TextContext->setColor (CRGBA(255, 255, 255, 255));
		TextContext->setFontSize (14);
		TextContext->printfAt (ConfigFile.getVar("FpsX").asFloat(),
			ConfigFile.getVar("FpsY").asFloat(), "%.0ffps", fps);

		Driver->swapBuffers ();

		// Pump user input messages
		Driver->EventServer.pump();

		// Manage the keyboard

		if (Driver->AsyncListener.isKeyDown (KeyESCAPE)) {
			NeedExit = true;
		}
		if (Driver->AsyncListener.isKeyPushed (KeyH)) {
			toggleVisibility(Body.HullInstance);
		}
		if (Driver->AsyncListener.isKeyPushed (KeyM)) {
			toggleVisibility(Body.Instance);
		}
		if (Driver->AsyncListener.isKeyPushed (KeyC)) {
			toggleVisibility(Body.CenterOfMassInstance);
		}
		if (Driver->AsyncListener.isKeyPushed (KeyG)) {
			Body.Force = &gravForce;
		}
		if (Driver->AsyncListener.isKeyPushed (KeyP)) {
			Body.Torque = &pseudoTorque;
		}
		if (Driver->AsyncListener.isKeyPushed (KeyN)) {
			Body.Force = &resistiveForce;
			Body.Torque = &resistiveTorque;
		}
		if (Driver->AsyncListener.isKeyDown (KeyR)) {
			Body.Force = &resistiveForce;
			Body.Torque = &resistiveTorque;
			Body.reset();
			Body.setPosition(CVector::Null);
			Body.setRot(CQuat::Identity);
		}

		Camera.setMatrix (MouseListener.getViewMatrix());
	}
	nlinfo("Exiting...");
}


void toggleVisibility(UInstance& instance) {
	if(instance.empty())
		return;
	if(instance.getVisibility() != NL3D::UTransform::Hide)
		instance.hide();
	else
		instance.show();
}