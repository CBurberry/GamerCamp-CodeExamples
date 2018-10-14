// Fill out your copyright notice in the Description page of Project Settings.

// Main header
#include "Specter.h"

//Inclusive headers
#include "../Hound/ASMeleeEnemy.h"
#include "Actors/Kira/EchoesCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Crystals/CrystalSpawnerActorComponent.h"
#include "EchoesGameMode.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Projectile/ProjectileEmitter.h"
#include "ProgressBar.h"
#include "SpecterAIController.h"
#include <stdexcept>
#include "TestClasses/BeamCollisionTestActor.h"
#include "UISystems/Widgets/MenuWidgetPlayerController.h"
#include "UnrealMathUtility.h"
#include "UserWidget.h"
#include <Vector.h>
#include "WidgetTree.h"


// Sets default values
ASpecter::ASpecter()
: m_fMaxHealth					( 200.0f )
, m_fCurrentHealth				( 200.0f )
, m_fSightRadius				( 2500.0f )
, m_fMovementSpeed				( 100.0f )
, m_fVision						( 180.0f )
, m_pcBeamActorInstance			( nullptr )
, m_pcBossHPWidgetClass			( nullptr )
, m_pcBossHPWidgetInstance		( nullptr )
, m_fResetSeenTime				( 2.0f )
, m_fTimeOfContinuousBeamStart	( 0.0f )
, m_pcProjectileEmitter			( nullptr )
, m_bStartWithShieldActive		( true )
, m_bIsShieldActive				( true )
, m_bIsDead						( false )
, m_fHPRegenPerSecond			( 25.0f )
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Sensing component to detect players by visibility.
	m_pcPawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>( TEXT( "PawnSensingComp" ) );
	// Set the vision radius to both sides
	m_pcPawnSensingComp->SetPeripheralVisionAngle( m_fVision );
	// Set sight distance
	m_pcPawnSensingComp->SightRadius = m_fSightRadius;

	// Set the movement speed through Character Movement Component											
	UCharacterMovementComponent* pcCharacterMovement = Cast< UCharacterMovementComponent >( GetMovementComponent() );
	pcCharacterMovement->MaxWalkSpeed = m_fMovementSpeed;

	// Create and attach the projectile emitter
	m_pcProjectileEmitter = CreateDefaultSubobject<UProjectileEmitter>( TEXT( "ProjectileEmitter" ) );
	AddOwnedComponent( m_pcProjectileEmitter );

	// Create the shield collider - set to only collide with pawns
	m_pcShieldCollider = CreateDefaultSubobject<USphereComponent>( TEXT("Shield") );
	m_pcShieldCollider->SetSphereRadius( 300.0f );
	m_pcShieldCollider->bHiddenInGame = true;
	m_pcShieldCollider->SetupAttachment( RootComponent );
	m_pcShieldCollider->SetCollisionProfileName( UCollisionProfile::Pawn_ProfileName );
	m_pcShieldCollider->CanCharacterStepUpOn = ECB_No;
	m_pcShieldCollider->bShouldUpdatePhysicsVolume = true;
	m_pcShieldCollider->SetMobility( EComponentMobility::Movable );
	m_pcShieldCollider->ComponentTags.AddUnique(TEXT("Shield"));
}

