#include "H2MOD.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "SplitScreenCommon.h"
#include "VideoFixes.h"
#include "Util/Hooks/Hook.h"
#include <vector>

std::vector<IDirect3DSurface9*> backbuffer_surfaces;


typedef void(__cdecl* tdraw_motion_sensor_update)(float *arg0, float a2);
tdraw_motion_sensor_update pdraw_motion_sensor_update;

typedef IDirect3DSurface9* (__cdecl* tGetD3DSurfaceFromIndex)(int Index);
tGetD3DSurfaceFromIndex GetD3DSurfaceFromIndex;

typedef void(__cdecl *rasterizer_set_target)(signed __int16, __int16, char);
rasterizer_set_target p_rasterizer_set_target;

typedef char(__cdecl* trender_geometry)(render_geometry_type a1);
trender_geometry p_render_geometry;

typedef void(__cdecl* tCalculateHudPoint)(int arg0, float *x);
tCalculateHudPoint pCalculateHudPoint;

bool render_geo_texture = true;
bool rendering_geo = false;
IDirect3DSurface9* custom_depth;

int getRenderingPlayerIndex()
{
	return *(int*)h2mod->GetAddress(0x4E6800);
}

int getSplitType()
{
	return *(int*)h2mod->GetAddress(0x4E6970);
}

IDirect3DDevice9* getDevice()
{
	return *(IDirect3DDevice9**)h2mod->GetAddress(0xA3C6B4);
}

s_video_settings* getVideoSettings()
{
	return (s_video_settings*)h2mod->GetAddress(0xA3D9F8);
}

struct VertexArray
{
	float verts[5* 4];
};


/*
Used for correcting the vertex shader constants.
*/
VertexArray* CorrectVertexForRes()
{
	typedef bool(__thiscall* tCorrectVertexRes)(void*,int, void*, int);
	tCorrectVertexRes engineCorrectVertexRes = (tCorrectVertexRes)h2mod->GetAddress(0x4FF5B);

	VertexArray* v = new VertexArray;
	short g_d3dViewport_Height = *(int*)h2mod->GetAddress(0xA3DFB8);
	short g_d3dViewport_Width = *(int*)h2mod->GetAddress(0xA3DFBA);
	short g_screen_res_height = *(short*)h2mod->GetAddress(0xA3DFBC);
	short g_screen_res_width = *(short*)h2mod->GetAddress(0xA3DFBE);

	int v2 = g_screen_res_width - g_d3dViewport_Width;
	int v3 = g_screen_res_height - g_d3dViewport_Height;
	float v6 = 1.0 / v2;
	float v4 = 0.0;
	float v5 = 0.0;
	float v7 = 1.0 / v3;
	v->verts[0] = v6 * 2.0;
	v->verts[1] = 0.0;
	v->verts[2] = 0.0;
	v->verts[3] = v4 - (v6 + 1.0);
	v->verts[4] = 0.0;
	v->verts[5] = v7 * -2.0;
	v->verts[6] = 0.0;
	v->verts[7] = (v5+v7) + 1.0;

	engineCorrectVertexRes((void*)h2mod->GetAddress(0xA3C7B0), 177, v, 5);

	return v;
}

