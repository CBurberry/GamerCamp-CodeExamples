// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerStatsUserWidget.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "ProgressBar.h"
#include "WidgetTree.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "WhiteBoxProjectCharacter.h"
#include "TextBlock.h"


UPlayerStatsUserWidget::UPlayerStatsUserWidget(const FObjectInitializer& objectInitializer )
: Super						( objectInitializer )
, m_pcHPFill				( nullptr )
, m_pcEnergyFill			( nullptr )
, m_pcStaminaFill			( nullptr )
, m_pcBoxElementIcon		( nullptr )
, m_pcFireElementIcon		( nullptr )
, m_pcIceElementIcon		( nullptr )
, m_pcLightningElementIcon	( nullptr )
, m_bFirstElementSwapped	( false )
, m_bInitialised			( false )
{
	bCanEverTick = true;
}

bool UPlayerStatsUserWidget::Initialize() 
{
	if( !m_bInitialised )
	{
		m_bInitialised = true;

		Super::Initialize();

		//Get textures from path.
		UTexture2D* pcElementBoxTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/HUDElementIconBoxV2_UI" ) ) );
		UTexture2D* pcHPOutlineTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/HUDHPbaroutline_UI" ) ) );
		UTexture2D* pcEnergyOutlineTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/HUDEnergyOutline_UI" ) ) );
		UTexture2D* pcStaminaOutlineTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/HUDStaminaOutline_UI" ) ) );
		UTexture2D* pcHPBGTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/HUDHPbarbackground_UI" ) ) );
		UTexture2D* pcEnergyBGTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/HUDEnergyBackground_UI" ) ) );
		UTexture2D* pcStaminaBGTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/HUDStaminaBackground_UI" ) ) );
		UTexture2D* pcHPFillTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/HUDHPbarinside_UI" ) ) );
		UTexture2D* pcEnergyFillTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/HUDEnergyInside_UI" ) ) );
		UTexture2D* pcStaminaFillTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/HUDStaminaInside_UI" ) ) );

		//Set member variable textures.
		UTexture2D* pcIceElementTex = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/IceIconV2_UI" ) ) );
		UTexture2D* pcFireElementTex = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/FireIconV2_UI" ) ) );
		UTexture2D* pcLightningElementTex = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *FString( "/Game/HUD/PlayerStatus/EnergyIconV2_UI" ) ) );

		//Instantiate images for ElementBox, ElementIcon, HP/Energy/Stamina outlines.
		UImage* pcHPOutline = WidgetTree->ConstructWidget<UImage>( UImage::StaticClass(), TEXT( "HPOutline" ) );
		UImage* pcEnergyOutline = WidgetTree->ConstructWidget<UImage>( UImage::StaticClass(), TEXT( "EnergyOutline" ) );
		UImage* pcStaminaOutline = WidgetTree->ConstructWidget<UImage>( UImage::StaticClass(), TEXT( "StaminaOutline" ) );
		m_pcIceElementIcon = WidgetTree->ConstructWidget<UImage>( UImage::StaticClass(), TEXT( "ElementIcon_Ice" ) );
		m_pcFireElementIcon = WidgetTree->ConstructWidget<UImage>( UImage::StaticClass(), TEXT( "ElementIcon_Fire" ) );
		m_pcLightningElementIcon = WidgetTree->ConstructWidget<UImage>( UImage::StaticClass(), TEXT( "ElementIcon_Lightning" ) );
		m_pcBoxElementIcon = WidgetTree->ConstructWidget<UImage>( UImage::StaticClass(), TEXT( "ElementIcon" ) );


		//Instantiate progress bars for HP/Energy/Stamina
		m_pcHPFill = WidgetTree->ConstructWidget<UProgressBar>( UProgressBar::StaticClass(), TEXT( "HPFill" ) );
		m_pcEnergyFill = WidgetTree->ConstructWidget<UProgressBar>( UProgressBar::StaticClass(), TEXT( "EnergyFill" ) );
		m_pcStaminaFill = WidgetTree->ConstructWidget<UProgressBar>( UProgressBar::StaticClass(), TEXT( "StaminaFill" ) );

		//Set image properties & progress bar properties (first called is rendered last / on top)
		SetupUProgressBar( m_pcHPFill, pcHPBGTexture, pcHPFillTexture, FLinearColor::White, FVector2D( 180.0f, 55.0f ), FVector2D( 455.0f, 70.0f ) );
		SetupUProgressBar( m_pcEnergyFill, pcEnergyBGTexture, pcEnergyFillTexture, FLinearColor::White, FVector2D( 220.0f, 105.0f ), FVector2D( 390.0f, 60.0f ) );
		SetupUProgressBar( m_pcStaminaFill, pcStaminaBGTexture, pcStaminaFillTexture, FLinearColor::White, FVector2D( 178.0f, 150.0f ), FVector2D( 350.0f, 55.0f ) );
		SetupUImage( pcHPOutline, pcHPOutlineTexture, FVector2D( 180.0f, 55.0f ), FVector2D( 455.0f, 70.0f ) );
		SetupUImage( pcEnergyOutline, pcEnergyOutlineTexture, FVector2D( 220.0f, 105.0f ), FVector2D( 392.0f, 60.0f ) );
		SetupUImage( pcStaminaOutline, pcStaminaOutlineTexture, FVector2D( 178.0f, 150.0f ), FVector2D( 350.0f, 55.0f ) );
		SetupUImage( m_pcBoxElementIcon, pcElementBoxTexture, FVector2D::ZeroVector, FVector2D( 700.0f, 250.0f ) );
		SetupUImage( m_pcFireElementIcon, pcFireElementTex, FVector2D::ZeroVector, FVector2D( 700.0f, 250.0f ) );
		SetupUImage( m_pcIceElementIcon, pcIceElementTex, FVector2D::ZeroVector, FVector2D( 700.0f, 250.0f ) );
		SetupUImage( m_pcLightningElementIcon, pcLightningElementTex, FVector2D::ZeroVector, FVector2D( 700.0f, 250.0f ) );

		m_pcFireElementIcon->SetVisibility( ESlateVisibility::Hidden );
		m_pcIceElementIcon->SetVisibility( ESlateVisibility::Hidden );
		m_pcLightningElementIcon->SetVisibility( ESlateVisibility::Hidden );
	}

	return m_bInitialised;
}

