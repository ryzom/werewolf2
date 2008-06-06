string getShader() {
	string ret = """
	struct a2v {
		float4 Position		: POSITION;
		float4 Color		: COLOR0;
		float3 Normal		: NORMAL;
		float2 TexCoord		: TEXCOORD0;
	};

	struct modelParams {
		float3 Tangent		: TANGENT;
		float3 Binormal		: BINORMAL;
	};

	struct v2f {
		float4 Position		: POSITION;
		float4 Color		: COLOR0;
		float4 Specular		: COLOR1;
		float2 TexCoord		: TEXCOORD0;
		float3 VPosition	: TEXCOORD1;
		float3 Tangent		: TEXCOORD2;
		float3 Binormal		: TEXCOORD3;
		float3 Normal		: TEXCOORD4;
		float4 Light		: TEXCOORD5;
	};

	struct engineParams {
		float4x4 ModelViewProjection;
		float4x4 ModelView;
		float4 LightVector[1];
	};

	struct userParams {
		float4 DiffuseColor;
		float4 SpecularColor;
	};

	v2f main(a2v IN,
			modelParams MODEL,
			uniform engineParams ENGINE,
			uniform userParams USER) {
		v2f OUT;

		float3x3 ModelViewRot;
		ModelViewRot[0] = ENGINE.ModelView[0].xyz;
		ModelViewRot[1] = ENGINE.ModelView[1].xyz;
		ModelViewRot[2] = ENGINE.ModelView[2].xyz;
		
		OUT.Position = mul(ENGINE.ModelViewProjection, IN.Position);
		OUT.VPosition = mul(ENGINE.ModelView, IN.Position).xyz;

		OUT.Light = ENGINE.LightVector[0];

		OUT.Tangent = mul(ModelViewRot, MODEL.Tangent);
		OUT.Binormal = mul(ModelViewRot, MODEL.Binormal);
		OUT.Normal = mul(ModelViewRot, IN.Normal);

		OUT.Color = USER.DiffuseColor;
		OUT.Specular = USER.SpecularColor;
		OUT.TexCoord = IN.TexCoord;

		return OUT;
	}
	""";
	return ret;
}

int getType() {
	return 0; // vertex program
}
