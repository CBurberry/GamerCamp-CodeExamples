// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraTriggerBox.h"
#include "CameraProgressiveTriggerBox.generated.h"

/**
 * Progressive Camera Trigger Volume:
 * Trigger volume that manipulates the player's camera based on player movement / position within the volume (along an axis)
 * The progressive axis (forward) for position check is the local object's X-axis.
 * Extends ACameraTriggerBox.

 *	TODO -	There's some imprecision that arises from entering and leaving the same box on the same end.
 *			Normalisation traunicates a small number that goes beyond clamp scale.
 */
UCLASS()
class WHITEBOXPROJECT_API ACameraProgressiveTriggerBox : public ACameraTriggerBox
{
	GENERATED_BODY()
	
private:
	//Use an additive curve's values to add the difference in progression as spring arm distance change.
	virtual bool ProcessSpringArmCurve		() override;

	//Use an additive curve's values to add the difference in progression as FOV change.
	virtual bool ProcessFieldofViewCurve	() override;

	/*
		//Use an additive curve's values to add the difference in progression as rotation change.
		Note: X = Roll, Y = Pitch, Z = Yaw.
	*/
	virtual bool ProcessCameraAngleCurve	() override;
	
	/*
		Get the position of the actor in world space and convert it into a vector relative
		to the volume's center. Also factors out rotation of the volume.
	*/
	FVector ConvertWorldPositionToRelative	(FVector sActorWorldPosition);

	/*
		Returns a normalised [0.0f, 1.0f] value based on where the given vector is within the volume.
		Parameter is a vector that has been transformed / rotated to align with the volume.
	*/
	float	GetProgressionValue				(FVector sRelativePosition);

	//The position the player entered within the volume space as progression value (along the x-axis)
	float m_fEntryProgressionValueSnapshot;

	//The current position the player is currently within the volume as progression value (along x-axis).
	float m_fUpdatedProgressionValue;

	//Was the player overlapping with the trigger volume in the previous frame?
	bool m_bWasPlayerOverlappingLastFrame;

protected:

public:
	//Non-default Constructor
	ACameraProgressiveTriggerBox();

	//Overriden tick method to get progression vaues on a frame by frame basis.
	virtual void Tick (float fDeltaTime) override;
};
