string getShader() {
	string ret = """
struct app2vert
{   
    float4 Position     : POSITION;
    float4 Normal       : NORMAL;
    float3 TexCoord0    : TEXCOORD0;
};

struct vert2frag
{
    float4 HPosition	: POSITION;
    float3 oPosition	: TEXCOORD2;
    float3 ePosition	: TEXCOORD3;
    float3 Normal		: TEXCOORD1; 
    float3 TexCoord0	: TEXCOORD0;
    float4 Diffuse		: COLOR0;
    float4 Specular		: COLOR1;
    float4 viewerPos	: TEXCOORD5;
    float4 lightPos		: TEXCOORD4;
	float4 Reflect		: TEXCOORD6;
};

struct engineParams {
	float4x4 ModelViewProjection;
	float4x4 ModelView;
	float4x4 ModelViewIT;
	float4x4 ModelViewI;
	float4 LightVector[1];
};

struct userParams {
	float4 DiffuseColor;
	float4 SpecularColor;
	float2 fresnel;
};

vert2frag main(app2vert IN, 
               uniform engineParams ENGINE,
			   uniform userParams USER)
{
    vert2frag OUT;

    OUT.HPosition = mul(ENGINE.ModelViewProjection, IN.Position);
	OUT.oPosition = IN.Position.xyz;

	OUT.Normal = normalize(IN.Normal.xyz);
	OUT.Diffuse = USER.DiffuseColor;
	OUT.Specular = USER.SpecularColor;

    float3 outLightPos = mul(ENGINE.ModelViewI, ENGINE.LightVector[0]).xyz;
    OUT.lightPos.xyz = outLightPos;

    OUT.viewerPos.xyz = float3(ENGINE.ModelViewI[0][3], ENGINE.ModelViewI[1][3], ENGINE.ModelViewI[2][3]);
    OUT.viewerPos.w = length(outLightPos - IN.Position.xyz);
	
 	OUT.TexCoord0 = IN.TexCoord0;

	float3 normal = normalize(mul(ENGINE.ModelViewIT, IN.Normal).xyz);
    float3 eyeToVert = normalize(mul(ENGINE.ModelView, IN.Position).xyz);

    // compute the fresnel term
    float oneMCosAngle = 1+dot(eyeToVert,normal);
    oneMCosAngle = pow(oneMCosAngle, USER.fresnel.x);
    OUT.Reflect.a = lerp(oneMCosAngle, 1, USER.fresnel.y);
	OUT.Reflect.xyz = reflect(eyeToVert,normal);

    return OUT;
}
	""";
	return ret;
}

int getType() {
	return 0; // vertex program
}
