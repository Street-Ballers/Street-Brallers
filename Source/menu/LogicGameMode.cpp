// Fill out your copyright notice in the Description page of Project Settings.

#include "LogicGameMode.h"
#include "Logic.h"
#include "Action.h"

#define MYLOG(category, message, ...) UE_LOG(LogTemp, category, TEXT("ALogicGameMode (%s) " message), (GetWorld()->IsNetMode(NM_ListenServer)) ? TEXT("server") : TEXT("client"), ##__VA_ARGS__)

ALogicGameMode::ALogicGameMode() {
  // use our custom PlayerController class
  PlayerControllerClass = ALogicPlayerController::StaticClass();
  DefaultPawnClass = nullptr;
  SpectatorClass = nullptr;
  GameStateClass = AFightGameState::StaticClass();
  // setting this to null just causes it to fall back to APlayerState
  // PlayerStateClass = nullptr;
}

void ALogicGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) {
  Super::InitGame(MapName, Options, ErrorMessage);
  HAction::init();
  HCharacter::init();
  check(HAction(IActionIdle).character() == HChar1);
  check(HCharacter(IChar1).idle() == HActionIdle);
  playerCount = 0;
}

void ALogicGameMode::PreLogin(const FString& Options,
                              const FString& Address,
                              const FUniqueNetIdRepl& UniqueId,
                              FString& ErrorMessage) {
  if (playerCount > 1) {
    ErrorMessage = "Server is full";
    MYLOG(Warning, "SERVER IS FULL");
  }
  else {
    MYLOG(Display, "PRELOGIN SUCEEDED!");
  }
}

void ALogicGameMode::PostLogin(APlayerController* NewPlayer) {
  int playerNumber = playerCount++;
  MYLOG(Display, "PostLogin: player %i", playerNumber);
  if (GetWorld()->IsNetMode(NM_ListenServer)) {
    MYLOG(Display, "PostLogin: is on server");
  }
  else {
    MYLOG(Warning, "PostLogin: is on client");
  }
  ALogicPlayerController* c = Cast<ALogicPlayerController>(NewPlayer);
  c->ServerPostLogin(playerNumber);
  c->ClientPostLogin(playerNumber);
}