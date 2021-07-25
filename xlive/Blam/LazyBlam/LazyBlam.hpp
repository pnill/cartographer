#pragma once
#include "H2MOD/Tags/TagInterface.h"
#include "TagTable.h"
#include "Loader/Biped.hpp"


namespace lazy_blam
{
	namespace rebase
	{
		void object(char* data, tags::tag_instance* instance, int base);
		void unit(char* data, tags::tag_instance* instance, int base);

		void biped(char* data, tags::tag_instance* instance, int base);
		void vehicle(char* data, tags::tag_instance* instance, int base);
	}

	bool init_cache_file(std::string map_name);
	void close_cache_file();
	datum get_datum_from_name(std::string tag_name, blam_tag type);
	char* init_tag_data(datum tag_index);
	tags::tag_instance* get_tag_instance(datum tag_datum);
	std::string get_name_from_datum(datum tag_datum);
	std::ifstream* get_map_stream();
	s_cache_header* get_cache_header();
	s_tag_table_data* get_tag_table();
	void clear_loaded_tags();
	unsigned int resolve_data_offset(unsigned int offset);

	template<typename T>
	T* get_tag_data(datum tag_datum)
	{
		auto map_stream = get_map_stream();
		auto map_header = get_cache_header();
		auto tag_table = get_tag_table();
		char** tag_data;
		tags::tag_instance* tag_inst = get_tag_instance(tag_datum);
		for (auto &inst : tag_table->table)
			if (std::get<0>(inst).datum_index == tag_inst->datum_index)
				tag_data = &std::get<2>(inst);

		*tag_data = new char[tag_inst->size];
		map_stream->seekg(resolve_data_offset(tag_inst->data_offset));
		map_stream->read(*tag_data, tag_inst->size);

		int newBase = (int)((unsigned long)*tag_data - int(*Memory::GetAddress<int**>(0x47CD54)));

		switch (tag_inst->type.tag_type)
		{
			case blam_tag::tag_group_type::biped:
				rebase::biped(*tag_data, tag_inst, newBase);
				break;
			case blam_tag::tag_group_type::vehicle:
				rebase::vehicle(*tag_data, tag_inst, newBase);
				break;
			case blam_tag::tag_group_type::none: break;
			case blam_tag::tag_group_type::model: break;
			case blam_tag::tag_group_type::rendermodel: break;
			case blam_tag::tag_group_type::collisionmodel: break;
			case blam_tag::tag_group_type::physicsmodel: break;
			case blam_tag::tag_group_type::bitmap: break;
			case blam_tag::tag_group_type::colortable: break;
			case blam_tag::tag_group_type::multilingualunicodestringlist: break;
			case blam_tag::tag_group_type::unit: break;
			case blam_tag::tag_group_type::scenery: break;
			case blam_tag::tag_group_type::crate: break;
			case blam_tag::tag_group_type::creature: break;
			case blam_tag::tag_group_type::physics: break;
			case blam_tag::tag_group_type::object: break;
			case blam_tag::tag_group_type::contrail: break;
			case blam_tag::tag_group_type::weapon: break;
			case blam_tag::tag_group_type::light: break;
			case blam_tag::tag_group_type::effect: break;
			case blam_tag::tag_group_type::particle: break;
			case blam_tag::tag_group_type::particlemodel: break;
			case blam_tag::tag_group_type::particlephysics: break;
			case blam_tag::tag_group_type::globals: break;
			case blam_tag::tag_group_type::sound: break;
			case blam_tag::tag_group_type::soundlooping: break;
			case blam_tag::tag_group_type::item: break;
			case blam_tag::tag_group_type::equipment: break;
			case blam_tag::tag_group_type::antenna: break;
			case blam_tag::tag_group_type::lightvolume: break;
			case blam_tag::tag_group_type::liquid: break;
			case blam_tag::tag_group_type::cellularautomata: break;
			case blam_tag::tag_group_type::cellularautomata2d: break;
			case blam_tag::tag_group_type::stereosystem: break;
			case blam_tag::tag_group_type::cameratrack: break;
			case blam_tag::tag_group_type::projectile: break;
			case blam_tag::tag_group_type::device: break;
			case blam_tag::tag_group_type::devicemachine: break;
			case blam_tag::tag_group_type::devicecontrol: break;
			case blam_tag::tag_group_type::devicelightfixture: break;
			case blam_tag::tag_group_type::pointphysics: break;
			case blam_tag::tag_group_type::scenariostructurelightmap: break;
			case blam_tag::tag_group_type::scenariostructurebsp: break;
			case blam_tag::tag_group_type::scenario: break;
			case blam_tag::tag_group_type::shader: break;
			case blam_tag::tag_group_type::shadertemplate: break;
			case blam_tag::tag_group_type::shaderlightresponse: break;
			case blam_tag::tag_group_type::shaderpass: break;
			case blam_tag::tag_group_type::vertexshader: break;
			case blam_tag::tag_group_type::pixelshader: break;
			case blam_tag::tag_group_type::decoratorset: break;
			case blam_tag::tag_group_type::decorators: break;
			case blam_tag::tag_group_type::sky: break;
			case blam_tag::tag_group_type::wind: break;
			case blam_tag::tag_group_type::soundenvironment: break;
			case blam_tag::tag_group_type::lensflare: break;
			case blam_tag::tag_group_type::planarfog: break;
			case blam_tag::tag_group_type::patchyfog: break;
			case blam_tag::tag_group_type::meter: break;
			case blam_tag::tag_group_type::decal: break;
			case blam_tag::tag_group_type::colony: break;
			case blam_tag::tag_group_type::damageeffect: break;
			case blam_tag::tag_group_type::dialogue: break;
			case blam_tag::tag_group_type::itemcollection: break;
			case blam_tag::tag_group_type::vehiclecollection: break;
			case blam_tag::tag_group_type::weaponhudinterface: break;
			case blam_tag::tag_group_type::grenadehudinterface: break;
			case blam_tag::tag_group_type::unithudinterface: break;
			case blam_tag::tag_group_type::newhuddefinition: break;
			case blam_tag::tag_group_type::hudnumber: break;
			case blam_tag::tag_group_type::hudglobals: break;
			case blam_tag::tag_group_type::multiplayerscenariodescription: break;
			case blam_tag::tag_group_type::detailobjectcollection: break;
			case blam_tag::tag_group_type::soundscenery: break;
			case blam_tag::tag_group_type::hudmessagetext: break;
			case blam_tag::tag_group_type::userinterfacescreenwidgetdefinition: break;
			case blam_tag::tag_group_type::userinterfacelistskindefinition: break;
			case blam_tag::tag_group_type::userinterfaceglobalsdefinition: break;
			case blam_tag::tag_group_type::userinterfacesharedglobalsdefinition: break;
			case blam_tag::tag_group_type::textvaluepairdefinition: break;
			case blam_tag::tag_group_type::multiplayervariantsettingsinterfacedefinition: break;
			case blam_tag::tag_group_type::materialeffects: break;
			case blam_tag::tag_group_type::garbage: break;
			case blam_tag::tag_group_type::style: break;
			case blam_tag::tag_group_type::character: break;
			case blam_tag::tag_group_type::aidialogueglobals: break;
			case blam_tag::tag_group_type::aimissiondialogue: break;
			case blam_tag::tag_group_type::scenariosceneryresource: break;
			case blam_tag::tag_group_type::scenariobipedsresource: break;
			case blam_tag::tag_group_type::scenariovehiclesresource: break;
			case blam_tag::tag_group_type::scenarioequipmentresource: break;
			case blam_tag::tag_group_type::scenarioweaponsresource: break;
			case blam_tag::tag_group_type::scenariosoundsceneryresource: break;
			case blam_tag::tag_group_type::scenariolightsresource: break;
			case blam_tag::tag_group_type::scenariodevicesresource: break;
			case blam_tag::tag_group_type::scenariodecalsresource: break;
			case blam_tag::tag_group_type::scenariocinematicsresource: break;
			case blam_tag::tag_group_type::scenariotriggervolumesresource: break;
			case blam_tag::tag_group_type::scenarioclusterdataresource: break;
			case blam_tag::tag_group_type::scenariocreatureresource: break;
			case blam_tag::tag_group_type::scenariodecoratorsresource: break;
			case blam_tag::tag_group_type::scenariostructurelightingresource: break;
			case blam_tag::tag_group_type::scenariohssourcefile: break;
			case blam_tag::tag_group_type::scenarioairesource: break;
			case blam_tag::tag_group_type::scenariocommentsresource: break;
			case blam_tag::tag_group_type::breakablesurface: break;
			case blam_tag::tag_group_type::materialphysics: break;
			case blam_tag::tag_group_type::soundclasses: break;
			case blam_tag::tag_group_type::multiplayerglobals: break;
			case blam_tag::tag_group_type::soundeffecttemplate: break;
			case blam_tag::tag_group_type::soundeffectcollection: break;
			case blam_tag::tag_group_type::chocolatemountain: break;
			case blam_tag::tag_group_type::modelanimationgraph: break;
			case blam_tag::tag_group_type::cloth: break;
			case blam_tag::tag_group_type::screeneffect: break;
			case blam_tag::tag_group_type::weathersystem: break;
			case blam_tag::tag_group_type::soundmix: break;
			case blam_tag::tag_group_type::sounddialogueconstants: break;
			case blam_tag::tag_group_type::soundcachefilegestalt: break;
			case blam_tag::tag_group_type::cachefilesound: break;
			case blam_tag::tag_group_type::mousecursordefinition: break;
			case blam_tag::tag_group_type::uldg: break;
			default: ;
		}
		


		return (T*)*tag_data;
		//TODO: Can the initializer for blam_loader be passed to biped_loader? Maybe transfer blam_loader to just be part of Lazy_Blam.. Simplify
		//auto a = biped_loader(tag_inst, map_stream, map_header, tag_table);
		//TODO: Rebase based off the actual tag_data address, would be a bit of a hack but would work for accessing tag_blocks without any issues..
		//return a.get_tag_data(0);
	}

};

