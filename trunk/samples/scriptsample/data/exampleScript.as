// Example AngelScript code.

void getFoobar() {
	float val = 1;
	
	NelInfo("test logging with NeL!");

	CMatrix matrix;
	matrix.identity();

	CVector vec;
	vec.x = 1.0;

	NelInfo("vector x is: " + vec.x);

	vec.x = 2.0;
	//NelInfo("vector is: " + vec.toString());
}

void testArguments(CMatrix @matrix) {
	NelInfo("testing arguments");
	matrix.setScale(1.1f);
}

void testVectors(CVector @testVec) {
	NelInfo("testing vectors");
	NelInfo("testing vector handles: " + testVec.x + "," + testVec.y + "," + testVec.z );
}
