// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraTriggerBox.h"
#include "Components/ShapeComponent.h"
#include "WhiteBoxProjectCharacter.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include <math.h>

ACameraTriggerBox::ACameraTriggerBox()
: m_bIsPlayerOverlapping	(false)
, m_bIsCameraChanging		(false)
, m_fAccumulatedTime		(0.0f)
, m_fFrameDuration			(0.0f)
, m_psDistanceCurve			(nullptr)
, m_psFOVCurve				(nullptr)
, m_psCameraRotation		(nullptr)
, m_pcOverlappedPlayerRef	(nullptr)
, m_sCameraRotationSnapshot	(FRotator::ZeroRotator)
, m_fCameraArmLengthSnapshot(0.0f)
, m_fCameraFOVSnapshot		(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//Add collision callback events
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic( this, &ACameraTriggerBox::OnOverlapBegin );
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic( this, &ACameraTriggerBox::OnOverlapEnd );

	//Set collision to only respond to overlapping pawn and ignore all other channels.
	FCollisionResponseContainer sCollisionResponses = FCollisionResponseContainer();
	sCollisionResponses.SetAllChannels(ECollisionResponse::ECR_Ignore);
	sCollisionResponses.SetResponse(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetCollisionComponent()->SetCollisionResponseToChannels( sCollisionResponses );
}

void ACameraTriggerBox::BeginPlay()
{
	Super::BeginPlay();
}

void ACameraTriggerBox::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);

	if ( m_bIsCameraChanging ) 
	{
		m_fAccumulatedTime += fDeltaTime;
		m_fFrameDuration = fDeltaTime;

		//Unlink the player if there was no change in any curve values.
		//Note: Using bitwise-AND to prevent short-circuit evaluation.
		if (!ProcessSpringArmCurve() & !ProcessFieldofViewCurve() & !ProcessCameraAngleCurve())
		{
			m_fAccumulatedTime = 0.0f;
			m_pcOverlappedPlayerRef = nullptr;
			m_bIsCameraChanging = false;
		}
	}
}

void ACameraTriggerBox::OnOverlapBegin(UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& k_sSweepResult) 
{
	if (!m_bIsPlayerOverlapping)
	{
		//Check if the player is the overlapped actor
		AWhiteBoxProjectCharacter* pcPlayer = Cast<AWhiteBoxProjectCharacter>(pcOtherActor);

		if (pcPlayer != nullptr)
		{
			//If camera is unlocked, run, else ignore.
			if ( !(pcPlayer->CameraLockCheck()) )
			{
				m_bIsPlayerOverlapping = true;
				m_bIsCameraChanging = true;

				//Set all the camera starting values for interpolation.
				m_pcOverlappedPlayerRef = pcPlayer;
				m_sCameraRotationSnapshot = m_pcOverlappedPlayerRef->GetCameraRotation();
				m_fCameraArmLengthSnapshot = m_pcOverlappedPlayerRef->GetSpringArmLength();
				m_fCameraFOVSnapshot = m_pcOverlappedPlayerRef->GetCameraFOV();
			}
		}
	}
}

void ACameraTriggerBox::OnOverlapEnd(UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex)
{
	if (m_bIsPlayerOverlapping)
	{
		m_bIsPlayerOverlapping = false;
	}
}

bool ACameraTriggerBox::ProcessSpringArmCurve()
{
	bool bResult = false;

	if ( IsDistanceCurveValid() )
	{
		//Get lengths and compare, *100 and int traunication to deal with float comparison issues.
		float fCurveValue		= m_psDistanceCurve->GetFloatValue(m_fAccumulatedTime);
		float fPrevCurveValue	= m_psDistanceCurve->GetFloatValue(m_fAccumulatedTime - m_fFrameDuration);

		//If there was a change, update the arm.
		if ( !AreFloatsEqual(fCurveValue, fPrevCurveValue) )
		{
			float fNewArmLength = fCurveValue + m_fCameraArmLengthSnapshot;
			m_pcOverlappedPlayerRef->SetArmDistance(fNewArmLength);
			bResult = true;
		}
	}

	return bResult;
}

bool ACameraTriggerBox::ProcessFieldofViewCurve()
{
	bool bResult = false;

	if ( IsFOVCurveValid() ) 
	{
		//Get values and compare, *10000 (5sf) and int traunication to deal with float comparison issues.
		float fCurveValue = m_psFOVCurve->GetFloatValue(m_fAccumulatedTime);
		float fPrevCurveValue = m_psFOVCurve->GetFloatValue(m_fAccumulatedTime - m_fFrameDuration);

		//If there was a change, update the arm.
		if ( !AreFloatsEqual(fCurveValue, fPrevCurveValue) )
		{
			float fNewFOV = m_fCameraFOVSnapshot + fCurveValue;
			m_pcOverlappedPlayerRef->SetFOV(fNewFOV);
			bResult = true;
		}
	}

	return bResult;
}

bool ACameraTriggerBox::ProcessCameraAngleCurve()
{
	bool bResult = false;

	if ( IsRotationCurveValid() )
	{
		//Get values and compare, *10000 (5sf) and int traunication to deal with float comparison issues.
		FVector sCurveValues = m_psCameraRotation->GetVectorValue(m_fAccumulatedTime);
		FVector sPrevCurveValues = m_psCameraRotation->GetVectorValue(m_fAccumulatedTime - m_fFrameDuration);

		//if there was a change, update the camera rotation
		if ( !AreVectorsEqual(sCurveValues, sPrevCurveValues) ) 
		{
			FRotator sNewRotation = m_sCameraRotationSnapshot;
			sNewRotation.Pitch += sCurveValues.Y;
			sNewRotation.Yaw += sCurveValues.Z;
			sNewRotation.Roll += sCurveValues.X;

			m_pcOverlappedPlayerRef->SetControlRotation(sNewRotation.Pitch, sNewRotation.Yaw, sNewRotation.Roll);
			bResult = true;
		}
	}

	return bResult;
}

float ACameraTriggerBox::GetArmDistanceValue( float fPositionValue )
{
	return m_psDistanceCurve->GetFloatValue(fPositionValue);
}

float ACameraTriggerBox::GetFOVValue( float fPositionValue )
{
	return m_psFOVCurve->GetFloatValue(fPositionValue);
}

FVector ACameraTriggerBox::GetCameraRotationVector( float fPositionValue )
{
	return m_psCameraRotation->GetVectorValue(fPositionValue);
}

bool ACameraTriggerBox::AreFloatsEqual( float fParamA, float fParamB )
{
	return ( fabs( fabs(fParamA) - fabs(fParamB) ) < m_fFloatComparisonTolerance );
}

bool ACameraTriggerBox::AreVectorsEqual( FVector sParamA, FVector sParamB )
{
	return ( AreFloatsEqual(sParamA.X, sParamB.X) && AreFloatsEqual(sParamA.Y, sParamB.Y) && AreFloatsEqual(sParamA.Z, sParamB.Z) );
}
