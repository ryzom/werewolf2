string getShader() {
	string ret = """
	struct a2v {
		float4 Position		: POSITION;
		float4 Normal		: NORMAL;
		float4 Color		: COLOR0;
		float4 TexCoord0	: TEXCOORD0;
	};

	struct v2f {
		float4 Position		: POSITION;
		float4 Color		: COLOR0;
		float4 TexCoord0	: TEXCOORD0;
		float3 Refract0		: TEXCOORD1;
		float3 Refract1		: TEXCOORD2;
		float3 Refract2		: TEXCOORD3;
		float4 Reflect		: TEXCOORD4;
	};

	#define OFresnel OUT.Reflect.w
	#define OReflect OUT.Reflect.xyz

	struct engineParams {
		float4x4 ModelViewProjection;
		float4x4 ModelViewI;
		float4x4 ChangeBasis;
	};

	struct userParams {
		float3 Fresnel;
		float3 Eta;
	};

	#define power fresnelValues.x
	#define scale fresnelValues.y
	#define bias fresnelValues.z

	float fast_fresnel(float3 I, float3 N, float3 fresnelValues) {
		return bias + pow(1.0 - dot(I, N), power) * scale;
	}

	float3 transform_dir(float3 v, float4x4 m) {
		float3 o;
		o.x = dot(v, m._11_12_13);
		o.y = dot(v, m._21_22_23);
		o.z = dot(v, m._31_32_33);
		return o;
	}

	v2f main(a2v IN,
			uniform engineParams ENGINE,
			uniform userParams USER) {
		v2f OUT;

		OUT.Position = mul(ENGINE.ModelViewProjection, IN.Position);
		OUT.TexCoord0 = IN.TexCoord0;
		OUT.Color = IN.Color;

		float3 eye = float3(ENGINE.ModelViewI[0][3], ENGINE.ModelViewI[1][3], ENGINE.ModelViewI[2][3]);
		float3 normal = normalize(IN.Normal.xyz);
		float3 incident = normalize(IN.Position.xyz - eye);

		OUT.Refract0 = transform_dir(refract(incident, normal, USER.Eta.x), ENGINE.ChangeBasis);
		OUT.Refract1 = transform_dir(refract(incident, normal, USER.Eta.y), ENGINE.ChangeBasis);
		OUT.Refract2 = transform_dir(refract(incident, normal, USER.Eta.z), ENGINE.ChangeBasis);
		OReflect = transform_dir(reflect(incident, normal), ENGINE.ChangeBasis);

		OFresnel = fast_fresnel(-incident, normal, USER.Fresnel);
		
		return OUT;
	}

	""";
	return ret;
}

int getType() {
	return 0; // vertex program
}
