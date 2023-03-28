#include "stdafx.h"

#include "KantTesting.h"
#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"
#include "Blam\Cache\TagGroups\biped_definition.hpp"
#include "Blam\Cache\TagGroups\globals_definition.hpp"
#include "Blam\Cache\TagGroups\model_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_lightmap_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_structure_bsp_definition.hpp"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "Blam\Engine\Game\GameEngineGlobals.h"
#include "Blam\Engine\Game\GameGlobals.h"
#include "Blam\Engine\Players\Players.h"
#include "Blam\LazyBlam\LazyBlam.hpp"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "Blam\Engine\Memory\bitstream.h"
#include "Blam\Engine\Memory\bitstream.h"
#include "H2MOD\Modules\PlayerRepresentation\PlayerRepresentation.h"
#include "H2MOD\Tags\MetaExtender.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "Util\Hooks\Hook.h"
#include "H2MOD/Modules/Shell/JsonConfig/JsonConfig.cpp"


namespace KantTesting
{
	void MapLoad()
	{
		if (h2mod->GetEngineType() == _multiplayer)
		{
		}
	}
	
	void Initialize()
	{
		json_config config(L"test.json");
		config.defaultValues_["float"] = 1.0f;
		config.defaultValues_["int"] = 10;
		config.defaultValues_["string"] = "string";

		int c_int = config.get<int>("int", 99);
		float c_float = config.get<float>("float", 99.0f);
		std::string c_string = config.get<std::string>("string", "default_string");


		int c_intb = config.get<int>("int");
		float c_floatb = config.get<float>("float");
		std::string c_stringb = config.get<std::string>("string" );

		config.set<int>("test_int", 20);
		config.set<float>("test_float", 123.0f);
		config.set("test_string", "123456");
		config.set("test_int2", 20);
		config.set("test_float2", 123.0f);
		auto bb = real_point3d(1, 2, 3);
		config.set("test_point3d", bb);

		auto tpoint = config.get<real_point3d>("test_point3d");
		//need to make it so can pass char* and wchar_t*
		config.set("test_string_std", std::string("123456"));

		config.save();

		LOG_INFO_GAME("hmmm");
		if (ENABLEKANTTEST) {
		//	if (!Memory::isDedicatedServer())
			//{
			//tags::on_map_load(MapLoad);
		//	}
		}
	}
}