// Called when the game starts or when spawned
void ASpecter::BeginPlay()
{
	m_fCurrentHealth = m_fMaxHealth;

	m_pcSpectreController = Cast< ASpecterAIController >( GetController() );

	if( m_pcSpectreController )
	{
		m_pcSpectreController->UpdateCurrentHealth( m_fCurrentHealth );
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "Specter Has The Wrong AI Controller" ) );
	}

	//Register the function that is going to fire when the character sees a Pawn
	if( m_pcPawnSensingComp )
	{
		m_pcPawnSensingComp->bSeePawns = true;
		m_pcPawnSensingComp->OnSeePawn.AddDynamic( this, &ASpecter::OnSeePlayer );
	}

	//If the classtype is a valid actor
	if( m_pcBeamActorClasstype )
	{
		FActorSpawnParameters sSpawnParams;
		sSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		m_pcBeamActorInstance = Cast<ABeamCollisionTestActor>(GetWorld()->SpawnActor<AActor>( m_pcBeamActorClasstype, sSpawnParams ) );
		m_pcBeamActorInstance->AttachToActor( this, FAttachmentTransformRules( EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true ) );
		m_pcBeamActorInstance->SetActorLocation( GetActorLocation() );
	}
	else 
	{
		UE_LOG( LogTemp, Error, TEXT("BeamActorClasstype was not valid on boss!") );
	}

	//Discover the bp particle systems (if they exist) - workaround for bug UE-40054
	//Particle system components in code do not properly parent to actor and spawn in weird locations
	if( !m_pcBeamPE || !m_pcStunPE )
	{
		TArray<UActorComponent*> apcFoundComponents;
		GetComponents<UActorComponent>( apcFoundComponents );
		for( UActorComponent* pcFoundComp : apcFoundComponents )
		{
			UParticleSystemComponent* pcPSC = Cast<UParticleSystemComponent>( pcFoundComp );
			if (pcPSC != nullptr)
			{
				if ( pcPSC->GetName() == FString( "BeamPE" ) )
				{
					m_pcBeamPE = pcPSC;
					m_pcBeamPE->SetVisibility( true );
					m_pcBeamPE->SetActive( false );
				}
				else if ( pcPSC->GetName() == FString( "StunPE" ) )
				{
					m_pcStunPE = pcPSC;
					m_pcStunPE->SetVisibility( true );
					m_pcStunPE->SetActive( false );
				}
			}
		}

		//Error log outs if either component wasn't found.
		if (!m_pcBeamPE) { UE_LOG( LogTemp, Error, TEXT( "Attack Beam Particle System missing from Boss!" ) ); }
		if (!m_pcStunPE) { UE_LOG( LogTemp, Error, TEXT( "Stun Effect Particle System missing from Boss!" ) ); }
	}

	//Start with shield active?
	SetShieldActive( m_bStartWithShieldActive );

	//check the boss HP (don't show here) - set visible hp to full
	if( m_pcBossHPWidgetClass && !m_pcBossHPWidgetInstance )
	{
		m_pcBossHPWidgetInstance = Cast<UUserWidget>( CreateWidget<UUserWidget>( GetWorld(), m_pcBossHPWidgetClass ) );
		UProgressBar* pcHPBar = Cast<UProgressBar>( m_pcBossHPWidgetInstance->WidgetTree->FindWidget( TEXT( "HPBar" ) ) );
		pcHPBar->SetPercent( 1.0f );
	}

	Super::BeginPlay();
}

//Reaction when we see a player
void ASpecter::OnSeePlayer( APawn* pcPawn )
{
	// Tell the controller that the player is found
	// which in response makes specter move to that direction
	if( m_pcSpectreController )
	{
		m_pcSpectreController->SetTarget( pcPawn );

		//Show the hp bar
		if( m_pcBossHPWidgetInstance && !m_pcBossHPWidgetInstance->IsInViewport() )
		{
			UProgressBar* pcHPBar = Cast<UProgressBar>( m_pcBossHPWidgetInstance->WidgetTree->FindWidget( TEXT( "HPBar" ) ) );
			pcHPBar->SetPercent( m_fCurrentHealth / m_fMaxHealth );
			m_pcBossHPWidgetInstance->AddToViewport();
		}
	}
	m_fLastSeenTimePassed	= 0.0f;
	m_pcTargetPlayer		= Cast< AEchoesCharacter >( pcPawn );
}

// Called every frame
void ASpecter::Tick(float fDeltaTime)
{
	Super::Tick( fDeltaTime );

	//Step seen time and reset player detection if lost sight.
	m_fLastSeenTimePassed += fDeltaTime;
	PlayerTargetReset();

	//Heal over time if the player is unseen, damaged, & player not visible
	if( !m_bIsDead && (m_fCurrentHealth < m_fMaxHealth) && !IsPlayerInSight() ) 
	{
		m_fCurrentHealth += ( m_fHPRegenPerSecond * fDeltaTime );
		m_fCurrentHealth = FMath::Clamp( m_fCurrentHealth, 0.0f, m_fMaxHealth );
	}
}

