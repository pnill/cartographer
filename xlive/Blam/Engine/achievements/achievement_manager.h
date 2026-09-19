#pragma once

/* classes */

class c_achievement_manager
{
public:
	void start_upload(void);
	void submit_event(int32* event);

	void handle_live_signin_notification(bool signed_in);
	void start_level_chosen(int8 level);
	void disable_all_achievements(void);

private:
	int8 m_field_0;
	int8 m_field_1;
	int8 m_level;
	int8 m_achievement;
	bool m_signed_in;
	int8 pad[3];
	int8 m_gap[164];
	class c_achievement_live_interface* m_live_interface;

	bool field_AF;
	bool field_B0;
	bool field_B1;

	real32 m_time_satisfied;
	uint8 m_player_minimum_satisfied;
};

/* prototypes */

c_achievement_manager* achievement_manager_get(void);

class c_panorama_user_history* achievement_history_get(void);
