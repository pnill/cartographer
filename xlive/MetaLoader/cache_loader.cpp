#include"..\stdafx.h"
#include "cache_loader.h"

cache_loader::cache_loader(std::string file_loc):cache_file_loc(file_loc)
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
	//some end of the line tasks
	//disk saving stuff
	std::list<cache_BLOCK>::iterator i = cache_list.begin();
	int start_off = 0;
	//gonna have to load the data first
	//and calculate the starting offset of the BLOCKS
	while (i != cache_list.end())
	{
		if (i->data == nullptr)
		{
			cache_in.seekg(i->offset);
			i->data = new char[i->size];
			cache_in.read(i->data, i->size);
			i->offset = -1;
		}
		start_off += i->name.length() + 1 + sizeof(int) * 2;
		i++;
	}
	//close the input stream and open the output stream
	cache_in.close();
	cache_out.open(cache_file_loc.c_str(), std::ios::out | std::ios::binary);
	cache_out.seekp(0x0);
	//then write the tables
	i = cache_list.begin();
	while (i != cache_list.end())
	{
		cache_out.write(i->name.c_str(), i->name.length() + 1);
		cache_out.write((char*)&start_off, 0x4);
		cache_out.write((char*)&i->size, 0x4);
			
		start_off += i->size;
		i++;
	}
	//then write the blocks
	i = cache_list.begin();
	while (i != cache_list.end())
	{
		cache_out.write(i->data, i->size);
		i++;
	}
	//cleaning stuff
	i = cache_list.begin();
	while (i != cache_list.end())
	{
		delete[] i->data;
		i++;
	}
	cache_list.clear();

	cache_out.close();
}
void cache_loader::add_BLOCK(std::string name, int size, char* data)
{
	cache_BLOCK temp;

	temp.name = name;
	temp.offset = -1;
	temp.size = size;
	temp.data = data;

	cache_list.push_back(temp);
}
cache_BLOCK* cache_loader::get_BLOCK(std::string name)
{
	cache_BLOCK* ret = new cache_BLOCK();

	std::list<cache_BLOCK>::iterator i = cache_list.begin();
		
	while (i != cache_list.end())
	{
		const char* str1 = i->name.c_str();
		const char* str2 = name.c_str();

		int t = strcmp(str1, str2);
		if (!t)
		{
			cache_in.seekg(i->offset);
			i->data = new char[i->size];
			cache_in.read(i->data, i->size);

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