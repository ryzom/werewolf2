string getShader() {
	string ret = """
struct a2v {
    float4          Position    : POSITION;
    float2          TexCoord    : TEXCOORD0;
    float3          Normal      : NORMAL;
};

struct modelParams {
    float3          Tangent     : TANGENT;
    float3          Binormal    : BINORMAL;
};


struct v2f {
    float4          Position    : POSITION;
    float2          TexCoord    : TEXCOORD0;
    float3          Light       : TEXCOORD1;
    float3          Eye         : TEXCOORD2;
};

struct engineParams {
	float4x4 ModelViewProjection;
	float4x4 ModelViewI;
    float4  LightVector[1];
};


v2f main(a2v IN,
		modelParams MODEL,
		uniform engineParams ENGINE) {

    v2f OUT;

    OUT.Position = mul(ENGINE.ModelViewProjection, IN.Position);
    OUT.TexCoord = IN.TexCoord;

	float3x3 T = float3x3(MODEL.Binormal, -MODEL.Tangent, IN.Normal);

	float3 osLight = mul(ENGINE.ModelViewI, ENGINE.LightVector[0]).xyz;
	
	float3 osEye = mul(ENGINE.ModelViewI, float4(0, 0, 0, 1)).xyz;
	
    OUT.Light = mul(T, osLight - IN.Position.xyz);
    OUT.Eye   = mul(T, osEye - IN.Position.xyz);

    return OUT;
}
	""";
	return ret;
}

int getType() {
	return 0; // vertex program
}
