#pragma once
enum class ReceiverCategories
{
	kNone = 0,
	kScene = 1 << 0,
	kPlayerAircraft = 1 << 1,
	kAlliedAircraft = 1 << 2,
	kEnemyAircraft = 1 << 3,
	kPlayerAircraft2 = 1 << 4,
	kAllPlayers = kPlayerAircraft + kPlayerAircraft2,
	kSoundEffect = 1 << 8,
	kNetwork = 1 << 9,
};

//A message that would be sent to all aircraft
//unsigned int all_aircraft = ReceiverCategories::kPlayerAircraft | ReceiverCategories::kAlliedAircraft | ReceiverCategories::kEnemyAircraft;