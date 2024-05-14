#include "stdafx.h"
#include "user_interface_guide.h"


c_user_interface_guide_state_manager* user_interface_guide_state_manager_get(void)
{
	return Memory::GetAddress<c_user_interface_guide_state_manager*>(0x9712C8, 0x994A18);
}