/*
	Used for correcting the vertex shader constants for Text specifically.
*/
VertexArray* CorrectTextVertex()
{
	typedef bool(__thiscall* tCorrectVertexRes)(void*, int, void*, int);
	tCorrectVertexRes engineCorrectVertexRes = (tCorrectVertexRes)h2mod->GetAddress(0x4FF5B);

	VertexArray* v = new VertexArray;
	short g_d3dViewport_Height = *(int*)h2mod->GetAddress(0xA3DFB8);
	short g_d3dViewport_Width = *(int*)h2mod->GetAddress(0xA3DFBA);
	//short g_screen_res_height = *(short*)h2mod->GetAddress(0xA3DFBC);
	//short g_screen_res_width = *(short*)h2mod->GetAddress(0xA3DFBE);
	short g_screen_res_height = getVideoSettings()->ScreenResY;
	short g_screen_res_width = getVideoSettings()->ScreenResX;

	int v2 = g_screen_res_width - g_d3dViewport_Width;
	int v3 = g_screen_res_height - g_d3dViewport_Height;
	float v6 = 1.0 / v2;
	float v4 = 0.0;
	float v5 = 0.0;
	float v7 = 1.0 / v3;
	v->verts[0] = v6 * 2.0;
	v->verts[1] = 0.0;
	v->verts[2] = 0.0;
	v->verts[3] = v4 - (v6 + 1.0);
	v->verts[4] = 0.0;
	v->verts[5] = v7 * -2.0;
	v->verts[6] = 0.0;
	v->verts[7] = (v5 + v7) + 1.0;

	engineCorrectVertexRes((void*)h2mod->GetAddress(0xA3C7B0), 177, v, 5);

	return v;
}


void CreateCustomDepth()
{
	D3DFORMAT fmt = *(D3DFORMAT*)h2mod->GetAddress(0xA3E458);
	D3DMULTISAMPLE_TYPE sample_type = *(D3DMULTISAMPLE_TYPE*)h2mod->GetAddress(0xA3C644);
	DWORD mspq = *(DWORD*)h2mod->GetAddress(0xA3C644);
	getDevice()->CreateDepthStencilSurface(getVideoSettings()->ScreenResX, getVideoSettings()->ScreenResY, fmt, sample_type, mspq, 0, &custom_depth, 0);
}




/*
	The function is responsible for rendering each piece of geo in the game or geo related data,
	We have to hook it to determine what type of geometry is being rendered to ensure we only apply our custom depth buffer to specific layers as lighting and other passes have good depth buffers already.
*/
char __cdecl render_geometry_hook(render_geometry_type a1)
{

	if (isSplitScreen())
	{
		rendering_geo = true;


		if (a1 != render_geometry_type::texture_accumulate && a1 != shadow_buffer_apply && a1 != shadow_buffer_generate && a1 != stencil_shadows && a1 != spherical_harmonics_PRT)
			render_geo_texture = false;
		else
		{

			render_geo_texture = true;
		}
	}

	char ret = p_render_geometry(a1);

	rendering_geo = false;
	return ret;
}


/*
	When the game is updating the depth buffer we want to update it to our own custom one so it's not nulled out,
	We check the boolean variables set in render_geometry_hook to ensure we're only applying the depth buffer where necessary as to not cause additional bugs.
*/
void __cdecl h_rasterizer_set_target(signed __int16 a1, __int16 a2, char a3)
{
	if (isSplitScreen())
	{
		if (getRenderingPlayerIndex() == 1) // This fix is only necessary for additional players, hard-coding this to player_index 1 means players 3-4 won't have this fix.
		{

			p_rasterizer_set_target(a1, a2, a3);

			if (render_geo_texture == false && rendering_geo == true)
				return;

			if (a1 == 0)
				getDevice()->SetDepthStencilSurface(custom_depth);

			return;
		}
	}

	p_rasterizer_set_target(a1, a2, a3);

	return;
}

/*
	Used to reduce code repetition, was doing the same thing over and over again, this allows just calling this function to do the calculations but will need to be modified if there's ever more than 2 players.
*/
RECT* GetSplitFixedRect()
{
	RECT sRect;

	switch (getSplitType())
	{
		case horizontal_split:
			if (getRenderingPlayerIndex() == 1)
			{
				sRect.bottom = getVideoSettings()->ScreenResY;
				sRect.top = getVideoSettings()->ScreenResY / 2;
			}
			else{
				sRect.bottom = getVideoSettings()->ScreenResY / 2;
				sRect.top = 0;
			}

			sRect.left = 0;
			sRect.right = getVideoSettings()->ScreenResX;

		break;


		case vertical_split:
			if (getRenderingPlayerIndex() == 1)
			{
				sRect.left = getVideoSettings()->ScreenResX / 2;
				sRect.right = getVideoSettings()->ScreenResX;
			}
			else {
				sRect.left = 0;
				sRect.right = getVideoSettings()->ScreenResX / 2;
			}
			
			sRect.bottom = getVideoSettings()->ScreenResY;
			sRect.top = 0;
		break;
	}

	return &sRect;
}


