//--------------------------------------------------------------------------------------
// Constant buffers
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );        
SamplerState samLinear : register( s0 );     

cbuffer ConstantBuffer : register( b0 )
{
       matrix World;              
       matrix View;               
       matrix Projection;   
}

//--------------------------------------------------------------------------------------
// Data formats
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;  
	float3 Norm : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
    float2 Tex : TEXCOORD0;  
	float3 Norm : NORMAL;
};
//--------------------------------------------------------------------------------------
// Vertex shaders
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input)
{	
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
	output.worldPos = mul(input.Pos, World);
	output.Norm = mul(input.Norm, (float3x3)World);
//	float PI = 3.141592654f;
	output.Tex = input.Tex;
    return output;
}
//--------------------------------------------------------------------------------------
// Pixel shaders
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
	/*float4 lightcolor = float4(1.0f,1.0f,1.0f,1.0f);
	float4 lightpos = float4(0.0f, 0.0f, 0.0f, 1.0f);                       in developing... something is wrong with normals
	float dt = dot(input.Norm, lightpos);
	float4 diffuse = txDiffuse.Sample(samLinear, input.Tex);
	return float4(dt * diffuse * lightcolor);*/
	return txDiffuse.Sample(samLinear, input.Tex);
}
float4 PS_no_light(PS_INPUT input) : SV_Target
{
	return txDiffuse.Sample(samLinear, input.Tex);
}
