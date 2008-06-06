string getShader() {
	string ret = """
struct vert2frag
{
    float4 hPosition            : POSITION;
    float3 oPosition            : TEXCOORD2;
    float3 ePosition            : TEXCOORD3;
    float3 normal               : TEXCOORD1; 
    float3 texCoord0            : TEXCOORD0;
    float4 diffuseColor         : COLOR0;
    float4 specularColor        : COLOR1;
	float4 viewerPos            : TEXCOORD5;
    float4 lightPos             : TEXCOORD4;
};

struct userParams {
	float2 LightCoeffs;
	float3 OrenNayarCoeffs;
	sampler2D decal;
	float2 OrenNayarCoeffs2;
};

#define diffuseCoeff    USER.LightCoeffs.x
#define ambientCoeff    USER.LightCoeffs.y
#define rho_pi          USER.OrenNayarCoeffs.x
#define sigma			USER.OrenNayarCoeffs.y
#define lightDistance   IN.viewerPos.w
#define A				USER.OrenNayarCoeffs2.x
#define B				USER.OrenNayarCoeffs2.y


#define PI              3.14159265358979

float4 main(vert2frag       IN,
            uniform userParams  USER) : COLOR
{ 
    float3 E = normalize(IN.viewerPos.xyz - IN.oPosition);
    float3 L = normalize(IN.lightPos.xyz - IN.oPosition);
    float3 N = normalize(IN.normal);
    float3 H = normalize(L+E);
    
    float EdotH = dot(E, H);
    float LdotH = dot(L, H);
    float NdotH = dot(N, H);
    float NdotL = dot(N, L);
    float NdotE = dot(N, E);

    // calculate the zenith angles
    float sinTheta_r = length(cross(E,N));
    float cosTheta_r = max(NdotE,0.001);
    float sinTheta_i = length(cross(L,N));
    float cosTheta_i = max(NdotL,0.001);
    float tanTheta_i = sinTheta_i / cosTheta_i;
    float tanTheta_r = sinTheta_r / cosTheta_r;

    // calculate the azimuth angles
    float3 E_p = normalize(E-NdotE*N);
    float3 L_p = normalize(L-NdotL*N);
    float cosAzimuth = dot(E_p, L_p);

    float inten = rho_pi * cosTheta_i * 
        (A + B * max(0, cosAzimuth) * 
        max(sinTheta_r, sinTheta_i) * min(tanTheta_i, tanTheta_r));

	float4 diffuseColor = IN.diffuseColor * tex2D(USER.decal, IN.texCoord0);

    // calculate the I term
    float4 I = diffuseColor * diffuseCoeff * max(0,inten);

    diffuseColor.rgb = diffuseColor.rgb * ambientCoeff + I.rgb;
	return diffuseColor;
}
	""";
	return ret;
}

int getType() {
	return 1; // fragment program
}
