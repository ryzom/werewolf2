// Author: Henri Kuuste

#ifndef __CLEAFCLUSTERSHAPE_H__
#define __CLEAFCLUSTERSHAPE_H__

//
// NeL Includes
//

//
// Werewolf includes
//
#include "CBBClusterShape.h"

namespace NL3D {

class CLeafClusterModel;
// 0x63626263 0x6C757374 is ASCII for "clfclust"
const NLMISC::CClassId LeafClusterModelClassId =  NLMISC::CClassId(0x636C6663, 0x6C757374);

class CLeafClusterShape : public CBBClusterShape {
public:
	class CLeafClusterBuild : public CBBClusterBuild {
	public:
		// for dynamic casting one needs a virtual function
		virtual void	noop() { /* does nothing */ };

		class CLeafBuildSlot : public CBBBuildSlot {
		public:
			// for dynamic casting one needs a virtual function
			virtual void	noop() { /* does nothing */ };

			float		useFaceNormal;
			float		Translucency;
		};
	};

	CLeafClusterShape();
	~CLeafClusterShape();

	NLMISC_DECLARE_CLASS(CLeafClusterShape);

	virtual void		build(CBBClusterBuild *build);
	virtual void		serial(NLMISC::IStream &f) throw(NLMISC::EStream);

	virtual	NL3D::CTransformShape	*createInstance(NL3D::CScene &scene);

	friend class CLeafClusterModel;

	enum {
		UseBBCastShadows = 0x100
	};

	class CLeafSlot : public CBBSlot {
	public:
		CLeafSlot();
		~CLeafSlot();
		
		NLMISC_DECLARE_CLASS(CLeafClusterShape::CLeafSlot);

		virtual void		serial(NLMISC::IStream &f) throw(NLMISC::EStream);

		float				useFaceNormal;
		float				Translucency;

		bool				useBBCastShadows();

		friend class CLeafClusterShape;

	private:
		virtual void		clearAll();

	};

	friend class CLeafSlot;
protected:

	virtual CBBSlot*		newSlot() { return new CLeafSlot(); };
};

} // NL3D

#endif // __CLEAFCLUSTERSHAPE_H__
