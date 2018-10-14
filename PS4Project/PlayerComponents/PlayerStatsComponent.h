// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatsComponent.generated.h"

//fwd decls
class UPlayerStatsUserWidget;
class UUserWidget;

// Declare an event for upgrade health/energy complete
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FUpgradeEvent_UpgradeComplete );

/*
*	UplayerStatsComponent is an actor component for the player to handle and assign various 
*	statistics (e.g. health, energy and upgrades) instead of handling all this logic in the player
*	class directly.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOES_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerStatsComponent();

	// The event used to fire an upgrade complete event
	UPROPERTY( BlueprintAssignable, BlueprintCallable, Category = "Upgrade Events" )
	FUpgradeEvent_UpgradeComplete OnUpgradeComplete;

	// Called every frame
	virtual void TickComponent( float fDeltaTime, ELevelTick eTickType, FActorComponentTickFunction* psThisTickFunction ) override;

	//////////////////////////////////////////////////////////////////////////
	// Player Stat accessor functions
	//////////////////////////////////////////////////////////////////////////

	// Function that returns the max amount of Health the character has
	// Return: float value of the max amount of Health
	UFUNCTION( BlueprintPure, Category = "Health" )
	float GetMaxHealth() { return m_fMaxHealth; };

	// Set the maximum health of the player
	// Param1: The new maximum health of the player character
	void SetMaxHealth( float fMaxHealth );

	// Upgrade max health by the amount given
	void UpgradeMaxHealth();

	// Function that returns the max amount of Energy the character has
	// Return: float value of the max amount of Energy
	UFUNCTION( BlueprintPure, Category = "Energy" )
	float GetMaxEnergy() { return m_fMaxEnergy; };

	// Set the maximum energy of the player
	// Param1: The new maximum energy of the player character
	void SetMaxEnergy( float fMaxEnergy );

	// Upgrade max energy by the amount given
	void UpgradeMaxEnergy();

	// Function that returns the current amount of health the character has
	UFUNCTION( BlueprintPure, Category = "Health" )
	float GetPlayerCurrentHealth() { return m_fCurrentHealth; };

	// Accessor function for current energy 
	UFUNCTION( BlueprintPure, Category = "Energy" )
	float GetPlayerCurrentEnergy() { return m_fCurrentEnergy; }

	// Accessor function for energy dash cost 
	float GetPlayerDashCost() { return m_fEnergyDashCost; }

	// Accessor function for energy attack cost 
	UFUNCTION ( BlueprintCallable )
	float GetPlayerAttackCost() { return m_fEnergyAttackCost; }

	// Accessor function for energy charged attack cost 
	UFUNCTION ( BlueprintCallable )
	float GetPlayerChargedAttackCost ( ) { return m_fEnergyChargedAttackCost; }

	// Accessor function for energy jump cost 
	float GetPlayerJumpCost ( ) { return m_fEnergyJumpCost; }

	// Accessor function for energy double jump cost 
	float GetPlayerDoubleJumpCost ( ) { return m_fEnergyDoubleJumpCost; }

	// Check if the player has died
	bool IsPlayerDead();

	//Getter for player damage
	float GetPlayerDamage() { return m_fPlayerDamage; };

	//Getter for damage multiplier
	//@Param1: which multiplier to get [1-3] (logs on invalid param & returns 0)
	float GetDamageMultiplier( int iMultiplierNumber );

	UFUNCTION(BlueprintCallable)
	void SetMeleeAttackCost(float fNewAttackCost);

	//Setter for player damage
	void SetPlayerDamage( float fNewPlayerDamage );

	//Setter for damage multiplier
	//@Param1: new damage value
	//@Param2: which multiplier to set [1-3] (logs on invalid param & returns 0)
	void SetDamageMultiplier( float fNewValue, int iMultiplierNumber );

	//////////////////////////////////////////////////////////////////////////

	// Increase the amount of skill points that the player has
	// Param1: The amount of skill points to add to the players total
	void AddResources( int iResourceAmount );

	// Return the amount of resources the player currently has
	// Return: The amount of resources that the player has
	UFUNCTION(BlueprintCallable, Category = "Resources")
	int GetResourceAmount() { return m_iResourceAmount; };

	// Return the amount of resources required to upgrade a stat
	// Return: The amount of resources required to upgrade a stat
	int GetUpgradeRequired() { return m_iUpgradeRequirement; };

	// Accessor function for adding energy
	void AddHealth( float fValueToAdd );

	// Accessor function for adding energy
	UFUNCTION ( BlueprintCallable )
	void AddEnergy( float fValueToAdd );

	//Function to reset in game stats (e.g. health and stamina after death)
	void LiveStatReset();

	//////////////////////////////////////////////////////////////////////////
	// Upgrade Counters - TODO: Leaving in public as sorting merge - change access
	//////////////////////////////////////////////////////////////////////////

	// Current Upgrade Counters 

	// Used to store information about current Health Upgrade count 
	int m_iHealthCurrentUpgradeCount;

	// Used to store information about current Energy Upgrade count
	int m_iEnergyCurrentUpgradeCount;

	// Used to store information about current Pickaxe Damage Upgrade count
	int m_iPickaxeDamageCurrentUpgradeCount;

	// Max Upgrade Counters
	// Used to store information about current Health Upgrade count 
	int m_iHealthMaxUpgradeCount;

	// Used to store information about current Energy Upgrade count
	int m_iEnergyMaxUpgradeCount;

	// Used to store information about current Pickaxe Damage Upgrade count
	int m_iPickaxeDamageMaxUpgradeCount;

	//////////////////////////////////////////////////////////////////////////

	// The duration for which the player will be invulnerable after the attack
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Designer Values|Invulnerable" , DisplayName = "Invulnerable Duration" )
	float m_fInvulnerableDuration;

	// Check if the damage is coming from the Corruption - To stop Invulnerability whilst in Corruption
	UPROPERTY()
	bool m_bIsOverlappingCorruption;

	// Setter method for Overlap status with Corruption
	void SetCorruptionOverlapping ( bool bOverlapStatus ) { m_bIsOverlappingCorruption = bOverlapStatus; };

	// Getter method for returning Overlap status with Corruption
	bool GetCorruptionOverlapping () { return m_bIsOverlappingCorruption; };

	//getter method for player invulnerable
	UFUNCTION ( BlueprintPure , Category = "Invulnerability" )
	bool IsPlayerInvulnerable ( ) { return m_bIsPlayerInvulnerable; }

	//setter method for setting player invulnerability
	void SetPlayerInvulnerability ( bool bIsInvulnerable ) { m_bIsPlayerInvulnerable = bIsInvulnerable; }

	//setter method for setting the current invulnerability duration
	void SetCurrentInvulnerabilityDuration ( float fDuration ) { m_fCurrentInvulnerabilityDuration = fDuration; }

	//getter method for getting the current invulnerability duration left
	UFUNCTION ( BlueprintPure , Category = "Invulnerability" )
	float GetCurrentInvulnerabilityDuration ( ) { return m_fCurrentInvulnerabilityDuration; }

	//method to decrement the current invulnerability duration
	void DecrementInvulnerabilityDuration ( float fDelta ) { m_fCurrentInvulnerabilityDuration -= fDelta; }

	//getter for the double jump force
	float GetDoubleJumpForce() { return m_fDoubleJumpForce; }

	//The bp widget class for our Master HUD
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets", DisplayName = "HUD Class" )
	TSubclassOf<UPlayerStatsUserWidget> m_pcHUDClass;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets", DisplayName = "Upgrade Prompt" )
	TSubclassOf<UUserWidget> m_pcUpgradePromptClass;

private:
	//The bp widget instance for our Master HUD
	UPROPERTY()
	UPlayerStatsUserWidget* m_pcHUDInstance;

	UPROPERTY( Transient )
	UUserWidget* m_pcUpgradePromptInst;

	//////////////////////////////////////////////////////////////////////////
	// Player Bar Stat Values
	//////////////////////////////////////////////////////////////////////////

	float m_fEnergyDoubleDashCost;
	
	float m_fMeleeAttackCost;

	// Variable is the max amount of health the character has 
	UPROPERTY( EditAnywhere, Category = "Designer Values|Health", DisplayName = "Max Health" )
	float m_fMaxHealth;

	// Variable is the Current health amount the character has
	UPROPERTY( VisibleAnywhere, Category = "Designer Values|Health", DisplayName = "Current Health" )
	float m_fCurrentHealth;

	// The original hp value - before modifications
	float m_fBaseHealth;

	// Variable for the Current Energy the player has
	UPROPERTY( VisibleAnywhere, Category = "Designer Values|Energy", DisplayName = "Current Energy" )
	float m_fCurrentEnergy;

	// Variable for the Maximum Energy the player can have
	UPROPERTY( EditAnywhere, Category = "Designer Values|Energy", DisplayName = "Max Energy" )
	float m_fMaxEnergy;

	// The original energy value - before modifications
	float m_fBaseEnergy;

	// The amount of health to add on upgrade
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Upgrades", DisplayName = "Health Upgrade Amount" )
	float m_fHealthUpgrade;

	// The amount of stamina to add on upgrade
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Upgrades", DisplayName = "Energy Upgrade Amount" )
	float m_fEnergyUpgrade;

	//////////////////////////////////////////////////////////////////////////
	// Health based variables
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Health", DisplayName = "Health Regen Rate (Per Second)" )
	float m_fHealthRegenRate;

	// The amount of health to regen up to
	float m_fRegenToHealth;

	//////////////////////////////////////////////////////////////////////////
	// Energy Based variables
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY( EditAnywhere, Category = "Designer Values|Energy", DisplayName = "Energy Regen Rate" )
	float m_fEnergyAmountToRegen;

	UPROPERTY( EditAnywhere, Category = "Designer Values|Energy", DisplayName = "Dash Energy Cost" )
	float m_fEnergyDashCost;

	UPROPERTY( EditAnywhere, Category = "Designer Values|Energy", DisplayName = "Attack Energy Cost" )
	float m_fEnergyAttackCost;

	UPROPERTY ( EditAnywhere , Category = "Designer Values|Energy" , DisplayName = "Charged Attack Energy Cost" )
	float m_fEnergyChargedAttackCost;

	UPROPERTY ( EditAnywhere , Category = "Designer Values|Energy" , DisplayName = "Jump Energy Cost" )
	float m_fEnergyJumpCost;

	UPROPERTY ( EditAnywhere , Category = "Designer Values|Energy" , DisplayName = "Double Jump Energy Cost" )
	float m_fEnergyDoubleJumpCost;

	//////////////////////////////////////////////////////////////////////////
	// Combat related designer stat variables
	//////////////////////////////////////////////////////////////////////////

	// players damage variable
	UPROPERTY( EditAnywhere, Category = "Designer Values|Melee Attack", DisplayName = "Player Base Damage" )
	float m_fPlayerDamage;

	// first attack damage multiplier
	UPROPERTY( EditAnywhere, Category = "Designer Values|Melee Attack|Multipliers", DisplayName = "First Attack Damage Multiplier" )
	float m_fAttackOneDamageMultiplier;

	// secondary attack damage multiplier
	UPROPERTY( EditAnywhere, Category = "Designer Values|Melee Attack|Multipliers", DisplayName = "Second Attack Damage Multiplier" )
	float m_fAttackTwoDamageMultiplier;

	// Tertiary attack damage multiplier
	UPROPERTY( EditAnywhere, Category = "Designer Values|Melee Attack|Multipliers", DisplayName = "Third Attack Damage Multiplier" )
	float m_fAttackThreeDamageMultiplier;


	//////////////////////////////////////////////////////////////////////////
	// Skill point based variables
	//////////////////////////////////////////////////////////////////////////

	// The amount of skill points that the player has
	int m_iResourceAmount;

	// The max amount of resources that need to be collected before an upgrade is possible
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Resource Requirement" )
	int m_iUpgradeRequirement;

	//boolean to check weather player is invulnerable 
	bool m_bIsPlayerInvulnerable;

	//current vulnerability duration for which the player will be invulnerable
	float m_fCurrentInvulnerabilityDuration;

	//////////////////////////////////////////////////////////////////////////
	// Jump-related variables variables
	//////////////////////////////////////////////////////////////////////////

	//How much force is applied for the second jump
	UPROPERTY( EditAnywhere, Category = "Designer Values|Jump", DisplayName = "Double Jump Force" )
	float m_fDoubleJumpForce;

	// Called on tick to update the players stamina
	// Param1: The time since the last tick of the game loop
	void UpdateStamina( float fDeltaTime );

	// Called on tick if needed to regen some health on player
	// Param1: The time since the last tick of the game loop
	void RegenHealth( float fDeltaTime );
};