void UPlayerStatsUserWidget::NativeTick ( const FGeometry& MyGeometry , float DeltaTime )
{
	Super::NativeTick ( MyGeometry, DeltaTime );
	AWhiteBoxProjectCharacter* pcPlayerCharacter = Cast<AWhiteBoxProjectCharacter>( UGameplayStatics::GetPlayerCharacter ( GetWorld (), 0 ) );

	//Set the percentages of the HUD progress bars if valid.
	if ( pcPlayerCharacter )
	{
		m_pcHPFill->SetPercent( pcPlayerCharacter->GetPlayerCurrentHealth() / pcPlayerCharacter->GetMaxHealth() );
		m_pcEnergyFill->SetPercent( pcPlayerCharacter->GetPlayerCurrentMana() / pcPlayerCharacter->GetMaxMana() );
		m_pcStaminaFill->SetPercent( pcPlayerCharacter->GetCurrentStamina() / pcPlayerCharacter->GetMaxStamina() );
	}
}

void UPlayerStatsUserWidget::SetupUImage( UImage* pcImage, UTexture2D* pcAsset, FVector2D sPosition, FVector2D sSize )
{
	if (sSize == FVector2D::ZeroVector)
	{
		sSize = FVector2D( pcAsset->GetImportedSize() );
	}

	if ( pcImage && m_pcRootWidget )
	{
		if ( pcAsset )
		{
			pcImage->SetBrushFromTexture( pcAsset );
		}
		else 
		{
			pcImage->SetVisibility( ESlateVisibility::Hidden );
		}
		m_pcRootWidget->AddChild( pcImage );
		UCanvasPanelSlot* pcOutlineSlot = Cast<UCanvasPanelSlot>( pcImage->Slot );
		pcOutlineSlot->SetPosition( sPosition );
		pcOutlineSlot->SetSize( sSize );
	}
}

void UPlayerStatsUserWidget::SetupUProgressBar( UProgressBar* pcProgressBar, UTexture2D* pcBGTex, UTexture2D* pcFillTex, FLinearColor sAppearance, FVector2D sPosition, FVector2D sSize )
{
	if (sSize == FVector2D::ZeroVector)
	{
		sSize = FVector2D( pcBGTex->GetImportedSize() );
	}

	if ( pcProgressBar && pcFillTex && pcBGTex )
	{
		pcProgressBar->WidgetStyle.BackgroundImage.SetResourceObject( pcBGTex );
		pcProgressBar->WidgetStyle.BackgroundImage.DrawAs = ESlateBrushDrawType::Image;
		pcProgressBar->WidgetStyle.FillImage.SetResourceObject( pcFillTex );
		pcProgressBar->WidgetStyle.FillImage.DrawAs = ESlateBrushDrawType::Image;
		pcProgressBar->FillColorAndOpacity = sAppearance;
		m_pcRootWidget->AddChild( pcProgressBar );
		UCanvasPanelSlot* pcProgressBarSlot = Cast<UCanvasPanelSlot>( pcProgressBar->Slot );
		pcProgressBarSlot->SetPosition( sPosition );
		pcProgressBarSlot->SetSize( sSize );
	}
}

void UPlayerStatsUserWidget::SetElement( EElementAttribute eSetElement )
{
	if( m_bInitialised )
	{
		m_pcIceElementIcon->SetVisibility( ESlateVisibility::Hidden );
		m_pcFireElementIcon->SetVisibility( ESlateVisibility::Hidden );
		m_pcLightningElementIcon->SetVisibility( ESlateVisibility::Hidden );
		switch( eSetElement )
		{
			case EElementAttribute::EElementAttribute_Ice:
				m_pcIceElementIcon->SetVisibility( ESlateVisibility::Visible );
				m_bFirstElementSwapped = true;
				break;
			case EElementAttribute::EElementAttribute_Fire:
				m_pcFireElementIcon->SetVisibility( ESlateVisibility::Visible );
				m_bFirstElementSwapped = true;
				break;
			case EElementAttribute::EElementAttribute_lightning:
				m_pcLightningElementIcon->SetVisibility( ESlateVisibility::Visible );
				m_bFirstElementSwapped = true;
				break;
			default:
				//Do nothing
				break;
		}

		if( m_bFirstElementSwapped && IsValid( m_pcBoxElementIcon ) )
		{
			m_pcBoxElementIcon->SetVisibility( ESlateVisibility::Hidden );
			m_pcBoxElementIcon->RemoveFromParent();
			m_pcBoxElementIcon = nullptr;
		}
	}
}
