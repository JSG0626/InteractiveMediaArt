// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Delegates/DelegateCombinations.h"
#include "ExhibitionGameInstance.generated.h"

USTRUCT(BlueprintType)
 struct FRoomInfo
 {
	 GENERATED_BODY()

	 //UPROPERTY(BlueprintReadOnly)
	 //FString roomName;
	 UPROPERTY(BlueprintReadOnly)
	 FString hostName;
	 //UPROPERTY(BlueprintReadOnly)
	 //int32 maxPlayerCount;
	 //UPROPERTY(BlueprintReadOnly)
	 //int32 currentPlayerCount;
	 //UPROPERTY(BlueprintReadOnly)
	 //int32 pingMS;

	 int32 index;

	 FString ToString()
	 {
		return FString::Printf(TEXT("%d) [%s]"), index, /**roomName,*/ *hostName/*, currentPlayerCount, maxPlayerCount, pingMS*/);
	 }
 };

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchSignature, const struct FRoomInfo&, info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionSearchComplete);

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
	// ����� ��û -> UI���� ȣ��
	void CreateMySession(/*FString roomName, int32 playerCount*/);

	UFUNCTION()
	// ����� ����
	void OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// ã�� ���� ���
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	// �� ã�� ��û -> UI���� ȣ��
	UFUNCTION()
	void FindSessions();

	// �� ã�� ����
	void OnMyFindSessionsCompleteDelegates(bool bWasSuccessful);

	// �� ���� ��û
	UFUNCTION()
	void JoinSession(int32 index);

	// �� ���� ����	
	void OnMyJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type);

	// �� ���� ��û -> UI���� ȣ��
	void ExitSession();

	UFUNCTION(Server, Reliable)
	void ServerRPCExitSession();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCExitSession();

	// �� ���� ����	
	void OnMyDestroySessionComplete(FName SessionName, bool bWasSuccessful);
};
