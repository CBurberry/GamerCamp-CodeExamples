// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "BeamCollisionTestActor.generated.h"

//fwd dcls
class UCapsuleComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USceneComponent;

/*	Beam Collision Test Actor Class
*	The purpose of this class is to test if a beam can have a dynamically resizing collision component (capsule)
*	This is to prevent the ghosting issue with the traveling box that was previously done for trap and execute.
*	It was found that that collision method was subpar for faster moving beams.

	Learned:
	*
*/

UCLASS()
class ECHOES_API ABeamCollisionTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeamCollisionTestActor();

	// Called every frame
	virtual void Tick( float fDeltaTime ) override;

	//Fixed length beam with a delay before the collider becomes active
	//Collider delay is the time before the capsule collider is activated (for beam startup)
	//bAimAtPlayer overrides the target location with the players location after delay
	UFUNCTION( BlueprintCallable )
	void FixedBeamFire( float fBeamLength, float fColliderDelay = 0.75f, float fDuration = 1.5f, bool bAimAtPlayer = false );

	// Rotate the actor over time
	UFUNCTION( BlueprintCallable )
	void RotateBeamActor( FRotator sAddedRotation, float fRotationDelay = 1.0f, float fDuration = 0.75f );

	//Move the target actor to a given location ( & update the capsule to match )
	UFUNCTION( BlueprintCallable )
	void SetTargetLocation( FVector sTargetLocation, bool bAdjustBeamLength = false );

	//Setter for beam damage
	UFUNCTION( BlueprintCallable )
	void SetBeamDamage( float fDamage ) { m_fDamageDealt = fDamage; }

	//Getter for if this beam is currently active
	UFUNCTION( BlueprintPure )
	bool IsBeamActive();

protected:
	enum ECapsulePoint
	{
		Start,
		End
	};

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The collision component
	UPROPERTY( EditAnywhere )
	UCapsuleComponent* m_pcCapsuleComponent;

	// The particle system to be displayed
	UPROPERTY( EditAnywhere )
	UParticleSystem* m_pcPSTemplate;

	// The particle system component that will be displaying the template
	UPROPERTY( EditAnywhere )
	UParticleSystemComponent* m_pcPSC;

	// The particle system that will be displaying the hitspark template
	UPROPERTY( EditAnywhere )
	UParticleSystem* m_pcHitsparkTemplate;

	// The particle system component that will be displaying the hitspark
	UPROPERTY( EditAnywhere )
	UParticleSystemComponent* m_pcHitsparkPSC;

	//Callback method that will update fixed beam
	UFUNCTION()
	void CBFixedBeamUpdate( float fColliderDelay, float fDuration );

	// Method that begins the resize process.
	// Parameters are the target size values of the capsule collider & duration of resize operation.
	UFUNCTION( BlueprintCallable )
	void BeginColliderResizing( float fTargetCapsuleHalfHeight = 1000.0f, float fUpdateDelay = 0.1f, float fDuration = 3.f );

	// Callback method that will update the size of the collider
	UFUNCTION()
	void CBUpdateResizingCollider( float fTargetCapsuleHalfHeight, float fDuration );

	UFUNCTION()
	void CBUpdateRotation( FRotator sAddedRotation, float fRotationDelay, float fDuration );

	// Function that is called when capsule collider overlaps another entity
	UFUNCTION()
	void OnCapsuleOverlap( UPrimitiveComponent* pcHitComponent, AActor* pcOtherActor,
		UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex, bool bFromSweep, const FHitResult & rSweepResult );

	// Calculate where the start point of the beam is (useful for post re-size of capsule)
	FVector GetCapsulePoint( ECapsulePoint ePointToReturnLocation );

private:
	//Handles the time it takes for the Fixed Beam to be displayed
	FTimerHandle					m_fthFixedBeamHandle;

	//Timer delegate for the Fixed Beam to be displayed
	FTimerDelegate					m_ftdFixedBeamDelegate;

	//Handles the time it takes for the Beam to be reset
	FTimerHandle					m_fthResizeHandle;

	//Timer delegate for continuous beam attack
	FTimerDelegate					m_ftdResizeDelegate;

	//Handles the time it takes for actor rotation
	FTimerHandle					m_fthRotationHandle;

	//Timer delegate for actor rotation
	FTimerDelegate					m_ftdRotationDelegate;

	//World time when fixed beam started
	float							m_fFixedBeamStartWorldTime;

	//World time when resize started
	float							m_fResizeStartWorldTime;

	//World time when rotation started
	float							m_fRotationStartWorldTime;

	//Initial capsule radius (before resize operation)
	float							m_fPreCapsuleRadius;

	//Initial capsule half-size (before resize operation)
	float							m_fPreCapsuleHSize;

	//Pre-callback actor rotation for rotation callback
	FRotator						m_sPreActorRotation;

	//Delay for the end of beam (should not instantly disappear)
	UPROPERTY( EditAnywhere )
	float							m_fEndOfBeamDelay;

	//Beam radius for all beams
	UPROPERTY( EditAnywhere )
	float							m_fBeamRadius;

	//Beam length for currently firing beam
	float							m_fFiredBeamMaxLength;

	//Damage the beam applies
	float							m_fDamageDealt;

	//The scene component (root) to which the beam will attach to
	UPROPERTY( VisibleAnywhere )
	USceneComponent*				m_pcBeamStartPoint;

	//The actor to which the beam (end point) will attach to
	UPROPERTY( VisibleAnywhere )
	AActor*							m_pcTargetAttachmentActor;

	//Create attachment actor
	void CreateAttachmentActor();

	//Toggle collider
	void ToggleBeamCollision( bool bActive );

	//Reset beam
	void ResetBeam();

	//Beam hit check
	//Resize beam & collider to match the first hit target with raycast
	void BeamHitUpdate();

	//Toggle beam and hitspark vfx
	void ToggleParticleSystems( bool bActive );

	//Update the target actor location (and particle effect)
	void SetTargetActorLocation( FVector sTargetLocation );
};
