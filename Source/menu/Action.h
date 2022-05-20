#pragma once

#include "Button.h"
#include "Hitbox.h"
#include <optional>
#include <map>

UENUM(BlueprintType)
enum EAnimation { Idle, WalkBackward, WalkForward, FJump, Damaged, Block, StHP, StLP };

enum class ActionType { Idle, Walk, Jump, Other };

class HAction;

// Defines things such as
// - animation
// - hitboxes
// - hurtboxes
// - could map select inputs to other Actions in the case of e.g.
//   target combos
class Action {
public:
  int character;

  enum EAnimation animation;

  std::optional<Hitbox> collision; // leave None to use character's default collision box

  Hitbox hitbox;
  Hitbox hurtbox;
  int damage;
  int lockedFrames; // number of frames that the player is locked into
                    // this action
  int animationLength; // length of the actual animation. The player
                       // may cancel the end of the animation by
                       // pressing a button after lockedFrames have
                       // passed, but if they don't cancel, the action
                       // will continue for a total of animationLength
                       // frames.

  enum ActionType type;

  FVector velocity; // constant velocity that a player moves forward
                    // during a action. Probably only useful for
                    // walking.

  int specialCancelFrames; // number of frames that the player cannot
                           // special cancel
  std::map<enum Button, HAction> chains; // actions that chain from
                                         // this one when pressed
                                         // after specialCancelFrames
                                         // have passed

  Action(int character, enum EAnimation animation, std::optional<Hitbox> collision, Hitbox hitbox, Hitbox hurtbox, int damage, int lockedFrames, int animationLength, enum ActionType type = ActionType::Other, FVector velocity = FVector(0.0, 0.0, 0.0), int specialCancelFrames = 0, std::map<enum Button, HAction> chains = {}): character(character), animation(animation), collision(collision), hitbox(hitbox), hurtbox(hurtbox), damage(damage), lockedFrames(lockedFrames), animationLength(animationLength), type(type), velocity(velocity), specialCancelFrames(specialCancelFrames), chains(chains) {};

  // don't use this constructor
  Action(): Action(-1, EAnimation::Idle, Hitbox(), Hitbox(), Hitbox(), 0, 0, 0) {};
};

class HCharacter;

// handle to an action because references and pointers are bad
class HAction {
private:
  int h;
  #define N_ACTIONS 128
  static Action actions[N_ACTIONS];

public:
  HAction(int h): h(h) {};
  HAction(): HAction(-1) {};
  static void init();

  HCharacter character() const;
  enum EAnimation animation() const;
  const Hitbox& collision() const;
  const Hitbox& hitbox() const;
  const Hitbox& hurtbox() const;
  int damage() const;
  int lockedFrames() const;
  int animationLength() const;
  FVector velocity() const;
  bool isWalkOrIdle() const;
  enum ActionType type() const;
  int specialCancelFrames() const;
  const std::map<enum Button, HAction>& chains() const;

  bool operator==(const HAction& b) const;
  bool operator!=(const HAction& b) const;
};

// this is to assign integers to action names, needed for the next
// step
enum IAction {
  IActionIdle = 0,
  IActionWalkForward = 1,
  IActionWalkBackward = 2,
  IActionDamaged = 3,
  IActionBlock = 4,
  IActionStHP = 5,
  IActionStLP = 6,
  IActionFJump = 7
};

// this is to assign HActions to action names. All other code should
// use these symbols to reference Actions
#define HActionIdle (HAction(IActionIdle))
#define HActionWalkForward (HAction(IActionWalkForward))
#define HActionWalkBackward (HAction(IActionWalkBackward))
#define HActionDamaged (HAction(IActionDamaged))
#define HActionBlock (HAction(IActionBlock))
#define HActionStHP (HAction(IActionStHP))
#define HActionStLP (HAction(IActionStLP))
#define HActionFJump (HAction(IActionFJump))

// Actions themselves are defined in HAction::actions[] in Logic.cpp,
// for now

// A character is a collection of Actions. This class captures the
// regularity in characters, such as each character having walking
// actions and basic button actions. This allows each action to not
// have to store handles to all these common actions.
class Character {
public:
  Hitbox collision; // default collision box
  HAction idle;
  HAction walkForward;
  HAction walkBackward;
  HAction fJump;
  HAction damaged;
  HAction block;
  HAction sthp;
  HAction stlp;

  Character(Hitbox collision, HAction idle, HAction walkForward, HAction walkBackward, HAction fJump, HAction damaged, HAction block, HAction sthp, HAction stlp): collision(collision), idle(idle), walkForward(walkForward), walkBackward(walkBackward), fJump(fJump), damaged(damaged), block(block), sthp(sthp), stlp(stlp) {};

  // don't use this constructor
  Character(): Character(Hitbox({Box(0, 0, 0, 0)}), HAction(), HAction(), HAction(), HAction(), HAction(), HAction(), HAction(), HAction()) {};
};

class HCharacter {
private:
  int h;
  #define N_CHARACTERS 8
  static Character characters[N_CHARACTERS];

public:
  HCharacter(int h): h(h) {};
  static void init();
  const Hitbox& collision() const;
  HAction idle() const;
  HAction walkForward() const;
  HAction walkBackward() const;
  HAction fJump() const;
  HAction damaged() const;
  HAction block() const;
  HAction sthp() const;
  HAction stlp() const;

  bool operator==(const HCharacter& b) const;
  bool operator!=(const HCharacter& b) const;
};

enum ICharacter {
  IChar1 = 0
};

#define HChar1 (HCharacter(IChar1))

// This function needs to be called early in the game startup to
// populate the actions and character arrays
extern void init_actions();

#define JUMP_LENGTH 22
extern float jumpHeights[JUMP_LENGTH];

extern std::map<enum Button, std::vector<std::vector<enum Button>>> motionCommands;
