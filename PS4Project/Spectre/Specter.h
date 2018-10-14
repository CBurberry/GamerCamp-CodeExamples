// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "Specter.generated.h"

//fwd dcls
class AASMeleeEnemy;
class ABeamCollisionTestActor;
class ASpecterAIController;
class AEchoesCharacter;
class UAIHealthWidgetComponent;
class UParticleSystemComponent;
class UPawnSensingComponent;
class UBehaviorTree;
class UProjectileEmitter;
class USphereComponent;

//delegate to trigger flinching animation montages for boss
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FBossTakingDamage );

//delegate to trigger firing projectile animation montage for boss
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FBossFiringProjectile );

//delegate to trigger the appearance of the shield 
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FBossShieldActivated );

//delegate to trigger the disappearance of the shield 
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FBossShieldDeactivated );

UCLASS()
class ECHOES_API ASpecter : public ACharacter
{
	GENERATED_BODY()

private:
	// The Component which is used for the "seeing" sense of the AI
    UPROPERTY(VisibleAnywhere, Category = "AI|Boss")
    UPawnSensingComponent*			m_pcPawnSensingComp;

	// The Behavior Tree of the Character
	UPROPERTY(EditAnywhere, Category = "AI|Boss", DisplayName = "Behaviour Tree")
	UBehaviorTree*					m_pcBehaviorTree;

	// Specter max health
	UPROPERTY( EditAnywhere, Category = "AI|Boss", DisplayName = "Max Health" )
	float							m_fMaxHealth;

	// Specter current
	float							m_fCurrentHealth;

	//The distance the enemies can see the player
	UPROPERTY( EditAnywhere, Category = "AI|Boss", DisplayName = "Sight Radius" )
	float							m_fSightRadius;

	// The vision to the right and left of specter
	UPROPERTY( EditAnywhere, Category = "AI|Boss", DisplayName = "Peripheral  Vision Angle" )
	float							m_fVision;

	// The movement speed of specter
	UPROPERTY(EditAnywhere, Category = "AI|Boss", DisplayName = "Movement Speed")
	float							m_fMovementSpeed;

	// The time that has passed since sensing th player
	float							m_fLastSeenTimePassed;

	float							m_fResetSeenTime;

	// Attack target of specter
	AEchoesCharacter*				m_pcTargetPlayer;

	// Current controller of specter
	ASpecterAIController*			m_pcSpectreController;

	// Beam particle emitter
	UPROPERTY( EditDefaultsOnly )
	UParticleSystemComponent*		m_pcBeamPE;

	// Stun particle emitter
	UPROPERTY( EditDefaultsOnly )
	UParticleSystemComponent*		m_pcStunPE;

	// Beam actor class type (with capsule collision)
	UPROPERTY( EditDefaultsOnly, Category = "AI|Boss" )
	TSubclassOf<AActor>				m_pcBeamActorClasstype;

	// Beam actor instance (with capsule collision)
	UPROPERTY( Transient )
	ABeamCollisionTestActor*		m_pcBeamActorInstance;

	UPROPERTY( Transient, EditDefaultsOnly, Category = "AI|Boss" )
	TSubclassOf<UUserWidget>		m_pcBossHPWidgetClass;

	UPROPERTY( Transient )
	UUserWidget*					m_pcBossHPWidgetInstance;

	//Handles the time it takes for the Beam to be reset
	FTimerHandle					m_fthBeamReset;

	//Timer delegate for continuous beam attack
	FTimerDelegate					m_ftdContinuousBeamCaller;

	//Timer handle for continuous beam attack
	FTimerHandle					m_fthContinousBeamTimer;

	//Timer delegate for delay between parallel crystal trap spawns & beam collision actor spawn
	FTimerDelegate					m_ftdContinuousBeamDelayCaller;

	//Timer handle for continuous beam attack delay
	FTimerHandle					m_fthContinousBeamDelayTimer;

	//Timer handle for repeated crystal spawns along a line
	FTimerHandle					m_fthSpawnCrystalLinesTimer;

	//Timer delegate for repeated crystal spawns along a line
	FTimerDelegate					m_ftdSpawnCrystalLinesCaller;

	//Starting time of the beam attack for timer delegate comparisons
	float							m_fTimeOfContinuousBeamStart;

	//Start with shield active?
	UPROPERTY( EditDefaultsOnly, Category = "AI|Boss" )
	bool							m_bStartWithShieldActive;

	//Is the shield currently active
	bool							m_bIsShieldActive;

	//Is the boss dead (boolean for animation)
	bool							m_bIsDead;

	//Health regened per second (while player is not detected)
	UPROPERTY( EditDefaultsOnly, Category = "AI|Boss" )
	float							m_fHPRegenPerSecond;

protected:
	// Sphere component for shield collision
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite )
	USphereComponent*				m_pcShieldCollider;

	// Called when the game starts or when spawned
	virtual void	BeginPlay		() override;

	// Called every frame
	virtual void	Tick			( float fDeltaTime ) override;

	// Function that allows the damage to take damage and process it accordingly
	virtual float	TakeDamage		( float fDamageAmount, struct FDamageEvent const&  rsDamageEvent,
												  AController* pcEventInstigator, AActor* pcDamageCauser ) override;

	//Called to check if the player is still visible (used in tick & for Reset())
	void			PlayerTargetReset();

	//Function for all collected variable setting / function calls to end the boss sequence
	void			DeathHandling();

