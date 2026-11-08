# Dragon and the Knight
Authors: [InkWang](https://github.com/1nkWang), [Lizi Luo](https://github.com/ChestnutLUO), [Jingtong Hu](https://github.com/TNY233), [Eric Qian](https://github.com/eqian008),[Nathan Chen](https://github.com/evolTWM).

## Project Description

Dragon and the Knight is a 2D adventure role-playing game developed in C++. Player is a knight who explores different areas, using his skill cards to fight monsters, gains experience, and becomes stronger throughout the adventure.
The game takes place in a fantasy world. The player can travel through different maps, defeat enemies, collect useful items, and follow a storyline. The player can gain coins from beat monsters. After a few rounds that player kills monsters there will have chance to open shop, then player can buy new cards to improve their attack skills. The game will also include hidden areas and secret levels that players can discover by exploring the map or completing certain challenges.
The main goal is to follow the storyline, become stronger, and defeat the final boss, a dragon.
Your project description should summarize the project you are proposing. Be sure to include:

* Why is it important or interesting to you?  
  
  * It is a game, and I have never made a game before.  
  * Compared to other types of projects, we find RPG games more interesting and think we would have more fun developing one. Since I have never designed or developed a game before, I am very interested in this project.  
* What languages/tools/technologies do you plan to use? (This list may change over the course of the project)  
  - languages C++
  - TUI Framwork: [FTXUI](https://github.com/ArthurSonzogni/FTXUI) (FTXUI is a modern C++ library for building interactive terminals.)
* What will be the input/output of your project?  

  * User's click and type will be the input, output will be shown in the terminal.
* What are the features that the project provides?

    - Turn-Bsaed Card Combat:Players can use different cards to attack enemies, defend themselves, or restore health during battles.

    - Card Collections and Deck Building: Players can collect new cards by defeating enemies or purchasing them from shops. They can replace existing cards to improve their decks.

    - Randomized Levels: Players can explore different levels and choose between paths that lead to battles, shops, or special events.

    - Enemy and Boss Battles: Players will encounter different enemies and challenging bosses as they progress through the game.

    - Shop System: Players can use in-game currency to purchase cards, upgrades, and other useful items.


## User Interface Specification- (Navigation Diagram&Screen Layouts)
<img width="7331" height="3908" alt="shapes at 26-10-24 16 48 25" src="https://github.com/user-attachments/assets/36d55315-6924-4178-b5f0-2f43197e6931" />

Please check online version by following URL ⬇️


[Team UI Sketch Board](https://www.tldraw.com/p/O_-2-Nw9QFSGrI7shEAze)

## Class Diagram

<img width="3209" height="3723" alt="shapes at 26-10-24 16 54 44" src="https://github.com/user-attachments/assets/f481bbf6-c273-4300-9aa0-ad42ff62cba8" />


[Class UML White Board](https://www.tldraw.com/p/SBypERcPwHj0Lrw89Z-Dw)

### Class Diagram Description

The Game class controls the main game process including starting the game, running the game, moving to the next floor, and ending the game. The FloorMap class manages the map and the paths that players can choose. The Level class is an abstract base class, and Combat, Shop, Rest, Event, and Entertainment inherit from it. Each level has different functions. For example, players can fight monsters in Combat, buy cards in Shop, and recover HP in Rest.

The Player class stores the player's information including HP, maximum HP, block, energy, gold, and deck. It has functions like TakeDamage(), Heal(), AddBlock(), GainGold(), and SpendGold(). It also has getter functions that allow other classes to access the player's information.

The Card class stores information about each card including its name, type, cost, rarity, and effect value. The Deck class manages the player's cards including drawing, shuffling, and discarding cards. The Battle class controls the turn based combat system. It manages player turns, enemy turns, playing cards, and checking whether the battle has ended.

The Enemy class represents the monsters that players fight during the game. Different enemies can have different abilities.

Our class diagram also shows different relationships between classes, including inheritance, composition, and association. These relationships help us connect different parts of the game.

## Testing

We follow **Test-Driven Development (TDD)** for this project. All test code is located in the `/test` directory.

Prof. Watkinson allows the use of AI **only for TDD-related testing** (in Oct 20 Tue), not for implementing any project's source code.

To ensure transparency, here is repository to record all AI usage:

**[AI-TDD Transparency Reports](https://github.com/DragonandtheKnight/AI-TDD-Transparency)**

The reports follow the same directory structure as our source code and record AI inputs, outputs, tools used, and responsible contributors.
