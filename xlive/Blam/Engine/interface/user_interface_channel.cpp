#include "stdafx.h"
#include "user_interface_channel.h"

#include "user_interface_widget_window.h"

/* public code */

bool c_user_interface_channel::active_or_incoming_screen_exists(void)
{
    return m_active_screen || m_incoming_screen;
}

void c_user_interface_channel::transition_out(void)
{
    if (m_active_screen)
    {
        m_active_screen->start_widget_animation(3);
    }

    return;
}