//Called by the engine when this actor receives damage
float ASpecter::TakeDamage( float fDamageAmount, struct FDamageEvent const& krsDamageEvent, AController* pcEventInstigator, AActor* pcDamageCauser )
{
	if (m_pcSpectreController)
	{
		m_fCurrentHealth -= fDamageAmount;
		m_fCurrentHealth = FMath::Clamp( m_fCurrentHealth, 0.0f, m_fMaxHealth );

		m_pcSpectreController->UpdateCurrentHealth( m_fCurrentHealth );

		//Update the hp bar
		if( m_pcBossHPWidgetInstance && m_pcBossHPWidgetInstance->IsInViewport() )
		{
			UProgressBar* pcHPBar = Cast<UProgressBar>( m_pcBossHPWidgetInstance->WidgetTree->FindWidget( TEXT( "HPBar" ) ) );
			pcHPBar->SetPercent( m_fCurrentHealth / m_fMaxHealth );
		}

		//Destruction will now be handled via animation - turn off AI behaviors (following death)
		if (m_fCurrentHealth <= 0.0f)
		{
			DeathHandling();
		}

		//Broadcast delegate for animation
		if( m_dBossTakingDamage.IsBound() )
		{
			m_dBossTakingDamage.Broadcast();
		}
	}
	UE_LOG( LogTemp, Warning, TEXT("Specter current health %f" ), m_fCurrentHealth );
	return fDamageAmount;
}

void ASpecter::PlayerTargetReset()
{
	// Check if the last time we sensed a player is beyond the time out value to prevent bot from endlessly following a player.  
	float fProximityThreshold = 2000.0f;
	if ( m_pcTargetPlayer && m_fLastSeenTimePassed > m_fResetSeenTime 
		&& ( GetActorLocation() - m_pcTargetPlayer->GetActorLocation() ).Size() > fProximityThreshold )
	{
		if (m_pcSpectreController)
		{
			// Reset the target to follow
			m_pcSpectreController->SetTarget( nullptr );
			m_pcTargetPlayer = nullptr;

			//Hide the hp bar
			if( m_pcBossHPWidgetInstance && m_pcBossHPWidgetInstance->IsInViewport() )
			{
				m_pcBossHPWidgetInstance->RemoveFromViewport();
			}
		}
	}
}

void ASpecter::DeathHandling()
{
	//Destroy(); - disabled to let animation states play / we don't actually want to remove the actor (might change)
	m_pcSpectreController->DisableBehaviourTree();
	SetShieldActive( false );
	m_bIsDead = true;
}

