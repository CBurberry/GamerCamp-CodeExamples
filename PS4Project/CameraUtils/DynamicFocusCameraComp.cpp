// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicFocusCameraComp.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "FadeComponent.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "GCFramework/GCStaticHelperLibrary.h"

UDynamicFocusCameraComp::UDynamicFocusCameraComp()
	: m_sFocusPointName		(FString("FocusPoint"))
	, m_fFadeCheckTimer		(0.0f)
	, m_fCameraCheckDelay	(0.15f)
	, m_bEnableCameraRaycast(false)
	, m_bForceFocusPlayer(false)
{
	//Register for ticking
	PrimaryComponentTick.bCanEverTick = true;

	//incl player to actors to ignore for camera
	m_aActorsToIgnore.Add(this->GetAttachmentRootActor());
}

void UDynamicFocusCameraComp::FocusCameraAtPoint( float fX, float fY, float fZ )
{
	FVector vFocusPoint = FVector(fX, fY, fZ);
	UE_LOG( LogTemp, Warning, TEXT("FocusCameraAtPoint: %s"), *vFocusPoint.ToString() )

	FVector vStartLocation = this->GetComponentLocation();
	FRotator sCameraRotation = UKismetMathLibrary::FindLookAtRotation( vStartLocation, vFocusPoint );
	this->SetWorldRotation( sCameraRotation );
}

void UDynamicFocusCameraComp::FocusCameraAtRoot()
{
	//Only run if there is a child component
	USceneComponent* pcFocusPoint = GetFocusPoint();
	if ( pcFocusPoint != nullptr ) 
	{
		FVector vTargetLocation = pcFocusPoint->GetComponentLocation();
		FVector vStartLocation = this->GetComponentLocation();

		//Set camera focus to the position of the child. Clamp the y-axis so the rear wall doesn't fill screen.
		FRotator sCameraRotation = UKismetMathLibrary::FindLookAtRotation(vStartLocation, vTargetLocation);
		float fPitch = FMath::ClampAngle(sCameraRotation.Pitch, mk_fMinCameraAngle, mk_fMaxCameraAngle);
		sCameraRotation.Pitch = fPitch;
		//UE_LOG(LogTemp, Warning, TEXT("Camera Pitch: %f"), fPitch)
		this->SetWorldRotation( sCameraRotation );
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("Child by name of 'FocusPoint' missing!"))
	}
}

void UDynamicFocusCameraComp::ShowChildren()
{
	TArray<USceneComponent*> acChildren;
	this->GetChildrenComponents(false, acChildren);
	for ( USceneComponent* cElem : acChildren ) 
	{
		UE_LOG( LogTemp, Warning, TEXT( "Child Name: %s" ), *cElem->GetName() )
	}
}

void UDynamicFocusCameraComp::BeginPlay()
{
	//Super call
	Super::BeginPlay();

	//Start the game focused at the root
	FocusCameraAtRoot();
}

void UDynamicFocusCameraComp::TickComponent( float fDeltaTime, ELevelTick eTickType, FActorComponentTickFunction * psThisTickFunction )
{
	//Super call
	Super::TickComponent( fDeltaTime, eTickType, psThisTickFunction );

	//Only force focus on the player if enabled.
	if (m_bForceFocusPlayer)
	{
		FocusCameraAtRoot();
	}

	//Only do raycasting if enabled.
	if (m_bEnableCameraRaycast)
	{
		//Run camera check.
		CameraRaycastUpdate(fDeltaTime);
	}
}

USceneComponent* UDynamicFocusCameraComp::GetFocusPoint()
{
	TArray<USceneComponent*> acChildren;
	this->GetAttachmentRoot()->GetChildrenComponents( false, acChildren );
	for ( USceneComponent* cElem : acChildren )
	{
		if ( cElem->GetName() == m_sFocusPointName )
		{
			return cElem;
		}
	}

	return nullptr;
}

