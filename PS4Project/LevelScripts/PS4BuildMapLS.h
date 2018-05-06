// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "PS4BuildMapLS.generated.h"


// Fwd Decl.
class UPlayerStatsUserWidget;

/**
 * Base class of the Exploration and Interaction Test Room Level Blueprint.
 *	This class provides members and functions to be used as part of the PS4BuildMap level.
 *	Uses a lot of the same logic from EI_TestRoomLS.h/cpp (migrated code)
 */
UCLASS()
class WHITEBOXPROJECT_API APS4BuildMapLS : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	//Hound class that needs to be set in the editor.
	UPROPERTY(EditAnywhere, Category = "Spawnable Entities", DisplayName = "Hound")
	TSubclassOf<AActor> m_pcHoundClass = nullptr;

	//FireCrystal class that needs to be set in the editor.
	UPROPERTY(EditAnywhere, Category = "Spawnable Entities", DisplayName = "Fire Crystals")
	TSubclassOf<AActor> m_pcFireCrystalClass = nullptr;

	//Function to demo ability of trigger
	UFUNCTION(BlueprintCallable, Category = "LevelScript")
	void NPCTriggerIceGainSpawns();

	//Spawn a hound with the given transform
	//Param: Transform to spawn at
	UFUNCTION(BlueprintCallable, Category = "LevelScript")
	void SpawnHound(FTransform& rsTransform);

	//Spawn a fire crystal with the given transform
	//Param: Transform to spawn at
	UFUNCTION(BlueprintCallable, Category = "LevelScript")
	void SpawnFireCrystal(FTransform& rsTransform);

	//Trigger character to equip ability
	//Param: ability to set on character
	UFUNCTION(BlueprintCallable, Category = "LevelScript")
	void SetCharacterAbilityType(EElementAttribute eAbilityType);

	// Changes the element icon on the hud
	// Param1: The element to change the HUD to
	void SetHUDElement( EElementAttribute eElement );

	//Array of transforms that can be added to level blueprint for spawn locations.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPCIceTrigger", DisplayName = "Hound Spawn Transforms")
	TArray<FTransform> m_asHoundTransforms;

	//Array of transforms that can be added to level blueprint for spawn locations.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPCIceTrigger", DisplayName = "Fire Crystal Spawn Transforms")
	TArray<FTransform> m_asCrystalTransforms;

private:
	//TODO - Memory handling in the current level for level spawned actors as we do not have a fully tested memory management system.
	UPROPERTY()
	TArray<AActor*> m_apcSpawnedActors;

	UPROPERTY ( )
	UPlayerStatsUserWidget*		m_pPlayerStatsWidget;   // Player stats widget

protected:

	//Override BeginPlay method to add menu widget in it
	virtual void BeginPlay ( ) override;

};
