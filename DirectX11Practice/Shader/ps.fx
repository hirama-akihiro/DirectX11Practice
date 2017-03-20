struct pixelIn 
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

float4 ps_main(pixelIn IN) : SV_Target
{
	pixelIn OUT;
	OUT.col = IN.col;
	
	return OUT.col;
}