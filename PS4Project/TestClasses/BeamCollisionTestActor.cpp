// Fill out your copyright notice in the Description page of Project Settings.

#include "BeamCollisionTestActor.h"

#include "Actors/Kira/EchoesCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GCFramework/GCStaticHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Public/DrawDebugHelpers.h"

// Sets default values
ABeamCollisionTestActor::ABeamCollisionTestActor()
: m_pcCapsuleComponent		( nullptr )
, m_pcPSTemplate			( nullptr )
, m_pcPSC					( nullptr )
, m_pcHitsparkTemplate		( nullptr )
, m_pcHitsparkPSC			( nullptr )
, m_fFixedBeamStartWorldTime( 0.0f )
, m_fResizeStartWorldTime	( 0.0f )
, m_fRotationStartWorldTime	( 0.0f )
, m_fPreCapsuleRadius		( 0.0f )
, m_fPreCapsuleHSize		( 0.0f )
, m_sPreActorRotation		( FRotator::ZeroRotator )
, m_fEndOfBeamDelay			( 0.35f )
, m_fBeamRadius				( 50.0f )
, m_fFiredBeamMaxLength		( 50.0f )
, m_fDamageDealt			( 20.0f )
, m_pcBeamStartPoint		( nullptr )
, m_pcTargetAttachmentActor	( nullptr )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialise the start scene component for the beam attachment
	m_pcBeamStartPoint = CreateDefaultSubobject<USceneComponent>( TEXT( "BeamAttachPoint" ) );
	SetRootComponent( m_pcBeamStartPoint );

	//Initialise and orient capsule
	m_pcCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>( TEXT("CapsuleComponent") );
	m_pcCapsuleComponent->InitCapsuleSize( 0.0f, 0.0f );
	m_pcCapsuleComponent->SetWorldRotation( FRotator( 0.0f, 0.0f, 0.0f ) );
	m_pcCapsuleComponent->SetWorldLocation( RootComponent->GetComponentLocation() );
	m_pcCapsuleComponent->SetupAttachment( m_pcBeamStartPoint );
	m_pcCapsuleComponent->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
	ToggleBeamCollision( false );

	//Set overlap callback
	m_pcCapsuleComponent->OnComponentBeginOverlap.AddDynamic( this, &ABeamCollisionTestActor::OnCapsuleOverlap );

	//Initialise particle system component
	m_pcPSC = CreateDefaultSubobject<UParticleSystemComponent>( TEXT("BeamPSC") );
	m_pcPSC->SetupAttachment( m_pcBeamStartPoint );
	m_pcPSC->bAutoActivate = false;

	//Initialise particle system component
	m_pcHitsparkPSC = CreateDefaultSubobject<UParticleSystemComponent>( TEXT( "HitsparkPSC" ) );
	m_pcHitsparkPSC->SetupAttachment( m_pcBeamStartPoint );
	m_pcHitsparkPSC->bAutoActivate = false;

	ToggleParticleSystems( false );
}

// Called every frame
void ABeamCollisionTestActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	ACharacter* pcPlayerCharacter = UGameplayStatics::GetPlayerCharacter( this, 0 );

	//If is overlapping player, print
	if( pcPlayerCharacter && m_pcCapsuleComponent->IsOverlappingActor( pcPlayerCharacter ) )
	{
		UE_LOG( LogTemp, Log, TEXT( "Player overlapping capsule (tick)!" ) );
		UGameplayStatics::ApplyDamage( pcPlayerCharacter, m_fDamageDealt, GetInstigatorController(), this, UDamageType::StaticClass() );
	}

	//Only do collision raycast while the beam collision is active
	if( m_pcTargetAttachmentActor && m_pcCapsuleComponent->bGenerateOverlapEvents )
	{
		BeamHitUpdate();
	}
}

