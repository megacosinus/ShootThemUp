// Shoot Them Up Game. All Rights Reserved

#include "Pickups/STUAmmoPickup.h"

DEFINE_LOG_CATEGORY_STATIC(LogAmmoPickup, All, All);

bool ASTUAmmoPickup::GivePickUpTo(APawn* PlayerPawn)
{
    UE_LOG(LogAmmoPickup, Display, TEXT("Ammo was taken"));
    return true;
}