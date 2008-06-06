// Author: Henri Kuuste

#ifndef __CBBCLUSTERSHAPE_H__
#define __CBBCLUSTERSHAPE_H__

//
// NeL Includes
//
#include <nel/misc/class_id.h>
#include <3d/shape.h>
#include <3d/driver.h>
#include <3d/transform_shape.h>
#include <nel/misc/vector_2f.h>

namespace NL3D {

class CBBClusterModel;

// 0x63626263 0x6C757374 is ASCII for "cbbclust"
const NLMISC::CClassId BBClusterModelClassId =  NLMISC::CClassId(0x63626263, 0x6C757374);

class CBBClusterShape : public IShape {
public:

	enum				TAlignment {
		eAutoAligned,
		eScreenAligned,
		eViewAligned,
		eAxisConstrained,
		eNumAlignments
	};

	class CBBClusterBuild {
	public:
		CBBClusterBuild();
		~CBBClusterBuild();

		// for dynamic casting one needs a virtual function
		virtual void	noop() { /* does nothing */ };

		class CBBBuildSlot {
		public:
			// for dynamic casting one needs a virtual function
			virtual void	noop() { /* does nothing */ };

			std::vector<NLMISC::CVector>	BBPositions;
			std::vector<NLMISC::CRGBA>		BBColors;
			std::vector<NLMISC::CVector2f>	BBSizes;
			std::vector<float>				BBOrientations;
			std::vector<NLMISC::CVector>	BBConstrainAxes;

			float				DistMax;
			float				BlendLength;
			uint32				Flags;
			NLMISC::CVector2f	BBRadius;
			CBBClusterShape::TAlignment			Alignment;
		};

		float							Radius;
		NL3D::CMaterial					Mat;
		std::vector<CBBBuildSlot*>		Lods;

		void			clear();
		void			addLod(CBBBuildSlot *slot);
	};

	CBBClusterShape();
	~CBBClusterShape();

	NLMISC_DECLARE_CLASS(CBBClusterShape);

	virtual void		build(CBBClusterBuild *build);
	virtual void		serial(NLMISC::IStream &f) throw(NLMISC::EStream);

	virtual	NL3D::CTransformShape	*createInstance(NL3D::CScene &scene);

	virtual void		render(NL3D::IDriver *drv, NL3D::CTransformShape *trans, bool opaquePass) {};
	virtual bool		clip(const std::vector<NLMISC::CPlane>	&pyramid, const NLMISC::CMatrix &worldMatrix);
	virtual	void		getAABBox(NLMISC::CAABBox &bbox) const;
	virtual float		getNumTriangles (float distance);
	virtual void		flushTextures (NL3D::IDriver &driver, uint selectedTexture);

	enum
	{
		BlendIn				=	0x01,
		BlendOut			=	0x02,
		IsOpaque			=	0x04,
		IsTransparent		=	0x08,
		UseBBColors			=	0x10,
		UseBBSizes			=	0x20,
		UseBBOrientations	=	0x40,
		UseRadialNormals	=	0x80
	};

	class CBBSlot : public NLMISC::IStreamable	{
	public:
		CBBSlot ();
		~CBBSlot ();

		NLMISC_DECLARE_CLASS(CBBClusterShape::CBBSlot);

		NLMISC::CVector2f		BBRadius;
		float					DistMax;
		float					BlendLength;

		float					A;
		float					B;
		float					EndPolygonCount;

		uint32					Flags;

		virtual void			serial(NLMISC::IStream &f) throw(NLMISC::EStream);

		bool					isOpaque() { return (Flags&IsOpaque)!=0; }
		bool					isTransparent() { return (Flags&IsTransparent)!=0; }

		void					setBBPositions(std::vector<NLMISC::CVector> &posVec);
		void					getBBPositions(std::vector<NLMISC::CVector> &v);
		NLMISC::CVector*		getBBPositionsArray() { return m_BBPositions; };

		void					setBBColors(std::vector<NLMISC::CRGBA> &colVec);
		void					getBBColors(std::vector<NLMISC::CRGBA> &v);
		NLMISC::CRGBA*			getBBColorsArray() { return m_BBColors; };
		
		void					setBBSizes(std::vector<NLMISC::CVector2f> &sizeVec);
		void					getBBSizes(std::vector<NLMISC::CVector2f> &v);
		NLMISC::CVector2f*		getBBSizesArray() { return m_BBSizes; };

		void					setBBOrientations(std::vector<float> &orientVec);
		void					getBBOrientations(std::vector<float> &v);
		float*					getBBOrientationsArray() { return m_BBOrientations; };

		void					setBBConstrainAxes(std::vector<NLMISC::CVector> &axesVec);
		void					getBBConstrainAxes(std::vector<NLMISC::CVector> &v);
		NLMISC::CVector*		getBBConstrainAxesArray() { return m_BBConstrainAxes; };

		bool					useBBColors();
		bool					useBBSizes();
		bool					useBBOrientations();
		bool					useBBConstrainAxes();
		bool					useRadialNormals() { return Flags&UseRadialNormals; };

		uint					getNumBBs() { return m_numBBs; };
		float					getPolyCount() { return m_numBBs * 2.f; };

		CBBClusterShape::TAlignment	Alignment;
		float					CompareDistance;

	protected:
		virtual void			clearAll();

	private:
		void					clearBBPositions();
		void					clearBBSizes();
		void					clearBBColors();
		void					clearBBOrientations();
		void					clearBBConstrainAxes();

		int						m_numBBs;
		NLMISC::CVector*		m_BBPositions;
		NLMISC::CRGBA*			m_BBColors;
		NLMISC::CVector2f*		m_BBSizes;
		float*					m_BBOrientations;
		NLMISC::CVector*		m_BBConstrainAxes;
	};


protected:
	std::vector<CBBSlot*>		m_LodVector;

	void						clear();

	NL3D::CMaterial				m_Mat;
	float						Radius;

	virtual CBBSlot*			newSlot() { return new CBBSlot(); };

	void						compileRunTime();
	void						compileDistMax();

private:
	NLMISC::CAABBox				m_BBox;
	bool						m_bBBoxClean;

	friend class CBBClusterModel;
};

} // NL3D

#endif // __CBBCLUSTERSHAPE_H__