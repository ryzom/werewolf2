string getShader() {
	string ret = """
	struct v2f {
		float4 Color		: COLOR0;
		float4 TexCoord0	: TEXCOORD0;
		float3 Refract0		: TEXCOORD1;
		float3 Refract1		: TEXCOORD2;
		float3 Refract2		: TEXCOORD3;
		float4 Reflect		: TEXCOORD4;
	};

	#define IFresnel IN.Reflect.w
	#define IReflect IN.Reflect.xyz
	
	struct userParams {
		samplerCUBE env;
	};

	float4 main(v2f IN,
				uniform userParams USER) : COLOR {

		float3 refractColor;
		
		refractColor.r = texCUBE(USER.env, IN.Refract0).r;
		refractColor.g = texCUBE(USER.env, IN.Refract1).g;
		refractColor.b = texCUBE(USER.env, IN.Refract2).b;

		float3 reflectColor = texCUBE(USER.env, IReflect).rgb;
		return float4(lerp(refractColor, reflectColor, float3(IFresnel)), 1.0);
	}

	""";
	return ret;
}

int getType() {
	return 1; // fragment program
}
