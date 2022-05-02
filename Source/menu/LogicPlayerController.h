// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FightInput.h"
#include "LogicPlayerController.generated.h"

UCLASS()
class MENU_API ALogicPlayerController : public APlayerController
{
  GENERATED_BODY()

private:
  int playerNumber;
  bool readiedUp;
  AFightInput* input;
  AFightInput* opponentInput;

public:
  ALogicPlayerController();

  UFUNCTION (Server, Reliable)
  void ServerPostLogin(int playerNumber_);
  UFUNCTION (Client, Reliable)
  void ClientPostLogin(int playerNumber_);
  void BeginPlay();

  UFUNCTION (Server, Reliable)
  void ServerReadyUp();

  void Tick(float deltaSeconds);

  UFUNCTION (Server, Reliable)
  void ServerButtons(int targetFrame);
  UFUNCTION (Server, Reliable)
  void ServerButtons1(int targetFrame);
  UFUNCTION (Server, Reliable)
  void ServerButtons2(int targetFrame);
};
