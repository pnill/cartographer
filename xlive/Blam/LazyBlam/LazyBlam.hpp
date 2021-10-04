#pragma once
#include "H2MOD/Tags/TagInterface.h"
#include "TagTable.h"


namespace lazy_blam
{

	namespace rebase
	{
		void object(lazy_blam_tag_instance* instance, unsigned int base);
		void unit(lazy_blam_tag_instance* instance, unsigned int base);

		void biped(lazy_blam_tag_instance* instance, unsigned int base);
		void vehicle(lazy_blam_tag_instance* instance, unsigned int base);
		void weapon(lazy_blam_tag_instance* instance, unsigned int base);
		void globals(lazy_blam_tag_instance* instance, unsigned int base);
	}
	class lazy_blam
	{
		private:
			std::string map_file;
			std::ifstream* map_stream;
			std::string maps_dir;
			std::string mods_dir;

			s_cache_header map_header;
			datum scenario_datum;
			s_tag_table_data tag_table;
			class loader
			{
			public:
				static void object(lazy_blam* blam, lazy_blam_tag_instance* instance);
				static void unit(lazy_blam* blam, lazy_blam_tag_instance* instance);
				static void biped(lazy_blam* blam, lazy_blam_tag_instance* instance);
				static void weapon(lazy_blam* blam, lazy_blam_tag_instance* instance);
			};
		public:


			bool init_cache_file(std::string map_name);
			void close_cache_file();
			datum get_datum_from_name(std::string tag_name, blam_tag type);
			
			lazy_blam_tag_instance* get_tag_instance(datum tag_datum);
			std::string get_name_from_datum(datum tag_datum);
			std::ifstream* get_map_stream();
			s_cache_header* get_cache_header();
			s_tag_table_data* get_tag_table();
			void clear_loaded_tags();
			unsigned int resolve_data_offset(unsigned int offset);
			std::vector<datum> find_tags(blam_tag type);

			char* load_tag_data(lazy_blam_tag_instance* instance);
			static void rebase_tag_data(lazy_blam_tag_instance* instance);

			lazy_blam()
			{
				
			}
			explicit lazy_blam(std::string map_name)
			{
				init_cache_file(map_name);
			}
			template<typename T>
			T* get_tag_data(datum tag_datum)
			{
				auto map_stream = get_map_stream();
				auto map_header = get_cache_header();
				auto tag_table = get_tag_table();
				auto tag_inst = get_tag_instance(tag_datum);

				if (tag_inst == nullptr)
					return nullptr;
				
				if (tag_inst->data.size == 0)
				{
					LOG_INFO_GAME("[{}] Init Data for: {:x} {:x} {}", __FUNCTION__, tag_datum, (unsigned int)tag_inst->data.buffer, tag_inst->name);
					load_tag_data(tag_inst);
					rebase_tag_data(tag_inst);
				}
				else
				{
					rebase_tag_data(tag_inst);
				}

				return (T*)tag_inst->data.buffer;
			}
	};
};