void fixSplitStrechSurface(IDirect3DSurface9* srcSurface,IDirect3DSurface9* dstSurface,D3DTEXTUREFILTERTYPE filter)
{
	RECT nRect;
	nRect = *GetSplitFixedRect();
	if (!srcSurface)
	{

		IDirect3DSurface9* backSurface;
		getDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backSurface);
		getDevice()->StretchRect(backSurface, &nRect, dstSurface, 0, filter);
		backSurface->Release();
	}
	else{
		getDevice()->StretchRect(srcSurface, &nRect, dstSurface, 0, filter);
	}
}

/*
	This StrechRect call has no known impact yet, so it's called "UknownFix"
*/
void __stdcall StrechRect_UnknownFix()
{
	IDirect3DSurface9* dstSurface = GetD3DSurfaceFromIndex(17); // This is the surface the game was originally trying to render on we're re-implementing the function
	IDirect3DSurface9* srcSurface = *(IDirect3DSurface9**)h2mod->GetAddress(0xA3C64C); // not really sure what this source surface is, probably where it got the data by default.

	if (isSplitScreen() && getRenderingPlayerIndex() == 1)
	{
		fixSplitStrechSurface(0,dstSurface,D3DTEXF_LINEAR);
		return;
	}

	getDevice()->StretchRect(srcSurface, 0, dstSurface, 0, D3DTEXF_LINEAR);

	return;
}

/*
	Unknown fixes for specific shaders which attempt to get the current screen data and re-render it during draw,
	The function limits the StrechRect function to the specific rectangle where player specific data is being rendered.

	The function also replaces the source surface with the backbuffer as that's where our additional player's information is being drawn.
*/
void __stdcall StrechRect_ShaderFix2()
{
		IDirect3DSurface9* dstSurface = GetD3DSurfaceFromIndex(21);
		IDirect3DSurface9* srcSurface = *(IDirect3DSurface9**)h2mod->GetAddress(0xA3C64C);

		if (isSplitScreen())
		{
			if (getRenderingPlayerIndex() == 1)
			{
			
				fixSplitStrechSurface(0, dstSurface,D3DTEXF_LINEAR);
				return;
			}
			
			if (getRenderingPlayerIndex() == 0)
			{
				fixSplitStrechSurface(srcSurface, dstSurface, D3DTEXF_LINEAR);
				return;
			}
		}

		getDevice()->StretchRect(srcSurface, 0, dstSurface, 0, D3DTEXF_LINEAR);

		return;
}

/*
	Additional shader fixes using the same methods as specified in ShaderFix2
*/
void __stdcall StrechRect_ShaderFix1()
{
	IDirect3DSurface9* dstSurface = GetD3DSurfaceFromIndex(17);
	IDirect3DSurface9* srcSurface = *(IDirect3DSurface9**)h2mod->GetAddress(0xA3C64C);

	if (isSplitScreen())
	{

		if (getRenderingPlayerIndex() == 1)
		{
			fixSplitStrechSurface(0, dstSurface, D3DTEXF_LINEAR);
			return;
		}

		if (getRenderingPlayerIndex() == 0)
		{
			fixSplitStrechSurface(srcSurface, dstSurface, D3DTEXF_LINEAR);
			return;
		}
	}

	getDevice()->StretchRect(srcSurface, 0, dstSurface, 0, D3DTEXF_LINEAR);

	return;

	
}

