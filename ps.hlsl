struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 color = ObjTexture.Sample( ObjSamplerState, input.TexCoord );
	if(color.a == 0.0) clip(-1.0);
    return color;
}