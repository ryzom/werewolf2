string getShader() {
	string ret = """
struct a2v {
	float4 position : POSITION;
	float3 normal   : NORMAL;
	float2 texture  : TEXCOORD0;
};

struct engineParams {
	float4x4 ModelViewProjection;
	float4 LightVector[1]; //in world space
	float3 EyePosition; //in object space
	float4x4 ModelViewI;
};

struct modelParams {
	float3 Tangent		: TANGENT;
	float3 Binormal		: BINORMAL;
};

struct v2f {
	float4 HPOS : POSITION;
	float4 COL0 : COLOR0;   //light vector in tangent space
	float4 COL1 : COLOR1;   //half angle vector in tangent space
	float4 TEX0 : TEXCOORD0;
};


v2f main(a2v I,
		uniform engineParams ENGINE,
		modelParams MODEL)
{
	v2f O;

	// pass texture coordinates for fetching the normal map
	O.TEX0.xy = I.texture.xy;

	// compute the 3x3 tranform from tangent space to object space
	float3x3 obj2tangent;
	obj2tangent[0] = MODEL.Tangent;
	obj2tangent[1] = MODEL.Binormal;
	obj2tangent[2] = I.normal;

	// transform normal from object space to tangent space and pass it as a color
	float3 outLightPos = mul(ENGINE.ModelViewI, ENGINE.LightVector[0]).xyz;
	outLightPos = normalize(mul(obj2tangent, outLightPos));
	O.COL0.xyz = 0.5 * outLightPos + 0.5.xxx;

	// compute view vector
	float3 viewVector = normalize(ENGINE.EyePosition - I.position.xyz);

	// compute half angle vector in object space
	float3 halfAngleVector = normalize(ENGINE.LightVector[0] + viewVector);

	// transform half angle vector from object space to tangent space and pass it as a color
	O.COL1.xyz = 0.5 * mul(obj2tangent, halfAngleVector) + 0.5.xxx;

	// transform position to projection space
	O.HPOS = mul(ENGINE.ModelViewProjection, I.position);

	return O;
}
	""";
	return ret;
}

int getType() {
	return 0; // vertex program
}
