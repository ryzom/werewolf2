/**
 * \file CMayaUtils.h
 * \date October 2007
 * \author Henri Kuuste
 */

/* Copyright, 2007 Ironic Entertainment
 *
 * This file is part of Werewolf.
 * Werewolf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * Werewolf is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Werewolf; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef __CMAYAUTILS_H__
#define __CMAYAUTILS_H__

#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MPoint.h>
#include <maya/MEulerRotation.h>
#include <maya/MTransformationMatrix.h>

#include <nel/misc/vector.h>
#include <nel/misc/quat.h>
#include <nel/misc/matrix.h>

class MDagPath;
class MSpace;
class MStatus;

struct TNeLTransforms {
	NLMISC::CVector pos;
	NLMISC::CQuat rot;
	NLMISC::CVector rotE;
	NLMISC::CVector scale;
	NLMISC::CVector pivot;
	NLMISC::CMatrix matrix;
};

class CMayaUtils {
public:
	static MDagPath getTransformPath(const MDagPath& path, MStatus &status);
	static TNeLTransforms getWorldNeLTransforms(const MDagPath& path);
	static TNeLTransforms getWorldNeLTransforms(const MTransformationMatrix& matrix, bool correctRotation = false);
	static NLMISC::CVector getCVector(const MVector &vector, bool correct = true);
	static NLMISC::CVector getCVector(const MPoint &point, bool correct = true);
	static NLMISC::CVector getCVector(const double vector[3], bool correct = true);
	static NLMISC::CVector getCVector(const MEulerRotation erot);
	static NLMISC::CMatrix getCMatrix(const MMatrix &matrix, bool correct = true);
	static NLMISC::CQuat getCQuat(const MQuaternion &quat, bool correct = true);
	static MQuaternion getMQuaternion(const NLMISC::CQuat &quat);
	static void printCMatrix(const NLMISC::CMatrix& mat);
};

#endif // __CMAYAUTILS_H__
