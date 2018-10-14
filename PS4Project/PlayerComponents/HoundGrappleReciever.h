// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HoundGrappleReciever.generated.h"

//Fwd dcls
class UInputComponent;
class UUserWidget;

/* 
*	Hound Grapple Reciever is the actor component that handles an AI hound enemy behaviour for grappling.
*	This component is responsible for detecting attachment, player input to remove hound from parent 
*	& all related balancing stats that can be edited in editor for the actor this component is attached to.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOES_API UHoundGrappleReciever : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHoundGrappleReciever();

public:	
	// Called every frame
	virtual void TickComponent(float fDeltaTime, ELevelTick eTickType, FActorComponentTickFunction* psThisTickFunction) override;

	//Adds a binding for right thumbstick
	UFUNCTION( BlueprintCallable, Category = "AI Tests" )
	void AddRightStickBinding( UInputComponent* pcInputComponent );

	//Can this reciever be attached to (is back socket in use?)
	UFUNCTION( BlueprintPure, Category = "AI Tests" )
	bool CanBeAttachedTo();

	//Check on the parent actor mesh if a socket with the required FName exists
	UFUNCTION( BlueprintPure, Category = "AI Tests" )
	bool DoesBackSocketExist();

	//Attach the passed actor to the parent
	UFUNCTION( BlueprintCallable, Category = "AI Tests" )
	void AttachHound( AActor* pcEnemyToAttach );

	//Force the grapple reception behavior without a hound
	//A callback can be passed for the behavior that should happen when detaching (in addition to regular detachment)
	void ForceGrappleBehaviour();

	//Removes removal progress bar widget from viewport and restores character speed.
	UFUNCTION( BlueprintCallable, Category = "AI Tests" )
	void DetachHound();

	//Getter for Removal Progression
	UFUNCTION( BlueprintPure, Category = "AI Tests" )
	float GetRemovalProgression() { return m_fRemovalProgression; }


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//The bp widget class that appears to show progression in removing AI
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "AI Tests", DisplayName = "Progress Widget" )
	TSubclassOf<UUserWidget> m_pcProgressWidgetClass;

	//The bp widget instance that appears to show progression in removing AI
	UPROPERTY()
	UUserWidget* m_pcProgressWidgetInstance;

	//Name of the socket that the AI tries to attach to
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "AI Tests", DisplayName = "Attachment Socket Name" )
	FName m_cAttachmentSocketName;

private:
	//variable to keep track of last y axis value on right thumb stick
	float m_fRightThumbStickLastXAxisValue;

	//variable to keep track of the last dualshock accelerator value
	float m_fDualshockLastAcceleratorValue;

	//Variable that indicates if there is something equiped to the socket (for use by the AI on 'grab')
	bool m_bIsBackSocketInUse;

	//How far the has player progressed in [0-1] scale to get the hound off.
	float m_fRemovalProgression;

	//How much RemovalProgression increases for every key press
	UPROPERTY( EditAnywhere, Category = "AI Tests", DisplayName = "Progression Per Keypress" )
	float m_fProgressionPerKeypress;

	//Decrease rate of the RemovalProgression variable.
	UPROPERTY( EditAnywhere, Category = "AI Tests", DisplayName = "Reduction Per Second" )
	float m_fReductionPerSecond;

	//Boolean to represent whether or not the target socket exists on parent mesh
	bool m_bDoesSocketExist;

	//Currently attached actor
	AActor* m_pcAttachedEnemy;

	//Threshold for thumbstick movement delta (if the different in thumbstick position in X axis is grater than this number, add to progression)
	UPROPERTY( EditAnywhere, Category = "AI Tests", DisplayName = "Thumbstick delta limit" )
	float m_fThumbstickDeltaLimit;

	//Threshold for accelerator movement delta (if the accelerator in thumbstick position in X axis is grater than this number, add to progression)
	UPROPERTY( EditAnywhere, Category = "AI Tests", DisplayName = "Accelerator delta limit" )
	float m_fAcceleratorDeltaLimit;

	//Adds removal progress bar widget to viewport and slows player
	UFUNCTION( BlueprintCallable, Category = "AI Tests" )
	void GetAIOff();

	//Decrement the removal progression based on deltatime (use in Tick)
	void DecrementRemovalProgression( float fDeltaTime );

	//Check for the given key - we"re not using input bindings here as will need to be reworked to gamepad.
	void UpdateRemovalProgression();

	//Check parent mesh for back socket - sets m_bDoesSocketExist
	//N.B if not found, this component will log error and cannot be attached to
	void FindParentAttachmentSocket();
};
