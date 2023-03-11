#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* CommandTypes.h
*
* Used to see what the user would like to do when the program is started  
*/

enum CommandType {
	RegisterUser = 100,
	GetWaitingGames = 1,
	JoinExistingGame = 2,
	StartNewGame = 3,
	IsGameStarted = 4,
	IsMyTurn = 5,
	PlayTurn = 6,
};