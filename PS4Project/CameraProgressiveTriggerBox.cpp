// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraProgressiveTriggerBox.h"
#include "Components/BoxComponent.h"
#include "WhiteBoxProjectCharacter.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"

ACameraProgressiveTriggerBox::ACameraProgressiveTriggerBox()
: m_fEntryProgressionValueSnapshot	(0.0f)
, m_fUpdatedProgressionValue		(0.0f)
, m_bWasPlayerOverlappingLastFrame	(false)
{
	//Set the box to static to prevent transform changes (non-static calculation is more compilcated)
	GetRootComponent()->SetMobility(EComponentMobility::Static);
}

bool ACameraProgressiveTriggerBox::ProcessSpringArmCurve()
{
	bool bResult = false;
	if ( IsDistanceCurveValid() )
	{
		float fCurveValueAtSnapshot = GetArmDistanceValue( m_fEntryProgressionValueSnapshot );
		float fCurveValueCurrent = GetArmDistanceValue( m_fUpdatedProgressionValue );
		if ( !AreFloatsEqual( fCurveValueAtSnapshot, fCurveValueCurrent ) )
		{
			float fDelta = fCurveValueCurrent - fCurveValueAtSnapshot;
			m_pcOverlappedPlayerRef->SetArmDistance(m_fCameraArmLengthSnapshot + fDelta);
			bResult = true;
		}
	}

	return bResult;
}

bool ACameraProgressiveTriggerBox::ProcessFieldofViewCurve()
{
	bool bResult = false;
	if ( IsFOVCurveValid() ) 
	{
		float fCurveValueAtSnapshot = GetFOVValue( m_fEntryProgressionValueSnapshot );
		float fCurveValueCurrent = GetFOVValue( m_fUpdatedProgressionValue );
		if ( !AreFloatsEqual(fCurveValueAtSnapshot, fCurveValueCurrent) ) 
		{
			float fDelta = fCurveValueCurrent - fCurveValueAtSnapshot;
			m_pcOverlappedPlayerRef->SetFOV( m_fCameraFOVSnapshot + fDelta );
			bResult = true;
		}
	}

	return bResult;
}

bool ACameraProgressiveTriggerBox::ProcessCameraAngleCurve()
{
	bool bResult = false;
	if ( IsRotationCurveValid() ) 
	{
		FVector sRotationCurveAtSnapshot = GetCameraRotationVector( m_fEntryProgressionValueSnapshot );
		FVector sRotationCurveCurrent = GetCameraRotationVector( m_fUpdatedProgressionValue );

		if ( !AreVectorsEqual(sRotationCurveAtSnapshot, sRotationCurveCurrent) ) 
		{
			FVector sDeltaRotation = sRotationCurveCurrent - sRotationCurveAtSnapshot;
			FRotator sNewRotation = m_sCameraRotationSnapshot;
			sNewRotation.Pitch += sDeltaRotation.Y;
			sNewRotation.Yaw += sDeltaRotation.Z;
			sNewRotation.Roll += sDeltaRotation.X;

			m_pcOverlappedPlayerRef->SetControlRotation( sNewRotation.Pitch, sNewRotation.Yaw, sNewRotation.Roll );
			bResult = true;
		}
	}

	return bResult;
}

FVector ACameraProgressiveTriggerBox::ConvertWorldPositionToRelative( FVector sActorWorldPosition )
{
	//Translate the vector
	FVector sResult = sActorWorldPosition - GetActorLocation();

	//Apply (3D rotation * -1.0) as we want to apply the inverse rotation
	FRotator sVolumeRotation = GetActorRotation() * -1.0f;
	sResult = sVolumeRotation.RotateVector(sResult);

	return sResult;
}

float ACameraProgressiveTriggerBox::GetProgressionValue( FVector sRelativePosition )
{
	UBoxComponent* psBoxComponent = Cast<UBoxComponent>( GetCollisionComponent() );

	//Get the normalised value as a factor of extent [0.0f, 1.0f].
	float fNormalisedProgression = sRelativePosition.X / psBoxComponent->GetScaledBoxExtent().X;

	//Default normalised progression yields value of range [-1.0f, 1.0f], applying the following to get [0.0f, 1.0f]
	fNormalisedProgression += 1.0f;
	fNormalisedProgression /= 2.0f;
	fNormalisedProgression = FMath::Clamp( fNormalisedProgression, 0.0f, 1.0f );
	return fNormalisedProgression;
}

void ACameraProgressiveTriggerBox::Tick( float fDeltaTime )
{
	if ( IsPlayerOverlapping() )
	{
		UBoxComponent* psBoxComponent = Cast<UBoxComponent>( GetCollisionComponent() );

		//Check if this is the first frame of overlap.
		if ( !m_bWasPlayerOverlappingLastFrame )
		{
			//Get initial snapshot of player position.
			FVector sPlayerEntryPositionSnapshot = ConvertWorldPositionToRelative( m_pcOverlappedPlayerRef->GetActorLocation() );
			m_fEntryProgressionValueSnapshot = GetProgressionValue( sPlayerEntryPositionSnapshot );
		}
		else
		{
			//Get the normalised value as a factor of extent [0.0f, 1.0f]
			m_fUpdatedProgressionValue = GetProgressionValue( ConvertWorldPositionToRelative( m_pcOverlappedPlayerRef->GetActorLocation() ) );

			//Update the camera values
			ProcessSpringArmCurve();
			ProcessFieldofViewCurve();
			ProcessCameraAngleCurve();
		}
		m_bWasPlayerOverlappingLastFrame = true;
	}
	else 
	{
		m_bWasPlayerOverlappingLastFrame = false;
	}
}
