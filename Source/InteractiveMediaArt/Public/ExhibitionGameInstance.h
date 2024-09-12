// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ExhibitionGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UExhibitionGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
	/** 柯扼牢 技记 牢磐其捞胶 */
	IOnlineSessionPtr SessionInterface;

	UPROPERTY()
	FString MySessionName = FString("Lee Session");
	
	UFUNCTION()
	// 规积己 夸没
	void CreateMySession(/*FString roomName, int32 playerCount*/);

	UFUNCTION()
	// 规积己 览翠
	void OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful);

//	// 技记 包访 窃荐甸
//	void FindOrCreateSession(); //技记 茫扁
//	void OnFindSessionsComplete(bool bWasSuccessful);//技记 积己肯丰
//	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
//	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
//	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
//protected:
//
//
//
//protected:
//	
//	/** 技记 八祸 按眉 */
//	TSharedPtr<FOnlineSessionSearch> SessionSearch;

};
