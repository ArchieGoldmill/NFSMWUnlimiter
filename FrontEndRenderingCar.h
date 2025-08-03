#pragma once

#include "stdio.h"
#include "InGameFunctions.h"

#pragma pack(push, 1)
typedef struct FrontEndRenderingCar
{
    /* 0x0000 */ FrontEndRenderingCar* Next;
    /* 0x0004 */ FrontEndRenderingCar* Prev;
    /* 0x0008 */ DWORD mRideInfo[0xC4];
    /* 0x0318 */ DWORD* RenderInfo;
    /* 0x031C */ int ViewID;
    /* 0x0320 */ bVector3 Position;
    /* 0x0330 */ bMatrix4 BodyMatrix;
    /* 0x0370 */ bMatrix4 TireMatrices[4];
    /* 0x0470 */ bMatrix4 BrakeMatrices[4];
    /* 0x0570 */ DWORD* OverrideModel;
    /* 0x0574 */ int Visible;
    /* 0x0578 */ int nPasses;
    /* 0x057C */ int Reflection;
    /* 0x0580 */ int LightsOn;
    /* 0x0584 */ int CopLightsOn;
    /* 0x0588 */ float ExtraPitch;
    /* 0x058C */ BYTE padding[4];
} FrontEndRenderingCar; /* size: 0x0590 */
#pragma pack(pop)

bool __fastcall FrontEndRenderingCar_LookupWheelPosition(FrontEndRenderingCar* _car, void* EDX_Unused, unsigned int index, bVector4 *position)
{
    DWORD* _CarRenderInfo; // eax
    DWORD* TheRideInfo;
    DWORD* CarPart;
    float* AttrPtr; // eax
    float FECompression = 0; // st7
    float ExtraPitch = 0; // st7

    _CarRenderInfo = _car->RenderInfo;
    if (!_CarRenderInfo || !position)
        return 0;

    // Write tire offsets
    Attrib_Gen_ecar_LayoutStruct* eCarAttributes = (Attrib_Gen_ecar_LayoutStruct*)_CarRenderInfo[1517];
    bVector4 TireOffsets = eCarAttributes->TireOffsets[index];
    position->x = TireOffsets.x; // x
    position->y = TireOffsets.y; // y
    position->z = TireOffsets.z; // z
    position->w = TireOffsets.w; // w
    
    // Handle FECompressions
    AttrPtr = (float*)Attrib_Instance_GetAttributePointer((DWORD*)(_CarRenderInfo + 1515), 0x8D5BEB72, index >> 1); // FECompressions
    if (!AttrPtr)
        AttrPtr = (float*)Attrib_DefaultDataArea();
    FECompression = *AttrPtr;

    // Get value from custom attribute

    TheRideInfo = (DWORD*)_CarRenderInfo[33]; // CarRenderInfo->pRideInfo
    if (TheRideInfo)
    {
        CarPart = RideInfo_GetPart(TheRideInfo, CAR_SLOT_ID::DAMAGE_LEFT_BRAKELIGHT); // BODY_KIT
        if (CarPart)
        {
            // Read offset attributes from rear fender
            FECompression = CarPart_GetAppliedAttributeFParam(CarPart, 0, index >> 1 ? bStringHash((char*)"REAR_FECOMPRESSION") : bStringHash((char*)"FRONT_FECOMPRESSION"), FECompression);
        }
    }

    TheRideInfo = (DWORD*)_CarRenderInfo[33]; // CarRenderInfo->pRideInfo
    if (TheRideInfo)
    {
        CarPart = RideInfo_GetPart(TheRideInfo, CAR_SLOT_ID::DAMAGE_LEFT_HEADLIGHT); // BODY_KIT
        if (CarPart)
        {
            // Read offset attributes from front fender
            FECompression = CarPart_GetAppliedAttributeFParam(CarPart, 0, index >> 1 ? bStringHash((char*)"REAR_FECOMPRESSION") : bStringHash((char*)"FRONT_FECOMPRESSION"), FECompression);
        }
    }
    // Write FECompression
    position->w = 1.0;
    position->z += FECompression;

    
    // Handle ExtraPitch
    AttrPtr = (float*)Attrib_Instance_GetAttributePointer((DWORD*)(_CarRenderInfo + 1515), 0xE95257C2, 0); // ExtraPitch
    if (!AttrPtr)
        AttrPtr = (float*)Attrib_DefaultDataArea();
    ExtraPitch = *AttrPtr;

    if (TheRideInfo && CarPart)
        ExtraPitch = CarPart_GetAppliedAttributeFParam(CarPart, 0, bStringHash((char*)"EXTRA_PITCH"), ExtraPitch);

    _car->ExtraPitch = ExtraPitch;
    
    // Calculate wheel pos with ExtraPitch
    float pi = *(float*)_pi;
    float ExtraPitchCalculated = tan(ExtraPitch * pi / 180.0f) * ((eCarAttributes->TireOffsets[0].x - eCarAttributes->TireOffsets[2].x) * 0.5f);
    if (index > 1) ExtraPitchCalculated = -ExtraPitchCalculated;

    // Write ExtraPitch
    position->w = 1.0;
    position->z += ExtraPitchCalculated;
    
    return 1;
}

bool __fastcall FrontEndRenderingCar_LookupWheelRadius(FrontEndRenderingCar* _car, void* EDX_Unused, unsigned int index, bVector4* position)
{
    DWORD* _CarRenderInfo; // eax
    DWORD* TheRideInfo;
    DWORD* BodyKitCarPart;
    float Radius = 0; // st7

    _CarRenderInfo = _car->RenderInfo;
    if (!_CarRenderInfo || !position)
        return 0;

    // Write tire offsets
    Attrib_Gen_ecar_LayoutStruct* eCarAttributes = (Attrib_Gen_ecar_LayoutStruct*)_CarRenderInfo[1517];
    bVector4 TireOffsets = eCarAttributes->TireOffsets[index];
    Radius = TireOffsets.w;

    // Get value from custom attribute
  

    TheRideInfo = (DWORD*)_CarRenderInfo[33]; // CarRenderInfo->pRideInfo
    if (TheRideInfo)
    {
        BodyKitCarPart = RideInfo_GetPart(TheRideInfo, CAR_SLOT_ID::DAMAGE_LEFT_BRAKELIGHT); 
        if (BodyKitCarPart)
        {
            
            Radius = CarPart_GetAppliedAttributeFParam(BodyKitCarPart, 0, index >> 1 ? bStringHash((char*)"REAR_TIRE_RADIUS") : bStringHash((char*)"FRONT_TIRE_RADIUS"), Radius);
        }
    }

    TheRideInfo = (DWORD*)_CarRenderInfo[33]; // CarRenderInfo->pRideInfo
    if (TheRideInfo)
    {
        BodyKitCarPart = RideInfo_GetPart(TheRideInfo, CAR_SLOT_ID::DAMAGE_LEFT_HEADLIGHT); 
        if (BodyKitCarPart)
        {
            
            Radius = CarPart_GetAppliedAttributeFParam(BodyKitCarPart, 0, index >> 1 ? bStringHash((char*)"REAR_TIRE_RADIUS") : bStringHash((char*)"FRONT_TIRE_RADIUS"), Radius);
        }
    }

    // Write radius
    position->x = Radius;

    return 1;
}
