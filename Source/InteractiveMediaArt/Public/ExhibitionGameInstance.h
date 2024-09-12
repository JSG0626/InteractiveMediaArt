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
	
	/** �¶��� ���� �������̽� */
	IOnlineSessionPtr SessionInterface;

	UPROPERTY()
	FString MySessionName = FString("Lee Session");
	
	UFUNCTION()
	// ����� ��û
	void CreateMySession(/*FString roomName, int32 playerCount*/);

	UFUNCTION()
	// ����� ����
	void OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful);

//	// ���� ���� �Լ���
//	void FindOrCreateSession(); //���� ã��
//	void OnFindSessionsComplete(bool bWasSuccessful);//���� �����Ϸ�
//	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
//	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
//	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
//protected:
//
//
//
//protected:
//	
//	/** ���� �˻� ��ü */
//	TSharedPtr<FOnlineSessionSearch> SessionSearch;

};
