string getShader() {
	string ret = """

struct v2f {
	float4 HPOS : POSITION;
	float4 COL0 : COLOR0;   //light vector in tangent space
	float4 COL1 : COLOR1;   //half angle vector in tangent space
	float2 TEX0 : TEXCOORD0;
};

struct userParams {
	sampler2D decal;
	sampler2D normal;
};

float4 FastNormalizeExpand(float4 inVec)
{
	float distSq = 1 - (dot( 2 * (inVec.xyz - 0.5), 2 * (inVec.xyz - 0.5)));
    return (distSq * (inVec - 0.5)) + (2 * (inVec - 0.5));
}

void main(v2f I,
			 out float4 color : COLOR,
			 uniform userParams USER) 
{
	float4 baseColor = tex2D(USER.decal, I.TEX0);
	float4 bumpNormal = tex2D(USER.normal, I.TEX0);
	bumpNormal = FastNormalizeExpand(bumpNormal);
	
	//expand iterated light vector to [-1,1]
	float4 lightVector = 2 * (I.COL0 - 0.5);

	float diffIntensity = saturate(dot(bumpNormal.xyz, lightVector.xyz));
	color = diffIntensity * baseColor;
	color.a = baseColor.a;
}

	""";
	return ret;
}

int getType() {
	return 1; // fragment program
}
