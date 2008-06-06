string getShader() {
	string ret = "";
	ret += """
	struct fragin {
		float4 Color0	: COLOR0;
		float2 Tex0		: TEXCOORD0;
	};

	struct userParams {
		sampler2D decal;
	};

	float4 main(fragin IN,
				uniform userParams USER) : COLOR {
		float4 c = IN.Color0 * tex2D(USER.decal, IN.Tex0);
		return c;
	}
	
	""";
	return ret;
}

int getType() {
	return 1; // fragment program
}

