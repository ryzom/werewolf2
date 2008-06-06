string getShader() {
	string ret = """
struct vert2frag
{
    float4 hPosition            : POSITION;
    float3 oPosition            : TEXCOORD2;
    float3 ePosition            : TEXCOORD3;
    float3 normal               : TEXCOORD1; 
    float3 texCoord0            : TEXCOORD0;
    float4 diffuseColor         : COLOR0;
    float4 specularColor        : COLOR1;

    // note that the fourth component of the viewerPos 
    // contains distance to the light
    float4 viewerPos            : TEXCOORD5;
    // note that the fourth component of the lightPos
    // contains specular exponent
    float4 lightPos             : TEXCOORD4;
};

struct app2vert
{
    float4 position             : POSITION;
    float4 normal               : NORMAL;
	float4 diffuseColor			: DIFFUSE;
	float4 specularColor		: SPECULAR;
    float4 texCoord0            : TEXCOORD0;
};

struct engineParams {
	float4x4 ModelViewProjection;
	float4x4 ModelViewIT;
	float4x4 ModelView;
	float4x4 ModelViewI;
	float4 LightVector[1];
};

struct userParams {
	float4 DiffuseColor;
	float4 SpecularColor;
};

vert2frag main(app2vert IN,
			 uniform engineParams ENGINE,
			 uniform userParams USER)
{
    vert2frag OUT;

    OUT.hPosition = mul(ENGINE.ModelViewProjection, IN.position);
    OUT.oPosition = IN.position.xyz;

    OUT.normal = normalize(IN.normal.xyz);
    OUT.texCoord0 = IN.texCoord0.xyz;
    OUT.diffuseColor = USER.DiffuseColor;
    OUT.specularColor = USER.SpecularColor;

    // Transform the light position into object space 
    //   (so the lighting calculations can be done in object space)
    float3 outLightPos = mul(ENGINE.ModelViewI, ENGINE.LightVector[0]).xyz;
    OUT.lightPos.xyz = outLightPos;


    // Calculate viewer position in object space (the compiler should be smart enough to optimize this)
    OUT.viewerPos.xyz = float3(ENGINE.ModelViewI[0][3], ENGINE.ModelViewI[1][3], ENGINE.ModelViewI[2][3]);

    // Calculate the distance to the light so that we can do attenuation
    OUT.viewerPos.w = length(outLightPos - IN.position.xyz);

    return OUT;
}
	""";
	return ret;
}

int getType() {
	return 0; // vertex program
}
