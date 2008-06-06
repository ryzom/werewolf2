string getShader() {
	string ret = """
struct fragin 
{
	float4 Diffuse				: COLOR0;
	float4 Specular				: COLOR1;
    float2 TexCoords            : TEXCOORD0;
    float4 TangentToEyeMat0     : TEXCOORD4;
    float3 TangentToEyeMat1     : TEXCOORD5;
    float3 TangentToEyeMat2     : TEXCOORD6;
    float3 EyeSpacePosition     : TEXCOORD7;
};

struct userParams {
	sampler2D decal;
	sampler2D normal;
	sampler2D gloss;
	float2 LightCoeffs;
};

#define SpecularExp USER.LightCoeffs.x
#define Ka			USER.LightCoeffs.y

struct engineParams {
	float3 LightVector[1];
};

float4 main(fragin IN,
		uniform userParams USER,
		uniform engineParams ENGINE) : COLOR
{  
    float4 kd = IN.Diffuse * tex2D(USER.decal, IN.TexCoords);   // diffuse albedo
    float4 ks = IN.Specular * tex2D(USER.gloss, IN.TexCoords);    // specular "gloss" map

    // Eye-space eye vector.    
    float3 v = normalize(-IN.EyeSpacePosition);

    // Eye-space light vector.
    float3 l = normalize(ENGINE.LightVector[0] - IN.EyeSpacePosition);
    
    // Eye-space half-angle vector
    float3 h = normalize(v + l);

    // Tangent-space normal vector from normal map.
    float3 bumpScale = float3(IN.TangentToEyeMat0.ww, 1);   
    float3 TangentSpaceNormal = bumpScale * (2*tex2D(USER.normal, IN.TexCoords).xyz-1);
    
    // Transform normal vector into eye-space.
    float3 n;
    n.x = dot(IN.TangentToEyeMat0.xyz, TangentSpaceNormal);
    n.y = dot(IN.TangentToEyeMat1, TangentSpaceNormal);
    n.z = dot(IN.TangentToEyeMat2, TangentSpaceNormal);

    float4 coeffs;

    n = normalize(n);

    // Compute Blinn-Phong lighting.
    coeffs = lit(dot(n, l), dot(n, h), SpecularExp);
    float4 color = Ka * kd * (1 - coeffs.y) + (coeffs.y * kd + coeffs.z * ks);
	color.a = kd.a;
	return color;
}

	""";
	return ret;
}

int getType() {
	return 1; // fragment program
}
