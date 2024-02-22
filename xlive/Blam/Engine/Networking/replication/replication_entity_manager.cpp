#include "stdafx.h"
#include "replication_entity_manager.h"


void c_replication_entity_manager::reset(void)
{
	for (uint32 i = 0; i < 16; i++)
	{
		this->m_views[i]->reset();
	}
	csmemset(this->m_entity_data, 0, sizeof(m_entity_data));
	m_field_2048 = 0;
	return;
}

void c_replication_entity_manager::initialize(void)
{
	this->m_client = NULL;
	this->m_view_mask = 0;

	for (uint32 i = 0; i < 16; i++)
	{
		this->m_views[i] = NULL;
	}

	this->reset();
	return;
}
