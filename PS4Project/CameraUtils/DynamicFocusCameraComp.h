// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "DynamicFocusCameraComp.generated.h"

/**
 * 
 */
UCLASS()
class WHITEBOXPROJECT_API UDynamicFocusCameraComp : public UCameraComponent
{
	GENERATED_BODY()

public:
	UDynamicFocusCameraComp();

	UFUNCTION( exec )
	void FocusCameraAtPoint( float fX, float fY, float fZ );

	UFUNCTION( exec )
	void FocusCameraAtRoot();

	UFUNCTION( exec )
	void ShowChildren();

protected:
	virtual void BeginPlay() override;

private:
	virtual void TickComponent( float fDeltaTime, enum ELevelTick eTickType, FActorComponentTickFunction* psThisTickFunction ) override;
	
	//Check if a child component exists.
	USceneComponent* GetFocusPoint();

	// Function to draw trace lines between player and camera, any actors in-between should have their alpha increased.
	void CameraTrace();

	// Command Line function for toggling an actor hiding (note the actor name might be different to it's name in world outlier)
	UFUNCTION(exec)
	void ToggleMeshHide(FString cActorName, bool bHide);

	//Variables for dynamic focus
	FString m_sFocusPointName;
	const float mk_fMinCameraAngle = -70.0f;
	const float mk_fMaxCameraAngle = 0.0f;

	//Actor list to be ignored by the camera rays
	UPROPERTY()
	TArray<AActor*> m_aActorsToIgnore;

	//List of meshes currently hidden
	UPROPERTY()
	TArray<class AStaticMeshActor*> m_aHiddenMeshes;

	//List of meshes hit by the current frame raycast
	UPROPERTY()
	TArray<AStaticMeshActor*> m_aHitMeshes;

	//Timer that delimits the number of fade checks done in a number of frames.
	float m_fFadeCheckTimer;

	//How long the camera raycast interval is
	float m_fCameraCheckDelay;

	//Editor boolean for toggling functionality
	UPROPERTY(EditAnywhere, DisplayName = "Enable Camera Raycast")
	bool m_bEnableCameraRaycast;

	//Editor boolean for toggling functionality - CURRENTLY UNIMPLEMENTED
	UPROPERTY(EditAnywhere, DisplayName = "Enable Camera ForceFocus")
	bool m_bForceFocusPlayer;

	//Run trace for intersecting geometry, use in Tick.
	void CameraRaycastUpdate( float fDeltaTime );

	//Call on loop to check and change visibilities of actors that intersect the camera.
	void UpdateFadeActors();

	//Check hit/hidden arrays for changes in what is seen by camera. Updates arrays accordingly.
	void UpdateMeshVisibility();

	//@param pcMesh is the mesh to which the fade component is attached
	void ToggleFadeComponent(AStaticMeshActor* pcMesh, bool bHideMesh);
};
