
# Project: Chess.AI

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/Unified-Projects/Chess.AI/blob/main/LICENCE.md) ![GitHub release (with filter)](https://img.shields.io/github/v/release/Unified-Projects/Chess.AI?filter=*)

This projects purpose is to get an understanding of efficiency and the Min-Max algorithm used in a competitive chess engine. Efficiency has taken a big part in the development due to the engine needing to be able to predict several moves ahead without adding much delay to the game. As of now our engine has a NPS (Nodes / Moves per second) of  `300,000` which may seem like a lot but other implementations are in the millions.

We have yet to start on the actual "AI" aspect of this project due to being held up by a failing implementation of chess that only worked up to 5 moves. We still have checks that need to be implemented, such as Castling not allowing you to move through check, and so it will continue to be delayed untill the chess engine is fully functional and passes all checks implemented using C-Test.

### Note:

Currently there is no competition against a functional chess AI / Engine

## Current Features

We do have a GUI library that allows you to play a game of chess locally on one window or even though a online session using our server app. They do work and well enough to be able to play a game happily.

![Screenshot of the GUI app locally open with no moves](https://github.com/Unified-Projects/Chess.AI/blob/main/images/Standard.png)
![Screenshot showing the possible move highlights](https://github.com/Unified-Projects/Chess.AI/blob/main/images/Highlights.png)
![Screenshot showing server application open with info screen open](https://github.com/Unified-Projects/Chess.AI/blob/main/images/Server.png)

### Controls

In both the server based app and local app

    I   : Will open the info menu
    Esc : Will quit the game

For normal moves, you can drag pieces.

Local app has access to the undo move function using `Ctrl + Z`

## Installation

This will compile the whole feature-set:

```bash
  mkdir Build
  cmake ..
  make
```

The GUI based apps will be located in the test folder, or can be downloaded from the **release page**!

## Authors

- [@Seggys116](https://www.github.com/Seggys116)
- [@Wilbur42](https://www.github.com/Wilbur42)
