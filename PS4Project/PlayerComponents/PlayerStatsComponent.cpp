// Fill out your copyright notice in the Description page of Project Settings.

//Main include
#include "PlayerStatsComponent.h"

//Inclusive includes
#include "Kismet/GameplayStatics.h"
#include "UISystems/HUD/PlayerStatsUserWidget.h"
#include "WidgetBlueprintLibrary.h"

// Sets default values for this component's properties
UPlayerStatsComponent::UPlayerStatsComponent()
: m_iHealthCurrentUpgradeCount			( 0 )
, m_iEnergyCurrentUpgradeCount			( 0 )
, m_iPickaxeDamageCurrentUpgradeCount	( 0 )
, m_iHealthMaxUpgradeCount				( 10 )
, m_iEnergyMaxUpgradeCount				( 10 )
, m_iPickaxeDamageMaxUpgradeCount		( 10 )
, m_fInvulnerableDuration				( 2.0f )
, m_bIsOverlappingCorruption			( false )
, m_pcHUDInstance						( nullptr )
, m_pcHUDClass							( nullptr )
, m_fEnergyDoubleDashCost				( 5.f )
, m_fMeleeAttackCost					( 5.0f )
, m_fMaxHealth							( 100.f )
, m_fCurrentHealth						( 0.0f )
, m_fBaseHealth							( 0.0f )
, m_fCurrentEnergy						( 0.0f )
, m_fMaxEnergy							( 100.f )
, m_fBaseEnergy							( 0.f )
, m_fHealthUpgrade						( 25.f )
, m_fEnergyUpgrade						( 25.f )
, m_fHealthRegenRate					( 5.f )
, m_fRegenToHealth						( 0.f )
, m_fEnergyAmountToRegen				( 5.f )
, m_fEnergyDashCost						( 10.f )
, m_fEnergyAttackCost					( 5.0f )
, m_fEnergyChargedAttackCost            ( 6.0f )
, m_fEnergyJumpCost						( 2.0f )
, m_fEnergyDoubleJumpCost				( 15.0f )
, m_fPlayerDamage						( 10.0f )
, m_fAttackOneDamageMultiplier			( 1.f )
, m_fAttackTwoDamageMultiplier			( 1.f )
, m_fAttackThreeDamageMultiplier		( 1.f )
, m_iResourceAmount						( 0 )
, m_iUpgradeRequirement					( 5 )
, m_bIsPlayerInvulnerable               ( false )
, m_fCurrentInvulnerabilityDuration     ( 0.0f )
, m_fDoubleJumpForce					( 800.0f )
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Set the base health to max
	m_fBaseHealth = m_fMaxHealth;

	// Set the base energy to max
	m_fBaseEnergy = m_fMaxEnergy;

	// Initialize the current health to the maximum amount of health
	m_fCurrentHealth = m_fMaxHealth;

	// Initialize the current energy to the maximum amount of energy
	m_fCurrentEnergy = m_fMaxEnergy;

	// Initalise the regen health to current health
	m_fRegenToHealth = m_fCurrentHealth;
}

// Called when the game starts
void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	//check and show the player stats at top
	if (!m_pcHUDInstance)
	{
		m_pcHUDInstance = Cast<UPlayerStatsUserWidget>( UWidgetBlueprintLibrary::Create( this, m_pcHUDClass, GetWorld()->GetFirstPlayerController() ) );
	}

	//Add player stats widget on the screen
	if (m_pcHUDInstance)
	{
		m_pcHUDInstance->AddToViewport();
	}
}

void UPlayerStatsComponent::TickComponent( float fDeltaTime, ELevelTick eTickType, FActorComponentTickFunction * psThisTickFunction )
{
	Super::TickComponent( fDeltaTime, eTickType, psThisTickFunction );

	//While player is alive, continue to update stamina
	if ( !IsPlayerDead() ) 
	{
		if( m_fCurrentEnergy < m_fMaxEnergy )
		{
			UpdateStamina( fDeltaTime );
		}
		if( m_fCurrentHealth < m_fRegenToHealth )
		{
			RegenHealth( fDeltaTime );
		}
	}
}

void UPlayerStatsComponent::SetMaxHealth( float fMaxHealth )
{
	m_fMaxHealth = fMaxHealth;
	if ( m_fCurrentHealth > fMaxHealth )
	{
		m_fCurrentHealth = fMaxHealth;
	}
}

//////////////////////////////////////////////////////////////////////////
// Upgrade max health by the amount given
//////////////////////////////////////////////////////////////////////////
void UPlayerStatsComponent::UpgradeMaxHealth()
{
	// Detract cost and update health 
	m_iResourceAmount -= m_iUpgradeRequirement;

	SetMaxHealth( GetMaxHealth() + m_fHealthUpgrade );
	AddHealth( GetMaxHealth() );

	float fBarSizeIncrease =  m_fHealthUpgrade / m_fBaseHealth;
	fBarSizeIncrease += 1.0f;
	m_pcHUDInstance->ResizeHealthBar( fBarSizeIncrease );

	// Broadcast upgrade complete event
	OnUpgradeComplete.Broadcast();
}