// Update the beam size to match either the first raycast hit location or maximum size
void ABeamCollisionTestActor::BeamHitUpdate()
{
	AActor* pcParent = RootComponent->GetAttachmentRootActor();
	TArray<AActor*> tapcActorsToIgnore = { this, pcParent };
	FHitResult sHitResult;

	//Get the direction vector between attachment actor & start component
	FVector sDirectionVector = ( m_pcTargetAttachmentActor->GetActorLocation() - m_pcBeamStartPoint->GetComponentLocation() );
	sDirectionVector.Normalize();

	FVector sTraceEndpoint = ( ( sDirectionVector * m_fFiredBeamMaxLength ) + m_pcBeamStartPoint->GetComponentLocation() );

	//N.B. Need to trace between the max beam size (not the target actor location)
	GCStaticHelpers::TraceForSingleTarget(
		GetWorld(),
		tapcActorsToIgnore,
		m_pcBeamStartPoint->GetComponentLocation(),
		sTraceEndpoint,
		sHitResult,
		false,
		ECollisionChannel::ECC_OverlapAll_Deprecated
	);

	if( IsValid( sHitResult.GetActor() ) ) 
	{
		//UE_LOG( LogTemp, Warning, TEXT( "Beam Hit Actor: %s" ), *sHitResult.GetActor()->GetName() );

		//Resize the capsule and move target actor location to match 
		//Set capsule half size to the hit location distance
		m_pcCapsuleComponent->SetCapsuleHalfHeight( ( sHitResult.Location - m_pcBeamStartPoint->GetComponentLocation() ).Size() * 0.5f );
		//Set the target actor location to the hit area
		SetTargetActorLocation( sHitResult.Location );
	}
	else 
	{
		m_pcCapsuleComponent->SetCapsuleHalfHeight( m_fFiredBeamMaxLength / 2.0f );
		SetTargetActorLocation( sTraceEndpoint );
	}
}

void ABeamCollisionTestActor::SetTargetActorLocation( FVector sTargetLocation )
{
	if( m_pcTargetAttachmentActor ) 
	{
		m_pcTargetAttachmentActor->SetActorLocation( sTargetLocation );
		m_pcHitsparkPSC->SetWorldLocation( sTargetLocation );
	}
}

// Called when the game starts or when spawned
void ABeamCollisionTestActor::BeginPlay()
{
	//Create attachment target
	CreateAttachmentActor();

	//Enable particle system template if one is set
	if( m_pcPSTemplate ) 
	{
		m_pcPSC->SetTemplate( m_pcPSTemplate );
		m_pcPSC->SetActorParameter( "BeamTarget", m_pcTargetAttachmentActor );
		if( m_pcHitsparkTemplate ) 
		{
			m_pcHitsparkPSC->SetTemplate( m_pcHitsparkTemplate );
		}
	}

	//We call super at the end as we always want the code to fire before any BP override.
	Super::BeginPlay();
}

void ABeamCollisionTestActor::FixedBeamFire( float fBeamLength, float fColliderDelay, float fDuration, bool bAimAtPlayer )
{
	//Reset the previous beam
	ResetBeam();

	m_ftdFixedBeamDelegate.BindUFunction(
		this,
		FName("CBFixedBeamUpdate"),
		fColliderDelay,
		fDuration,
		bAimAtPlayer
	);

	//Set beam size
	m_pcCapsuleComponent->SetCapsuleHalfHeight( fBeamLength / 2.0f );
	m_pcCapsuleComponent->SetCapsuleRadius( m_fBeamRadius );
	m_fFiredBeamMaxLength = fBeamLength;

	//Update the end point
	if( bAimAtPlayer )
	{
		SetTargetLocation( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 )->GetActorLocation() );
	}

	//Displace the capsule relative to the start point (currently is in the origin of capsule)
	FVector sCapsuleTransform = m_pcCapsuleComponent->GetRelativeTransform().GetLocation();
	sCapsuleTransform.Z = (fBeamLength / 2.0f);
	m_pcCapsuleComponent->SetRelativeLocation( sCapsuleTransform );

	//If attachment isn't valid, setup a new one
	if( !m_pcTargetAttachmentActor )
	{
		UE_LOG( LogTemp, Warning, TEXT( "m_pcTargetAttachmentActor Invalid! (Attempting to create a new one...)" ) );
		CreateAttachmentActor();
	}

	SetTargetActorLocation( GetCapsulePoint( ECapsulePoint::End ) );

	//Enable the beam & collider to false
	ToggleParticleSystems( true );
	ToggleBeamCollision( false );

	//Reset start time
	m_fFixedBeamStartWorldTime = UGameplayStatics::GetTimeSeconds( GetWorld() );

	//Begin beam attack
	GetWorld()->GetTimerManager().SetTimer( m_fthFixedBeamHandle, m_ftdFixedBeamDelegate, 0.1f, true );
}

