// Fill out your copyright notice in the Description page of Project Settings.

#include "ComponentAttachTest.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AComponentAttachTest::AComponentAttachTest()
: m_pcComponentInstance		( nullptr )
, m_pcActorToSpawn			( nullptr )
, m_pcActorInstance			( nullptr )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//Spawn the component if it's valid
	m_pcComponentInstance = CreateDefaultSubobject<UCapsuleComponent>( TEXT( "TESTCOMPONENT" ) );
	Cast<UCapsuleComponent>( m_pcComponentInstance )->SetCapsuleSize( 100.0f, 200.0f );
	m_pcComponentInstance->bHiddenInGame = false;

	if( HasValidRootComponent() ) 
	{
		m_pcComponentInstance->SetupAttachment( RootComponent );
	}
	else 
	{
		SetRootComponent( m_pcComponentInstance );
	}	
}

// Called when the game starts or when spawned
void AComponentAttachTest::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG( LogTemp, Warning, TEXT( "BeginPlay: Size of PS Array is %d" ), m_tapcParticleSystems.Num() );

	FString strName = "PSC_";
	//Initialise PSC's here as the PS array has no elements within constructor
	for( int iLoop = 0; iLoop < m_tapcParticleSystems.Num(); ++iLoop )
	{
		FName cName = FName( *( strName + FString::FromInt(iLoop) ) );
		UParticleSystemComponent* pcNewPSC = NewObject<UParticleSystemComponent>( this, UParticleSystemComponent::StaticClass(), cName );
		pcNewPSC->AttachToComponent( RootComponent, FAttachmentTransformRules( EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true ) );
		pcNewPSC->RegisterComponent();
		pcNewPSC->SetTemplate( m_tapcParticleSystems[ iLoop ] );
		pcNewPSC->SetWorldRotation( FRotator( 0.0f, 90.0f * iLoop, 0.0f ) );
		pcNewPSC->SetActive( true );
		m_tapcPSCs.Add( pcNewPSC );

		UE_LOG( LogTemp, Warning, TEXT( "ParticleSystem: %s added!" ), *cName.ToString() );
	}

	//Spawn the actor if it's valid
	if( m_pcActorToSpawn )
	{
		m_pcActorInstance = GetWorld()->SpawnActor<AActor>( m_pcActorToSpawn, GetActorLocation(), GetActorRotation() );
		m_pcActorInstance->AttachToActor( this, FAttachmentTransformRules( EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true ) );
		m_pcActorInstance->SetActorLocation( GetActorLocation() );
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "Actor was not spawned!" ) );
	}
}

// Called every frame
//void AComponentAttachTest::Tick(float fDeltaTime)
//{
//	Super::Tick(fDeltaTime);
//}

