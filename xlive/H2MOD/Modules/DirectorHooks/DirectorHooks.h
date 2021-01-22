#pragma once

namespace DirectorHooks
{
	enum e_director_mode
	{
		e_game = 1,
		e_editor = 2,
		e_firstperson = 4
	};
	void SetDirectorMode(e_director_mode mode);
	void ApplyHooks();
	void Initialize();
}