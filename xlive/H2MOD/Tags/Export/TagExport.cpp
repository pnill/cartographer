#include "TagExport.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "Blam/Cache/TagGroups/render_model_definition.hpp"


namespace TagExtractor
{
	void extract_render_model(std::string tagname)
	{
		auto m_datum = tags::find_tag(blam_tag::tag_group_type::rendermodel, tagname);
		auto pmodel = tags::get_tag<blam_tag::tag_group_type::rendermodel, s_render_model_group_definition>(m_datum);
		//Create a copy of the tag that can be manipulated without modifying the one being used in-game
		auto pinstance = tags::datum_to_instance(m_datum);
		auto pmem = (void*)calloc(1, pinstance->size);
		int start = (int)&pmem;
		int current = start;
		memcpy(pmem, pmodel, sizeof(s_render_model_group_definition));
		current += sizeof(s_render_model_group_definition);
	}
}
