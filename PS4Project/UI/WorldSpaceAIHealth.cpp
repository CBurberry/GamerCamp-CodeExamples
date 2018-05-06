// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSpaceAIHealth.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "ProgressBar.h"
#include "WidgetTree.h"
#include "Runtime/UMG/Public/Components/ProgressBar.h"

UWorldSpaceAIHealth::UWorldSpaceAIHealth ( const FObjectInitializer & objectInitializer )
	: Super( objectInitializer )
	, m_sWidgetSize ( FVector( 100.0f, 15.0f ) )
	, m_pcEnemyHP	( nullptr )
{
}

bool UWorldSpaceAIHealth::Initialize()
{
	Super::Initialize();

	m_pcEnemyHP = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("Enemy HP Bar"));
	m_pcEnemyHP->SetFillColorAndOpacity(FLinearColor::Red);
	m_pcEnemyHP->SetPercent(100.f);

	//Add CanvasPanelSlot to Canvas to create custom size
	UCanvasPanelSlot* pcCanvasSlot = Cast<UCanvasPanelSlot>( m_pcRootWidget->Slot );

	if ( pcCanvasSlot )
	{
		pcCanvasSlot->SetSize( m_sWidgetSize );
	}

	// Add Progress Bar as child of rootWidget
	m_pcRootWidget->AddChild( m_pcEnemyHP );

	// Add CanvasPanelSlot to enemyHP
	if ( m_pcEnemyHP != nullptr )
	{
		UCanvasPanelSlot* pcEnemyHPSlot = Cast<UCanvasPanelSlot> ( m_pcEnemyHP->Slot );

		// If the slot exists, set the size of the panel
		if ( pcEnemyHPSlot )
		{
			pcEnemyHPSlot->SetSize( m_sWidgetSize );
		}
	}
	return true;
}

void UWorldSpaceAIHealth::SetBarPercentage ( float fValue ) 
{
	fValue = FMath::Clamp(fValue, 0.0f, 1.0f);
	m_pcEnemyHP->SetPercent(fValue);
	
}