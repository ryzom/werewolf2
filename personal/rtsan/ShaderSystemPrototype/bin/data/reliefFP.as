string getShader() {
	string ret = """
	struct v2f {
		float4 Color		: COLOR0;
		float4 Specular		: COLOR1;
		float2 TexCoord		: TEXCOORD0;
		float3 VPosition	: TEXCOORD1;
		float3 Tangent		: TEXCOORD2;
		float3 Binormal		: TEXCOORD3;
		float3 Normal		: TEXCOORD4;
		float4 Light		: TEXCOORD5;
	};

	struct userParams {
		sampler2D decal;
		sampler2D relief;
		float4 Coeffs;
	};

#define depth USER.Coeffs.x
#define specularExp USER.Coeffs.y
#define tile USER.Coeffs.z
#define Ka USER.Coeffs.w
	
	float ray_intersect_rm(in sampler2D reliefmap, in float2 dp, in float2 ds) {
		const int linear_search_steps=15;
		
		// current size of search window
		float size=1.0/linear_search_steps;
		// current depth position
		float d=0.0;
		// search front to back for first point inside object
		for( int i=0;i<linear_search_steps-1;i++ ) {
			float4 t=tex2D(reliefmap,dp+ds*d);
			if (d<t.w)
				d+=size;
		}
		
		const int binary_search_steps=5;
		
		// recurse around first point (depth) for closest match
		for( int i=0;i<binary_search_steps;i++ ) {
			size*=0.5;
			float4 t=tex2D(reliefmap,dp+ds*d);
			if (d<t.w)
				d+=2*size;
			d-=size;
		}
		
		return d;
	}


	float4 main(v2f IN,
				uniform userParams USER) : COLOR {
		float3 p = IN.VPosition;
		float3 v = normalize(IN.VPosition);
		float a = dot(IN.Normal, -v);
		float3 s = float3(dot(v, IN.Tangent), dot(v, IN.Binormal), a);
		s *= depth/a;
		float2 ds = s.xy;
		float2 dp = IN.TexCoord*tile;
		float d = ray_intersect_rm(USER.relief, dp, ds);

		float2 uv = dp + ds*d;
		float4 t = tex2D(USER.relief, uv);
		float4 c = tex2D(USER.decal, uv);

		t.xyz -= 0.5;
		t.xyz = normalize(t.x*IN.Tangent - t.y*IN.Binormal + t.z*IN.Normal);

		p += v*d/(a*depth);
		float3 l = normalize(p - IN.Light.xyz);

		float att = saturate(dot(-l, IN.Normal));
		float diff = saturate(dot(-l, t.xyz));
		float spec = saturate(dot(normalize(-l-v), t.xyz));

		float4 finalColor;
		finalColor.rgb = Ka*c + att*(c*IN.Color*diff + IN.Specular*pow(spec,specularExp));
		finalColor.a = 1.0;
		return finalColor;
	}
	""";
	return ret;
}

int getType() {
	return 1; // fragment shader
}
