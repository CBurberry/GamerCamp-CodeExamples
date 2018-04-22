// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "CameraTriggerBox.generated.h"

/**
 * TriggerBox that provides the ability to change the camera based on input curve(s)
 * This may be used to change camera zoom, spring arm length, Pitch/Yaw/Roll based on keyframes.
 */
UCLASS()
class WHITEBOXPROJECT_API ACameraTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

private:

	//Function called when the collision component begins overlap with another Actor.
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& k_sSweepResult);

	//Function called when the collision component ends overlap with another Actor.
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex);

	/* 
		Curve should be a multiplier curve that takes the snapshot value and multiplies 
		based on elapsed time since triggering. 
		Return true if there was a change in curve value since last frame (check to 5 sf.)
	*/
	virtual bool ProcessSpringArmCurve		();

	/*
		Curve should be a multiplier curve that takes the snapshot value and multiplies
		based on elapsed time since triggering.
		Return true if there was a change in curve value since last frame (check to 5 sf.)
	*/
	virtual bool ProcessFieldofViewCurve	();

	/* 
		Curves should be additive values (added to snapshot) 
		Note: X = Roll, Y = Pitch, Z = Yaw.
	*/
	virtual bool ProcessCameraAngleCurve	();

	//Boolean to prevent for double player overlap.
	bool m_bIsPlayerOverlapping;

	//Boolean to track curve changes in progress
	bool m_bIsCameraChanging;

	//Value to track duration since overlap started.
	float m_fAccumulatedTime;

	//Value to track how long in seconds the last frame took (replacement for passing parameters)
	float m_fFrameDuration;

	//The value by which floats are compared to determine if they are different
	const float m_fFloatComparisonTolerance = 0.005f;

	UPROPERTY(EditAnywhere, Category = "Camera Curves", DisplayName = "Spring Arm Length Curve")
	class UCurveFloat* m_psDistanceCurve;

	UPROPERTY(EditAnywhere, Category = "Camera Curves", DisplayName = "Field-Of-View Curve")
	class UCurveFloat* m_psFOVCurve;

	UPROPERTY(EditAnywhere, Category = "Camera Curves", DisplayName = "Camera Rotation Curve")
	class UCurveVector* m_psCameraRotation;

protected:
	//Get the value of the curve members based on the input value.
	float	GetArmDistanceValue		(float fPositionValue);
	float	GetFOVValue				(float fPositionValue);
	FVector	GetCameraRotationVector	(float fPositionValue);
	bool	IsCameraChanging		()						{ return m_bIsCameraChanging; }

	//Compares two floats using the multiplier.
	bool	AreFloatsEqual			(float fParamA, float fParamB);

	//Compares two vectors using the multiplier.
	bool	AreVectorsEqual			(FVector sParamA, FVector sParamB);

	//Check if curves are valid
	bool	IsDistanceCurveValid	()						{ return ( m_psDistanceCurve != nullptr ); }
	bool	IsFOVCurveValid			()						{ return ( m_psFOVCurve != nullptr ); }
	bool	IsRotationCurveValid	()						{ return ( m_psCameraRotation != nullptr ); }

	UPROPERTY()
	class AWhiteBoxProjectCharacter* m_pcOverlappedPlayerRef;

	//Camera snapshot properties at beginning of effect
	FRotator m_sCameraRotationSnapshot;
	float m_fCameraArmLengthSnapshot;
	float m_fCameraFOVSnapshot;

public:
	ACameraTriggerBox();

	//Public getter
	bool IsPlayerOverlapping() { return m_bIsPlayerOverlapping; };

	virtual void BeginPlay() override;

	virtual void Tick(float fDeltaTime) override;
};
