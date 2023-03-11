#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* GameOutcomes.h 
* 
* stoes enum for all the possibble game outcomes which are used by the client and server 
* 
*/


enum GameOutcome {
	InvalidMove = 1,
	Continue = 2,
	User1Win = 3,
	User2Win = 4,
	Tie = 5,
	YouWin = 11,
	YouLose = 12,
};