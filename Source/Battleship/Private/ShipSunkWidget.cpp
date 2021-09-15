// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipSunkWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Internationalization/Text.h"

void UShipSunkWidget::NativeConstruct()
{
    Super::NativeConstruct();

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName("Canvas"));
	WidgetTree->RootWidget = Root;
	UWidget* root = GetRootWidget();

    ShipSunkText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ShipText"));
}