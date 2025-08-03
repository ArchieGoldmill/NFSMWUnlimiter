#pragma once

namespace PartLinker
{
	typedef unsigned int Hash;

	inline CAR_SLOT_ID KitwSlots[] = { CAR_SLOT_ID::DAMAGE_FRONT_BUMPER, CAR_SLOT_ID::DAMAGE_REAR_BUMPER, CAR_SLOT_ID::DAMAGE_BUSHGUARD, CAR_SLOT_ID::DAMAGE_LEFT_HEADLIGHT, CAR_SLOT_ID::DAMAGE_RIGHT_HEADLIGHT, CAR_SLOT_ID::HOOD, CAR_SLOT_ID::SPOILER };

	inline Hash FromIndex(const char* str, int index)
	{
		char buff[128];
		sprintf(buff, str, index);
		return bStringHash(buff);
	}

	template<typename T>
	struct Attribute
	{
		Hash Name;
		T Value;
	};

	struct CarPart
	{
		int GetAppliedAttributeIParam(Hash attributeHash, int valueIfNotFound)
		{
			auto _GetAppliedAttributeIParam = (int(__thiscall*)(void*, unsigned int, int))0x747AC0;
			return _GetAppliedAttributeIParam(this, attributeHash, valueIfNotFound);
		}

		template<typename T>
		Attribute<T>* GetAttribute(Hash hash)
		{
			static auto _GetAttribute = (void* (__thiscall*)(CarPart*, Hash, int))0x00739730;
			return (Attribute<T>*)_GetAttribute(this, hash, 0);
		}

		bool HasKitW(int kitw)
		{
			for (int i = 0; i < 99; i++)
			{
				auto attr = this->GetAttribute<int>(FromIndex("KITW_%d", i));
				if (!attr)
				{
					if (i == 0 && kitw == 0)
					{
						return true;
					}

					break;
				}

				if (attr->Value == kitw)
				{
					return true;
				}
			}

			return false;
		}
	};

	struct RideInfo
	{
		int Type;

		CarPart* GetPart(CAR_SLOT_ID slot)
		{
			static auto _GetPart = (CarPart * (__thiscall*)(RideInfo*, CAR_SLOT_ID))0x00739C70;
			return _GetPart(this, slot);
		}

		void SetPart(CAR_SLOT_ID slot, CarPart* part, bool updatePartsEnabled = true)
		{
			static auto _SetPart = (void(__thiscall*)(RideInfo*, CAR_SLOT_ID, CarPart * part, bool updatePartsEnabled))0x00756E90;
			_SetPart(this, slot, part, updatePartsEnabled);
		}
	};

	struct CarPartDatabase
	{
		static inline auto Instance = (CarPartDatabase*)_CarPartDB;

		CarPart* NewGetCarPart(int CarTypeID, CAR_SLOT_ID CarSlotID, Hash CarPartNameHash, CarPart* PreviousPart, int UpgradeLevel = -1)
		{
			auto _NewGetCarPart = (CarPart * (__thiscall*)(CarPartDatabase*, int, CAR_SLOT_ID, Hash, CarPart*, int))0x00747C40;
			return _NewGetCarPart(this, CarTypeID, CarSlotID, CarPartNameHash, PreviousPart, UpgradeLevel);
		}

		CarPart* GetByKitW(CAR_SLOT_ID slot, int carId, int kitw)
		{
			CarPart* part = 0;
			while (true)
			{
				part = this->NewGetCarPart(carId, slot, 0, part);
				if (!part)
				{
					break;
				}

				if (part->HasKitW(kitw))
				{
					return part;
				}
			}

			return NULL;
		}
	};

	void InstallBodyPart(RideInfo* rideInfo, CAR_SLOT_ID slot, int kit)
	{
		auto carId = rideInfo->Type;

		auto part = CarPartDatabase::Instance->GetByKitW(slot, carId, kit);
		if (!part)
		{
			kit = 0;
		}

		auto installedPart = rideInfo->GetPart(slot);
		if (installedPart && installedPart->HasKitW(kit))
		{
			return;
		}

		if (!part)
		{
			part = CarPartDatabase::Instance->GetByKitW(slot, carId, 0);
		}

		rideInfo->SetPart(slot, part);
	}

	void LinkBodyParts(DWORD* pRideInfo, unsigned int carPart, int kitNumber)
	{
		auto rideInfo = (RideInfo*)pRideInfo;
		auto part = (CarPart*)carPart;

		for (auto kiwSlot : KitwSlots)
		{
			InstallBodyPart(rideInfo, kiwSlot, kitNumber);
		}
	}
};