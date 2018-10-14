// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "Projectile.h"
#include "Engine/StaticMesh.h"


// Returns the static mesh component of the projectile, returns nullptr if the static mesh component can not be found
UStaticMeshComponent* AProjectile::GetStaticMeshComponentOfProjectile()
{
	TArray<UActorComponent*> pcTempActorComps; // Create array of pointers to actor components
	GetComponents<UActorComponent>( pcTempActorComps ); // gets all the projectile components
	for( UActorComponent* pcCompfound : pcTempActorComps )
	{
		// casts each component to static mesh comp if the cast completes successfully then return mesh, else, return nullptr
		UStaticMeshComponent* pcStaticMeshFound = Cast<UStaticMeshComponent>( pcCompfound );
		if( pcStaticMeshFound != nullptr )
		{
			return pcStaticMeshFound;
		}	
	}
	return nullptr;
}