//Called by the Behavior Tree to prepare and spring the trap
void ASpecter::ParallelTrapPlayer( float fBeamDuration, float fDamage, float fDelay, float fTrapWidth, float fBeamWidth, float fCrystalSpacing, float fInterSpawnDelay, float fParallelTrapLength )
{
	if ( IsValid( m_pcTargetPlayer ) )
	{
		FVector sPlayerLocation = m_pcTargetPlayer->GetActorLocation();

		//Does not check character is jumping
		FVector sBossGroundedLocation = GetActorLocation();
		//sBossGroundedLocation.Z = sPlayerLocation.Z;
		sPlayerLocation.Z = sBossGroundedLocation.Z;

		FVector sPositionVectorBossPlayer = sBossGroundedLocation - sPlayerLocation;
		FVector sPositionVectorPlayerBoss = sPlayerLocation - sBossGroundedLocation;
		FVector sDirectionVectorPlayerToBoss = sPositionVectorBossPlayer;
		FVector sDirectionVectorBossToPlayer = sPositionVectorPlayerBoss;
		sDirectionVectorPlayerToBoss.Normalize();
		sDirectionVectorBossToPlayer.Normalize();
		float fRowDistanceFromCentre = fTrapWidth / 2.0f;
		FVector sCrossProductLeft = FVector::CrossProduct( sPositionVectorBossPlayer, m_pcTargetPlayer->GetActorUpVector() );
		FVector sCrossProductRight = FVector::CrossProduct( m_pcTargetPlayer->GetActorUpVector(), sPositionVectorBossPlayer );
		sCrossProductLeft.Normalize();
		sCrossProductRight.Normalize();
		sCrossProductLeft *= fRowDistanceFromCentre;
		sCrossProductRight *= fRowDistanceFromCentre;
		FVector sTrapStart = ((sPositionVectorPlayerBoss.Size() - (fParallelTrapLength / 2.0f)) * sDirectionVectorBossToPlayer) + sBossGroundedLocation;
		FVector sTrapEnd = ((sPositionVectorPlayerBoss.Size() + (fParallelTrapLength / 2.0f) ) * sDirectionVectorBossToPlayer) + sBossGroundedLocation;
		FVector sTrapStartLeftPoint = sTrapStart + sCrossProductLeft;
		FVector sTrapStartRightPoint = sTrapStart + sCrossProductRight;
		FVector sTrapEndLeftPoint = sTrapEnd + sCrossProductLeft;
		FVector sTrapEndRightPoint = sTrapEnd + sCrossProductRight;

		/*DrawDebugPoint( GetWorld(), sTrapStartLeftPoint, 10.0f, FColor::Green, false, 0.5f );
		DrawDebugPoint( GetWorld(), sTrapStartRightPoint, 10.0f, FColor::Green, false, 0.5f );
		DrawDebugPoint( GetWorld(), sTrapEndLeftPoint, 10.0f, FColor::Blue, false, 0.5f );
		DrawDebugPoint( GetWorld(), sTrapEndRightPoint, 10.0f, FColor::Blue, false, 0.5f );
		DrawDebugPoint( GetWorld(), sTrapStart, 10.0f, FColor::White, false, 0.5f );
		DrawDebugPoint( GetWorld(), sTrapEnd, 10.0f, FColor::White, false, 0.5f );
		DrawDebugLine( GetWorld(), sTrapStart, sTrapEnd, FColor::Red, false, 5.0f, 0, 5.0f );*/

		//Crystal spawning callback
		m_ftdSpawnCrystalLinesCaller.BindUFunction(
			this, 
			FName("CBUpdateCrystalTrapSpawning"),
			sTrapStartLeftPoint,
			sTrapEndLeftPoint,
			sTrapStartRightPoint,
			sTrapEndRightPoint,
			fCrystalSpacing,
			fInterSpawnDelay
		);
		GetWorld()->GetTimerManager().SetTimer( m_fthSpawnCrystalLinesTimer, m_ftdSpawnCrystalLinesCaller, 0.05f, true );

		//Delay activation of the beam attack for a short time
		m_ftdContinuousBeamDelayCaller.BindUFunction(
			this,
			FName("CBAttackWithBeamAlongLine"),
			sTrapStart, 
			sTrapEnd, 
			fDamage, 
			fBeamWidth,
			fBeamDuration
		);

		//Begin beam attack
		if ( fDelay > 0.0f ) 
		{
			GetWorld()->GetTimerManager().SetTimer( m_fthContinousBeamDelayTimer, m_ftdContinuousBeamDelayCaller, fDelay, false );
		}
		else 
		{
			CBAttackWithBeamAlongLine( sTrapStart, sTrapEnd, fDamage, fBeamWidth, fBeamDuration );
		}
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("m_pcTargetPlayer is NULL when executing ParallelTrap"));
	}
}

void ASpecter::SimpleBeamFire( FVector sTargetLocation, float fDamage, float fAttackDuration, bool bAimAtPlayer )
{
	//Only run if the beam collision actor is valid!
	if( m_pcBeamActorInstance )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Beam Firing!" ) );

		m_pcBeamActorInstance->SetActorLocation( m_pcBeamPE->GetComponentLocation() );
		m_pcBeamActorInstance->SetBeamDamage( fDamage );
		m_pcBeamActorInstance->FixedBeamFire( ( (sTargetLocation - m_pcBeamPE->GetComponentLocation() ) * 2.0f ).Size(), 0.75f, fAttackDuration, bAimAtPlayer );

		//If we're not aiming at the player, set the aim location
		if( !bAimAtPlayer ) 
		{
			m_pcBeamActorInstance->SetTargetLocation( sTargetLocation );
		}
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "Could not fire beam as actor was not valid!" ) );
	}
}

