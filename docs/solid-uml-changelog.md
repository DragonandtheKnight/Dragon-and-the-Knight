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
