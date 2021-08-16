#pragma once

#include "BlamPrimitiveType.h"

struct blam_tag
{
	enum class tag_group_type : __int32
	{
		none = -1,
		model = 'hlmt',
		rendermodel = 'mode',
		collisionmodel = 'coll',
		physicsmodel = 'phmo',
		bitmap = 'bitm',
		colortable = 'colo',
		multilingualunicodestringlist = 'unic',
		unit = 'unit',
		biped = 'bipd',
		vehicle = 'vehi',
		scenery = 'scen',
		crate = 'bloc',
		creature = 'crea',
		physics = 'phys',
		object = 'obje',
		contrail = 'cont',
		weapon = 'weap',
		light = 'ligh',
		effect = 'effe',
		particle = 'prt3',
		particlemodel = 'PRTM',
		particlephysics = 'pmov',
		globals = 'matg',
		sound = 'snd!',
		soundlooping = 'lsnd',
		item = 'item',
		equipment = 'eqip',
		antenna = 'ant!',
		lightvolume = 'MGS2',
		liquid = 'tdtl',
		cellularautomata = 'devo',
		cellularautomata2d = 'whip',
		stereosystem = 'BooM',
		cameratrack = 'trak',
		projectile = 'proj',
		device = 'devi',
		devicemachine = 'mach',
		devicecontrol = 'ctrl',
		devicelightfixture = 'lifi',
		pointphysics = 'pphy',
		scenariostructurelightmap = 'ltmp',
		scenariostructurebsp = 'sbsp',
		scenario = 'scnr',
		shader = 'shad',
		shadertemplate = 'stem',
		shaderlightresponse = 'slit',
		shaderpass = 'spas',
		vertexshader = 'vrtx',
		pixelshader = 'pixl',
		decoratorset = 'DECR',
		decorators = 'DECP',
		sky = 'sky ',
		wind = 'wind',
		soundenvironment = 'snde',
		lensflare = 'lens',
		planarfog = 'fog ',
		patchyfog = 'fpch',
		meter = 'metr',
		decal = 'deca',
		colony = 'coln',
		damageeffect = 'jpt!',
		dialogue = 'uldg',
		itemcollection = 'itmc',
		vehiclecollection = 'vehc',
		weaponhudinterface = 'wphi',
		grenadehudinterface = 'grhi',
		unithudinterface = 'unhi',
		newhuddefinition = 'nhdt',
		hudnumber = 'hud#',
		hudglobals = 'hudg',
		multiplayerscenariodescription = 'mply',
		detailobjectcollection = 'dobc',
		soundscenery = 'ssce',
		hudmessagetext = 'hmt ',
		userinterfacescreenwidgetdefinition = 'wgit',
		userinterfacelistskindefinition = 'skin',
		userinterfaceglobalsdefinition = 'wgtz',
		userinterfacesharedglobalsdefinition = 'wigl',
		textvaluepairdefinition = 'sily',
		multiplayervariantsettingsinterfacedefinition = 'goof',
		materialeffects = 'foot',
		garbage = 'garb',
		style = 'stly',
		character = 'char',
		aidialogueglobals = 'adlg',
		aimissiondialogue = 'mdlg',
		scenariosceneryresource = '*cen',
		scenariobipedsresource = '*ipd',
		scenariovehiclesresource = '*ehi',
		scenarioequipmentresource = '*qip',
		scenarioweaponsresource = '*eap',
		scenariosoundsceneryresource = '*sce',
		scenariolightsresource = '*igh',
		scenariodevicesresource = 'dgr*',
		scenariodecalsresource = 'dec*',
		scenariocinematicsresource = 'cin*',
		scenariotriggervolumesresource = 'trg*',
		scenarioclusterdataresource = 'clu*',
		scenariocreatureresource = '*rea',
		scenariodecoratorsresource = 'dc*s',
		scenariostructurelightingresource = 'sslt',
		scenariohssourcefile = 'hsc*',
		scenarioairesource = 'ai**',
		scenariocommentsresource = '/**/',
		breakablesurface = 'bsdt',
		materialphysics = 'mpdt',
		soundclasses = 'sncl',
		multiplayerglobals = 'mulg',
		soundeffecttemplate = '<fx>',
		soundeffectcollection = 'sfx+',
		chocolatemountain = 'gldf',
		modelanimationgraph = 'jmad',
		cloth = 'clwd',
		screeneffect = 'egor',
		weathersystem = 'weat',
		soundmix = 'snmx',
		sounddialogueconstants = 'spk!',
		soundcachefilegestalt = 'ugh!',
		cachefilesound = '$#!+',
		mousecursordefinition = 'mcsr',
	};

	blam_tag()
	{
	}

	constexpr blam_tag(tag_group_type data) :
		tag_type(data)
	{
	}

	inline std::string as_string() const
	{
		if (is_none())
			return "NONE";
		if (is_null())
			return "";
		std::string out;
		out += c_data[3];
		out += c_data[2];
		out += c_data[1];
		out += c_data[0];
		return out;
	}

	static blam_tag from_string(std::string string)
	{
		if (string.length() != 4)
			return tag_group_type::none;
		char a[4];
		a[0] = string[3];
		a[1] = string[2];
		a[2] = string[1];
		a[3] = string[0];
		auto result = blam_tag(tag_group_type(*(int*)a));
		return result;
	}

	constexpr int as_int() const
	{
		return static_cast<int>(tag_type);
	}

	constexpr bool is_null() const
	{
		return as_int() == NULL;
	}

	constexpr bool is_none() const
	{
		return tag_type == tag_group_type::none;
	}

	constexpr bool is_set() const
	{
		return !is_null() && !is_none();
	}

	bool is_printable() const
	{
		return isprint(c_data[0]) && isprint(c_data[1]) && isprint(c_data[2]) && isprint(c_data[3]);
	}

	constexpr bool operator==(const blam_tag& other) const
	{
		return this->as_int() == other.as_int();
	}

	constexpr bool operator!=(const blam_tag& other) const
	{
		return this->as_int() != other.as_int();
	}

	constexpr static blam_tag none()
	{
		return tag_group_type::none;
	}

	union {
		char c_data[4];
		tag_group_type tag_type;
	};
};
CHECK_STRUCT_SIZE(blam_tag, 4);