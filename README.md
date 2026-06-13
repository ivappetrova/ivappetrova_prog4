[![Build Status](https://github.com/avadae/minigin/actions/workflows/cmake.yml/badge.svg)](https://github.com/avadae/cmake/actions)
[![Build Status](https://github.com/avadae/minigin/actions/workflows/emscripten.yml/badge.svg)](https://github.com/avadae/emscripten/actions)

<!-- Game Title -->
<h2 align="center">Bubble Bobble</h2>
<h3 align="center">Made by Iva Petrova, 2DAE11 </h3>

<img width="600" height="320" alt="bubbob" src="https://github.com/user-attachments/assets/d87a9bb1-e325-47af-a70a-e1e0c0ab872e" />


<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li> <a href="#about-the-game">About the game</a> </li>
	 <li> <a href="#controls">Controls</a> </li>
     <li> <a href="#design-choices">Design Choices</a> </li>
	  <li> <a href="#contacts">Contacts</a> </li>
  </ol>
</details>

<!-- About the game -->
# About the game

Bubble Bobble is all about a green dragon named Bub and a blue dragon named Bob, who need to save their girlfriends from a world known as the Cave of Monsters. 
In each level, Bub and Bob must defeat each enemy present by trapping them in bubbles blown from their mouths and then popping the bubbles. 

I implemented the first 3 levels of the game. In my implementation there are 3 modes:
- Singleplayer: Play as Bub and try to complete all levels.
- Co-op: Play as Bub and Bob together and try to complete all levels.
- PvP: Play as Bub and Maita and try to defeat each other

<!-- Controls -->
# Controls
## Keyboard
### Menu
- W/S - navigate
- A - select
- F2 - mute sounds
### Game
- A/D - move left/right
- W - jump
- P - pewpew (shoot)
- F2 - mute sounds
- F1 - skip level
- Esc - go to menu
## Gamepad
### Menu
- D-pad up/down - navigate
- A - select
- Left shoulder - mute sounds
### Game
- D-pad left/right - move left/right
- A - jump
- B - pewpew (shoot)
- Left shoulder - mute sounds
- Right shoulder - skip level
- Back - go to menu

<!-- Design Choices -->
# Design Choices — Game Programming Patterns by Robert Nystrom

## Mandatory Patterns

### Game Loop
- `Minigin::RunOneFrame()` uses a fixed timestep with variable rendering
- `m_Lag` accumulates real elapsed time, `FixedUpdate` drains it in discrete 1/60s steps for deterministic physics, then `Update` runs once with the real delta time and the renderer presents
- A sleep + spin at the end caps the frame rate

### Update Method
- Every entity exposes `Update(float deltaTime)` called once per frame by the game loop
- `FixedUpdate` runs separately at a locked 60 Hz so physics and collision resolution stay frame-rate independent

### Command
- `Command.h` defines an abstract `Execute()` base. Concrete commands are bound to keyboard scancodes or controller buttons in `InputManager` and dispatched each frame
- Because input and action are fully decoupled, any action can be rebound to any key without touching game logic
- Commands used: `MoveCommand`, `JumpCommand`, `ShootCommand`, `SkipLevelCommand`, `MuteCommand`, `PlaySoundCommand`, `GoToMenuCommand`, `MenuNavigateCommand`, `MenuConfirmCommand`

### Observer
- `Subject` holds a list of `IObserver*` and calls `Notify()` when something noteworthy happens. Observers react without the subject knowing who is listening
- Compile-time SDBM hashing in `Event.h` gives string-based event IDs at zero runtime cost
- Used for: HUD updates on hit/death (`HealthDisplayComponent`), score display updates (`ScoreDisplayComponent`), playing sounds on game events (`SoundObserver`), and `PlayerComponent` reacting to its own health changes

### Event Queue / Thread-safe Audio
- In `SoundSystem::Impl`, play requests are pushed onto a `std::queue<PlayRequest>` protected by a `std::mutex`
- A dedicated worker thread sleeps on a `std::condition_variable` and wakes to drain the queue, keeping all audio loading and playback entirely off the main thread
- This is Nystrom's Event Queue pattern applied to audio

### Pimpl
- `Controller` and `SoundSystem` expose a clean public interface while hiding platform-specific details behind a private `Impl` pointer
- `ControllerImpl` contains XInput code on Windows and a no-op stub elsewhere. `SoundSystem::Impl` contains all SDL_mixer and threading internals
- Neither leaks platform headers into the rest of the codebase

### Component
- A `GameObject` is an empty container. All behaviour and data are added as components, each running independently every frame
- This avoids deep inheritance hierarchies and lets objects be assembled from reusable parts
- `PhysicsComponent` applies gravity and resolves collisions, `HealthComponent` tracks HP and fires events, `BubbleProjectileComponent` runs the full bubble lifecycle (traveling → trapped → burst), `PickUpComponent` checks for player overlap and awards points, `LevelCollisionComponent` answers geometric queries against SVG-loaded polygon data

### State
- The player has a state machine where each state represents a distinct mode of behaviour. States don't own game data — they borrow it from `PlayerComponent` and drive it differently. When conditions change, the old state is deleted and a new one takes over
- `IdleState` — waits for input
- `MoveState` — calls `RequestMove()` each frame, transitions to idle when direction is zero
- `JumpState` — fires the jump impulse on enter, transitions to fall when velocity flips positive
- `ShootState` — spawns the bubble projectile on enter, times out back to idle
- `TakeDmgState` — applies knockback and invincibility on enter, waits out the timer
- `FallState` — allows air movement, transitions to idle on landing

## Extra Patterns

### Service Locator
- `ServiceLocator.h` provides a global access point to the active `ISoundSystem`
- Defaults to `NullSoundSystem` so the system is always in a valid state even before registration
- In `main.cpp`, a `LoggingSoundSystem` wrapping a real `SoundSystem` is registered in debug builds

### Decorator
- `LoggingSoundSystem` wraps any `ISoundSystem`, logs every call to stdout, then forwards to the wrapped implementation
- No existing code changes — the logging layer is added or removed purely at the registration site in `main.cpp`

### Singleton
- `ScoreManager` and `EnemyTypeRegistry` implement the Meyer's singleton pattern manually
- Provides a single globally accessible instance for leaderboard data and enemy type definitions respectively

### Dirty Flag
- In `Transform`, `m_IsDirty` is set whenever local position changes and propagated down the entire child hierarchy via `SetPositionDirty()`
- `GetWorldPosition()` only recalculates when dirty, avoiding redundant work every frame
- Also present in `TextComponent::m_NeedsUpdate` (texture only regenerated when text or colour changes) and `PhysicsComponent::m_BlockedDirX` (suppresses movement while still pushing into a wall)

### Flyweight
- `EnemyType` and `EnemyTypeRegistry` store one immutable data object per enemy type (id, texture path, dimensions), loaded once from JSON
- Every `EnemyComponent` holds a raw `const EnemyType*` to the shared instance
- Adding a new enemy type requires only a JSON entry, not a new class

### Subclass Sandbox
- `GameObjectCommand` exposes `GetGameObject()` as a protected helper
- Concrete commands like `ShootCommand` and `SkipLevelCommand` inherit a safe, consistent way to reach their target object without repeating the retrieval and null-check logic

### Type Object
- Instead of one subclass per enemy, a single `EnemyComponent` holds a pointer to a shared, immutable `EnemyType` data object loaded from JSON
- The type defines what the enemy looks like and how big it is
- Adding a new enemy variant is purely a data change

<!-- Contacts -->
## Contacts
* [Github- Iva Petrova](https://github.com/ivappetrova/ivappetrova_prog4)
* email: - iva.petrova@student.howest.be
