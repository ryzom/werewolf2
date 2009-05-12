/**
 * \file CMayaUtils.cpp
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

#include <CMayaUtils.h>
#include <CArgs.h>

#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MAngle.h>
#include <maya/MGlobal.h>

void CMayaUtils::printCMatrix(const NLMISC::CMatrix& mat) {
	const float* fm = mat.get();
	for(int i = 0; i < 4; ++i) {
		printf("|%7.3f %7.3f %7.3f %7.3f|\n", fm[i*4], fm[i*4 + 1], fm[i*4 + 2], fm[i*4 + 3]);
	}
}

MDagPath CMayaUtils::getTransformPath(const MDagPath& path, MStatus &status) {
	MObject transformObject = path.transform();
	MDagPath transformPath(path);
	status = MStatus::kFailure;
	while(transformPath.length() > 0) {
		if(transformPath.node() == transformObject) {
			status = MStatus::kSuccess;
			break;
		} else
			transformPath.pop();
	}
	return transformPath;
}
/* Transformations in Maya are as follows
 * inv[Sp]x[S]x[Sh]x[Sp]x[St]xinv[Rp]x[Ro]x[R]x[Rp]x[Rt]x[T]
 * TODO figure out exactly HOW to get this into NeL
 * NeL
 * [T]x[p]x[R]x[S]xinv[p]
 * Or should this be the other way around?
 * S = [S]x[Sh]
 * R = [Ro]x[R]
 * T = [T]
 * p = [Rp] (given that Sp == Rp - write error message if not)
 * St and Rt are discarded (write error messages)
 */
TNeLTransforms CMayaUtils::getWorldNeLTransforms(const MDagPath& path) {
	MStatus status;
	MDagPath tp = CMayaUtils::getTransformPath(path, status);
	return getWorldNeLTransforms(tp.inclusiveMatrix());
}

TNeLTransforms CMayaUtils::getWorldNeLTransforms(const MTransformationMatrix& matrix, bool correctRotation) {
	TNeLTransforms t;
	t.matrix = getCMatrix(matrix.asMatrix(), true);
	t.pos = getCVector(matrix.translation(MSpace::kWorld));
	t.rot = getCQuat(matrix.rotationOrientation()*matrix.rotation(), correctRotation);
	if(matrix.rotatePivotTranslation(MSpace::kWorld) != MVector::zero) {
		printf("WARNING! The transform has a non-zero rotate pivot translation, this is not supported by NeL!\n");
	}
	if(matrix.scalePivotTranslation(MSpace::kWorld) != MVector::zero) {
		printf("WARNING! The transform has a non-zero scale pivot translation, this is not supported by NeL!\n");
	}
	// How to get scale and pivot?
	// Using Maya's own atm
	double scale[3];
	matrix.getScale(scale, MSpace::kWorld);
	t.scale = getCVector(scale, true);
	if(matrix.scalePivot(MSpace::kWorld) != matrix.rotatePivot(MSpace::kWorld)) {
		printf("WARNING! The scale and rotate pivots are not equal, this is not supported by NeL!\n");
	}
	t.pivot = NLMISC::CVector::Null;
	return t;
}

NLMISC::CVector CMayaUtils::getCVector(const MVector &vector, bool correct) {
	MVector conv = correct ? CArgs::instance().conversionMatrix * vector : vector;
	return NLMISC::CVector((float)conv.x, (float)conv.y, (float)conv.z);
}

NLMISC::CVector CMayaUtils::getCVector(const MPoint &point, bool correct) {
	MPoint conv = correct ? CArgs::instance().conversionMatrix * point : point;
	return NLMISC::CVector((float)conv.x, (float)conv.y, (float)conv.z);
}

NLMISC::CVector CMayaUtils::getCVector(const double vector[3], bool correct) {
	MVector v(vector);
	MVector conv = correct ? CArgs::instance().scaleConversionMatrix * v : v;
	return NLMISC::CVector((float)conv.x, (float)conv.y, (float)conv.z);
}

NLMISC::CQuat CMayaUtils::getCQuat(const MQuaternion &quat, bool correct) {
	MQuaternion conv = quat;
	if(correct) {// not sure if this works
//		printf("Original rot matrix\n");
//		printCMatrix(getCMatrix(quat.asMatrix(), false));
//		printf("Z-up rot matrix\n");
//		printCMatrix(getCMatrix(quat.asMatrix(), true));
		MVector axis;
		double angle;
		quat.getAxisAngle(axis, angle);
		axis = axis * CArgs::instance().conversionMatrix;
		conv = MQuaternion(angle, axis);
//		printf("NeL Z-up rot matrix\n");
//		printCMatrix(getCMatrix(conv.asMatrix(), false));
	}
	return NLMISC::CQuat((float)conv.x, (float)conv.y, (float)conv.z, (float)conv.w);
}

MQuaternion CMayaUtils::getMQuaternion(const NLMISC::CQuat &quat) {
	return MQuaternion(quat.x, quat.y, quat.z, quat.w);
}

NLMISC::CMatrix CMayaUtils::getCMatrix(const MMatrix &matrix, bool correct) {
	MMatrix conv = correct ? matrix * CArgs::instance().conversionMatrix : matrix;
	NLMISC::CMatrix mat;
	float tmp[4][4];
	conv.get(tmp);
	mat.set(&tmp[0][0]);
	return mat;
}
