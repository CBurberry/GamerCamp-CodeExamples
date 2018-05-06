// Fill out your copyright notice in the Description page of Project Settings.

#include "AIHealthWidgetComponent.h"

#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "ProgressBar.h"

#include "UI/CustomComponents/WorldSpaceAIHealth.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UAIHealthWidgetComponent::UAIHealthWidgetComponent ( const FObjectInitializer & objectInitializer )
	: Super					( objectInitializer )
	, m_pcWorldAIHealth		( nullptr )
{
}

void UAIHealthWidgetComponent::InitWidget()
{
	//Super implementation call.
	Super::InitWidget();

	//Set the UClass type and cast the derived class instance of that type to the set type.
	SetWidgetClass(UWorldSpaceAIHealth::StaticClass());
	m_pcWorldAIHealth = Cast<UWorldSpaceAIHealth>(Widget);
}

void UAIHealthWidgetComponent::BeginPlay()
{
	//Force call InitWidget in case the engine has not already called it.
	InitWidget();
}

void UAIHealthWidgetComponent::UpdateHealthBar( float fCurrentAIHealth, float fMaxAIHealth )
{
	//Update progress bar to normalized value.
	m_pcWorldAIHealth->SetBarPercentage( fCurrentAIHealth / fMaxAIHealth );
}

void UAIHealthWidgetComponent::RotateToFaceCamera()
{
	//Get the player camera manager for player controller 0 if it exists.
	APlayerCameraManager* pcPlayerCameraManager = UGameplayStatics::GetPlayerCameraManager( GetWorld(), 0 );
	if ( IsValid(pcPlayerCameraManager) ) 
	{
		//Get the look at rotation and the camera rotation
		FVector sTargetLocation = pcPlayerCameraManager->GetCameraLocation();
		FVector sStartLocation = GetComponentLocation();
		FRotator sLookAtRotation = UKismetMathLibrary::FindLookAtRotation(sStartLocation, sTargetLocation);
		FRotator sCameraRotation = pcPlayerCameraManager->GetCameraRotation();

		//Apply the inverse of the camera rotation and use the look-at rotation pitch and roll.
		FRotator sNewRotation = FRotator(sLookAtRotation.Pitch, sCameraRotation.Yaw + 180.0f, sLookAtRotation.Roll );
		SetWorldRotation(sNewRotation);
	}
}