/*
	Fixes the Sniper viewport for the second player, it's usually black without this.
*/
void __stdcall StrechRect_SniperFix(IDirect3DDevice9* m_pD3DDev, IDirect3DSurface9 * srcSurface, RECT* srcRect, IDirect3DSurface9 *dstSurface, RECT* dstRect, D3DTEXTUREFILTERTYPE filter)
{
	if (isSplitScreen() && getRenderingPlayerIndex() == 1)
	{
		fixSplitStrechSurface(0, dstSurface, filter);
		return;
	}

	if (isSplitScreen() && getRenderingPlayerIndex() == 0)
	{
		fixSplitStrechSurface(srcSurface, dstSurface, filter);
		return;
	}

	getDevice()->StretchRect(srcSurface, 0, dstSurface, 0, filter);

	return;
}

/*
	Fixes water, this has to be applied to both players.

	Water shader copies the currently rendered image and refracts it with some distortion, normally it tries to copy the entire surface.
*/
void __stdcall StrechRect_WaterFix(IDirect3DSurface9* srcSurface, RECT* srcRect, IDirect3DSurface9 *dstSurface, RECT* dstRect, D3DTEXTUREFILTERTYPE filter)
{
	if (isSplitScreen())
	{
		if (getRenderingPlayerIndex() == 1)
		{
			fixSplitStrechSurface(0, dstSurface, filter);
			return;
		}

		if (getRenderingPlayerIndex() == 0)
		{
			fixSplitStrechSurface(srcSurface, dstSurface, filter);
			return;
		}
	}

	getDevice()->StretchRect(srcSurface, 0, dstSurface, 0, filter);

	return;

}

/*
	When the first player is rendering the gamma shader applies to the whole screen,
	We abuse this by forcing it to get the backbuffer instead of the primary surface when applying the shader, this way it copies 2nd player data into the primary surface for us.
*/
void __stdcall StrechRect_BloomHook(IDirect3DDevice9* m_pd3dDev, IDirect3DSurface9* srcSurface, RECT* srcRect, IDirect3DSurface9* dstSurface, RECT* dstRect, D3DTEXTUREFILTERTYPE filter)
{

	if (isSplitScreen() && getRenderingPlayerIndex() == 1)
	{
		fixSplitStrechSurface(0, dstSurface, D3DTEXF_POINT);
		return;
	}

	m_pd3dDev->StretchRect(srcSurface, srcRect, dstSurface, dstRect, filter);
}

short orig_res_value = 0;
short orig_res_value2 = 0;
short orig_res_value3 = 0;
short orig_res_value4 = 0;

// Fixes the scaling of the radar blip, as well as positioning for player 2.
void draw_motion_sensor_update_fix(float *arg0, float a2)
{

	if (isSplitScreen())
	{
		orig_res_value = *(short*)((BYTE*)h2mod->GetAddress(0x4E66F8) + 2);
		orig_res_value2 = *(short*)((BYTE*)h2mod->GetAddress(0x4E66FC) + 2);
		orig_res_value3 = *(short*)((BYTE*)h2mod->GetAddress(0x4E66F8));
		orig_res_value4 = *(short*)((BYTE*)h2mod->GetAddress(0x4E66FC));

		//*(arg0 + 1) = 200.0f;

		if (getRenderingPlayerIndex() == 0)
		{
			if (getSplitType() == vertical_split)
			{
				*(short*)((BYTE*)h2mod->GetAddress(0x4E66FC) + 2) = getVideoSettings()->ScreenResX;
			}
			else
			{

				//orig_res_value = 0
				//orig_res_value2 = 1920
				//orig_res_value3 = 0
				//orig_res_value4 = 540
				//arg[0] = 8.0f;
				//arg[1] = 536.0;

				*(short*)((BYTE*)h2mod->GetAddress(0x4E66FC)) = getVideoSettings()->ScreenResY;

			}
		}

		if (getRenderingPlayerIndex() == 1)
		{
			if (getSplitType() == vertical_split)
			{
				//X offset of where to render radar, player 2 needs it exactly half of resolution+8
				*arg0 = (getVideoSettings()->ScreenResX / 2) + 8;
				*(short*)((BYTE*)h2mod->GetAddress(0x4E66F8) + 2) = 0;

				// screen_bounds value used to scale the radar width
				*(short*)((BYTE*)h2mod->GetAddress(0x4E66FC) + 2) = getVideoSettings()->ScreenResX;
			}
			else {

				// original_res_value1: 0
				// original_res_value2: 1920
				// original_res_value3: 540
				// original_res_value4: 1080
				// arg0[0]: 8
				// arg0[1]: 1612

				*(arg0 + 1) = (getVideoSettings()->ScreenResY) - 8;
				*(short*)((BYTE*)h2mod->GetAddress(0x4E66F8)) = 0;
				*(short*)((BYTE*)h2mod->GetAddress(0x4E66FC)) = getVideoSettings()->ScreenResY;
			}
		}


		pdraw_motion_sensor_update(arg0, a2);
		*(short*)((BYTE*)h2mod->GetAddress(0x4E66F8) + 2) = orig_res_value;
		*(short*)((BYTE*)h2mod->GetAddress(0x4E66FC) + 2) = orig_res_value2;
		*(short*)((BYTE*)h2mod->GetAddress(0x4E66F8)) = orig_res_value3;
		*(short*)((BYTE*)h2mod->GetAddress(0x4E66FC)) = orig_res_value4;




		return;
	}


	return pdraw_motion_sensor_update(arg0, a2);
}

