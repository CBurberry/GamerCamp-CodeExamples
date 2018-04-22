// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FadeComponent.generated.h"


UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITEBOXPROJECT_API UFadeComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	//The mesh actor to which the component is applied
	UPROPERTY()
	class AStaticMeshActor* m_pcParentMesh;

	//The pre-existing material (this component assumes there is only 1 material present)
	UPROPERTY()
	class UMaterial* m_pcBaseMaterial;

	//The translucent material that replaces the base material while in camera focus
	UPROPERTY()
	class UMaterialInstanceDynamic* m_pcFadeMaterialMID;

	//Boolean to check if the mesh is currently transparent / translucent
	bool m_bIsMeshHidden;

	//The current fade value
	float m_fCurrentFade;

	//The time elapsed between this frame and the last
	float m_fDeltaTime;

	//The rate at which fade is applied to the fade material while intersecting camera view
	float m_fFadeRate;

public:	
	// Sets default values for this component's properties
	UFadeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, Category = "Curve", DisplayName = "Fade Curve")
	class UCurveFloat* m_pcFadeCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Materials", DisplayName = "Fade Material")
	UMaterial* m_pcFadeMaterial;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Function to toggle visiblity
	void ToggleHideMesh( bool bIsHidden );

	//Getter
	bool IsMeshHidden();
	
};
