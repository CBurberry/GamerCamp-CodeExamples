// Fill out your copyright notice in the Description page of Project Settings.

#include "FadeComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Classes/Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"


// Sets default values for this component's properties
UFadeComponent::UFadeComponent()
	: m_pcParentMesh		(nullptr)
	, m_pcBaseMaterial		(nullptr)
	, m_pcFadeMaterialMID	(nullptr)
	, m_bIsMeshHidden		(false)
	, m_fCurrentFade		(1.0f)
	, m_fDeltaTime			(0.0f)
	, m_fFadeRate			(1.5f)
	, m_pcFadeCurve			(nullptr)
	, m_pcFadeMaterial		(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UFadeComponent::BeginPlay()
{
	Super::BeginPlay();

	// Event initialize component - do null checks on parent to prevent issues.
	m_pcParentMesh = Cast<AStaticMeshActor>( GetOwner() );
	if ( !m_pcParentMesh )
	{
		UE_LOG(LogTemp, Error, TEXT("Parent Mesh to Fade Component NOT initialised!"))
	}
	else 
	{
		m_pcBaseMaterial = Cast<UMaterial>(m_pcParentMesh->GetStaticMeshComponent()->GetMaterial(0) );
		if ( !m_pcBaseMaterial )
		{
			UE_LOG(LogTemp, Error, TEXT("Fade Component parent is missing material!"))
		}
		else 
		{
			if ( !m_pcFadeMaterial ) 
			{
				UE_LOG(LogTemp, Error, TEXT("Fade Material not set!"))
			}
			else 
			{
				//Create dynamic material instance
				m_pcFadeMaterialMID = UMaterialInstanceDynamic::Create( m_pcFadeMaterial, this );
			}
		}
	}
}


// Called every frame
void UFadeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if ( !m_pcParentMesh ) 
	{
		UE_LOG(LogTemp, Error, TEXT("Parent Mesh at FadeComponent is NOT VALID!"))
		return;
	}

	//Fade checks
	m_fDeltaTime = DeltaTime;
	if ( m_bIsMeshHidden ) 
	{
		//Get decreasing current fade value from deltatime, clamp to fade curve limits.
		m_fCurrentFade = FMath::Clamp( ( m_fCurrentFade - ( m_fDeltaTime * m_fFadeRate ) ), 0.0f, 1.0f);
	}
	else 
	{
		//Get increasing current fade value from deltatime, clamp to fade curve limits.
		m_fCurrentFade = FMath::Clamp( ( m_fCurrentFade + ( m_fDeltaTime * m_fFadeRate ) ), 0.0f, 1.0f);
	}

	//Get the current fade value from the curve.
	m_pcFadeMaterialMID->SetScalarParameterValue(FName("Fade"), m_pcFadeCurve->GetFloatValue(m_fCurrentFade));

	//If the fade is not in process and the object is not hidden, set it's material to normal.
	if ( (m_fCurrentFade >= 1.0f) && !m_bIsMeshHidden ) 
	{
		m_pcParentMesh->GetStaticMeshComponent()->SetMaterial(0, m_pcBaseMaterial);
	}
}

void UFadeComponent::ToggleHideMesh(bool bIsHidden)
{
	UE_LOG(LogTemp, Warning, TEXT("m_bIsMeshHidden: %d || bIsHidden: %d"), m_bIsMeshHidden, bIsHidden)
	if ( m_bIsMeshHidden != bIsHidden ) 
	{
		//Set mesh hidden value to parameter
		m_bIsMeshHidden = bIsHidden;
		if (m_bIsMeshHidden)
		{
			//Apply the fade material to slot 0 on SMComponent (again we assume there is only 1 material)
			UE_LOG(LogTemp, Warning, TEXT("Hiding Mesh!"))
			m_pcParentMesh->GetStaticMeshComponent()->SetMaterial(0, m_pcFadeMaterialMID);
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("Showing Mesh!"))
		}
	}
}

bool UFadeComponent::IsMeshHidden()
{
	return m_bIsMeshHidden;
}