short orig_value = 0;
short orig_value2 = 0;
short orig_value3 = 0;
short orig_value4 = 0;

// PatchCall on hud rendering only.
/*
	When HUD elements are rendered this function is responsible for calculating Anchor points for screen positioning.
	The first player's vertex constants appear to be fine while the first player's vertex constants aren't, so we update them for the first player and only adjust positioning for the second.
*/
void __cdecl CalculateHudPoint(int arg0, float *x)
{
	if (isSplitScreen())
	{
		if (getRenderingPlayerIndex() == 1)
		{
			orig_value = *(short*)((BYTE*)h2mod->GetAddress(0x4E6700) + 2);
			orig_value2 = *(short*)((BYTE*)h2mod->GetAddress(0x4E6704) + 2);
			orig_value3 = *(short*)((BYTE*)h2mod->GetAddress(0x4E6704));
			orig_value4 = *(short*)((BYTE*)h2mod->GetAddress(0x4E6700));

			if (getSplitType() == vertical_split)
			{
				// offset to draw on top of position, some built-in safe area.
				*(short*)((BYTE*)h2mod->GetAddress(0x4E6700) + 2) = 8;

				//x cordinate to draw at.
				*(short*)((BYTE*)h2mod->GetAddress(0x4E6704) + 2) = (getVideoSettings()->ScreenResX / 2);
			}
			else
			{
				*(short*)((BYTE*)h2mod->GetAddress(0x4E6700)) = 8;
				*(short*)((BYTE*)h2mod->GetAddress(0x4E6704)) = (getVideoSettings()->ScreenResY / 2) - 8;

			}


			pCalculateHudPoint(arg0, x);

			*(short*)((BYTE*)h2mod->GetAddress(0x4E6700) + 2) = orig_value;
			*(short*)((BYTE*)h2mod->GetAddress(0x4E6704) + 2) = orig_value2;
			*(short*)((BYTE*)h2mod->GetAddress(0x4E6704)) = orig_value3;
			*(short*)((BYTE*)h2mod->GetAddress(0x4E6700)) = orig_value4;
			return;

		}

		if (getRenderingPlayerIndex() == 0)
		{

			//float c177[4] = { 0.002083333, 0, 0, -1.001042 };
			//float c178[4] = { 0, -0.003703704, 0, 1.001852 };

			VertexArray *v = CorrectVertexForRes();
			float c177[4] = { v->verts[0], v->verts[1], v->verts[2],v->verts[3] };
			float c178[4] = { v->verts[4], v->verts[5], v->verts[6], v->verts[7] };

			// fixes scaling and positioning of hud elements for first player in split-screen.
			switch (getSplitType())
			{
			case vertical_split:
				getDevice()->SetVertexShaderConstantF(177, (float*)&c177, 1);
				//*(short*)((BYTE*)h2mod->GetAddress(0x4E6700) + 2) = 8;

				//x cordinate to draw at.
				//*(short*)((BYTE*)h2mod->GetAddress(0x4E6704) + 2) = (getVideoSettings()->ScreenResX);
				break;

			case horizontal_split:
				getDevice()->SetVertexShaderConstantF(178, (float*)&c178, 1);
				break;
			}

			pCalculateHudPoint(arg0, x);


			return;
		}
	}

	return pCalculateHudPoint(arg0, x);
}

