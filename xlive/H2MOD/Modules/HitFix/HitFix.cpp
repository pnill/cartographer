#include "stdafx.h"

#include "H2MOD.h"
#include "H2MOD\Tags\TagInterface.h"
#include "H2MOD\Modules\HitFix\Hitfix.h"

// object string, initial bullet speed, final bullet speed
std::vector<std::tuple<std::string, float, float>> weapon_projectiles = 
{
	std::make_tuple("objects\\weapons\\rifle\\battle_rifle\\projectiles\\battle_rifle_bullet", 400.f * 3.f, 400.f * 3.f),
	std::make_tuple("objects\\weapons\\rifle\\covenant_carbine\\projectiles\\carbine_slug\\carbine_slug", 400.f * 3.f, 400.f * 3.f),
	std::make_tuple("objects\\weapons\\rifle\\sniper_rifle\\projectiles\\sniper_bullet", 4000.0f, 4000.0f),
	std::make_tuple("objects\\vehicles\\warthog\\turrets\\gauss\\weapon\\gauss_bullet", 90.f * 2.f, 90.f * 2.f),
	std::make_tuple("objects\\weapons\\pistol\\magnum\\projectiles\\magnum_bullet", 400.f * 2.f, 400.f * 2.f),
	std::make_tuple("objects\\vehicles\\warthog\\turrets\\chaingun\\weapon\\bullet", 2000.0f, 2000.0f),
};

void HitFix::Initialize()
{
	for (auto& proj_tuple : weapon_projectiles)
	{
		auto proj_datum = tags::find_tag('proj', std::get<0>(proj_tuple));
		BYTE* projectile_tag_data = tags::get_tag<'proj', BYTE>(proj_datum);
		if (projectile_tag_data != nullptr)
		{
			*(float*)(projectile_tag_data + 380) = std::get<1>(proj_tuple);
			*(float*)(projectile_tag_data + 384) = std::get<2>(proj_tuple);
		}
	}
}