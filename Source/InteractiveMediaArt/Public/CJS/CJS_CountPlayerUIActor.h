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


    // 멀티플레이 카메라와 위치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
    ACameraActor* Art1_Multi_TargetCamera;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
    FTransform Art1_Multi1_TargetTransform;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
    FTransform Art1_Multi2_TargetTransform;
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
    TArray<FTransform> TargetTransforms;


    // Reference to UI widget
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UCJS_CountPlayerUI> WBP_CountPlayerUI;
    UCJS_CountPlayerUI* CountPlayerUI;


    // Save Clicked Player Info
	UPROPERTY()
	TArray<ACJS_LobbyPlayer*> ClickedPlayers;

    // The current number of players
    UPROPERTY(ReplicatedUsing=OnRep_CurPlayer, VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    int32 CurPlayer = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    int32 MaxPlayer = 2;


    // Replicated function called when CurPlayer is updated
    UFUNCTION()
    void OnRep_CurPlayer();

    // Function to add player count (only callable on the server)
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRPC_AddPlayerNum(ACJS_LobbyPlayer* Player, int32 AddNum);
    bool ServerRPC_AddPlayerNum_Validate(ACJS_LobbyPlayer* Player, int32 AddNum);
    UFUNCTION(Server, Reliable)
    void ServerRPC_RemovePlayerNum(ACJS_LobbyPlayer* Player, int32 RemoveNum);

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
