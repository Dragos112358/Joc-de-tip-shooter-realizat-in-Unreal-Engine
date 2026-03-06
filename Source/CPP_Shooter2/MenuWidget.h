// MyMenuWidget.h
#pragma once

#include "Widgets/SCompoundWidget.h"

class SMyMenuWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMyMenuWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FReply OnPlayClicked();
    FReply OnStoreClicked();
    FReply OnSettingsClicked();
    FReply OnAchievementsClicked();
};