void ASpecter::SweepBeamFire( FRotator sStartRotation, FRotator sEndRotation, float fDamage, float fAttackDuration, float fRotationDuration, float fBeamLength, float fRotationStartDelay )
{
	//Only run if the beam collision actor is valid!
	if( m_pcBeamActorInstance )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Beam Firing!" ) );

		m_pcBeamActorInstance->SetActorLocation( m_pcBeamPE->GetComponentLocation() );
		m_pcBeamActorInstance->SetBeamDamage( fDamage );
		FVector sTargetLocation = (sStartRotation.Vector() * fBeamLength) + m_pcBeamActorInstance->GetActorLocation();
		m_pcBeamActorInstance->FixedBeamFire( fBeamLength, 0.75f, fAttackDuration );
		m_pcBeamActorInstance->SetTargetLocation( sTargetLocation );
		m_pcBeamActorInstance->RotateBeamActor( ( sEndRotation - sStartRotation ), fRotationStartDelay, fRotationDuration );
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "Could not fire beam as actor was not valid!" ) );
	}
}

//Called by Parallel Trap Player to execute the beam attack part of the trap
void ASpecter::CBAttackWithBeamAlongLine(FVector sStartPosition, FVector sEndPosition, float fDamage, float fBeamWidth, float fAttackDuration)
{
	//Only run if the beamcollision actor is valid!
	if( m_pcBeamActorInstance )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Beam Firing!" ) );

		m_pcBeamActorInstance->SetActorLocation( m_pcBeamPE->GetComponentLocation() );
		m_pcBeamActorInstance->SetBeamDamage( fDamage );
		m_pcBeamActorInstance->FixedBeamFire( ( sEndPosition - sStartPosition ).Size(), 0.75f, fAttackDuration );
		m_pcBeamActorInstance->SetTargetLocation( sStartPosition );

		m_fTimeOfContinuousBeamStart = UGameplayStatics::GetTimeSeconds( GetWorld() );

		//Call function 'UpdateContinuousBeamAttack' every 0.1s looping.
		m_ftdContinuousBeamCaller.BindUFunction(
			this,
			FName( "CBUpdateContinuousBeamAttack" ),
			sStartPosition,
			sEndPosition,
			fAttackDuration
		);

		GetWorld()->GetTimerManager().SetTimer( m_fthContinousBeamTimer, m_ftdContinuousBeamCaller, 0.1f, true );
	}
	else 
	{
		UE_LOG( LogTemp, Error, TEXT( "Could not fire beam as actor was not valid!" ) );
	}
}

//Called by a timer to move (continue executing) the beam attack.
void ASpecter::CBUpdateContinuousBeamAttack(FVector& sStartPosition, FVector& sEndPosition, float fAttackDuration)
{
	float fElapsedTime = 0.0f;
	if (GetWorld())
	{
		FTimerManager& rsTimerManager = GetWorld()->GetTimerManager();
		fElapsedTime = UGameplayStatics::GetTimeSeconds(GetWorld()) - m_fTimeOfContinuousBeamStart;

		//Update the actor's location
		if( m_pcBeamActorInstance )
		{
			float fAlpha = FMath::Clamp( ( fElapsedTime / fAttackDuration ), 0.0f, 1.0f );
			FVector sNewPosition = FMath::Lerp( sStartPosition, sEndPosition, fAlpha );
			m_pcBeamActorInstance->SetTargetLocation( sNewPosition, true );
		}
		else
		{
			UE_LOG( LogTemp, Error, TEXT( "pcCollisionActor was not valid when trying to update it's position!" ) );
		}

		if (fElapsedTime >= fAttackDuration)
		{
			UE_LOG(LogTemp, Warning, TEXT("Time Elapsed has surpassed the attack duration!"));
			GetWorldTimerManager().ClearTimer(m_fthContinousBeamTimer);
			StopAttackWithBeam();
		}
	}
}

