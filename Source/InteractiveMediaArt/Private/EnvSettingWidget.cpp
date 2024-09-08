// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvSettingWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"

void UEnvSettingWidget::NativeConstruct()
{
	Path_Button->OnClicked.AddDynamic(this, &UEnvSettingWidget::SetPath);
}

void UEnvSettingWidget::SetPath()
{
}
