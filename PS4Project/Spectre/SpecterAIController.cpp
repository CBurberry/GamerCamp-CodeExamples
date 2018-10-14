// Fill out your copyright notice in the Description page of Project Settings.

#include "SpecterAIController.h"
#include "Specter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Actors/Kira/EchoesCharacter.h"
#include "BehaviorTree/BehaviorTree.h"

ASpecterAIController::ASpecterAIController( const FObjectInitializer& ObjectInitializer )
	: AAIController		( ObjectInitializer )
	, m_sTargetKeyName	( "Target" )
	, m_sHealthKeyName	( "CurrentHealth" )
	, m_sShieldKeyName	( "IsShieldActive" )
	, m_sStunnedStatus	( "Stunned" )
	, m_sStunJustEnded	( "StunJustEnded" )
{
	m_pcBehaviorComp	= CreateDefaultSubobject<UBehaviorTreeComponent>( TEXT( "BehaviorComp" ) );
	m_pcBlackboardComp	= CreateDefaultSubobject<UBlackboardComponent>( TEXT( "BlackboardComp" ) );
}

void ASpecterAIController::Possess( APawn * InPawn )
{
	Super::Possess( InPawn );
	ASpecter* pcSpecter = Cast< ASpecter >( InPawn );
	if( pcSpecter )
	{
		UBehaviorTree* pcBehaviourTree = pcSpecter->GetBehaviourTree();
		if( pcBehaviourTree != nullptr )
		{
			if( pcBehaviourTree->BlackboardAsset )
			{
				
				// Initialize blackboard component by using behavior tree blackboard assets from Specter
				m_pcBlackboardComp->InitializeBlackboard( *( pcBehaviourTree->BlackboardAsset ) );
				// Initialize behavior component with behavior tree from Specter
				m_pcBehaviorComp->StartTree( *pcBehaviourTree );
			}
		}
	}
}

void ASpecterAIController::UnPossess()
{
	Super::UnPossess();

	// Stop any behavior running as we no longer have a pawn to control  
	m_pcBehaviorComp->StopTree();
}

void ASpecterAIController::SetTarget( APawn * pTarget )
{
	if( m_pcBlackboardComp )
	{
		m_pcBlackboardComp->SetValueAsObject( m_sTargetKeyName, pTarget );
	}
}

float ASpecterAIController::GetCurrentPawnHealth()
{
	if (m_pcBlackboardComp)
	{
		return m_pcBlackboardComp->GetValueAsFloat( m_sHealthKeyName );
	}

	UE_LOG(LogTemp, Error, TEXT("Get AI Pawn health called but no blackboard associated with AI."));
	return 0.0f;
}

void ASpecterAIController::UpdateCurrentHealth( float fCurrentHealth )
{
	if( m_pcBlackboardComp )
	{
		m_pcBlackboardComp->SetValueAsFloat( m_sHealthKeyName, fCurrentHealth );
		float fMYHealth = m_pcBlackboardComp->GetValueAsFloat( m_sHealthKeyName );
	}
}

void ASpecterAIController::SetShieldActive( bool bIsShieldActive )
{
	if( m_pcBlackboardComp )
	{
		m_pcBlackboardComp->SetValueAsBool( m_sShieldKeyName, bIsShieldActive );
	}
	
}

void ASpecterAIController::SetStunned( bool bNewStunValue )
{
	if (m_pcBlackboardComp)
	{
		m_pcBlackboardComp->SetValueAsBool( m_sStunnedStatus, bNewStunValue );
	}
}

bool ASpecterAIController::GetStunJustEnded()
{
	bool bResult = false;
	if( m_pcBlackboardComp )
	{
		bResult = m_pcBlackboardComp->GetValueAsBool( m_sStunJustEnded );
	}
	return bResult;
}

void ASpecterAIController::SetStunJustEnded( bool bStunJustEnded )
{
	if( m_pcBlackboardComp )
	{
		m_pcBlackboardComp->SetValueAsBool( m_sStunJustEnded, bStunJustEnded );
	}
}

bool ASpecterAIController::IsStunned()
{
	bool bResult = false;
	if (m_pcBlackboardComp)
	{
		bResult = m_pcBlackboardComp->GetValueAsBool( m_sStunnedStatus );
	}
	return bResult;
}

void ASpecterAIController::DisableBehaviourTree()
{
	if( m_pcBehaviorComp )
	{
		m_pcBehaviorComp->StopLogic( "Disabled" );
	}
}

bool ASpecterAIController::IsTargetValid() 
{
	bool bResult = false;
	ACharacter* pcCharacter = nullptr;
	if( m_pcBlackboardComp )
	{
		UObject* pcObject = m_pcBlackboardComp->GetValueAsObject( m_sTargetKeyName );
		if( pcObject )
		{
			pcCharacter = Cast<ACharacter>( pcObject );
			if( pcCharacter ) 
			{
				bResult = true;
			}
		}
	}
	return bResult;
}

