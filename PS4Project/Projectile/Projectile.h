// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

//////////////////////////////////////////////////////////////////////////
// Class AProjectile: Base class for projectile objects within the game. Created
// to provide functionality that is the same in all of them such as setting
// the required weapon element. 
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// returns the static mesh component of the projectile, returns nullptr if the static mes component can not be found
	UStaticMeshComponent* GetStaticMeshComponentOfProjectile();

	void SetOwnerActor ( AActor* ownerActor ){ m_pcOwnerActor = ownerActor; }

protected:

	//Actor reference who shot this projectile to rebound back the projectile
	UPROPERTY ( )
	AActor* m_pcOwnerActor;

private:
};