// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Projectile/Projectile.h"
#include "Bullet.generated.h"

//////////////////////////////////////////////////////////////////////////
// Class ABullet: This class is the bullet style projectile that our game may
// have. It simply fires in a straight line and will continue to go until it
// hits something. Once it hits something it will destroy itself and trigger
// damage on the other actor that has been hit. If they are set up to recieve
// damage then that will be handled by them.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API ABullet : public AProjectile
{
	GENERATED_BODY()
	
private:
	//Directions of bounce enum - have to be at start of class for function declaration
	enum EBounceDirection
	{
		TowardsPlayer = 0,
		TowardsSender
	};

public:
	// Sets default values for this actor's properties
	ABullet();

	// Called every frame
	// Param1: The amount of time since last tick of this method
	virtual void Tick( float fDeltaTime ) override;

	//The function to rebound the bullet
	void ReboundBullet( EBounceDirection eBDirection, float fSpeedMultiplier );

	//Setter for damage
	UFUNCTION( BlueprintCallable )
	void SetBulletDamage( float fDamage ) { m_fDamage = fDamage; }

	//Setter for setting max bounce count
	UFUNCTION( BlueprintCallable )
	void SetMaxBounces( int iBounces ) { m_iMaxNumberOfBounces = iBounces; }

	//Getter for getting the current bounces
	UFUNCTION( BlueprintPure )
	int GetMaxBounces() { return m_iMaxNumberOfBounces; }

	//Getter for getting the current bounces
	UFUNCTION( BlueprintPure )
	int GetCurrentBounces() { return m_iBounceCount; }

	//Setter for travel speed
	UFUNCTION( BlueprintCallable )
	void SetTravelSpeed( float fNewSpeed ) { m_fTravelSpeed = fNewSpeed; }

	//Setter for rebound speed multiplier
	UFUNCTION( BlueprintCallable )
	void SetReboundSpeedMultiplier( float fMultiplier ) { m_fReboundSpeedMultiplier = fMultiplier; }

	//Setter for starting target actor
	UFUNCTION( BlueprintCallable )
	void SetStartingFollowTarget( AActor* pcTargetActor, float fUpdateDelay = 0.25f );

	//Setter for max angle delta per follow update
	UFUNCTION( BlueprintCallable )
	void SetMaxAngleDelta( float fValue ) { m_fMaxAngleDeltaPerUpdate = FMath::Clamp(fValue, 0.0f, 90.0f); }

	//Setter for bullet stun duration
	UFUNCTION( BlueprintCallable )
	void SetPlayerStunDuration( float fNewDuration ) { m_fStunDuration = fNewDuration; }

	//Setter for trap uses input or not
	UFUNCTION( BlueprintCallable )
	void SetBulletTrapsPlayer( bool bNewTrapValue ) { m_bInputDelimitedTrap = bNewTrapValue; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Lifetime the bullet is reset to after being rebounded
	UPROPERTY( BlueprintReadWrite, DisplayName = "Refreshed Life Spawn Duration" )
	float m_fRefreshedLifeSpan;

	//The minimum distance at which the bullet will hit the player. Decreasing this too much will disable the collision.
	UPROPERTY(EditAnywhere, DisplayName = "Minimum Distance for player Collision")
	float m_fMinCollisionDistance;

private:

	//////////////////////////////////////////////////////////////////////////
	// Start: Data Members
	// The static mesh component used for collision
	UPROPERTY( VisibleAnywhere, Category = "Collisions" )
	UStaticMeshComponent* m_pcCollisionMesh;
	// The amount of damage that this bullet deals
	UPROPERTY( VisibleAnywhere, Category = "Damage" )
	float m_fDamage;

	UPROPERTY ( VisibleAnywhere , Category = "Travel Speed" )
	float m_fTravelSpeed;

	//Number of times the projectile will bounce from the owner actor
	int	m_iMaxNumberOfBounces;

	//Number of times has bounced
	int m_iBounceCount;

	// The rebound speed multiplier
	float m_fReboundSpeedMultiplier;

	// The current traveling direction
	EBounceDirection m_eBounceDir;

	// Pointer to the actor that is the original target (e.g. player)
	UPROPERTY()
	AActor* m_pcOriginalTargetActor;

	// Pointer to the current target (for rebounds this will swap between the owner actor and player)
	UPROPERTY()
	AActor* m_pcCurrentTargetActor;

	// Max angle addition per frame
	float m_fMaxAngleDeltaPerUpdate;

	// How long does this bullet stun the player on hit?
	float m_fStunDuration;

	// Does projectile cause player to need to escape entrapment
	bool m_bInputDelimitedTrap;

	// End: Data Members
	//////////////////////////////////////////////////////////////////////////

	// The function called when this object overlaps another
	UFUNCTION()
	void OnOverlap( UPrimitiveComponent* pHitComponent, AActor* pOtherActor,
		UPrimitiveComponent* pOtherComponent, int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rHit );

	// To be called on a timer - update the travel direction of the projectile towards the current target
	// If the current target is null, no update occurs.
	void CBFollowTarget();
};
