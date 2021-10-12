#include "cache_loader.h"
#include"stdafx.h"

cache_loader::cache_loader(std::string file_loc) :cache_file_loc(file_loc)
{
	//size
	cache_in.open(file_loc.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
	if (cache_in.is_open())
	{
		int cache_size = cache_in.tellg();
		cache_in.close();
		//actual reading
		cache_in.open(file_loc.c_str(), std::ios::binary | std::ios::in);
		if (cache_size)
			do
			{
				cache_BLOCK temp;
				//logic to read a string
				temp.name = "";
				char ch;
				do
				{
					cache_in.read(&ch, 1);
					if (ch)
						temp.name += ch;

				} while (ch);
				cache_in.read((char*)&temp.offset, 4);
				cache_in.read((char*)&temp.size, 4);
				temp.data = nullptr;

				cache_list.push_back(temp);

			} while (cache_in.tellg() < cache_list.begin()->offset);
			//opens the file and add different BLOCKs in the listing
			_last_error = FILE_LOADED;
	}
	else
		_last_error = UNABLE_TO_LOCATE_FILE;
}
cache_loader::~cache_loader()
{									
	std::list<cache_BLOCK>::iterator i = cache_list.begin();
	int start_off = 0;

	while (i != cache_list.end())
	{		
		if (i->data != nullptr)
		{
			delete[] i->data;
			i->offset = -1;
			i->size = 0;
		}

		i++;
	}
	//close the input stream 
	cache_in.close();	
	cache_list.clear();
}

//returns pointer to a BLOCK from the cache file based on the name supplied
//no need to call delete,its a pointer maintained by cache_loader
cache_BLOCK* cache_loader::get_BLOCK(std::string name)
{
	cache_BLOCK* ret = new cache_BLOCK();

	std::list<cache_BLOCK>::iterator i = cache_list.begin();

	while (i != cache_list.end())
	{
		int t = i->name == name;
		if (t)
		{
			if (i->data == nullptr)
			{
				cache_in.seekg(i->offset);
				i->data = new char[i->size];
				cache_in.read(i->data, i->size);
			}
			ret->data = i->data;
			ret->size = i->size;

			return ret;//if we find something we return it
		}

		i++;
	}
	return nullptr;
}
int cache_loader::get_last_error()
{
	return _last_error;
}