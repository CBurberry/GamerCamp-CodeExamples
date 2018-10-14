// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SpecterAIController.generated.h"

/**
 * 
 */
class UBehaviorTreeComponent;

UCLASS()
class ECHOES_API ASpecterAIController : public AAIController
{
	GENERATED_BODY()

private:
	// Blackboard key for player target
	FName					m_sTargetKeyName;

	// Blackboard key for player current health
	FName					m_sHealthKeyName;

	// Blackboard key name for shield status
	FName					m_sShieldKeyName;

	// Blackboard key name for stunned status
	FName					m_sStunnedStatus;

	// Blackbopard key name for stun just ended
	FName					m_sStunJustEnded;

	// Component to start and stop the behavior tree
	UBehaviorTreeComponent* m_pcBehaviorComp;

	// Used to pass values to the behavior tree
	UBlackboardComponent*	m_pcBlackboardComp;
	
protected:
	ASpecterAIController	( const class FObjectInitializer& ObjectInitializer );

	// Called whenever the controller possesses a character bot  
	virtual void Possess	( class APawn* InPawn ) override;

	virtual void UnPossess	() override;


public:
	// Sets a value in the blackboard which makes
	// Specter move to the target
	void SetTarget( APawn* pTarget );

	//Does the controller have a valid target?
	UFUNCTION( BlueprintPure )
	bool IsTargetValid();

	// Get the current health of spectre from blackboard
	float GetCurrentPawnHealth();

	// Updates current health in behavior tree
	void UpdateCurrentHealth( float fCurrentHealth );
	
	// Sets shield as active in blackboard for behavior tree
	void SetShieldActive( bool bIsShieldActive );

	// Set the stun boolean value
	void SetStunned( bool bNewStunValue );

	// Get if stun just ended
	UFUNCTION( BlueprintPure )
	bool GetStunJustEnded();

	// Set stun just ended
	void SetStunJustEnded( bool bStunJustEnded );

	//Get is the AI stunned - not the BB value
	UFUNCTION( BlueprintPure )
	bool IsStunned();

	//Disable the behaviour tree
	void DisableBehaviourTree();
};
