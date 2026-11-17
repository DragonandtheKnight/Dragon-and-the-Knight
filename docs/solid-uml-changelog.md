# SOLID UML Change Log

This log records every change we made to our class diagram after applying the SOLID principles. Each entry records **who** made the change, **what changed** in the diagram, **which SOLID principle(s)** it applies, and how it helped.

To add an entry, copy the block from the [SOLID change entry template](templates/solid-change-template.md) and add it to the end of [Changes](#changes).

## SOLID Principles Reference

| Principle | Short name | In one sentence |
|---|---|---|
| Single Responsibility | SRP | A class should have only one reason to change. |
| Open/Closed | OCP | Classes should be open for extension but closed for modification. |
| Liskov Substitution | LSP | A subclass must be usable anywhere its base class is expected. |
| Interface Segregation | ISP | Clients should not depend on methods they do not use. |
| Dependency Inversion | DIP | Depend on abstractions, not on concrete classes. |

## New Class Diagram

<!-- Upload the new SOLID class diagram image here and add the link to its whiteboard. -->

<New class diagram image>

[New Class UML White Board](<link>)

## Changes

### 1. Separate Player Data from Combat Logic

- **Who:** Nathan Chen (@evolTWM)
- **Class(es):** `Player`, `Battle`
- **What changed:** We updated the Player class to focus on managing player information, including HP, shield, gold, maximum energy, and deck. We removed the current energy management functions from Player because Battle is responsible for tracking energy during combat.
- **SOLID principle(s):** SRP, Player focuses on managing player data while Battle handles combat logic.
- **How it helped:** This change makes each class have a clearer responsibility. It also makes the code easier to maintain and test because changes to combat rules do not always require changes to the Player class.

### 2. Improve Non-Combat Level Design

- **Who:** Nathan Chen (@evolTWM)
- **Class(es):** `Level`, `Shop`, `Rest`, `Event`, `Entertainment`
- **What changed:** We updated the Shop, Rest, Event, and Entertainment classes to match our implementation. Each class implements its own `Enter()` function, while `GetType()` is handled by the Level base class instead of being overridden in every subclass.
- **SOLID principle(s):** OCP, We can add new types of levels by creating new subclasses without changing the existing Level interface.
- **How it helped:** This design allows us to develop different level behaviors independently. It also makes the code easier to extend and test when we add more levels in the future.

### 3. Share Random Number Generation Between Levels

- **Who:** Nathan Chen (@evolTWM)
- **Class(es):** `Event`, `Entertainment`, `Rng`
- **What changed:** We updated the Event and Entertainment classes to receive an `Rng` object through their constructors instead of creating separate random number generators. Event uses it to generate random events, while Entertainment uses it to roll dice.
- **SOLID principle(s):** SRP, Rng handles random number generation, while Event and Entertainment focus on their own gameplay logic.
- **How it helped:** This reduces duplicated code and makes each class easier to maintain. It also makes testing easier because we can use the same random seed to reproduce results.

### 4. Keep Visited-Node Tracking in FloorMap

- **Who:** Ink Wang (@1nkWang)
- **Class(es):** `FloorMap`
- **What changed:** The diagram adds `visited_` and `IsVisited()` to `FloorMap` and specifies that `AvailablePaths()` only returns connected, unvisited nodes.
- **SOLID principle(s):** SRP — `FloorMap` owns navigation state and applies movement rules, while the map generator builds the layout.
- **How it helped:** The navigation rules can be tested independently of the generation algorithm, including whether a player can return along a two-way path to a node they already visited.

### 5. Make Map Generation Settings Explicit

- **Who:** Ink Wang (@1nkWang)
- **Class(es):** `FloorConfig`, `IMapGenerator`, `MapGenerator`
- **What changed:** The diagram adds `FloorConfig` and shows that `IMapGenerator` uses it. It groups settings for grid size, removed nodes, minimum nodes per column, and the number of extra and vertical connections.
- **SOLID principle(s):** SRP — `FloorConfig` holds the settings, while `MapGenerator` applies them to build the floor.
- **How it helped:** This design makes it possible to test different grid sizes and connection limits through configuration while reusing the generation algorithm.

### 6. Represent Connection Types Explicitly

- **Who:** Ink Wang (@1nkWang)
- **Class(es):** `LevelNode`, `PathEdge`, `PathKind`
- **What changed:** The diagram replaces the list of destination IDs in `LevelNode` with `PathEdge` objects. Each edge stores its destination and whether it is a base, extra, or vertical connection.
- **SOLID principle(s):** SRP — connection metadata belongs to the map graph, while level actions handle gameplay.
- **How it helped:** Navigation and map rendering can read the connection type directly from the edge, making it easier to verify and display each kind of route.

### 7. Make Rng::Shuffle Independent of Card

  - **Who:** ChestnutLUO (@ChestnutLUO)
  - **Class(es):** `Rng`, `Card`, `Deck`, `FloorMap`
  - **What changed:** The diagram changes `Rng::Shuffle(std::vector<Card>& cards)` to the template `Rng::Shuffle(std::vector<T>& items)`. Before, `Rng`
  depended on the `Card` class; after, `Rng` has no dependency on any game class, and `Deck` passes its own draw pile to it.
  - **SOLID principle(s):** DIP – the low-level random utility no longer depends on a concrete game class; game classes depend on `Rng`, not the other way
  around. It also supports OCP, because new element types can be shuffled without modifying `Rng`.
  - **How it helped:** `Rng` can be compiled and tested on its own with plain values, and the same shuffle is reused by `Deck` for cards and by `FloorMap`
  for its map-generation orderings without any change to `Rng`

### 8. Separate Card Data from Effect Behavior

- **Who:** Jingtong Hu (@TNY233)
- **Class(es):** `Card`, `CardEffect`, `DamageEffect`, `HealEffect`, `BlockEffect`
- **What changed:** We added separate classes for damage, healing, and shield effects. Card keeps the card information and its list of effects, while each effect class implements its own Apply() method.
- **SOLID principle(s):** SRP — Card manages card information, while each effect handles its own behavior.
- **How it helped:** Changing how damage works does not require changing the healing or shield effects. We can also test each effect separately.

### 9. Let Card Use the CardEffect Interface

- **Who:** Jingtong Hu (@TNY233)
- **Class(es):** `Card`, `CardEffect`
- **What changed:** We added AddEffect() and a list of CardEffect objects to Card. Card calls the same Apply() interface for every effect, and Clone() allows effects to be copied with the card.
- **SOLID principle(s):** OCP and DIP — Card depends on the CardEffect interface, so new effect classes can be added without changing how Card executes its effects.
- **How it helped:** One card can combine several effects. We can add more effect types without adding special cases to Card.

### 10. Separate Target Selection from Effect Execution

- **Who:** Jingtong Hu (@TNY233)
- **Class(es):** `Card`, `CardEffect`, `EffectContext`, `TargetType`, `Combat`
- **What changed:** We added TargetType to describe which targets an effect needs and EffectContext to pass the caster and targets to it. Each effect receives its own context, so one card can damage an enemy and heal the player.
- **SOLID principle(s):** SRP — Combat selects and checks targets and handles AP, while each effect applies its own behavior.
- **How it helped:** Effects do not need to select targets themselves. This also makes it easier to test cards whose effects act on different targets.

### 10. In empty enemy slots, use EmptyEnemy instead of nullptr.
- **Who:** Eric Qian (@eqian008)
- **Class(es):** `EmptyEnemy`, `Combat`
- **What changed:** In slots with no enemy, instead of using nullptr, I created an enemy type called empty_enemy, which starts dead when constructed.
- **SOLID principle(s):** LSP, EmptyEnemy inherits the same Enemy class as other enemies and uses the same interfaces, so I don't need to make is nullptr checks everytime I read a slot. I simply stuff a dead body into the slot and the combat code would naturally ignore it. If I used nullptr, I would have to tell wether the slot is enmpty every time I use enemy from the slots and I also face the risk of reading null pointers.
- **How it helped:** It reduced the complexity of the code and reduced the risk of reading a empty pointer.
