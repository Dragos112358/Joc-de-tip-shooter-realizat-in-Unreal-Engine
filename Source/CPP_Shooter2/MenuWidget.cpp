// MyMenuWidget.cpp
#include "MenuWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

void SMyMenuWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .Padding(5)
                [
                    SNew(SButton)
                        .Text(FText::FromString("Play"))
                        .OnClicked(this, &SMyMenuWidget::OnPlayClicked)
                ]
                + SVerticalBox::Slot()
                .Padding(5)
                [
                    SNew(SButton)
                        .Text(FText::FromString("Store"))
                        .OnClicked(this, &SMyMenuWidget::OnStoreClicked)
                ]
                + SVerticalBox::Slot()
                .Padding(5)
                [
                    SNew(SButton)
                        .Text(FText::FromString("Settings"))
                        .OnClicked(this, &SMyMenuWidget::OnSettingsClicked)
                ]
                + SVerticalBox::Slot()
                .Padding(5)
                [
                    SNew(SButton)
                        .Text(FText::FromString("Achievements"))
                        .OnClicked(this, &SMyMenuWidget::OnAchievementsClicked)
                ]
        ];
}

FReply SMyMenuWidget::OnPlayClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Play clicked"));
    return FReply::Handled();
}

FReply SMyMenuWidget::OnStoreClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Store clicked"));
    return FReply::Handled();
}

FReply SMyMenuWidget::OnSettingsClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Settings clicked"));
    return FReply::Handled();
}

FReply SMyMenuWidget::OnAchievementsClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Achievements clicked"));
    return FReply::Handled();
}