void ASpecter::CBUpdateCrystalTrapSpawning( FVector& sTrapStartLeft, FVector& sTrapEndLeft, FVector& sTrapStartRight, FVector& sTrapEndRight, float fCrystalSpacing, float fSpawnDelay )
{
	float fMaxSpawnDistance = ( sTrapEndLeft - sTrapStartLeft ).Size();

	//static local function variables (initialized once, persistent values between function calls)
	static float s_fCurrentSpawnDistance = 0.0f;
	static float s_fWorldTimeAtLastSpawn = 0.0f;
	static float s_fDeltaTimeSinceLastSpawn = 0.0f;

	//Spawn crystals at the drawn points between the trap points
	float fAlpha;
	FVector sCurrentSpawnLeft;
	FVector sCurrentSpawnRight;

	//Update delta time
	s_fDeltaTimeSinceLastSpawn = UGameplayStatics::GetTimeSeconds( GetWorld() ) - s_fWorldTimeAtLastSpawn;

	//Spawn crystals along the line
	if( s_fDeltaTimeSinceLastSpawn >= fSpawnDelay )
	{
		fAlpha = s_fCurrentSpawnDistance / fMaxSpawnDistance;
		fAlpha = FMath::Clamp( fAlpha, 0.0f, 1.0f );
		sCurrentSpawnLeft = FMath::Lerp( sTrapStartLeft, sTrapEndLeft, fAlpha );
		sCurrentSpawnRight = FMath::Lerp( sTrapStartRight, sTrapEndRight, fAlpha );
		s_fCurrentSpawnDistance += fCrystalSpacing;
		s_fDeltaTimeSinceLastSpawn = 0.0f;
		s_fWorldTimeAtLastSpawn = UGameplayStatics::GetTimeSeconds( GetWorld() );

		/*DrawDebugPoint( GetWorld(), sCurrentSpawnLeft, 10.0f, FColor::White, false, 6.0f );
		DrawDebugPoint( GetWorld(), sCurrentSpawnRight, 10.0f, FColor::White, false, 6.0f );*/

		UCrystalSpawnerActorComponent* pcCrystalSpawner = FindComponentByClass<UCrystalSpawnerActorComponent>();
		if( pcCrystalSpawner && pcCrystalSpawner->IsValidLowLevel() )
		{
			FRotator sDirectionRotation = ( sTrapEndLeft - sTrapStartLeft ).Rotation();
			pcCrystalSpawner->SpawnCrystalWithTransform( FTransform( sDirectionRotation, sCurrentSpawnLeft ), -1.0f );
			pcCrystalSpawner->SpawnCrystalWithTransform( FTransform( sDirectionRotation, sCurrentSpawnRight ), -1.0f );
		}
		else
		{
			UE_LOG( LogTemp, Error, TEXT( "pcCrystalSpawner IS NOT VALID!" ) );
		}

		if( s_fCurrentSpawnDistance >= fMaxSpawnDistance )
		{
			s_fCurrentSpawnDistance = 0.0f;
			s_fWorldTimeAtLastSpawn = 0.0f;
			s_fDeltaTimeSinceLastSpawn = 0.0f;
			GetWorldTimerManager().ClearTimer( m_fthSpawnCrystalLinesTimer );
		}
	}

}

void ASpecter::FireBeamAtTargetLocation( float fDistanceFromSelf, float fBeamLength, FVector sTargetLocation, float fDamage, float fBeamWidth, float fAttackDuration, bool bAimAtPlayer )
{
	//Find the start and end points of the beam
	FVector sBeamEndPoint;
	FVector sTravelDirectionVector = sTargetLocation - GetActorLocation();
	sTravelDirectionVector.Normalize();
	sBeamEndPoint = GetActorLocation() + sTravelDirectionVector * ( fDistanceFromSelf + fBeamLength );

	//Call CBAttackWithBeamAlongLine to fire beam
	SimpleBeamFire( sBeamEndPoint, fDamage, fAttackDuration, bAimAtPlayer );
}

