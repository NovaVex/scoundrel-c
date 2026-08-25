# Scoundrel (in C)

A terminal implementation of [Scoundrel](http://www.stfj.net/art/2011/Scoundrel.pdf), the single-player card game by Zach Gage and Kurt Bieg. Written in C with zero dependencies.

This is my first programming project. The game itself was almost beside the point. What I wanted was to take a simple, well-defined set of rules and deliberately over-engineer them into complete, modular, dependency-free code, so every concept I was learning had somewhere real to live.

**Current status: Alpha.** The game is fully playable, but it's a foundation. It's a working core I built so I'd have something real to practice computer science on, and most of the interesting features are still ahead of it.

*(This README is a first draft. It will grow alongside the project.)*

## Download and play

Download one file and run it. No installer, nothing to set up.

| Platform | File |
|---|---|
| Windows (64-bit) | `scoundrel-v0.1.0-windows-x86_64.exe` |
| Linux (64-bit) | `scoundrel-v0.1.0-linux-x86_64` |

Both are on the [latest release page](https://github.com/NovaVex/scoundrel-c/releases/latest).

### Windows

Download the `.exe` and double-click it.

The first time you run it, Windows will show a **"Windows protected your PC"** box. That's SmartScreen, and it flags any program that hasn't been code-signed. Signing requires a certificate, which isn't in my budget right now. I intend to get one once I can afford it. Until then, click **More info**, then **Run anyway**.

If you'd rather not run an unsigned binary, the source is right here and it builds in one command. See [Building and running](#building-and-running).

### Linux

Download the file, make it executable, then run it:

```bash
chmod +x scoundrel-v0.1.0-linux-x86_64
./scoundrel-v0.1.0-linux-x86_64
```

It's statically linked, so it doesn't care what distro you're on or what version of glibc you have.

### Reading the screen

The whole game is numbers on the keyboard. Type a number, press Enter.

Each room deals four cards, labelled by what they do rather than by suit:

- **M** is a monster. Fighting it costs you health equal to its value, less your weapon's value if your weapon can still be used on it.
- **W** is a weapon. Equipping one discards whatever you were already carrying.
- **P** is a potion. It heals you, but only the first one each turn.

The number beside the letter is the card's value. Everything else is in [The rules](#the-rules) below.

## Why C

C isn't the easy path, and that was part of the appeal. I chose it because I wanted to understand computers at the foundational level, and C gets you about as close to the machine as you can while still writing something readable. Along the way I picked up binary and hex, and spent a lot of time on what's actually happening under the hood of a programming language, digging past what the syntax does into why it exists in the first place.

That understanding fed directly back into the code. The architectural decisions here come from thinking about what the hardware is physically doing: how memory is laid out, why the card pool is a fixed array with a separate pool of link nodes, and why nothing allocates at runtime.

The same philosophy applied to my tools. I wrote the whole project in Notepad++ on default settings. The only things it gave me were syntax highlighting and a basic autocomplete that repeats words already typed in the open file. There was no error checking and no knowledge of my other files. If I wanted to call a function from another file, I had to actually know it existed and what it took, which was the point. The commit history even records the cost of that choice: Notepad++ left the files with messy line endings, which had to be normalized and locked down with a .gitattributes when the project moved to git. Having built the foundation the hard way, I'm now learning to use VS Code.

## What the project taught me

Working through this codebase is where I learned, in rough order:

- What header files and C files actually are, and why the separation exists
- How to write a function once and reuse it everywhere
- How C lets you build your own vocabulary on top of itself, abstractions on abstractions, until the code reads like a language you designed
- How pointers and their syntax work (and why they're so awful to read)
- How to take computer science fundamentals and put them into practice in real code

## Honest AI collaboration statement

I built this with AI assistance, and the history in this repo shows exactly what that looked like, including how the relationship changed as I learned.

I didn't come into this empty-handed. Years of optimizing Google Sheets, with formulas, the QUERY function, and restructuring data until it flowed, had already taught me a lot about data structures and how information wants to be organized. I just didn't know the vocabulary yet.

In the beginning, I used AI as a feedback loop for code I'd already written myself. I forced myself to write the C first, then asked AI two questions: is this syntax correct, and does it actually do what I think it does? That replaced the constant recompile-and-test cycle, so I could spend my energy where it mattered most: learning to actually *read* C, beyond getting it to run. Along the way I was still discovering all the little tricks the language gives you to make code more human readable, and testing each one as I found it.

Then I stopped and spent about a month just on computer science. I went deep with [Crash Course Computer Science](https://www.youtube.com/playlist?list=PL8dPuuaLjXtNlUrzyH5r6jN9ulIgZBpdo) and studied early computers and how the technology evolved. Somewhere in there it clicked. I stopped asking AI whether my code was right and started making the architectural decisions myself: how memory should be laid out, where the layer boundaries belonged, what the design rules were. I finally understood what the machine was doing underneath.

From that point on, AI's role shrank to what it's actually good for: accelerating the boring parts. Speeding up the UI, hunting syntax errors, and helping me settle on a consistent writing style. Some of what it generated had logic errors and style debt. The commit history shows me catching that, and the multi-round refactor where I fixed it against the actual rulebook.

Several of the early commits in this repo don't compile. I left them that way on purpose. That's what learning your first language in free time actually looks like, and I'd rather show the real arc than a fake clean one. The final version compiles clean under `gcc -Wall -Wextra -std=c11` and plays by the rulebook.

## Architecture

The code is split into strict layers, each with one job:

```
main.c            → hands control to the engine and nothing else
Game_master       → state machine conductor; owns the game loop
Scene_manager     → screens and menus; collects the player's choice
UI_manager        → all rendering and printing lives here
Input             → raw input handling
Game_mechanics    → pure rules logic; zero stdio, zero printing
Data_Structure.h  → shared types used by every layer
```

The design rule that governs everything: the game logic must be liftable into a future engine by replacing only `UI_manager` and `Input`, leaving `Data_Structure` and `Game_mechanics` untouched. `Game_mechanics.c` doesn't include stdio at all. It talks to the rest of the program through data alone.

Style rules are enforced across every file:

- Whole-word variable names, no abbreviations
- Single-responsibility functions
- Guard-clause / early-return style instead of nested if-else chains
- No third-party libraries; the project is dependency-free by design

## How a turn flows through the layers

Rather than documenting every function here (that's what code comments are for), here's the map of how one turn moves through the system. Once you have this, the code reads top to bottom:

1. `main.c` calls `gameMaster()`, which owns the top-level state machine (main menu, options, in-game).
2. Starting a game enters `gameLoop()`, which runs its own in-game state machine (active, paused, game over).
3. Each active turn, `Game_master` first asks `Game_mechanics` whether the room is finished (`isRoomComplete()`). If it is, the only legal action is to press on, so it shows the turn-complete screen and deals the next room itself. No menu, no wasted keypress.
4. Otherwise `Game_master` asks `Scene_manager` to draw the room and collect a choice. `Scene_manager` prints through `UI_manager` and reads through `Input`. It never touches game rules.
5. The choice comes back as a `PlayerInput` enum value. `Game_master` routes it: an encounter slot goes to `runEncounterScene()`, fleeing goes to `runFleeScene()`.
6. Before anything is resolved, `runEncounterScene()` calls `requiredEncounterPrompt()` in `Game_mechanics` to find out whether the player needs asking first: fight with the weapon or bare-handed, confirm a fight the weapon can't help with and the damage it will cost, confirm discarding an equipped weapon and its kill stack, or confirm burning an already-spent potion.
7. The answer is handed down as a `CombatChoice` into `encounterManager()`, which looks at the card's type and dispatches to exactly one of `combatManager()`, `healManager()`, or `equipWeapon()`. These change player and card state and nothing else. No printing, no input.
8. `encounterManager()` returns an `EncounterResult`. If the move was illegal, the scene renders the reason instead of failing silently. Control returns to `Game_master`, which checks for death, and the loop continues.

That one-way flow is the entire program: the master asks the scenes for input, routes it into the mechanics, the mechanics change state and report back a result code, and the master reads the new state. Because `Game_mechanics` can't print, a refusal has to travel back out as a return code. That's what keeps the rules layer portable.

## Building and running

Requires only gcc (or any C11 compiler). No libraries, nothing to install.

```
make
./scoundrel
```

There's a Makefile now, but it's only ever a convenience. The compiler line it runs is still the whole build:

```
gcc -Wall -Wextra -std=c11 *.c -o scoundrel
```

## The rules

Scoundrel is played with a standard 52-card deck with red face cards, red aces, and jokers removed. Clubs and spades are monsters, diamonds are weapons, and hearts are healing potions. You fight through the dungeon four cards at a time, deciding when to fight barehanded, when to spend a weapon's edge, and when to flee. Full rules are in the original PDF linked at the top.

### How the deck is represented

The code has no suits in it. A real deck needs them so you can tell a club from a heart while you sort the pile on your table, but after that they stop mattering and nothing in the game looks at a suit again.

So I generate the 44 cards the game actually uses and label each one the way the rulebook talks about it: monster, weapon, potion. There's no setup step stripping out red face cards and jokers, because those cards never get made. Face cards don't exist either. A jack is a monster worth 11.

That was one of the first design calls I made on my own, and the deck code got shorter for it.

### One rule I had to interpret

The PDF contradicts itself in one place. It says a used weapon can only slay monsters of "a lower value (less than equal)" than the last one it killed. Those two readings disagree, and the worked examples underneath (a 6 against a 12, a 12 against a 6) never land on the case where the values match.

I went with the parenthetical, so a weapon that killed an 8 can still take another 8. Most write-ups of the rules I found read it the same way.

The bigger reason is that it plays better. The strict reading burns weapons down faster, and you get more turns where nothing you're carrying helps and you just take the hit. Allowing equal values keeps a weapon useful longer and cuts down on games that were already lost a few turns before the player could tell. Flipping it back is one operator in `checkWeaponState`.

## What's next

This alpha is the platform for everything below, roughly in order:

1. **ASCII renderer**: a proper drawn screen instead of scrolling text
2. **View the discard pile and monster stack**: surfacing game state that already exists in the data structures but isn't shown to the player yet
3. **Save/load**: serializing game state to a human-readable format
4. **Replay feature**: recording a game's inputs so a full playthrough can be played back, which doubles as proof that the logic layer is truly deterministic and decoupled
5. **Demo mode**: a pre-recorded playthrough anyone can watch without learning the rules
6. **Engine port**: unhooking the game from the terminal and plugging it into a game engine. This is the whole architecture's final exam. If the layer separation holds, only `UI_manager` and `Input` get replaced, and `Game_mechanics` and `Data_Structure` move over untouched
7. **Portfolio page**: a site where the game is playable in two modes, the original terminal version and the engine version running in the browser

---

© Scoundrel game design by Zach Gage and Kurt Bieg (2011). This is an unofficial fan implementation for learning purposes.
