// Fill out your copyright notice in the Description page of Project Settings.

#include "PS4BuildMapLS.h"
#include "WhiteBoxProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/PlayerStatsUserWidget.h"
#include "UserWidget.h"


void APS4BuildMapLS::BeginPlay ( )
{
	//check and show the player stats at top
	if ( !m_pPlayerStatsWidget )
	{
		m_pPlayerStatsWidget = CreateWidget<UPlayerStatsUserWidget> ( GetWorld() , UPlayerStatsUserWidget::StaticClass ( ) );
	}

	//Add player stats widget on the screen
	m_pPlayerStatsWidget->AddToViewport ( );

}



void APS4BuildMapLS::NPCTriggerIceGainSpawns()
{
	for (FTransform& rsTransform : m_asHoundTransforms)
	{
		SpawnHound(rsTransform);
	}
	for (FTransform& rsTransform : m_asCrystalTransforms)
	{
		SpawnFireCrystal(rsTransform);
	}
	SetCharacterAbilityType(EElementAttribute::EElementAttribute_Ice);
}

void APS4BuildMapLS::SpawnHound(FTransform& rsTransform)
{
	if (m_pcHoundClass)
	{
		m_apcSpawnedActors.Add(GetWorld()->SpawnActor<AActor>(m_pcHoundClass, rsTransform));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HoundClass was NULL!"));
	}
}

void APS4BuildMapLS::SpawnFireCrystal(FTransform& rsTransform)
{
	if (m_pcFireCrystalClass)
	{
		m_apcSpawnedActors.Add(GetWorld()->SpawnActor<AActor>(m_pcFireCrystalClass, rsTransform));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FireCrystalClass was NULL!"));
	}
}

void APS4BuildMapLS::SetCharacterAbilityType(EElementAttribute eAbilityType)
{
	AWhiteBoxProjectCharacter* pcPlayer = Cast<AWhiteBoxProjectCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (pcPlayer)
	{
		pcPlayer->CBSetElement(eAbilityType);
	}
}

void APS4BuildMapLS::SetHUDElement( EElementAttribute eElement )
{
	m_pPlayerStatsWidget->SetElement( eElement );
}