void ASpecter::FireBeamAtGivenAngle( float fDistanceFromSelf, float fBeamLength, float fYawAngle, float fDamage, float fBeamWidth, float fAttackDuration )
{
	//Find the start and end points of the beam
	FVector sBeamEndPoint;
	FRotator sBeamDirectionRotation = GetActorRotation();
	sBeamDirectionRotation.Yaw += fYawAngle;
	FVector sTravelDirectionVector = sBeamDirectionRotation.Vector();
	sBeamEndPoint = GetActorLocation() + sTravelDirectionVector * ( fDistanceFromSelf + fBeamLength );

	//Call CBAttackWithBeamAlongLine to fire beam
	SimpleBeamFire( sBeamEndPoint, fDamage, fAttackDuration );
}

//Called at the end of the beam attack to stop it and end the beam.
void ASpecter::StopAttackWithBeam()
{
	if (m_pcProjectileEmitter != nullptr)
	{
		m_pcBeamPE->SetVisibility(false);
		UE_LOG(LogTemp, Warning, TEXT("Beam Stopping!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Beam is Nullptr!"));
	}
}

void ASpecter::Reset()
{
	m_fCurrentHealth = m_fMaxHealth;
	PlayerTargetReset();
	if ( m_pcSpectreController ) 
	{
		m_pcSpectreController->UpdateCurrentHealth( m_fCurrentHealth );
	}
}

void ASpecter::BecomeStunned( float fDuration )
{
	if ( m_pcStunPE ) 
	{
		m_pcStunPE->SetVisibility( true );
		m_pcStunPE->SetActive( true );
	}

	//Get the AI controller to update the blackboard
	m_pcSpectreController->SetStunned( true );

	//Disable the shield
	SetShieldActive( false );

	//Set a non-looping timer (for callback)
	FTimerHandle sTimer;
	GetWorld()->GetTimerManager().SetTimer( sTimer, this, &ASpecter::CBEndStun, fDuration, false );
}

void ASpecter::CBEndStun()
{
	if( !m_bIsDead ) 
	{
		//Re-enable the shield
		SetShieldActive( true );
	}

	//Get the AI controller to update the blackboard
	m_pcSpectreController->SetStunJustEnded( true );
	m_pcSpectreController->SetStunned( false );

	//Hide the particle effect
	if (m_pcStunPE)
	{
		m_pcStunPE->SetVisibility( false );
		m_pcStunPE->SetActive( false );
	}
}

void ASpecter::SetShieldActive( bool bIsActive )
{
	//Only set shield status if shield is a valid component
	if( m_pcShieldCollider ) 
	{
		m_pcShieldCollider->SetActive( bIsActive );
		m_bIsShieldActive = bIsActive;
		if( bIsActive )
		{
			m_pcShieldCollider->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );
			if( !m_bIsDead && m_dShieldActivated.IsBound() ) 
			{
				m_dShieldActivated.Broadcast();
			}
		}
		else 
		{
			m_pcShieldCollider->SetCollisionEnabled( ECollisionEnabled::NoCollision );
			if( !m_bIsDead &&m_dShieldDeactivated.IsBound() )
			{
				m_dShieldDeactivated.Broadcast();
			}
		}
	}
	else { UE_LOG(LogTemp, Error, TEXT("Shield sphere component on boss is missing!")) }
}

bool ASpecter::IsPlayerInSight()
{
	bool bResult = false;
	if( m_pcSpectreController ) 
	{
		bResult = m_pcSpectreController->IsTargetValid();
	};

	return bResult;
}

bool ASpecter::IsBossStunned()
{
	bool bResult = false;
	if( m_pcSpectreController )
	{
		bResult = m_pcSpectreController->IsStunned();
	};

	return bResult;
}

bool ASpecter::IsBeamCurrentlyActive()
{
	bool bResult = false;
	if( m_pcBeamActorInstance )
	{
		bResult = m_pcBeamActorInstance->IsBeamActive();
	};

	return bResult;
}

void ASpecter::DisableBossUI()
{
	if( m_pcBossHPWidgetInstance )
	{
		m_pcBossHPWidgetInstance->RemoveFromViewport();
		m_pcBossHPWidgetInstance->RemoveFromParent();
		m_pcBossHPWidgetInstance = nullptr;
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "Could not remove BossHP Widget!" ) );
	}
}

//Called by the controller to get the tree we have set up in the Blueprint for this pawn.
UBehaviorTree* ASpecter::GetBehaviourTree()
{
	return m_pcBehaviorTree;
}
