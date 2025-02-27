#include "common.h"
#include "screenspace_fog.h"

struct	v_vert
{
	float4 	pos	: POSITION;	// (float,float,float,1)
	float4	color	: COLOR0;	// (r,g,b,dir-occlusion)
};
struct 	v2p
{
	float4 c	: COLOR0;
	float  fog	: FOG;
	float4 hpos	: SV_Position;
};

v2p main (v_vert v)
{
	v2p 		o;

	o.hpos		= mul(m_VP, v.pos);				// xform, input in world coords
	o.c			= v.color;
	float fog	= calc_fogging(v.pos);			// fog, input in world coords
	o.fog 		= SSFX_FOGGING(1.0 - fog, v.pos.y); // Add SSFX Fog
	o.c.rgb 	= lerp(fog_color, o.c, o.fog * o.fog);	// fog blending

	float scale = s_tonemap.Load(int3(0,0,0)).x;
	o.c.rgb		= o.c.rgb*scale;      		// copy color, pre-scale by tonemap //float4 ( v.c.rgb*scale*2, v.c.a );
	o.c.a		= saturate(fog);//1.0f - o.fog;

	return o;
	
}