/*
	We're still not sure what this function is responsible for doing but it was causing odd issues with the way specific things were being displayed on the screen,
	It's possible it's related to being on/in a vehicle.
*/
int GetSlavePlayer(int playerIndex)
{
	if(isSplitScreen())
		return -1;

	// return normally otherwise.
	return -1;
}


/*
	During the rasterizer initialization we need to create a depth buffer for the additional player to render depth into.
	TODO(PermaNull): Additional research will most likely be needed if support is added for more than 2 players.
*/
typedef char(__cdecl *trasterizer_secondary_targets_initialize)();
trasterizer_secondary_targets_initialize prasterizer_secondary_targets_initialize;

bool rasterizer_secondary_targets_initialize()
{
	
	CreateCustomDepth();

	return prasterizer_secondary_targets_initialize();
}


/*
	Draw the split-screen separation line in the appropriate place.
	TODO(PermaNull): Support more than 2 screens.
*/
void DrawSplitScreenLine()
{
	typedef void(__cdecl* tdrawRectangle)(short* points,int rect_color);
	tdrawRectangle drawRectangle = h2mod->GetAddress<tdrawRectangle>(0x3B101);

	if (isSplitScreen())
	{
		short points[4];
		if (getSplitType() == horizontal_split)
		{
			points[0] = (getVideoSettings()->ScreenResY / 2) -1;
			points[1] = 0;
			points[2] = (getVideoSettings()->ScreenResY / 2) + 1;
			points[3] = getVideoSettings()->ScreenResX;
			drawRectangle(points, 0xFF000000);
		}
		else{
			points[0] = 0;
			points[1] = (getVideoSettings()->ScreenResX / 2) - 1;
			points[2] = getVideoSettings()->ScreenResY;
			points[3] = (getVideoSettings()->ScreenResX / 2) + 1;
			drawRectangle(points, 0xFF000000);
		}
	}


}

/*
	Anything custom we've introduced we need to release on the device reset or the d3d device will fail to reset causing the rendering to seize.
*/
void SplitFixDeviceReset()
{
	
	if(custom_depth)
		custom_depth->Release();
	

}

/*
	Hooks the rendering of text to the screen in order to fix the text size when playing in split-screen.

	TODO(PermaNull): Create an array of string IDs to reference against, 
	create a global bool to check against within this function when the specific string IDs are loaded to ensure we're not manipulating non-broken strings.
*/
void RenderText(int* a1, int* a2, int a3, int a4, int a5, float a6, char* a7)
{
	typedef void(__cdecl *tRenderText)(int *a1, int*, int, int, int, float, char*);
	tRenderText pRenderText = h2mod->GetAddress<tRenderText>(0x9975A);
	
	VertexArray *v = CorrectTextVertex();
	float c177[4] = { v->verts[0], v->verts[1], v->verts[2], v->verts[3] };
	float c178[4] = { v->verts[4], v->verts[5], v->verts[6], v->verts[7] };
	
	
	if (getRenderingPlayerIndex() == 0)
	{

		
		if (getSplitType() == vertical_split)
		{
			if ((a7[0] == 0x48 && a7[4] == 0x6F && a7[28] == 0x74) // Ho + 28 = t
				|| (a7[0] == 0x50 && a7[4] == 0x69) // Pi
				|| (a7[0] == 0x43 && a7[4] == 0x68 && a7[8] == 0x65) //Che
				|| (a7[0] == 0x4C && a7[4] == 0x6F && a7[8] == 0x61) // Loa
				|| (a7[0] == 0x57 && a7[44] == 0x72))// W+44 r)
			{
				getDevice()->SetVertexShaderConstantF(177, (float*)&c177, 1);
			}
			pRenderText(a1, a2, a3, a4, a5, a6, a7);

			return;

		}

		
		if (getSplitType() == horizontal_split)
		{
			if ((a7[0] == 0x48 && a7[4] == 0x6F && a7[28] == 0x74) // Ho + 28 = t
				|| (a7[0] == 0x50 && a7[4] == 0x69) // Pi
				|| (a7[0] == 0x43 && a7[4] == 0x68 && a7[8] == 0x65) //Che
				|| (a7[0] == 0x4C && a7[4] == 0x6F && a7[8] == 0x61) // Loa
				|| (a7[0] == 0x57 && a7[44] == 0x72))// W+44 r)
			{
				getDevice()->SetVertexShaderConstantF(178, (float*)&c178, 1);
			}
			pRenderText(a1, a2, a3, a4, a5, a6, a7);

			return;

		}

	}


	return pRenderText(a1, a2, a3, a4, a5, a6, a7);
}