public:
	// The component that emits/fires projectiles at Player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Emitter")
	UProjectileEmitter* m_pcProjectileEmitter;

	// Sets default values for this character's properties
	ASpecter();

	// Triggered by pawn sensing component when a pawn is spotted
	// When using functions as delegates they need to be marked with UFUNCTION()
	UFUNCTION()
	void			OnSeePlayer(APawn* Pawn);

	//For the Controller to get the tree we set up in BP
	UBehaviorTree * GetBehaviourTree();

	// Traps the player between lines of crystals
	UFUNCTION(BlueprintCallable)
	void			ParallelTrapPlayer(float fBeamDuration, float fDamage, float fDelay = 0.0f, float fTrapWidth = 500.0f, float fBeamWidth = 200.0f, float fCrystalSpacing = 150.0f, float fInterSpawnDelay = 0.15f, float fParallelTrapLength = 1500.0f);

	// Simple beam fire at a location, aim at player determines if the target location should be ignored and player location checked after delay
	UFUNCTION( BlueprintCallable )
	void			SimpleBeamFire( FVector sTargetLocation, float fDamage, float fAttackDuration, bool bAimAtPlayer = false );

	// Sweep beam fire
	UFUNCTION( BlueprintCallable )
	void			SweepBeamFire( FRotator sStartRotation, FRotator sEndRotation, float fDamage, float fAttackDuration, float fRotationDuration, float fBeamLength, float fRotationStartDelay );

	// Attacks player with a continuous beam that changes position along a line, sets timer delegate / handler
	// N.B. uses member variable to track elapsed time, won't work if several need to be active at once.
	UFUNCTION( BlueprintCallable )
	void			CBAttackWithBeamAlongLine( FVector sStartPosition, FVector sEndPosition, float fDamage, float fBeamWidth = 200.0f, float fAttackDuration = 2.0f );

	// Called by timer delegate to update temporary actor position
	UFUNCTION()
	void			CBUpdateContinuousBeamAttack( FVector& sStartPosition, FVector& sEndPosition, float fAttackDuration );

	// Called by timer delegate to update spawning of crystal trap
	UFUNCTION()
	void			CBUpdateCrystalTrapSpawning( FVector& sTrapStartLeft, FVector& sTrapEndLeft, FVector& sTrapStartRight, FVector& sTrapEndRight, float fCrystalSpacing, float fSpawnDelay );

	// Custom-use version of the beam attack from parallel trap (re-uses CBAttackWithBeamAlongLine)
	// Takes the distance from self as the starting point in the direction of the target, 
	// Fires the beam as far as the beam length provided (interpolates position over duration)
	UFUNCTION( BlueprintCallable )
	void			FireBeamAtTargetLocation( float fDistanceFromSelf, float fBeamLength, FVector sTargetLocation, float fDamage, float fBeamWidth = 200.0f, float fAttackDuration = 2.0f, bool bAimAtPlayer = false );

	// Custom-use version of the beam attack from parallel trap (re-uses CBAttackWithBeamAlongLine)
	// Same as FireProgressiveBeamAtTargetLocation for all variables except angle.
	// Beam is fired at the given yaw angle (local angles) with given length & duration.
	UFUNCTION( BlueprintCallable )
	void			FireBeamAtGivenAngle( float fDistanceFromSelf, float fBeamLength, float fYawAngle, float fDamage, float fBeamWidth = 200.0f, float fAttackDuration = 2.0f );

	// Allows blueprint access to the current health value of the boss
	// Return: (float) The current amount of health points that the boss has
	UFUNCTION( BlueprintCallable )
	float GetCurrentBossHealth() { return m_fCurrentHealth; };

	// Allows blueprint access to the max health value of the boss
	// Return: (float) The max amount of health points that the boss can have
	UFUNCTION( BlueprintCallable )
	float GetMaxBossHealth() { return m_fMaxHealth; };

	//End the beam Attack
	void			StopAttackWithBeam();

	// Reset this specter-based entity
	// Reset health, timers and player detection
	virtual void	Reset();

	//Set the boss bb variable & show particle effects
	UFUNCTION( BlueprintCallable )
	void			BecomeStunned( float fDuration );

	//Unset the boss bb variable & hide particle effects
	void			CBEndStun();

	//Is the shield currently active?
	UFUNCTION( BlueprintPure )
	bool			IsShieldActive() { return m_bIsShieldActive; }

	//Set shield activated/de-activated
	UFUNCTION( BlueprintCallable )
	void			SetShieldActive( bool bIsActive );

	//Getter for is the boss dead (animation)
	UFUNCTION( BlueprintPure, Category = "Boss Animation" )
	bool			IsBossDead() { return m_bIsDead; }

	//Getter for is the player currently in sight (animation)
	UFUNCTION( BlueprintPure, Category = "Boss Animation" )
	bool			IsPlayerInSight();

	//Getter for if the boss is currently stunned
	UFUNCTION( BlueprintPure, Category = "Boss Animation" )
	bool			IsBossStunned();

	//Getter for if a beam is currently active (animation)
	UFUNCTION( BlueprintPure, Category = "Boss Animation" )
	bool			IsBeamCurrentlyActive();

	//Disable Boss HP bar to be called during animation
	UFUNCTION( BlueprintCallable, Category = "Boss Animation" )
	void			DisableBossUI();

	//Delegate for boss taking damage (animation use)
	UPROPERTY( BlueprintAssignable, Category = "Boss Animation" )
	FBossTakingDamage m_dBossTakingDamage;

	//Delegate for projectile being fired (animation use)
	UPROPERTY( BlueprintAssignable, Category = "Boss Animation" )
	FBossFiringProjectile m_dFiringProjectile;

	//Delegate for shield being activated (animation use)
	UPROPERTY( BlueprintAssignable, Category = "Boss Animation" )
	FBossShieldActivated m_dShieldActivated;

	//Delegate for shield being deactivated (animation use)
	UPROPERTY( BlueprintAssignable, Category = "Boss Animation" )
	FBossShieldDeactivated m_dShieldDeactivated;
};
