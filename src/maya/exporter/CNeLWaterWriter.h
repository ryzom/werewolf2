#ifndef _CNELWATERWRITER_H_
#define _CNELWATERWRITER_H_

// Local includes
#include "CNeLMesh.h"

class CNeLWaterWriter : public CNeLMesh {
	public:
							CNeLWaterWriter (const MDagPath dagPath, MStatus& status);
							~CNeLWaterWriter ();
		virtual void		extract(bool verbose = true);

	protected:

		void				makeWater();

};

#endif // _CNELWATERWRITER_H_