#include "stdafx.h"

Game pGame;

void Game::Run() {
	// ran on loop
	Sleep(50);
}

bool Game::Initialize() {
	// init config here

	bInitialized = false;

	LOG_DEV("Game initialized!");
	XeLive.bGameInitialized = true;
	bInitialized = true;
	return true;
}

void Game::Shutdown() {

	XeLive.bShutdownCheats = false;
	LOG_DEV("Game uninitialized!");
}