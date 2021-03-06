// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Action.h"
#include "LogicMode.h"
#include "Button.h"
#include <optional>
#include <vector>
#include "FightInput.generated.h"

// UE doesn't support type aliases
#define int8 char

class intRingBuffer;

// ideally we'd only have one RingBuffer<T> class but unreal doesn't
// like templates and I don't want to figure out how to build it as an
// external library that can still be distributed to many platforms
// just yet.
class ButtonRingBuffer {
private:
  int n;
  int end;

public:
  std::vector<std::optional<enum Button>> v;
  void reserve(int size);
  void clear();

  void push(const std::optional<enum Button>& x);

  std::optional<enum Button>& last();

  std::optional<enum Button>& nthlast(int i);

  FString toString();
};

class intRingBuffer {
private:
  int n;
  int end;

public:
  std::vector<int> v;
  void reserve(int size);
  void clear();

  void push(int x);

  int last();

  int first();

  int nthlast(int i);

  FString toString();
};

enum class GuardLevel { High, Low, None };

// This class will decode input sequences and support replaying input
// in case of rollback. In the case that each move is triggered by a
// single button press, this is simply mapping the most recent button
// to a move. In the case of chorded moves or motion commands, we have
// to keep track of all buttons pressed over time.
UCLASS()
class MENU_API AFightInput : public AInfo {
  GENERATED_BODY()
private:
  int maxRollback;
  int n;

  // number of frames to "buffer" inputs. if there are no actions in
  // the latest frame to decode besides walking, then use inputs from
  // the latest frame within `buffer` frames away that results in an
  // action.
  int buffer;

  // artificial input delay. action() should decode an action based on
  // the inputs `delay` frames ago.
  int delay;

  int currentFrame;
  int needsRollbackToFrame;

  // for now, just allow one button at a time
  ButtonRingBuffer buttonHistory;
  // this assumes that the player cannot press opposite directions at
  // the same time
  ButtonRingBuffer directionHistoryX;
  ButtonRingBuffer directionHistoryY;

  enum LogicMode mode;

  int lastInputFrame;
  intRingBuffer latencyHistory;
  int avgLatency;
  int avgLatencyOther;

  bool is_button(const enum Button& b);
  // bool is_none(const Button& b);
  enum Button translateDirection(const enum Button& d, bool isOnLeft);
  std::optional<enum Button> translateDirection(std::optional<enum Button>& d, bool isOnLeft);
  // compute how far back in our history we have to look for the input
  // data for targetFrame
  int computeIndex(int targetFrame);

  // Make sure that AFightInput has some data for the new frame. We
  // will either do nothing or "predict" the input (assume nothing was
  // pressed or released).
  void ensureFrame(int targetFrame);

  // returns true if a sequence of `motion` inputs ends on `frame`
  bool checkMotionCommand(std::vector<enum Button>& motion, int n, int frame, bool isOnLeft);
  // return action using input `frame` frames ago as latest input
  HAction _action(HAction currentAction, int frame, bool isOnLeft, int actionFrame);

public:
  AFightInput();

  // initialize all member variables
  void init(int _maxRollback, int _buffer, int _delay);
  // clear all inputs and rollback state
  void reset();

  void setMode(enum LogicMode);

  // Returns the encoding given in `encoded' plus the button `b'
  // encoded into it
  static FString encodedButtonsToString(int8 e);
  static int8 encodeButton(enum Button b, int8 encoded=0);
  static int8 unsetButton(enum Button b, int8 encoded=0);
  static bool decodeButton(enum Button b, int8 encoded);

  // The player controller will call this function to say which
  // buttons were pressed and released on the given frame. frame is
  // the frame that the inputs should first appear. It is 1+ the frame
  // number stored in ALogic at the time that this function is called
  // by the player controller.
  void buttons(int8 buttonsPressed, int8 buttonsReleased, int targetFrame);

  UFUNCTION (Client, Reliable)
  void ClientButtons(int8 buttonsPressed, int8 buttonsReleased, int targetFrame, int avgLatencyOther_);

  // Returns the decoded action for the given targetFrame.
  HAction action(HAction currentAction, bool isOnLeft, int targetFrame, int actionStart);

  // guarding might not depend on the action but rather the inputs
  // (holding back or down-back), so we use a new method here. We
  // could also add a `guard' flag to some actions.
  enum GuardLevel isGuarding(bool isOnLeft, int targetFrame);

  int getCurrentFrame();
  bool needsRollback();
  int getNeedsRollbackToFrame();
  void clearRollbackFlags();
  int getAvgLatency() const;
  float getDesync() const;
  bool hasRecievedInputForFrame(int frame) const;
};
