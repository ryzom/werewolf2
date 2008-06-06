string getShader() {
	string ret = "";
	ret += """
// define inputs from application
struct appin
{
    float4 Position     : POSITION;
    float4 Tex0			: TEXCOORD0;
};

struct userInput
{
	float4 Color;
};

struct engineInput
{
	float4x4 ModelViewProjection;
};

// define outputs from vertex shader
struct vertout
{
    float4 HPosition    : POSITION;
    float4 Color0       : COLOR0;
    float4 Tex0			: TEXCOORD0;
};

vertout main(appin IN, 
             uniform userInput USER,
             uniform engineInput ENGINE)
{
    vertout OUT;

    // transform vertex position into homogenous clip-space
    OUT.HPosition = mul(ENGINE.ModelViewProjection, IN.Position);

	OUT.Color0 = USER.Color;

	OUT.Tex0 = IN.Tex0;
	
    return OUT;
}
""";
	return ret;
}

int getType() {
	return 0; // vertex program
}