void UDynamicFocusCameraComp::CameraTrace()
{
	//If null, set.
	//Only run if there is a child component
	USceneComponent* pcFocusPoint = GetFocusPoint();
	if (pcFocusPoint != nullptr)
	{

		//Clear the hit meshes list
		m_aHitMeshes.Empty();;

		//Get the camera's position in world space
		const FVector vStartPos = this->GetComponentLocation();

		//Get the mesh's position in world space
		const FVector vEndPos = pcFocusPoint->GetComponentLocation();
		//UE_LOG(LogTemp, Warning, TEXT("CameraFocalPoint: %s"), *vEndPos.ToString())


		if (!(pcFocusPoint))
		{
			UE_LOG(LogTemp, Error, TEXT("Camera or Mesh component not found!"))
		}
		else
		{
			//Trace data is stored here
			TArray<FHitResult> asHitData;

			//Run trace and debug output.
			if (GCStaticHelpers::TraceForMultipleTargets(GetWorld(), m_aActorsToIgnore, vStartPos, vEndPos, asHitData, true, ECollisionChannel::ECC_Pawn))
			{
				for (FHitResult& sHitData : asHitData)
				{
					AStaticMeshActor* pcMesh = Cast<AStaticMeshActor>(sHitData.GetActor());
					if (pcMesh != nullptr)
					{
						//add unique hits to Hit Meshes
						m_aHitMeshes.AddUnique(pcMesh);

						//Make it hidden if it has a fade component
						ToggleFadeComponent(pcMesh, true);
					}
				}
			}
		}
	}
}

void UDynamicFocusCameraComp::CameraRaycastUpdate(float fDeltaTime)
{
	//Check camera line trace every delay period.
	if (m_fFadeCheckTimer > m_fCameraCheckDelay)
	{
		UpdateFadeActors();
		m_fFadeCheckTimer = 0.0f;
	}
	else
	{
		m_fFadeCheckTimer += fDeltaTime;
	}
}

void UDynamicFocusCameraComp::UpdateFadeActors()
{
	//Call camera trace - updates m_aHitActors.
	CameraTrace();

	//Check the arrays of hidden and hit actors and update accordingly.
	UpdateMeshVisibility();
}

void UDynamicFocusCameraComp::UpdateMeshVisibility()
{
	if (m_aHitMeshes.Num() == 0)
	{
		//Set all actors that are hidden to visible and remove them from the array.
		for (AStaticMeshActor* pcMesh : m_aHiddenMeshes)
		{
			ToggleFadeComponent(pcMesh, false);
		}
		m_aHiddenMeshes.Empty();
	}
	else
	{
		//compare elements
		TArray<AStaticMeshActor*> aActorsToRemoveFromHidden;
		TArray<AStaticMeshActor*> aActorsToAddToHidden;

		//If an element is in hidden but not in current hit
		for (AStaticMeshActor* pcMesh : m_aHiddenMeshes)
		{
			if (m_aHitMeshes.Find(pcMesh) != -1)
			{
				//make visible
				ToggleFadeComponent(pcMesh, false);
				aActorsToRemoveFromHidden.Add(pcMesh);
			}
		}

		//If an element is hit but not in hidden
		for (AStaticMeshActor* pcMesh : m_aHitMeshes)
		{
			if (m_aHiddenMeshes.Find(pcMesh) != -1)
			{
				//make invisible
				ToggleFadeComponent(pcMesh, true);
				aActorsToAddToHidden.Add(pcMesh);
			}
		}

		//remove elements from hidden
		for (AStaticMeshActor* pcMesh : aActorsToRemoveFromHidden)
		{
			m_aHiddenMeshes.Remove(pcMesh);
		}

		//add elements to hidden
		for (AStaticMeshActor* pcMesh : aActorsToAddToHidden)
		{
			m_aHiddenMeshes.AddUnique(pcMesh);
		}
	}
}

void UDynamicFocusCameraComp::ToggleFadeComponent(AStaticMeshActor* pcMesh, bool bHideMesh)
{
	//Make it hidden if it has a fade component
	UActorComponent* pcActorComp = pcMesh->GetComponentByClass(UFadeComponent::StaticClass());
	UFadeComponent* pcFadeComp = Cast<UFadeComponent>(pcActorComp);
	if (pcFadeComp != nullptr && pcFadeComp->IsMeshHidden() != bHideMesh)
	{
		pcFadeComp->ToggleHideMesh(bHideMesh);
		m_aHiddenMeshes.AddUnique(pcMesh);
	}
}

void UDynamicFocusCameraComp::ToggleMeshHide(FString cActorName, bool bHide)
{
	for (TActorIterator<AStaticMeshActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AStaticMeshActor* pcMesh = *ActorItr;
		if (pcMesh != nullptr)
		{
			//If equal, do
			if (pcMesh->GetName().Compare(cActorName) == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Actor with name: %s found!"), *cActorName)
				ToggleFadeComponent(pcMesh, bHide);
			}
		}
	}
}
