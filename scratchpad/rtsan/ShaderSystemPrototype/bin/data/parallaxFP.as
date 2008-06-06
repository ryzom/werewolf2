string getShader() {
	string ret = """
struct v2f {
    float4          Position    : POSITION;
    float2          TexCoord    : TEXCOORD0;
    float3          Light       : TEXCOORD1;
    float3          Eye         : TEXCOORD2;
};


struct userParams {
	sampler2D  decal;
	sampler2D  normal;
	sampler2D  gloss;
	float4 Coeffs;
};

struct engineParams {
	float4 LightColor[1];
};

#define mult USER.Coeffs.w
#define Ka USER.Coeffs.x
#define SpecularExp USER.Coeffs.y

float4 main(v2f IN,
			uniform userParams USER,
			uniform engineParams ENGINE) : COLOR {

    const float3 light = normalize(IN.Light);
    const float3 eye   = normalize(IN.Eye);

    float bump = (tex2D(USER.normal, IN.TexCoord).w - 0.5) * mult;

    float2 offsetCoord = bump * float2(-eye.y, eye.x) + IN.TexCoord;
    const float4 surfColor = tex2D(USER.decal, offsetCoord);
	const float4 specularColor = tex2D(USER.gloss, offsetCoord) * ENGINE.LightColor[0];
    float3 normal = normalize((2*tex2D(USER.normal, offsetCoord).xyz) - 1);

    const float3 ambient = Ka;

	float3 halfAngle = normalize(eye + light);

	float4 coeffs = lit(dot(normal, light), dot(normal, halfAngle), SpecularExp);

    return float4(Ka * surfColor.rgb * (1 - coeffs.y) + (coeffs.y * surfColor.rgb + coeffs.z * specularColor.rgb), surfColor.w);
}
	""";
	return ret;
}

int getType() {
	return 1; // fragment program
}
