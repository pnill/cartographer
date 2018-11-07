#pragma once



namespace Input
{
	enum Type
	{
		CONTROLLER = 1,
		MOUSE = 0
	};

	namespace Patch
	{

	}

	namespace Tweaks
	{
		namespace Init
		{
			void RawMouseInput();
		}

		void SetSens(Type input_type, int sens);
	}
};
