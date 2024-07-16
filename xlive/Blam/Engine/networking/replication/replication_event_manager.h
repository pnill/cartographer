#pragma once

// TODO reverse class
class c_replication_event_manager
{
public:
	void reset(void);

private:
	int8 m_field_0[84];
};
ASSERT_STRUCT_SIZE(c_replication_event_manager, 84);