void ABeamCollisionTestActor::CBFixedBeamUpdate( float fColliderDelay, float fDuration )
{
	if( GetWorld() ) 
	{
		//Get the time elapsed
		FTimerManager& rsTimerManager = GetWorld()->GetTimerManager();
		float fElapsedTime = UGameplayStatics::GetTimeSeconds( GetWorld() ) - m_fFixedBeamStartWorldTime;

		//DrawDebugPoint( GetWorld(), m_pcBeamStartPoint->GetComponentLocation(), 10.0f, FColor::Red, false, 0.25f );
		//DrawDebugPoint( GetWorld(), m_pcTargetAttachmentActor->GetActorLocation(), 10.0f, FColor::Blue, false, 0.25f );

		// If the collider delay has already passed, enable collider.
		if( (fElapsedTime >= fColliderDelay) && !m_pcCapsuleComponent->bGenerateOverlapEvents )
		{
			ToggleBeamCollision( true );
		}
		else if( fElapsedTime >= ( fDuration + m_fEndOfBeamDelay ) )
		{
			GetWorldTimerManager().ClearTimer( m_fthFixedBeamHandle );
			ToggleParticleSystems( false );
			m_pcCapsuleComponent->SetCapsuleSize( 0.0f, 0.0f );
			ToggleBeamCollision( false );
		}
	}
}

void ABeamCollisionTestActor::BeginColliderResizing( float fTargetCapsuleHalfHeight, float fUpdateDelay, float fDuration )
{
	m_ftdResizeDelegate.BindUFunction(
		this, 
		FName("CBUpdateResizingCollider"),
		fTargetCapsuleHalfHeight,
		fDuration
	);

	//Store the initial size - we set the radius to both vars
	m_pcCapsuleComponent->SetCapsuleHalfHeight( m_fBeamRadius );
	m_pcCapsuleComponent->SetCapsuleRadius( m_fBeamRadius );
	m_fFiredBeamMaxLength = fTargetCapsuleHalfHeight * 2.0f;

	//Displace the capsule relative to the start point (currently is in the origin of capsule)
	FVector sCapsuleTransform = m_pcCapsuleComponent->GetRelativeTransform().GetLocation();
	sCapsuleTransform.Z += m_fBeamRadius;
	m_pcCapsuleComponent->SetRelativeLocation( sCapsuleTransform );

	m_fPreCapsuleRadius = m_pcCapsuleComponent->GetScaledCapsuleRadius();
	m_fPreCapsuleHSize = m_pcCapsuleComponent->GetScaledCapsuleHalfHeight();

	//Enable the beam & collider
	ToggleParticleSystems( true );
	ToggleBeamCollision( true );

	//Begin beam attack
	GetWorld()->GetTimerManager().SetTimer( m_fthResizeHandle, m_ftdResizeDelegate, fUpdateDelay, true );
}

