// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AIHealthWidgetComponent.generated.h"

// Fwd Decl#
class UWorldSpaceAIHealth;

/**
 * A Widget component derived class that shows the UWorldSpaceAIHealth widget on the parent actor.
 * Used to display health of an actor above the actor itself.
 */
UCLASS()
class WHITEBOXPROJECT_API UAIHealthWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	/* Generated constructor */
	UAIHealthWidgetComponent						( const FObjectInitializer& objectInitializer );

	/* 
		Call to initialize & instantiate the Widget member from derived class.
		Is called by engine if object exists in level. If spawned it needs to be called elsewhere.
	*/
	virtual void	InitWidget						() override;

	/* Called at the start of play, also calls InitWidget to ensure Widget member is correctly initialized. */
	virtual void	BeginPlay						() override;

	/* 
		Update the health bar (progress bar) member of UWorldSpaceAIHealth class
		fCurrentAIHealth: Current health of the parent actor
		fMaxAIHealth: Maximum health of the parent actor
	*/
	void			UpdateHealthBar					( float fCurrentAIHealth, float fMaxAIHealth );

	/*
		Rotate the UWorldSpaceAIHealth widget to always face the camera. 
		Note: This must be called outside of this class' tick function
		(a bug happens where this widget is invisible if TickComponent is overridden)
	*/
	void			RotateToFaceCamera				();
	
private:
	/* The Widget displayed above the actor */
	UPROPERTY()
	UWorldSpaceAIHealth*							m_pcWorldAIHealth;
};
