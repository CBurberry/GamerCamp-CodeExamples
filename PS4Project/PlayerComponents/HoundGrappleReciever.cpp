// Fill out your copyright notice in the Description page of Project Settings.

//Main header
#include "HoundGrappleReciever.h"

//Inclusive headers
#include "../Actors/Hound/ASMeleeEnemy.h"
#include "Blueprint/UserWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerInput.h"
#include "WidgetBlueprintLibrary.h"


// Sets default values for this component's properties
UHoundGrappleReciever::UHoundGrappleReciever()
: m_pcProgressWidgetClass			( nullptr )
, m_pcProgressWidgetInstance		( nullptr )
, m_cAttachmentSocketName			( TEXT( "backAttachSocket" ) )
, m_fRightThumbStickLastXAxisValue	( 0.0f )
, m_fDualshockLastAcceleratorValue	( 0.0f )
, m_bIsBackSocketInUse				( false )
, m_fRemovalProgression				( 0.0f )
, m_fProgressionPerKeypress			( 0.1f )
, m_fReductionPerSecond				( 0.1f )
, m_bDoesSocketExist				( false )
, m_pcAttachedEnemy					( nullptr )
, m_fThumbstickDeltaLimit			( 0.75f )
, m_fAcceleratorDeltaLimit			( 1.5f )
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UHoundGrappleReciever::BeginPlay()
{
	Super::BeginPlay();

	//Check for the parent mesh attachment socket, if it doesn't exist show error.
	FindParentAttachmentSocket();
	if ( !m_bDoesSocketExist ) 
	{
		UE_LOG
		(
			LogTemp, 
			Error, 
			TEXT("HoundGrappleComponent on %s will not work as socket %s was not found on parent skeletal mesh component!"), 
			*GetOwner()->GetName(), 
			*m_cAttachmentSocketName.ToString()
		);
	}

	//Check world is valid
	if (GetWorld())
	{
		//Create widget for AI tests
		m_pcProgressWidgetInstance = UWidgetBlueprintLibrary::Create( this, m_pcProgressWidgetClass, GetWorld()->GetFirstPlayerController() );
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("Could not create m_pcProgressWidgetInstance as GetWorld() was invalid."));
	}
}


// Called every frame
void UHoundGrappleReciever::TickComponent(float fDeltaTime, ELevelTick eTickType, FActorComponentTickFunction* psThisTickFunction)
{
	Super::TickComponent(fDeltaTime, eTickType, psThisTickFunction);

	//check if the hound is on the players back,
	if (m_bIsBackSocketInUse)
	{
		//AI Removal Input check & progression update
		DecrementRemovalProgression( fDeltaTime );
		UpdateRemovalProgression();

		if (m_fRemovalProgression >= 1.0f) 
		{
			DetachHound();
		}
	}
}


void UHoundGrappleReciever::DecrementRemovalProgression( float fDeltaTime )
{
	if (m_fRemovalProgression > 0.0f)
	{
		m_fRemovalProgression -= (fDeltaTime * m_fReductionPerSecond);
	}
}


void UHoundGrappleReciever::UpdateRemovalProgression()
{
	//Get input value from accelarator and right thumbstick
	//float fRightThumbStickXAxisValue = GetWorld()->GetFirstPlayerController()->PlayerInput->GetKeyValue( FKey( "Gamepad_LeftX" ) );
	//FVector sAcceleratorValues = GetWorld()->GetFirstPlayerController()->GetInputVectorKeyState( EKeys::Acceleration );
	//UE_LOG(LogTemp, Warning, TEXT("Accelerator: %s"), *sAcceleratorValues.ToString());
	//float fDualshockAcceleratorValue = sAcceleratorValues.Y;

	//If the player controller is valid
	APlayerController* pcPlayerController = GetWorld()->GetFirstPlayerController();
	if (PrimaryComponentTick.bCanEverTick && pcPlayerController != nullptr)
	{
		//Get the difference between the input reads
		//float fMovementOnRightThumbStick = FMath::Abs( fRightThumbStickXAxisValue - m_fRightThumbStickLastXAxisValue );
		//float fAcceleratorDelta = FMath::Abs( fDualshockAcceleratorValue - m_fDualshockLastAcceleratorValue );

		//Check if E key (for windows usage) or delta's were exceeded by inputs
		if ( ( pcPlayerController->WasInputKeyJustPressed( EKeys::E )
			|| pcPlayerController->WasInputKeyJustPressed( EKeys::Gamepad_FaceButton_Left ) )
			//|| (fMovementOnRightThumbStick > m_fThumbstickDeltaLimit)
			/*|| (fAcceleratorDelta > m_fAcceleratorDeltaLimit)*/ )
		{
			//Apply logic for removing AI from socket
			UE_LOG( LogTemp, Warning, TEXT( "RemovalProgression: %f" ), m_fRemovalProgression );
			m_fRemovalProgression += m_fProgressionPerKeypress;

			//Update the last frame value with current fame
			//m_fRightThumbStickLastXAxisValue = fRightThumbStickXAxisValue;
			//m_fDualshockLastAcceleratorValue = fDualshockAcceleratorValue;
		}
	}
}


