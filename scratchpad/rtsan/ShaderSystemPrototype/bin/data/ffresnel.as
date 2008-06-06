string getShader() {
	string ret = """
struct vert2frag {
    float4 hPosition            : POSITION;
    float3 oPosition            : TEXCOORD2;
    float3 ePosition            : TEXCOORD3;
    float3 normal               : TEXCOORD1; 
    float3 texCoord0            : TEXCOORD0;
    float4 diffuseColor         : COLOR0;
    float4 specularColor        : COLOR1;
    float4 viewerPos            : TEXCOORD5;
    float4 lightPos             : TEXCOORD4;
	float4 reflect				: TEXCOORD6;
};

struct userParams {
	float3 LightCoeffs;
	float2 Blinn;
	sampler2D decal;
	samplerCUBE env;
};

#define diffuseCoeff    USER.LightCoeffs.x
#define specularCoeff   USER.LightCoeffs.y
#define ambientCoeff    USER.LightCoeffs.z
#define n               USER.Blinn.x
#define c               USER.Blinn.y
#define lightDistance   IN.viewerPos.w


#define PI              3.14159265358979

float4 main(vert2frag IN,
			uniform userParams USER) : COLOR { 
    // generate all the vectors (note, all are in object space, and normalized)
    float3 E = normalize(IN.viewerPos.xyz - IN.oPosition);
    float3 L = normalize(IN.lightPos.xyz - IN.oPosition);
    float3 N = normalize(IN.normal);
    float3 H = normalize(L+E);
    

    // calculate all the dot products
    float NdotH = dot(N, H);
    float EdotH = dot(E, H);
    float LdotH = dot(L, H);
    float NdotL = dot(N, L);
    float NdotE = dot(N, E);


    // calculate the G term
    float Gcommon    = ( 2 * NdotH ) / EdotH;
    float Gmasking   = Gcommon * NdotE;
    float Gshadowing = Gcommon * NdotL;
    float G = min( min( 1, Gmasking ), Gshadowing );


    // calculate the F term
    float F = 1-NdotE;
    F = pow(F, 5);
    F = lerp(F, 1, n);

    
    // calculate the D term
    float D = ( c * c) / ( (NdotH) * ( c * c - 1) + 1 );
    D = D * D;

	float4 diffuseColor = IN.diffuseColor * tex2D(USER.decal, IN.texCoord0);
	float3 fresnel = texCUBE(USER.env, IN.reflect.xyz).rgb * IN.reflect.w * specularCoeff;

    // create an intermediate term DGF
    float DGF = D*G*F;

    // calculate the I term
    float4 I = diffuseColor * diffuseCoeff * max(0,NdotL) + 
               IN.specularColor * max( 0, (specularCoeff * DGF ) / NdotE );

    diffuseColor.rgb = diffuseColor.rgb * ambientCoeff + I.rgb + fresnel;
	return diffuseColor;
}
	""";
	return ret;
}

int getType() {
	return 1; // fragment program
}
