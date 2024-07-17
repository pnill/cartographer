#include "stdafx.h"
#include "panorama_user_history.h"

c_networking_panaroma_user_history* get_networking_panaroma_user_history(void)
{
    return Memory::GetAddress<c_networking_panaroma_user_history*>(0x518210, 0x5426A8);
}
