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
	
	/** 온라인 세션 인터페이스 */
	IOnlineSessionPtr SessionInterface;

	UPROPERTY()
	FString MySessionName = FString("Lee Session");
	
	UFUNCTION()
	// 방생성 요청 -> UI에서 호출
	void CreateMySession(/*FString roomName, int32 playerCount*/);

	UFUNCTION()
	// 방생성 응답
	void OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// 찾을 방의 목록
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	// 방 찾기 요청 -> UI에서 호출
	void FindSessions();

	// 방 찾기 응답
	void OnMyFindSessionsCompleteDelegates(bool bWasSuccessful);

	UPROPERTY()
	FOnSessionSearchComplete OnSearchSessionCompleteDelegate;

	//FSearchSignature OnSearchSignatureCompleteDelegate;

	//int32 SessionSearchIndex;


	// 방 입장 요청
	void JoinSession(int32 index);

	// 방 입장 응답
	void OnMyJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type);

	// 방 퇴장 요청 -> UI에서 호출
	void ExitSession();

	UFUNCTION(Server, Reliable)
	void ServerRPCExitSession();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCExitSession();

	// 방 퇴장 응답
	void OnMyDestroySessionComplete(FName SessionName, bool bWasSuccessful);
};
