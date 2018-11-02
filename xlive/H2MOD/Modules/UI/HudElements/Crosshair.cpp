
#include "H2MOD.h"
#include "../UI.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Utils\Utils.h"

#include "Blam/Maths/Point2D.h"


void UI::Tweaks::SetCrosshairsSize(int size, bool preset)
{
	if (h2mod->Server)
		return;

	DWORD offset = *(DWORD*)(h2mod->GetBase() + 0x479E70);

	DWORD BATRIF = offset + 0x7aa750;
	DWORD SMG = offset + 0x7A9F9C;
	DWORD CRBN = offset + 0x7ab970;
	DWORD BEAMRIF = offset + 0x7AA838;
	DWORD MAG = offset + 0x7AA33C;
	DWORD PLASRIF = offset + 0x7AA16C;
	DWORD SHTGN = offset + 0x7AA424;
	DWORD SNIP = offset + 0x7AA994;
	DWORD SWRD = offset + 0x7AA8AC;
	DWORD ROCKLAUN = offset + 0x7AA3B0;
	DWORD PLASPI = offset + 0x7AA0F8;
	DWORD BRUTESHOT = offset + 0x7AA7C4;
	DWORD NEED = offset + 0x7AA254;
	DWORD SENTBEAM = offset + 0x7AB5D0;

	DWORD WEAPONS[] = { BATRIF, SMG, CRBN, BEAMRIF, MAG, PLASRIF, SHTGN, SNIP, SWRD, ROCKLAUN, PLASPI, BRUTESHOT, NEED, SENTBEAM };

	short defaultSize[] = { 70, 70, 110, 110, 78, 52, 26, 10, 50, 50, 90, 90,110, 110, 20, 20, 110, 106, 126, 126, 106, 91, 102, 124, 112, 34, 70, 38 };
	short verySmall[] = { 30, 30, 40, 40, 39, 26, 26, 10, 25, 25, 45, 45, 65, 65, 12, 12, 55, 53, 63, 63, 53, 45, 51, 62, 56, 17, 35, 19 };
	short small[] = { 40, 40, 65, 65, 57, 38, 26, 10, 35, 35, 55, 55, 80, 80, 15, 15, 82, 79, 90, 90, 79, 68, 76, 93, 84, 25, 52, 27 };
	short large[] = { 80, 80, 130, 130, 114, 76, 52, 20, 70, 70, 110, 110, 160, 160, 30, 30, 164, 158, 180, 180, 158, 136, 152, 186, 168, 50, 104, 57 };
	int* configArray[] = { &H2Config_BATRIF_WIDTH, &H2Config_BATRIF_HEIGHT, &H2Config_SMG_WIDTH, &H2Config_SMG_HEIGHT, &H2Config_CRBN_WIDTH, &H2Config_CRBN_HEIGHT, &H2Config_BEAMRIF_WIDTH, &H2Config_BEAMRIF_HEIGHT, &H2Config_MAG_WIDTH, &H2Config_MAG_HEIGHT, &H2Config_PLASRIF_WIDTH, &H2Config_PLASRIF_HEIGHT, &H2Config_SHTGN_WIDTH, &H2Config_SHTGN_HEIGHT, &H2Config_SNIP_WIDTH, &H2Config_SNIP_HEIGHT, &H2Config_SWRD_WIDTH, &H2Config_SWRD_HEIGHT, &H2Config_ROCKLAUN_WIDTH, &H2Config_ROCKLAUN_HEIGHT, &H2Config_PLASPI_WIDTH, &H2Config_PLASPI_HEIGHT, &H2Config_BRUTESHOT_WIDTH, &H2Config_BRUTESHOT_HEIGHT, &H2Config_NEED_WIDTH, &H2Config_NEED_HEIGHT, &H2Config_SENTBEAM_WIDTH, &H2Config_SENTBEAM_HEIGHT };
	short* tempArray;

	if (preset)
	{
		switch (size) {
		case 2:
			tempArray = verySmall;
			break;
		case 3:
			tempArray = small;
			break;
		case 4:
			tempArray = large;
			break;
		default:
			tempArray = defaultSize;
			break;
		}

		for (int i = 0; i < 28; i++) {
			*configArray[i] = tempArray[i];
		}
	}

	if (h2mod->GetEngineType() == EngineType::MULTIPLAYER_ENGINE)
	{
		int k = 0;
		for (int i = 0; i < 14; i++) {
			if (size == 1)
			{
				reinterpret_cast<Blam::Maths::Integer::Short::Point2D*>(WEAPONS[i])->X = 0;
				reinterpret_cast<Blam::Maths::Integer::Short::Point2D*>(WEAPONS[i])->Y = 0;
			}
			else if (*configArray[k] == 1 || *configArray[k] < 0 || *configArray[k] > 65535)
			{
				reinterpret_cast<Blam::Maths::Integer::Short::Point2D*>(WEAPONS[i])->X = defaultSize[k];
				reinterpret_cast<Blam::Maths::Integer::Short::Point2D*>(WEAPONS[i])->Y = defaultSize[++k];
			}
			else
			{
				reinterpret_cast<Blam::Maths::Integer::Short::Point2D*>(WEAPONS[i])->X = *configArray[k];
				reinterpret_cast<Blam::Maths::Integer::Short::Point2D*>(WEAPONS[i])->Y = *configArray[++k];
			}
			k++;
		}
	}
}

void UI::Tweaks::SetCrosshairPos(float crosshair_offset) 
{
	if (h2mod->Server)
		return;

	if (!FloatIsNaN(crosshair_offset)) 
	{
		DWORD CrosshairY = *reinterpret_cast<DWORD*>(h2mod->GetBase() + 0x479E70) + 0x1AF4 + 0xF0 + 0x1C;
		*reinterpret_cast<float*>(CrosshairY) = crosshair_offset;
	}
}
