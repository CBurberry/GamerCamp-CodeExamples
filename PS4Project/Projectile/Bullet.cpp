// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "Bullet.h"

// Other includes
#include "Actors/Kira/EchoesCharacter.h"
#include "Actors/Spectre/Specter.h"
#include "Actors/Spectre/SpecterAIController.h"
#include "Components/StaticMeshComponent.h"
#include <Engine/Engine.h>
#include "Engine/World.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerComponents/PlayerStatsComponent.h"
#include "TimerManager.h"
//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
ABullet::ABullet()
: m_pcCollisionMesh			( nullptr )
, m_fRefreshedLifeSpan		( 5.0f )
, m_fDamage					( 20.f )
, m_fTravelSpeed			( 10.f )
, m_fMinCollisionDistance	( 150.0f )
, m_iMaxNumberOfBounces		( 0 )
, m_iBounceCount			( 0 )
, m_fReboundSpeedMultiplier	( 1.05f )
, m_eBounceDir				( EBounceDirection::TowardsPlayer )
, m_pcOriginalTargetActor	( nullptr )
, m_pcCurrentTargetActor	( nullptr )
, m_fMaxAngleDeltaPerUpdate	( 5.0f )
, m_fStunDuration			( 0.0f )
, m_bInputDelimitedTrap		( false )
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the static mesh used for collisions
	m_pcCollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>( FName( "CollisionMesh" ) );
	SetRootComponent( m_pcCollisionMesh );
}

//////////////////////////////////////////////////////////////////////////
// Called every frame
//////////////////////////////////////////////////////////////////////////
void ABullet::Tick( float fDeltaTime )
{
	Super::Tick( fDeltaTime );

	if ( IsValid(this) ) 
	{
		// Get the actors current position
		FVector ActorPosition = GetActorLocation();

		// Get the forward direction its facing, normalize, and multiple by 'speed'
		FVector ActorDirection = GetActorForwardVector();
		ActorDirection.Normalize();
		ActorDirection *= m_fTravelSpeed;

		// Set the new actor location
		ActorPosition += ( ActorDirection * fDeltaTime );
		SetActorLocation( ActorPosition );
	}
}

void ABullet::CBFollowTarget()
{
	//Only do change direction if target is valid
	if (IsValid( m_pcCurrentTargetActor ))
	{
		FVector vPrevDirection = GetActorForwardVector();
		FVector vNewDirection = m_pcCurrentTargetActor->GetActorLocation() - GetActorLocation();

		FRotator sResultingRotation = UKismetMathLibrary::FindLookAtRotation( vPrevDirection, vNewDirection );
		FRotator sClampedRotation = vPrevDirection.Rotation();

		float fDeltaP = FMath::ClampAngle( 
			sResultingRotation.Pitch - GetActorRotation().Pitch, 
			m_fMaxAngleDeltaPerUpdate * -1.0f, 
			m_fMaxAngleDeltaPerUpdate 
		);
		float fDeltaY = FMath::ClampAngle( 
			sResultingRotation.Yaw - GetActorRotation().Yaw, 
			m_fMaxAngleDeltaPerUpdate * -1.0f, 
			m_fMaxAngleDeltaPerUpdate 
		);
		sClampedRotation.Pitch += fDeltaP;
		sClampedRotation.Yaw += fDeltaY;

		sClampedRotation.Roll = 0.0f;
		SetActorRotation( sClampedRotation );
	}
}


//////////////////////////////////////////////////////////////////////////
// The function to rebound the bullet if player is blocking
//////////////////////////////////////////////////////////////////////////
void ABullet::ReboundBullet( EBounceDirection eBDirection, float fSpeedMultiplier )
{
	//Increase the speed or projectile travel
	m_fTravelSpeed *= fSpeedMultiplier;

	//Set current direction
	m_eBounceDir = eBDirection;

	//Change projectile direction
	if (m_pcOwnerActor)
	{
		FVector vDirection;
		if (eBDirection == EBounceDirection::TowardsSender)
		{
			vDirection = m_pcOwnerActor->GetActorLocation() - GetActorLocation();

			//Projectile should move to the sender
			m_pcCurrentTargetActor = m_pcOwnerActor;
		}
		else 
		{
			vDirection = GetActorLocation() - m_pcOwnerActor->GetActorLocation();

			//Projectile to go to the target (can be null)
			m_pcCurrentTargetActor = m_pcOriginalTargetActor;
		}

		vDirection.Z = 0.f;
		SetActorRotation( vDirection.Rotation() );

		//Clear any existing lifespan timer
		SetLifeSpan( 0.0f );

		//Refresh the bullet lifespan
		SetLifeSpan( m_fRefreshedLifeSpan );
	}
}