void UHoundGrappleReciever::GetAIOff()
{
	//Only run if not already visible
	if (!m_pcProgressWidgetInstance->GetIsVisible())
	{
		m_fRemovalProgression = 0.0f;
		if (m_pcProgressWidgetInstance)
		{
			m_pcProgressWidgetInstance->AddToViewport();
			ACharacter* pcCharacter = Cast<ACharacter>( GetOwner() );
			if (IsValid( pcCharacter ))
			{
				pcCharacter->GetCharacterMovement()->MaxWalkSpeed = 200.0f;
			}
		}
		else
		{
			UE_LOG( LogTemp, Warning, TEXT( "m_pcProgressWidgetInstance is NULL!" ) );
		}
	}
}


void UHoundGrappleReciever::AddRightStickBinding( UInputComponent* pcInputComponent )
{
	pcInputComponent->BindAxisKey( FKey( "Gamepad_RightX" ) );
}


bool UHoundGrappleReciever::CanBeAttachedTo()
{
	return !m_bIsBackSocketInUse && m_bDoesSocketExist;
}


bool UHoundGrappleReciever::DoesBackSocketExist()
{
	return m_bDoesSocketExist;
}


void UHoundGrappleReciever::AttachHound( AActor* pcEnemyToAttach )
{
	if ( IsValid(pcEnemyToAttach) && CanBeAttachedTo() )
	{
		pcEnemyToAttach->AttachToActor(
			Cast<ACharacter>(GetOwner()),
			FAttachmentTransformRules( EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, false ),
			m_cAttachmentSocketName
		);

		m_pcAttachedEnemy = pcEnemyToAttach;
		m_bIsBackSocketInUse = true;
		GetAIOff();
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("Attachment failed!"));
	}
}

void UHoundGrappleReciever::ForceGrappleBehaviour()
{
	m_bIsBackSocketInUse = true;
	GetAIOff();
}

void UHoundGrappleReciever::DetachHound()
{
	if (m_pcProgressWidgetInstance)
	{
		m_pcProgressWidgetInstance->RemoveFromViewport();
	}

	ACharacter* pcCharacter = Cast<ACharacter>( GetOwner() );
	if (IsValid( pcCharacter ))
	{
		pcCharacter->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}

	AASMeleeEnemy* pcHound = Cast<AASMeleeEnemy>( m_pcAttachedEnemy );
	if (pcHound)
	{
		pcHound->DetachFromPlayer();
	}

	m_pcAttachedEnemy = nullptr;
	m_bIsBackSocketInUse = false;
	m_fRightThumbStickLastXAxisValue = 0.0f;
	m_fRemovalProgression = 0.0f;
}


void UHoundGrappleReciever::FindParentAttachmentSocket()
{
	ACharacter* pcCharacter = Cast<ACharacter>( GetOwner() );
	if (pcCharacter)
	{
		USkeletalMeshComponent* pcSkeletalMesh = pcCharacter->GetMesh();
		m_bDoesSocketExist = pcSkeletalMesh->DoesSocketExist( m_cAttachmentSocketName );
	}
}
