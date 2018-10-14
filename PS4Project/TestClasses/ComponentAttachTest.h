// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ComponentAttachTest.generated.h"

//fwd decls
class UParticleSystem;
class UParticleSystemComponent;

/*	Component Attach Test Class
*	The purpose of this test is to narrow down the issue with regards to creating actor components 
*	in the class whose transforms do not properly attach or position themselves with the parent actor.
*	THIS IS A TEST CLASS - DELETE AFTER BUGFIXES ARE COMPLETE.

	Learned:
	* Do not create BP actors / components in the constructor since set editor uclass variables
	  are not initialised at this point.
	* Use setupattachment instead of Attachtocomponent if there is compiler errors with attachment in constructor.
	* Since BP members are necessarily initialised by the time in the code constructor, the root component may not be set.
	(So use SetupAttachment if this is the case as it defers attachment till after construction & bp's components are initialised)
	* Don't create BP components (either create components in class BP or use C++ component )
	* Use RegisterComponent for components that are created at runtime.
	* For most component usage, it's best to use CreateDefaultSubobject with C++ classes
	* Spawning an actor at runtime just requires SpawnActor & AttachToActor
*/
UCLASS()
class ECHOES_API AComponentAttachTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AComponentAttachTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The instance of the instantiated component type.
	UPROPERTY( VisibleAnywhere )
	USceneComponent* m_pcComponentInstance;

	// The class actor type that should be instantiated
	UPROPERTY( EditAnywhere )
	TSubclassOf<AActor> m_pcActorToSpawn;

	// The instance of the instantiated actor type
	UPROPERTY( VisibleAnywhere )
	AActor* m_pcActorInstance;

public:	
	// Called every frame
	//virtual void Tick(float fDeltaTime) override;

	//Array of particle effects (we're going to create a PSC for each)
	UPROPERTY( EditAnywhere )
	TArray<UParticleSystem*> m_tapcParticleSystems;

	//Array of particle system components
	UPROPERTY( EditAnywhere )
	TArray<UParticleSystemComponent*> m_tapcPSCs;
};
