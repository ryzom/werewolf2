string getShader() {
	string ret = """
struct v2f {
	float4 Color	: COLOR0;
	float4 Specular : COLOR1;
	float2 Tex0		: TEXCOORD0;
	float2 Tex1		: TEXCOORD1;
};

struct userParams {
	sampler2D decal;
	sampler2D aniso;
	sampler2D gloss;
	float2 Coeffs;
};

#define reflectivity USER.Coeffs.x
#define power USER.Coeffs.y

float4 main(v2f IN,
		uniform userParams USER) : COLOR {
	float4 baseColor = IN.Color * tex2D(USER.decal, IN.Tex0);
	float4 aniso = tex2D(USER.aniso, IN.Tex1);
	aniso.a = pow(aniso.a, power) * reflectivity * tex2D(USER.gloss, IN.Tex0);
	baseColor.rgb = baseColor.rgb + aniso.rgb * aniso.a * IN.Specular.rgb;
	return baseColor;
}
	""";
	return ret;
}

int getType() {
	return 1; // fragment program
}
