string getShader() {
	string ret = """
struct v2f
{
    float2 TexCoords            : TEXCOORD0;
    float4 TangentToEyeMat0     : TEXCOORD4;
    float4 TangentToEyeMat1     : TEXCOORD5;
    float3 TangentToEyeMat2     : TEXCOORD6;
    float4 EyeSpacePosition     : TEXCOORD7;
	float4 Position				: POSITION;
    float4 DiffuseColor			: COLOR0;
	float4 SpecularColor		: COLOR1;
};

struct a2v {
	float4 Position		: POSITION;
    float2 TexCoord0	: TEXCOORD0;
	float3 Normal		: NORMAL;
};

struct modelParams {
	float3 Tangent		: TANGENT;
	float3 Binormal		: BINORMAL;
};

struct userParams {
	float BumpScale;
	float4 DiffuseColor;
	float4 SpecularColor;
};

struct engineParams {
	float4x4 ModelViewProjection;
	float4x4 ModelView;
};

v2f main(a2v IN,
		modelParams MODEL,
		uniform userParams USER,
		uniform engineParams ENGINE) {
	v2f OUT;
	
    OUT.Position = mul(ENGINE.ModelViewProjection, IN.Position);
    OUT.DiffuseColor = USER.DiffuseColor;
	OUT.SpecularColor = USER.SpecularColor;

    OUT.TexCoords = IN.TexCoord0;
    
    OUT.EyeSpacePosition = mul(ENGINE.ModelView, IN.Position);

    OUT.TangentToEyeMat0.x = dot(ENGINE.ModelView[0].xyz, MODEL.Tangent);
    OUT.TangentToEyeMat0.y = dot(ENGINE.ModelView[0].xyz, MODEL.Binormal);
    OUT.TangentToEyeMat0.z = dot(ENGINE.ModelView[0].xyz, IN.Normal);
    OUT.TangentToEyeMat0.w = USER.BumpScale;
        
    OUT.TangentToEyeMat1.x = dot(ENGINE.ModelView[1].xyz, MODEL.Tangent);
    OUT.TangentToEyeMat1.y = dot(ENGINE.ModelView[1].xyz, MODEL.Binormal);
    OUT.TangentToEyeMat1.z = dot(ENGINE.ModelView[1].xyz, IN.Normal);

    OUT.TangentToEyeMat2.x = dot(ENGINE.ModelView[2].xyz, MODEL.Tangent);
    OUT.TangentToEyeMat2.y = dot(ENGINE.ModelView[2].xyz, MODEL.Binormal);
    OUT.TangentToEyeMat2.z = dot(ENGINE.ModelView[2].xyz, IN.Normal);
	return OUT;
}
	""";
	return ret;
}

int getType() {
	return 0; // vertex program
}
