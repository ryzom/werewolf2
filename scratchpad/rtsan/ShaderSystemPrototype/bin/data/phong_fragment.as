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

    // note that the fourth component of the viewerPos 
    // contains distance to the light
    float4 viewerPos            : TEXCOORD5;
    // note that the fourth component of the lightPos
    // contains specular exponent
    float4 lightPos             : TEXCOORD4;
};

struct userParams {
	float4 LightCoeffs;
	sampler2D decal;
	sampler2D gloss;
};

#define lightDistance   IN.viewerPos.w
#define Kd				USER.LightCoeffs.x
#define Ks				USER.LightCoeffs.y
#define Ka				USER.LightCoeffs.z
#define SpecularExp		USER.LightCoeffs.w

float4 main(vert2frag IN,
			uniform userParams USER) : COLOR
{ 
    float3 eye = normalize(IN.viewerPos.xyz - IN.oPosition);
    float3 light = normalize(IN.lightPos.xyz - IN.oPosition);
    float3 normal = normalize(IN.normal);

    
    float LDotN = dot(light, normal);
//    float RDotE = dot(normal, normalize(light+eye));
    float3 ref = reflect(-light, normal);
	float RDotE = dot(ref, eye);

    // raise to the specular exponent
    RDotE = max(RDotE, 0);
    RDotE = pow(RDotE, SpecularExp);

	float4 diffuseColor = IN.diffuseColor * tex2D(USER.decal, IN.texCoord0);

    float4 color;
	color.rgb = diffuseColor.rgb * Ka +
           (IN.specularColor.rgb * Ks * tex2D(USER.gloss, IN.texCoord0) * max(0,RDotE) + 
           diffuseColor.rgb * Kd * max(0,LDotN));
	color.a = diffuseColor.a;
	return color;
}
	""";
	return ret;
}

int getType() {
	return 1; // fragment program
}