void ABullet::SetStartingFollowTarget( AActor* pcTargetActor, float fUpdateDelay )
{
	m_pcOriginalTargetActor = pcTargetActor;
	m_pcCurrentTargetActor = pcTargetActor;

	if ( IsValid(pcTargetActor) ) 
	{
		//Set a looping timer since target was set
		FTimerHandle sTimer;
		GetWorld()->GetTimerManager().SetTimer( sTimer, this, &ABullet::CBFollowTarget, fUpdateDelay, true );
	}
	else 
	{
		//Clear all timers for this object
		GetWorld()->GetTimerManager().ClearAllTimersForObject( this );
	}
}

//////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
//////////////////////////////////////////////////////////////////////////
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	// Add overlap event handler
	m_pcCollisionMesh->OnComponentBeginOverlap.AddDynamic( this, &ABullet::OnOverlap );
}

//////////////////////////////////////////////////////////////////////////
// The function called when this object overlaps another
//////////////////////////////////////////////////////////////////////////
void ABullet::OnOverlap( UPrimitiveComponent* pHitComponent, AActor* pOtherActor,
	UPrimitiveComponent* pOtherComponent, int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rHit )
{
	// Check if the player is hit and the travel direction is correct (so overlap only fires once)
	// else check if collided with owner and handle.	
	AEchoesCharacter* pcPlayer = Cast<AEchoesCharacter>( pOtherActor );
	if( pcPlayer && m_eBounceDir == EBounceDirection::TowardsPlayer )
	{
		//If the player is attacking return the bullet back to the sender
		if ( pcPlayer->IsPlayerAttacking() && pOtherComponent->ComponentHasTag("PickaxeCollider") ) 
		{
			ReboundBullet( EBounceDirection::TowardsSender, m_fReboundSpeedMultiplier );
		}
		else if( FVector::Dist(GetActorLocation(), pOtherActor->GetActorLocation() ) <= m_fMinCollisionDistance)
		{
			//Apply damage to player after stun
			UGameplayStatics::ApplyPointDamage( pOtherActor, m_fDamage, GetActorLocation(), rHit,
				GetInstigatorController(), this, UDamageType::StaticClass() );

			//Stun if duration was set or input trap is true
			if( pcPlayer->GetStatsComponent()->GetPlayerCurrentHealth() > 0.0f && ( m_fStunDuration > 0.f || m_bInputDelimitedTrap ) )
			{
				pcPlayer->BecomeStunned( m_fStunDuration, m_bInputDelimitedTrap );
			}

			// Destroy the bullet
			m_pcCollisionMesh->DestroyComponent();
			Destroy();
		}
	}
	//check if it hit back the actor who thrown it
	else if (m_pcOwnerActor == pOtherActor && !pOtherComponent->ComponentHasTag(TEXT("Shield")) )
	{
		if ( m_iBounceCount < m_iMaxNumberOfBounces ) 
		{
			ReboundBullet( EBounceDirection::TowardsPlayer, m_fReboundSpeedMultiplier );
			m_iBounceCount++;
		}
		else 
		{
			UGameplayStatics::ApplyPointDamage( pOtherActor, m_fDamage, GetActorLocation(), rHit,
				GetInstigatorController(), this, UDamageType::StaticClass() );

			//Set stunned bb variable
			ASpecterAIController* pcAIController = Cast<ASpecterAIController>( Cast<ASpecter>( m_pcOwnerActor )->GetController() );  
			if (pcAIController)
			{
				pcAIController->SetStunned( true );
			}

			// Destroy the bullet
			m_pcCollisionMesh->DestroyComponent();
			Destroy();
		}
	}
}