void UPlayerStatsComponent::SetMaxEnergy( float fMaxEnergy )
{
	m_fMaxEnergy = fMaxEnergy;
	if ( m_fCurrentEnergy > fMaxEnergy )
	{
		m_fCurrentEnergy = fMaxEnergy;
	}
}

//////////////////////////////////////////////////////////////////////////
// Upgrade max energy by the amount given
//////////////////////////////////////////////////////////////////////////
void UPlayerStatsComponent::UpgradeMaxEnergy()
{
	// Detract cost and update energy
	m_iResourceAmount -= m_iUpgradeRequirement;

	SetMaxEnergy( GetMaxEnergy() + m_fEnergyUpgrade );
	AddEnergy( GetMaxEnergy() );

	float fBarSizeIncrease = m_fEnergyUpgrade / m_fBaseEnergy;
	fBarSizeIncrease += 1.0f;
	m_pcHUDInstance->ResizeEnergyBar( fBarSizeIncrease );

	// Broadcast upgrade complete event
	OnUpgradeComplete.Broadcast();
}

bool UPlayerStatsComponent::IsPlayerDead()
{
	return (m_fCurrentHealth <= 0.0f);
}

float UPlayerStatsComponent::GetDamageMultiplier( int iMultiplierNumber )
{
	float fReturnVal = 0.0f;

	switch (iMultiplierNumber)
	{
	case 1:
		fReturnVal = m_fAttackOneDamageMultiplier;
		break;
	case 2:
		fReturnVal = m_fAttackTwoDamageMultiplier;
		break;
	case 3:
		fReturnVal = m_fAttackThreeDamageMultiplier;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Could not get damage multiplier as invalid argument was passed, need values: [1-3]"));
	}

	return fReturnVal;
}

void UPlayerStatsComponent::SetMeleeAttackCost( float fNewAttackCost )
{
	m_fMeleeAttackCost = fNewAttackCost;
}

void UPlayerStatsComponent::SetPlayerDamage( float fNewPlayerDamage )
{
	m_fPlayerDamage = fNewPlayerDamage;
}

void UPlayerStatsComponent::SetDamageMultiplier( float fNewValue, int iMultiplierNumber )
{
	switch (iMultiplierNumber)
	{
	case 1:
		m_fAttackOneDamageMultiplier = fNewValue;
		break;
	case 2:
		m_fAttackTwoDamageMultiplier = fNewValue;
		break;
	case 3:
		m_fAttackThreeDamageMultiplier = fNewValue;
		break;
	default:
		UE_LOG( LogTemp, Warning, TEXT( "Could not get damage multiplier as invalid argument was passed, need values: [1-3]" ) );
	}
}


void UPlayerStatsComponent::AddHealth( float fValueToAdd )
{
	m_fRegenToHealth += fValueToAdd;
	m_fRegenToHealth = FMath::Clamp( m_fRegenToHealth, 0.f, m_fMaxHealth );

	if( fValueToAdd < 0.f )
	{
		m_fCurrentHealth += fValueToAdd;
		m_fCurrentHealth = FMath::Clamp( m_fCurrentHealth, 0.0f, m_fMaxHealth );
	}
}

void UPlayerStatsComponent::AddEnergy( float fValueToAdd )
{
	m_fCurrentEnergy += fValueToAdd;
	m_fCurrentEnergy = FMath::Clamp( m_fCurrentEnergy, 0.0f, m_fMaxEnergy );
}


void UPlayerStatsComponent::UpdateStamina( float fDeltaTime )
{
	m_fCurrentEnergy += ( fDeltaTime * m_fEnergyAmountToRegen );
	m_fCurrentEnergy = FMath::Clamp( m_fCurrentEnergy, 0.f, m_fMaxEnergy );
}

void UPlayerStatsComponent::RegenHealth( float fDeltaTime )
{
	m_fCurrentHealth += ( fDeltaTime * m_fHealthRegenRate );
	m_fCurrentHealth = FMath::Clamp( m_fCurrentHealth, 0.f, m_fMaxHealth );
}

void UPlayerStatsComponent::LiveStatReset()
{
	m_fCurrentHealth = m_fMaxHealth;
	m_fCurrentEnergy = m_fMaxEnergy;
}

void UPlayerStatsComponent::AddResources( int iResourceAmount )
{
	m_iResourceAmount += iResourceAmount;

	// If the player has enough resources to upgrade, display the prompt
	if( m_iResourceAmount >= m_iUpgradeRequirement )
	{
		if( m_pcUpgradePromptClass )
		{
			if( !m_pcUpgradePromptInst )
			{
				m_pcUpgradePromptInst = CreateWidget<UUserWidget>( UGameplayStatics::GetGameInstance( this ), m_pcUpgradePromptClass );
			}

			if( !m_pcUpgradePromptInst->IsInViewport() )
			{
				m_pcUpgradePromptInst->AddToViewport();
			}	
		}
	}
}