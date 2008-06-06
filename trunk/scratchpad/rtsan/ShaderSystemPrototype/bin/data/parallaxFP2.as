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

	float height = 1.0;
	float numSteps = 5;

	float2 offsetCoord = IN.TexCoord;
	float4 NB = tex2D(USER.normal, offsetCoord);
	
	numSteps = lerp(numSteps*2, numSteps, eye.z);

	float step;
	float2 delta;
	float bump = mult * (NB.a - 0.5);

	step = 1.0 / numSteps;
	delta = float2(-eye.y, eye.x) * bump / numSteps;
	while(NB.a < height) {
		height -= step;
		offsetCoord += delta;
		NB = tex2D(USER.normal, offsetCoord);
	}

	height = NB.a;

    const float4 surfColor = tex2D(USER.decal, offsetCoord);
	const float4 specularColor = tex2D(USER.gloss, offsetCoord) * ENGINE.LightColor[0];
    float3 normal = normalize((2*NB.xyz) - 1);

    const float3 ambient = Ka;

	const float3 halfAngle = normalize(eye + light);

	const float4 coeffs = lit(light.z, dot(normal, halfAngle), SpecularExp);

	float selfShadow = 0;
	if (coeffs.y > 0) {
		const int numShadowSteps = lerp(60, 5, light.z);
		step = 1.0 / numShadowSteps;
		delta = float2(light.x, -light.y) * bump / (numShadowSteps * light.z);
		height = NB.a + step * 0.1;
		while((NB.a < height) && (height < 1)) {
			height += step;
			offsetCoord += delta;
			NB = tex2D(USER.normal, offsetCoord);
		}

		selfShadow = (NB.a < height);
		if(selfShadow == 0) {
			selfShadow = 0.2;
		}
	}
	
    return float4(Ka * surfColor.rgb * (1 - coeffs.y) + (coeffs.y * surfColor.rgb * selfShadow + coeffs.z * specularColor.rgb), surfColor.w);
}
	""";
	return ret;
}

int getType() {
	return 1; // fragment program
}