void ABeamCollisionTestActor::CBUpdateResizingCollider( float fTargetCapsuleHalfHeight, float fDuration )
{
	if( GetWorld() )
	{
		//Get the time elapsed
		FTimerManager& rsTimerManager = GetWorld()->GetTimerManager();
		float fElapsedTime = UGameplayStatics::GetTimeSeconds( GetWorld() ) - m_fResizeStartWorldTime;

		//Update the capsule size - height only since no need for resizing radius
		float fAlpha = fElapsedTime / fDuration;
		fAlpha = FMath::Clamp( fAlpha, 0.0f, 1.0f );
		float fPreviousHalfHeight = m_pcCapsuleComponent->GetScaledCapsuleHalfHeight();
		float fNewHalfHeight = FMath::Lerp( m_fPreCapsuleHSize, fTargetCapsuleHalfHeight, fAlpha );
		m_pcCapsuleComponent->SetCapsuleHalfHeight( fNewHalfHeight );

		//Move the capsule so it's start point equals the start point component
		FVector sCapsuleTransform = m_pcCapsuleComponent->GetRelativeTransform().GetLocation();
		sCapsuleTransform.Z += (fNewHalfHeight - fPreviousHalfHeight);
		m_pcCapsuleComponent->SetRelativeLocation( sCapsuleTransform );

		//Update the end point
		SetTargetActorLocation( GetCapsulePoint( ECapsulePoint::End ) );

		//DrawDebugPoint( GetWorld(), m_pcBeamStartPoint->GetComponentLocation(), 10.0f, FColor::Red, false, 0.25f );
		//DrawDebugPoint( GetWorld(), m_pcTargetAttachmentActor->GetActorLocation(), 10.0f, FColor::Blue, false, 0.25f );

		//If the duration of this process has been met, clear timer and reset vars.
		if( fElapsedTime >= ( fDuration + m_fEndOfBeamDelay ) ) 
		{
			UE_LOG(LogTemp, Warning, TEXT("Resize end!"));
			GetWorldTimerManager().ClearTimer( m_fthResizeHandle );
			m_fPreCapsuleRadius = 0.0f;
			m_fPreCapsuleHSize = 0.0f;
			ToggleParticleSystems( false );
			m_pcCapsuleComponent->SetCapsuleSize( 0.0f, 0.0f );
			ToggleBeamCollision( false );

			//DrawDebugPoint( GetWorld(), m_pcBeamStartPoint->GetComponentLocation(), 10.0f, FColor::Red, true );
			//DrawDebugPoint( GetWorld(), m_pcTargetAttachmentActor->GetActorLocation(), 10.0f, FColor::Blue, true );
		}
	}
}

void ABeamCollisionTestActor::RotateBeamActor( FRotator sAddedRotation, float fRotationDelay, float fDuration )
{
	m_ftdRotationDelegate.BindUFunction(
		this,
		FName("CBUpdateRotation"),
		sAddedRotation,
		fRotationDelay,
		fDuration
	);

	//Save current rotation
	m_sPreActorRotation = GetActorRotation();

	//Reset rotation time
	m_fRotationStartWorldTime = UGameplayStatics::GetTimeSeconds( GetWorld() );

	//Begin rotation (update every 0.025f)
	GetWorld()->GetTimerManager().SetTimer( m_fthRotationHandle, m_ftdRotationDelegate, 0.025f, true );
}

void ABeamCollisionTestActor::CBUpdateRotation( FRotator sAddedRotation, float fRotationDelay, float fDuration )
{
	if( GetWorld() )
	{
		//Get the time elapsed
		FTimerManager& rsTimerManager = GetWorld()->GetTimerManager();
		float fElapsedTime = UGameplayStatics::GetTimeSeconds( GetWorld() ) - m_fRotationStartWorldTime;

		//Update the end point
		SetTargetActorLocation( GetCapsulePoint( ECapsulePoint::End ) );

		if( fElapsedTime >= fRotationDelay )
		{
			//Update the rotation of this actor
			float fAlpha = (fElapsedTime - fRotationDelay) / fDuration;
			fAlpha = FMath::Clamp( fAlpha, 0.0f, 1.0f );
			SetActorRotation( FMath::Lerp( m_sPreActorRotation, m_sPreActorRotation + sAddedRotation, fAlpha ) );

			//Clear timer
			if( fElapsedTime >= ( fDuration + fRotationDelay ) )
			{
				GetWorldTimerManager().ClearTimer( m_fthRotationHandle );
				m_sPreActorRotation = FRotator::ZeroRotator;
			}
		}
	}
}

void ABeamCollisionTestActor::OnCapsuleOverlap( UPrimitiveComponent* pcHitComponent, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rSweepResult )
{
	if( Cast<AEchoesCharacter>( pcOtherActor ) )
	{
		UE_LOG( LogTemp, Warning, TEXT("Player Begin Overlap Capsule!") );
	}
}

FVector ABeamCollisionTestActor::GetCapsulePoint( ECapsulePoint ePointToReturnLocation )
{
	//initialize structs
	FVector sOriginPoint = m_pcCapsuleComponent->GetComponentLocation();
	FRotator sCapsuleRot = m_pcCapsuleComponent->GetComponentRotation();
	FVector sStartPointUnrotated = sOriginPoint;

	//Apply Z transform from the capsule half height (displaces in Z and is then rotated)
	if( ePointToReturnLocation == ECapsulePoint::Start )
	{
		sStartPointUnrotated.Z -= m_pcCapsuleComponent->GetScaledCapsuleHalfHeight();
	}
	else 
	{
		sStartPointUnrotated.Z += m_pcCapsuleComponent->GetScaledCapsuleHalfHeight();
	}

	//Get the vector between the start point and origin
	FVector sDirectionVector = sStartPointUnrotated - sOriginPoint;

	//Rotate that vector by the rotation & make relative to origin location
	return ( sCapsuleRot.RotateVector( sDirectionVector ) + sOriginPoint );
}