void SplitFixVideoInitialize()
{

	//0x26370F - rasterizer reset
	prasterizer_secondary_targets_initialize = (trasterizer_secondary_targets_initialize)DetourFunc((BYTE*)h2mod->GetAddress(0x2628EB), (BYTE*)rasterizer_secondary_targets_initialize, 7);
	GetD3DSurfaceFromIndex = (tGetD3DSurfaceFromIndex)h2mod->GetAddress(0x28084B);

	PatchCall(h2mod->GetAddress(0x227A17), DrawSplitScreenLine);

	PatchCall(h2mod->GetAddress(0x223A4C), GetSlavePlayer);

	WriteValue<BYTE>(h2mod->GetAddress(0x4681B6), 0);
	WriteValue<BYTE>(h2mod->GetAddress(0x41F6AB), 0);
	//rasterizer_ray_of_buddha  +0x4681B6
	//patchy_fog_disable +0x41F6AB

	PatchWinAPICall(h2mod->GetAddress(0x26227D), StrechRect_BloomHook);
	NopFill(h2mod->GetAddress(0x262283), 2);
	
	PatchWinAPICall(h2mod->GetAddress(0x19F923), StrechRect_WaterFix);
	NopFill(h2mod->GetAddress(0x19F928), 3);

	PatchWinAPICall(h2mod->GetAddress(0x19A758), StrechRect_ShaderFix1);
	NopFill(h2mod->GetAddress(0x19A734), 8);
	NopFill(h2mod->GetAddress(0x19A75E), 34);

	PatchWinAPICall(h2mod->GetAddress(0x26751B), StrechRect_ShaderFix2);
	NopFill(h2mod->GetAddress(0x267521), 42);

	PatchWinAPICall(h2mod->GetAddress(0x269905), StrechRect_UnknownFix);
	NopFill(h2mod->GetAddress(0x26990B), 40);

	PatchWinAPICall(h2mod->GetAddress(0x27D3A3), StrechRect_SniperFix);
	NopFill(h2mod->GetAddress(0x27D3A9), 2);


	PatchCall(h2mod->GetAddress(0x271ADD), RenderText);
	p_render_geometry = (trender_geometry)DetourFunc(h2mod->GetAddress<BYTE*>(0x1A155C), (BYTE*)render_geometry_hook, 5);

	pCalculateHudPoint = (tCalculateHudPoint)((BYTE*)h2mod->GetAddress(0x223969));
	PatchCall(h2mod->GetAddress(0x222FC0), CalculateHudPoint);

	pdraw_motion_sensor_update = (tdraw_motion_sensor_update)DetourFunc((BYTE*)h2mod->GetAddress(0x284810), (BYTE*)draw_motion_sensor_update_fix, 5);
	p_rasterizer_set_target = (rasterizer_set_target)DetourFunc(h2mod->GetAddress<BYTE*>(0x25FDC0), (BYTE*)h_rasterizer_set_target, 6);


}
