string getShader() {
	string ret = """
struct appdata {
	float4 Position : POSITION;
	float3 Normal   : NORMAL;
	float4 Tex0		: TEXCOORD0;
};

struct vpconn {
	float4 HPosition	: POSITION;
	float4 Diffuse		: COLOR0;
	float4 Specular		: COLOR1;
	float4 Tex0			: TEXCOORD0;
	float4 Tex1			: TEXCOORD1;
};

struct engineParams {
	float4x4 ModelViewProjection;
	float3x3 ModelViewIT;
	float4x4 ModelView;
	float4 LightVector[1];
	float3 EyePosition;
};

struct userParams {
	float4 DiffuseColor;
	float4 SpecularColor;
};

vpconn main(appdata IN,
			uniform engineParams ENGINE,
			uniform userParams USER)
{
	vpconn OUT;

	float3 eyePos = mul(ENGINE.ModelView, IN.Position).xyz;
	float3 lightVect = normalize(ENGINE.LightVector[0].xyz - eyePos);	

	float3 normal = normalize(mul(ENGINE.ModelViewIT, IN.Normal));
	float3 vertToEye = normalize(-eyePos);
	float3 halfAngle = normalize(normal + vertToEye);
	OUT.Tex1.y = max(dot(lightVect, normal), 0.0);
	OUT.Tex1.x = max(dot(halfAngle, normal), 0.0);
	
    // transform into homogeneous-clip space
	OUT.HPosition = mul(ENGINE.ModelViewProjection, IN.Position);

	float3 color = USER.DiffuseColor.rgb * max(dot(normal, lightVect), 0.0);
	OUT.Diffuse = float4(color, USER.DiffuseColor.a);
	OUT.Specular = USER.SpecularColor;
	OUT.Tex0 = IN.Tex0;

	return OUT;
}

	""";
	return ret;
}

int getType() {
	return 0; // vertex program
}
