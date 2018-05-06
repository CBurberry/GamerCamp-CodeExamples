// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SuperUI.h"
#include "WorldSpaceAIHealth.generated.h"

// Fwd Decl
class UProgressBar;

/**
 * Simple widget class that displays a progress bar (USuperUI derives UUserWidget)
 */
UCLASS()
class WHITEBOXPROJECT_API UWorldSpaceAIHealth : public USuperUI
{
	GENERATED_BODY()

public:
	// Desired widget size.
	FVector2D m_sWidgetSize;
	
	// Generated constructor
	UWorldSpaceAIHealth	( const FObjectInitializer& objectInitializer );

	// Set health bar percentage
	void 			SetBarPercentage ( float fValue );
	
	// Instantiate member widget and set properties.
	virtual bool 	Initialize () override;
	
private:
	//Progress bar widget - constructed in Initialize()
	UProgressBar*	m_pcEnemyHP;
};
