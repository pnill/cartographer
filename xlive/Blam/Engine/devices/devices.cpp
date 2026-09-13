#include "stdafx.h"
#include "devices.h"


#include "memory/data.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"

/* public code */

data_array* device_groups_data_get(void)
{
	return *Memory::GetAddress<data_array**>(0x4E6680, 0x50E960);
}

int32 device_group_get_from_scenario_index(
	int16 scenario_device_group_index)
{
	int32 result = NONE;

	if (tmpl_IN_RANGE<int32>(scenario_device_group_index, 0, global_scenario_get()->device_groups.count - 1))
	{
		const int32 index = datum_absolute_index_to_index(device_groups_data_get(), scenario_device_group_index);
		const device_group_datum* device_group = device_group_get(index);
		
		if (!TEST_BIT(device_group->flags, _device_group_runtime_bit))
		{
			result = index;
		}
	}

	return result;
}
