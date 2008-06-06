string getShader(int numLights, int numShadowCasters, int numIntersectors) {
	string ret = "";
	
	if(numLights > 0 && numShadowCasters > 0) {
		ret +=	"bool raySphereIntersect(out float disc, float3 start, float3 end, float3 center, float radsqr) {\n"
				"	bool intersect = true;														\n"
				"	start = center - start;														\n"
				"	disc = dot(end, start);														\n"
				"	float sqrlength = dot(start, start);										\n"
				"	if(disc < 0.0) {															\n"
				"		if(sqrlength < radsqr) {												\n"
				"			float tdisc = -disc;												\n"
				"			disc = radsqr - (sqrlength - tdisc*tdisc);							\n"
				"			disc = disc * (radsqr - (tdisc*tdisc));								\n"
				"			disc = disc / (radsqr*radsqr);										\n"
				"			intersect = true;													\n"
				"		} else {																\n"
				"			intersect = false;													\n"
				"		}																		\n"
				"	} else {																	\n"
				"		disc = radsqr - (sqrlength - disc*disc);								\n"
				"		if(disc < 0.0) {														\n"
				"			intersect = false;													\n"
				"		}																		\n"
				"		disc = disc / radsqr;													\n"
				"	}																			\n"
				"	return intersect;															\n"
				"}																				\n"
				"float getShadow(float3 pos, float3 lightPos, float translucency, float rtranslucency, float penumbra,";
		ret +=	"float3 center[" + numShadowCasters + "],";
		ret +=	"float radius[" + numShadowCasters + "]) {										\n";
		ret +=	"	float shadow = 1;															\n";
		ret +=	"	for(int i = 0; i < " + numShadowCasters + "; i++) {							\n";
		ret +=	"		float disc = 1;															\n"
				"		if(raySphereIntersect(disc, pos, lightPos, center[i], radius[i])) {		\n"
				"			disc = saturate(1 - (disc + (1-penumbra)));							\n"
				"			if(penumbra != 0 && disc != 0)										\n"
				"				disc = disc / penumbra;											\n"
				"			shadow = shadow * disc;												\n"
				"		}																		\n"
				"	}																			\n"
				"	shadow = shadow * rtranslucency + translucency;								\n"
				"	return shadow;																\n"
				"}																				\n";
	} // numLights > 0 && numShadowCasters > 0
	
	if(numIntersectors > 0) {
//		ret +=	"void intersectAlpha(inout float3 pos, float3 normal,";
		ret +=	"void intersectAlpha(out float alpha, float3 pos, float3 normal,";
		ret +=	"float3 center[" + numIntersectors + "],";
		ret +=	"float radius[" + numIntersectors + "]) {										\n";
		ret +=	"	alpha = 1;																	\n"
				"	float3 toPos = normalize(pos);												\n";
		ret +=	"	for(int i = 0; i < " + numIntersectors + "; i++) {							\n";
		ret +=	"		float3 toCenter = normalize(center[i]);									\n"
				"		float dt = dot(toCenter, toPos);										\n"
				"		if(dt > 0)	{															\n"
				"			float3 proj = cross(normal, toCenter);								\n"
				"			normal = proj;														\n"
				"			proj = cross(toPos, proj);											\n"
				"			proj = cross(normal, proj);											\n"
				"			float cos = dot(toCenter, proj) + 0.4;								\n"
				"			alpha = min(alpha, saturate(1-(cos*cos)));							\n"
				"		}																		\n"
				"	}																			\n"
				"}																				\n";
	} // numIntersectors > 0
	
	ret +=	"struct appin {																	\n"
			"	float4 Position	:POSITION;													\n"
			"	float4 Normal	:NORMAL;													\n"
			"	float4 tex0		:TEXCOORD0;													\n"
			"};																				\n"
			"struct vertout {																\n"
			"	float4 Color0	:COLOR0;													\n"
			"	float4 HPosition:POSITION;													\n"
			"	float4 tex0		:TEXCOORD0;													\n"
			"};																				\n"
			"vertout main(appin IN,															\n"
			"			uniform float4x4 ModelViewProj,										\n"
			"			uniform float4x4 ModelViewIT,										\n"
			"			uniform float translucency,											\n"
			"			uniform float useFaceNormal,										\n"
			"			uniform float3 clusterPosition,										\n"
			"			uniform float penumbra,												\n"
			"			uniform float3 color												\n";
	
	if(numLights > 0) {
		ret +=	",\n			uniform float3 LightVector[" + numLights + "],					\n";
		ret +=	"			uniform float3 LightColor[" + numLights + "]";
	} // numLights > 0
	
	if(numShadowCasters > 0) {
		ret +=	",\n			uniform float3 CasterPositions[" + numShadowCasters + "],		\n";
		ret +=	"			uniform float CasterRadii[" + numShadowCasters + "]";
	} // numShadowCasters > 0
	
	if(numIntersectors > 0) {
		ret +=	",\n			uniform float3 IntersectorPositions[" + numIntersectors + "],	\n";
		ret +=	"uniform float IntersectorRadii[" + numIntersectors + "]";
	} // numIntersectors > 0
	
	ret +=	") {																			\n";
	ret +=	"	vertout OUT;																\n";
	
/*	if(numIntersectors > 0) {
		ret +=	"	intersectAlpha(IN.Position.xyz, normalize(IN.Normal), IntersectorPositions, IntersectorRadii);\n";
	} // numIntersectors > 0
*/
	ret +=	"	float3 normal = ((1 - useFaceNormal)*(IN.Position.xyz - clusterPosition) + (useFaceNormal * IN.Normal.xyz)); \n"
			"	normal = normalize(normal);													\n"
			"	float rtranslucency = (1 - translucency);									\n"
			"	float3 DiffuseColor = float3(0.0, 0.0, 0.0);								\n";
		
	if(numIntersectors > 0) {
		ret +=	"	intersectAlpha(OUT.Color0.a, IN.Position.xyz, normal, IntersectorPositions, IntersectorRadii);\n";
	} else { // numIntersectors > 0
		ret +=	"	OUT.Color0.a = 1;															\n";
	} // numIntersectors == 0
	
	if(numLights > 0) {
		ret +=	"	for(int i = 0; i < " + numLights + "; i++) {								\n";
		ret +=	"		float3 light = LightVector[i] - IN.Position.xyz;						\n";
		ret +=	"		light = normalize(light);												\n";
		if(numShadowCasters > 0)
			ret +=	"		float shadow = getShadow(IN.Position.xyz, light, translucency, rtranslucency, penumbra, CasterPositions, CasterRadii);\n";
		else
			ret +=	"		float shadow = 1;														\n";
		ret +=	"		float NdotL = dot(light, normal);										\n"
				"		NdotL = NdotL * rtranslucency + translucency;							\n"
				"		DiffuseColor = DiffuseColor + LightColor[i] * shadow * saturate(NdotL);	\n"
				"	}																			\n";
	} // numLights > 0
	
	ret +=	"	OUT.Color0.rgb = color * DiffuseColor;										\n"
			"	OUT.HPosition = mul(ModelViewProj, IN.Position);							\n"
			"	OUT.tex0.xy = IN.tex0.xy;													\n"
			"	ret =return OUT;															\n"
			"}																				\n";	
			
	return ret;
}