#include "stdafx.h"

#include "VariantSystem.h"

#include "Variants.h"
#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"

ICustomGameVariant* currentRunningVariant;
std::vector<ICustomGameVariant*> CustomVariantHandler::customVariants;

std::unordered_map<const wchar_t*, CustomVariantId> customGameVarStr =
{
	{ L"infection",		CustomVariantId::_id_infection	},
	{ L"zombies",		CustomVariantId::_id_infection	},
	{ L"gungame",		CustomVariantId::_id_gungame	},
	{ L"headhunter",	CustomVariantId::_id_graverobber },
	{ L"graverobber",	CustomVariantId::_id_graverobber },
	{ L"firefight",		CustomVariantId::_id_firefigth	},
	{ L"wareconomy",	CustomVariantId::_id_firefigth	},
};

void CustomVariantHandler::RegisterCustomVariants()
{
	customVariants.push_back(new Infection());
	customVariants.push_back(new GunGame());
	customVariants.push_back(new GraveRobber());
	customVariants.push_back(new FireFight());
}

ICustomGameVariant* CustomVariantHandler::GetGameVariant(CustomVariantId variantId)
{
	for (auto& variant : customVariants)
	{
		if (variant->GetVariantId() == variantId)
			return variant;
	}

	return nullptr;
}

ICustomGameVariant* CustomVariantHandler::GetCurrentGameVariant()
{
	return currentRunningVariant;
}

void CustomVariantHandler::GameVarianEnable(const wchar_t* variant)
{
	CustomVariantId variantId = _id_unknown;

	for (auto& varIds : customGameVarStr)
	{
		if (StrStrIW(variant, varIds.first))
		{
			variantId = varIds.second;
			break;
		}
	}

	currentRunningVariant = GetGameVariant(variantId);
}

bool CustomVariantHandler::ContainsGameVariant(const wchar_t* variant, CustomVariantId variantId)
{
	for (auto& varIds : customGameVarStr)
	{
		if (StrStrIW(variant, varIds.first) 
			&& varIds.second == variantId)
		{
			return true;
		}
	}

	return false;
}

void CustomVariantHandler::DisposeGameVariant()
{
	if (currentRunningVariant != nullptr)
		currentRunningVariant->Dispose();
	currentRunningVariant = nullptr;
}

bool CustomVariantHandler::VariantEnabled(CustomVariantId variantId)
{
	if (GetCurrentGameVariant() != nullptr)
		return GetCurrentGameVariant()->GetVariantId() == variantId;

	return false;
}

void CustomVariantHandler::OnMapLoad(ExecTime execTime, s_game_options* gameOptions)
{
	if (GetCurrentGameVariant() != nullptr)
		GetCurrentGameVariant()->OnMapLoad(execTime, gameOptions);
}

void CustomVariantHandler::OnPlayerSpawn(ExecTime execTime, datum playerIdx)
{
	if (GetCurrentGameVariant() != nullptr)
		GetCurrentGameVariant()->OnPlayerSpawn(execTime, playerIdx);
}

void CustomVariantHandler::OnPlayerDeath(ExecTime execTime, datum playerIdx)
{
	if (GetCurrentGameVariant() != nullptr)
		GetCurrentGameVariant()->OnPlayerDeath(execTime, playerIdx);
}

void CustomVariantHandler::OnObjectDamage(ExecTime execTime, datum unitDatumIdx, int a2, bool a3, bool a4)
{
	if (GetCurrentGameVariant() != nullptr)
		GetCurrentGameVariant()->OnObjectDamage(execTime, unitDatumIdx, a2, a3, a4);
}

bool CustomVariantHandler::OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx)
{
	if (GetCurrentGameVariant() != nullptr)
		return GetCurrentGameVariant()->OnAutoPickupHandler(execTime, playerIdx, objectIdx);

	return false;
}

bool CustomVariantHandler::OnPlayerScore(ExecTime execTime, void* thisptr, datum playerIdx, int a3, int a4, int a5, char a6)
{
	if (GetCurrentGameVariant() != nullptr)
		return GetCurrentGameVariant()->OnPlayerScore(execTime, thisptr, playerIdx, a3, a4, a5, a6);

	return false;
}