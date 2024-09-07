#include "stdafx.h"
#include "user_interface_guide.h"
#include "screens/screen_cartographer_account_manager.h"


c_user_interface_guide_state_manager* user_interface_guide_state_manager_get(void)
{
	return Memory::GetAddress<c_user_interface_guide_state_manager*>(0x9712C8, 0x994A18);
}

void c_user_interface_guide_state_manager::add_user_signin_task(bool sign_to_live, void* signin_callback)
{
	//INVOKE_TYPE(0xDD7550, 0x0, int(__thiscall*)(c_user_interface_guide_state_manager*, bool, void*), this, sign_to_live, signin_callback);
	this->m_callback_task = signin_callback;
	cartographer_account_manager_open_list();
}
