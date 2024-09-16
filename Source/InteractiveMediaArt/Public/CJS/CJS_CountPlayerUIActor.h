// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CJS_CountPlayerUIActor.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ACJS_CountPlayerUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACJS_CountPlayerUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// 작품 1의 체험 인원 표시 UI
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf <class UUserWidget> WBP_CountPlayerUI;
	//UPROPERTY()
	//class UCJS_CountPlayerUI* CountPlayerUI;

	//UPROPERTY(ReplicatedUsing = AddPlayerNum, EditDefaultsOnly, Category = "PlayerNum") // 현재 참여 인원 변수 동기화
	//int32 CurPlayer;
	//UPROPERTY(EditDefaultsOnly, Category = "PlayerNum")
	//int32 MaxPlayer = 2;

	//// Replicated function called when CurPlayer is updated
 //   UFUNCTION()
 //   void OnRep_CurPlayer();
	//UFUNCTION()
	//void AddPlayerNum();
	//void SubPlayerNum();
	//void InitCountPlayerUiActor();

	//int32 GetCurPlayerNum();
	//void SetCurPlayerNum(int32 addNum);

	//// RPC
	//UFUNCTION(Server, Reliable)
	//void ServerRPC_ChangePlayerNum(const int32 playerNum);

	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastRPC_ChangePlayerNum(const int32 playerNum);


	// The current number of players
    UPROPERTY(ReplicatedUsing=OnRep_CurPlayer, VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    int32 CurPlayer = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    int32 MaxPlayer = 2;

    // Reference to UI widget
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UCJS_CountPlayerUI> WBP_CountPlayerUI;
    UCJS_CountPlayerUI* CountPlayerUI;

	UPROPERTY()
	TArray<ACJS_LobbyPlayer*> ClickedPlayers;

    // Replicated function called when CurPlayer is updated
    UFUNCTION()
    void OnRep_CurPlayer();

    // Function to add player count (only callable on the server)
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRPC_AddPlayerNum(ACJS_LobbyPlayer* Player, int32 AddNum);
	//void ServerRPC_AddPlayerNum(int32 AddNum);
    //bool ServerRPC_AddPlayerNum_Validate(int32 AddNum);
    bool ServerRPC_AddPlayerNum_Validate(ACJS_LobbyPlayer* Player, int32 AddNum);

	// Function to add player Info
	//UFUNCTION(Server, Reliable)
    //void ServerRPC_AddPlayerInfo(ACJS_LobbyPlayer* Player);


	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastRPC_UpdatePlayerNum(int32 NewPlayerNum);

    // Initialize the UI Actor
    void InitCountPlayerUiActor(int32 curPlayer);

    // Override to ensure variable replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void PrintNetLog();
	void FindOwner();


private:
    // Function to update CurPlayer locally
    void UpdatePlayerNum(int32 NewPlayerNum);
	void StartInteractiveExperience();
};
