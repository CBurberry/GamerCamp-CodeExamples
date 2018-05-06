// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/SuperUI.h"
#include "Elements/ElementTypes.h"
#include "PlayerStatsUserWidget.generated.h"

//Forward declarations
class UImage;
class UTexture2D;
class UProgressBar;

/**
 * Class extending USuperUI/UUserWidget classes to display player HUD (Icon/HP/Mana/Stamina)
 * Uses Filesystem access for loading textures.
 */
UCLASS()
class WHITEBOXPROJECT_API UPlayerStatsUserWidget : public USuperUI
{
	GENERATED_BODY ()

	//Progress bar for health value
	UProgressBar* m_pcHPFill;

	//Progress bar for Energy/Mana value
	UProgressBar* m_pcEnergyFill;

	//Progress bar for Stamina value
	UProgressBar* m_pcStaminaFill;

	//Images to be accessed by element switching.
	UImage* m_pcBoxElementIcon;
	UImage* m_pcFireElementIcon;
	UImage* m_pcIceElementIcon;
	UImage* m_pcLightningElementIcon;

	bool m_bFirstElementSwapped;

	bool m_bInitialised;

public:
	//Standard generated constructor
	UPlayerStatsUserWidget ( const FObjectInitializer& objectInitializer );

	/*
		Startup method called by UE4 UI implementation when widget is created (See UUSerWidget::CreateWidget()),
		All UI elements are setup in this function.
	*/
	virtual bool Initialize() override;

	/*UE4 Tick implementation override. Gets values from player character and sets progress bar percentages with the values. */
	virtual void NativeTick( const FGeometry& MyGeometry , float InDeltaTime ) override;

	/*
		Create a UI image to be displayed with the given 2D Texture, position and size constraints.
		pcImage: Pointer to the UImage to set.
		pcAsset: Pointer to the Texture2D asset to set on the UImage.
		sPosition: Position of the (top-left corner) of the UImage.
		sSize: Size of the UImage in screen space, default is the texture size
	*/
	void SetupUImage( UImage* pcImage, UTexture2D* pcAsset, FVector2D sPosition, FVector2D sSize = FVector2D::ZeroVector );

	/*
		Create a ProgressBar to be displayed with the given textures, appearance, position and size constraints.
		pcProgressBar: Pointer to the UProgressBar to set.
		pcBGTex: Texture for progress bar background
		pcFillTex:  Texture for progress bar fill
		sAppearance: RGBA tint applied to all textures
		sPosition: Position of the (top-left corner) of the UProgressBar.
		sSize: Size of the UImage in screen space.
	*/
	void SetupUProgressBar( UProgressBar* pcProgressBar, UTexture2D* pcBGTex, UTexture2D* pcFillTex, FLinearColor sAppearance, FVector2D sPosition, FVector2D sSize = FVector2D::ZeroVector );

	/* 
		Set the icon on the HUD to match the passed element. 
		eSetElement: Element whose HUD icon to set. Invalid value will leave icon hidden.
	*/
	void SetElement( EElementAttribute eSetElement );
};
