#pragma once

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
		style = 'styl',
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
		udlg = 'udlg'
	};

	blam_tag() = default;

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

	static blam_tag from_string(const std::string string)
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
	//Yikers
	inline bool is_class_valid() const
	{
		switch(tag_type) { 
			case tag_group_type::model: 
			case tag_group_type::rendermodel: 
			case tag_group_type::collisionmodel: 
			case tag_group_type::physicsmodel: 
			case tag_group_type::bitmap: 
			case tag_group_type::colortable: 
			case tag_group_type::multilingualunicodestringlist: 
			case tag_group_type::unit: 
			case tag_group_type::biped: 
			case tag_group_type::vehicle: 
			case tag_group_type::scenery: 
			case tag_group_type::crate: 
			case tag_group_type::creature: 
			case tag_group_type::physics: 
			case tag_group_type::object: 
			case tag_group_type::contrail: 
			case tag_group_type::weapon: 
			case tag_group_type::light: 
			case tag_group_type::effect: 
			case tag_group_type::particle: 
			case tag_group_type::particlemodel: 
			case tag_group_type::particlephysics: 
			case tag_group_type::globals: 
			case tag_group_type::sound: 
			case tag_group_type::soundlooping: 
			case tag_group_type::item: 
			case tag_group_type::equipment: 
			case tag_group_type::antenna: 
			case tag_group_type::lightvolume: 
			case tag_group_type::liquid: 
			case tag_group_type::cellularautomata: 
			case tag_group_type::cellularautomata2d: 
			case tag_group_type::stereosystem: 
			case tag_group_type::cameratrack: 
			case tag_group_type::projectile: 
			case tag_group_type::device: 
			case tag_group_type::devicemachine: 
			case tag_group_type::devicecontrol: 
			case tag_group_type::devicelightfixture: 
			case tag_group_type::pointphysics: 
			case tag_group_type::scenariostructurelightmap: 
			case tag_group_type::scenariostructurebsp: 
			case tag_group_type::scenario: 
			case tag_group_type::shader: 
			case tag_group_type::shadertemplate: 
			case tag_group_type::shaderlightresponse: 
			case tag_group_type::shaderpass: 
			case tag_group_type::vertexshader: 
			case tag_group_type::pixelshader: 
			case tag_group_type::decoratorset: 
			case tag_group_type::decorators: 
			case tag_group_type::sky: 
			case tag_group_type::wind: 
			case tag_group_type::soundenvironment: 
			case tag_group_type::lensflare: 
			case tag_group_type::planarfog: 
			case tag_group_type::patchyfog: 
			case tag_group_type::meter: 
			case tag_group_type::decal: 
			case tag_group_type::colony: 
			case tag_group_type::damageeffect: 
			case tag_group_type::dialogue: 
			case tag_group_type::itemcollection: 
			case tag_group_type::vehiclecollection: 
			case tag_group_type::weaponhudinterface: 
			case tag_group_type::grenadehudinterface: 
			case tag_group_type::unithudinterface: 
			case tag_group_type::newhuddefinition: 
			case tag_group_type::hudnumber: 
			case tag_group_type::hudglobals: 
			case tag_group_type::multiplayerscenariodescription: 
			case tag_group_type::detailobjectcollection: 
			case tag_group_type::soundscenery: 
			case tag_group_type::hudmessagetext: 
			case tag_group_type::userinterfacescreenwidgetdefinition: 
			case tag_group_type::userinterfacelistskindefinition: 
			case tag_group_type::userinterfaceglobalsdefinition: 
			case tag_group_type::userinterfacesharedglobalsdefinition: 
			case tag_group_type::textvaluepairdefinition: 
			case tag_group_type::multiplayervariantsettingsinterfacedefinition: 
			case tag_group_type::materialeffects: 
			case tag_group_type::garbage: 
			case tag_group_type::style: 
			case tag_group_type::character: 
			case tag_group_type::aidialogueglobals: 
			case tag_group_type::aimissiondialogue: 
			case tag_group_type::scenariosceneryresource: 
			case tag_group_type::scenariobipedsresource: 
			case tag_group_type::scenariovehiclesresource: 
			case tag_group_type::scenarioequipmentresource: 
			case tag_group_type::scenarioweaponsresource: 
			case tag_group_type::scenariosoundsceneryresource: 
			case tag_group_type::scenariolightsresource: 
			case tag_group_type::scenariodevicesresource: 
			case tag_group_type::scenariodecalsresource: 
			case tag_group_type::scenariocinematicsresource: 
			case tag_group_type::scenariotriggervolumesresource: 
			case tag_group_type::scenarioclusterdataresource: 
			case tag_group_type::scenariocreatureresource: 
			case tag_group_type::scenariodecoratorsresource: 
			case tag_group_type::scenariostructurelightingresource: 
			case tag_group_type::scenariohssourcefile: 
			case tag_group_type::scenarioairesource: 
			case tag_group_type::scenariocommentsresource: 
			case tag_group_type::breakablesurface: 
			case tag_group_type::materialphysics: 
			case tag_group_type::soundclasses: 
			case tag_group_type::multiplayerglobals: 
			case tag_group_type::soundeffecttemplate: 
			case tag_group_type::soundeffectcollection: 
			case tag_group_type::chocolatemountain: 
			case tag_group_type::modelanimationgraph: 
			case tag_group_type::cloth: 
			case tag_group_type::screeneffect: 
			case tag_group_type::weathersystem: 
			case tag_group_type::soundmix: 
			case tag_group_type::sounddialogueconstants: 
			case tag_group_type::soundcachefilegestalt: 
			case tag_group_type::cachefilesound: 
			case tag_group_type::mousecursordefinition:
			case tag_group_type::udlg:
				return true;
		}
		return false;
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