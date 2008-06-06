string getShader(int numLights) {
	string ret = "";
	ret += """
// define inputs from application
struct appin
{
    float4 Position     : POSITION;
    float4 Normal       : NORMAL;
    float4 Tex0			: TEXCOORD0;
};

struct userInput
{
	float SpecularPower;
	float Ks;
	float Kd;
	float4 Color;
};

struct engineInput
{
	float4x4 ModelViewProjection;
	float4x4 ModelViewIT;
	float4x4 ModelView;
""";
	if(numLights > 0) {
	ret += "	float3 LightColor[" + numLights + "];\n";
	ret += "	float3 LightVector[" + numLights + "];\n";
	}
	
	ret += """
};

// define outputs from vertex shader
struct vertout
{
    float4 HPosition    : POSITION;
    float4 Color0       : COLOR0;
    float4 Tex0			: TEXCOORD0;
};

vertout main(appin IN, 
             uniform userInput USER,
             uniform engineInput ENGINE)
{
    vertout OUT;

    // transform vertex position into homogenous clip-space
    OUT.HPosition = mul(ENGINE.ModelViewProjection, IN.Position);

	float3 DiffuseColor = float3(0.0, 0.0, 0.0);
	float3 SpecularColor = float3(0.0, 0.0, 0.0);
	float3 normalVec = normalize(mul(ENGINE.ModelViewIT, IN.Normal).xyz);
	float3 eyePos = mul(ENGINE.ModelView, IN.Position).xyz;
	float3 eyeVec = normalize(-eyePos);
""";
	if(numLights > 0) {
	ret += "	for(int i = 0; i < " + numLights + "; i++) {\n";
	ret += """
		float3 lightVec = normalize(ENGINE.LightVector[i] - eyePos);

		float diffuse = max(0.0, dot(normalVec, lightVec));
		diffuse = 0.9*diffuse + 0.1;
		DiffuseColor = DiffuseColor + ENGINE.LightColor[i]*diffuse;

		float3 halfVec = normalize(lightVec + eyeVec);
		float specular = pow(max(0.0, dot(normalVec, halfVec)), USER.SpecularPower);
		SpecularColor = SpecularColor + ENGINE.LightColor[i]*specular;
	}
	""";
	}
	ret += """
	OUT.Color0.a = USER.Color.a;
	OUT.Color0.rgb = USER.Color.rgb * (USER.Kd*DiffuseColor) + (USER.Ks*SpecularColor);

	OUT.Tex0 = IN.Tex0;
	
    return OUT;
}
""";
	return ret;
}

int getType() {
	return 0; // vertex program
}
