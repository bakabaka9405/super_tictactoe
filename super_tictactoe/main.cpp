#include"Game.h"
#include"Grid.h"
#include"ConsoleOutput.h"
#include"Human.h"
#include"AI.h"
#include<iostream>
#include <cassert>
int main() {
	Game<Grid, ConsoleOutput, AI<Grid> ,Human<Grid>> game;
	game.Start();
	return 0;
}