void ABeamCollisionTestActor::CreateAttachmentActor()
{
	FActorSpawnParameters sSpawnParams;
	sSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//Create the attachment actor
	m_pcTargetAttachmentActor = GetWorld()->SpawnActor<AActor>( AActor::StaticClass(), GetActorLocation(), GetActorRotation(), sSpawnParams );
	USceneComponent* pcAttachmentActorRoot = NewObject<USceneComponent>( m_pcTargetAttachmentActor, TEXT( "CustomSceneRoot" ) );
	m_pcTargetAttachmentActor->SetRootComponent( pcAttachmentActorRoot );
	m_pcTargetAttachmentActor->AttachToActor( this, FAttachmentTransformRules( EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true ) );

	//Validity check
	if( !m_pcTargetAttachmentActor )
	{
		UE_LOG( LogTemp, Error, TEXT( "m_pcTargetAttachmentActor Invalid! (Creation failed!)" ) );
	}
}

void ABeamCollisionTestActor::ToggleBeamCollision( bool bActive )
{
	if( m_pcCapsuleComponent ) 
	{
		m_pcCapsuleComponent->bGenerateOverlapEvents = bActive;

		//Update the overlaps manually (if overlapping a non-moving object it won't be updated to fire an overlap event)
		if( bActive ) 
		{
			m_pcCapsuleComponent->UpdateOverlaps();
		}

		if( m_pcHitsparkTemplate )
		{
			bool bReset = !bActive && m_pcHitsparkPSC->IsActive();
			m_pcHitsparkPSC->SetActive( bActive, bReset );
		}
	}
}

void ABeamCollisionTestActor::ToggleParticleSystems( bool bActive )
{
	bool bReset = !bActive && m_pcPSC->IsActive();
	m_pcPSC->SetActive( bActive, bReset );
	m_pcHitsparkPSC->SetActive( bActive, bReset );
}

void ABeamCollisionTestActor::ResetBeam()
{
	GetWorldTimerManager().ClearAllTimersForObject( this );
	m_fPreCapsuleRadius = 0.0f;
	m_fPreCapsuleHSize = 0.0f;
	ToggleParticleSystems( false );
	m_pcCapsuleComponent->SetCapsuleSize( 0.0f, 0.0f );
	ToggleBeamCollision( false );
}

void ABeamCollisionTestActor::SetTargetLocation( FVector sTargetLocation, bool bAdjustBeamLength )
{
	//If the target attachment actor is valid
	if( IsValid( m_pcTargetAttachmentActor ) ) 
	{
		//Get the rotation needed to rotate (end - start) to (new end - start)
		FVector sTargetVector = m_pcBeamStartPoint->GetComponentLocation() - sTargetLocation;
		FRotator sNewRotation = sTargetVector.Rotation();
		sNewRotation.Pitch += 90.0f;

		//Set this actor rotation to the new rotation
		SetActorRotation( sNewRotation );

		//Adjust the length of the beam (and reposition) if required
		if( bAdjustBeamLength )
		{
			//Resize the collider to match vector size
			float fNewHalfHeight = sTargetVector.Size() / 2.0f;
			m_pcCapsuleComponent->SetCapsuleHalfHeight( fNewHalfHeight );

			//Move the capsule so it's start point equals the start point component
			FVector sCapsuleTransform = m_pcCapsuleComponent->GetRelativeTransform().GetLocation();
			sCapsuleTransform.Z = fNewHalfHeight;
			m_pcCapsuleComponent->SetRelativeLocation( sCapsuleTransform );
		}

		//Set the target attachment actor location to the new location
		SetTargetActorLocation( sTargetLocation );
	}
	else 
	{
		UE_LOG( LogTemp, Warning, TEXT("Could not set beam target location as the target actor was not valid!") );
	}
}

bool ABeamCollisionTestActor::IsBeamActive()
{
	return ( m_pcPSC->IsActive() );
}
