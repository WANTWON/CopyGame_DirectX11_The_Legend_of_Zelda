
texture2D		g_DiffuseTexture;
float4x4		g_WorldMatrix;
float4x4		g_ViewMatrix;
float4x4		g_ProjMatrix;


sampler LinearSampler = sampler_state
{	
	Filter = MIN_MAG_MIP_LINEAR;
};

sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
};


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float3		vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{

	VS_OUT			Out = (VS_OUT)0;


	//Out.vPosition = vector(In.vPosition, 1.f);
	//Out.vTexUV = In.vTexUV;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}


technique11 DefaultTechnique
{
	pass Default
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}