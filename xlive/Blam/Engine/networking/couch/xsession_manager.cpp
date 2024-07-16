#include "stdafx.h"
#include "xsession_manager.h"


XSESSION_INFO* c_xsession_manager::get_session(void)
{
	return &this->m_session;
}

bool c_xsession_manager::failed(void)
{
	return this->m_failed;
}

bool c_xsession_manager::session_valid(void)
{
	return this->m_session_valid;
}

c_xsession_manager* global_xsession_manager_get(void)
{
	return Memory::GetAddress<c_xsession_manager*>(0x968F08, 0x9910A8);
}
