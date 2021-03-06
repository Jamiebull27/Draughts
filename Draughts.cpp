/* Draughts.cpp : Defines the entry point for the console application. **
** Jamie Bull **
** 19/11/2017 */


#include "stdafx.h"
#include <iostream>
#include <math.h>

#define BOARD_SIZE 8	//8x8 square board
#define VALID_MOVES_SIZE 49	//size of validMoves array, 49 being theoretical maximum number of moves is board is 50% populated and every piece is a king.

/*pieces*/
#define NO_PIECE 0
#define W_PIECE 1
#define B_PIECE 2
#define W_KING 3
#define B_KING 4
#define BOARD_EDGE -1

/*AI*/
#define RELEVANT_MOVES 15	//Number of moves which will be used to look ahead
#define MOVES_AHEAD 8	//Number of move to look ahead by
#define N_PREV_BOARDS 8	//Number of board states to store from previous turns to prevent AI repeating itself.

//Rating parameters to judge value of board state
#define RATING_PARAM_JUMP_1 100
#define RATING_PARAM_JUMP_2 100
#define RATING_PARAM_DIST_1 2
#define RATING_PARAM_DIST_2 10
#define RATING_PARAM_KING 5

/*Board class*/
class Board {
	//Access specifier
	public:
		//board[8][8] array stores positions of pieces
		int board[BOARD_SIZE][BOARD_SIZE];

		//validMoves[8][8] array stores valid moves, much larger than it needs to be so end of current valid moves is marked by 4x(-1)
		int validMoves[VALID_MOVES_SIZE][4];	//[x][0] and [x][1] are coords of original position,
												//[x][2] and [x][3] are coored of position after move.
		int validJumps[VALID_MOVES_SIZE][4];	//Like validMoves but stores jumps instead.

		//default constructor sets up board with correct pieces
		Board() {

			//initialise board to all NO_PIECE's
			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					board[i][j] = NO_PIECE;
				}
			}

			//Now add pieces
			int pieces = 0,
				i = 0,
				j = 0;

			//Add white pieces
			do {	//Need to have every other element occupied by a piece up to 12 pieces.
				if (j >= BOARD_SIZE) {
					if (i % 2 == 0) {
						j = 1;
						i++;
					}
					else {
						j = 0;
						i++;
					}
				}
				board[i][j] = W_PIECE;
				board[BOARD_SIZE - 1 - i][BOARD_SIZE - 1 -j] = B_PIECE;
				pieces++;
				j += 2;
			} while (pieces < 12);
		}

		//Constructor which copies another board onto the new board.
		Board(int oldBoard[BOARD_SIZE][BOARD_SIZE]) {

			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					board[i][j] = oldBoard[i][j];
				}
			}
		}

		//clearValidMoves sets the valid moves array to all 0's.
		void clearValidMoves() {

			for (int i = 0; i < VALID_MOVES_SIZE; i++) {
				for (int j = 0; j < 4; j++) {
					validMoves[i][j] = 0;
				}
			}
		}

		//calculateMoves determines valid moves for the whole board and stores them in the validMoves array, returns number of valid moves found.
		int calculateMoves(int player) {

			clearValidMoves();

			int movesFound = 0;

			/*Make a copy of board with edges to prevent overrunning arrays later*/
			int newBoard[BOARD_SIZE + 2][BOARD_SIZE + 2];

			for (int i = 0; i < BOARD_SIZE + 2; i++) {
				for (int j = 0; j < BOARD_SIZE + 2; j++) {
					if (i == 0 || j == 0 || i == BOARD_SIZE + 1 || j == BOARD_SIZE + 1) {
						newBoard[i][j] = BOARD_EDGE;
					}
					else {
						newBoard[i][j] = board[i - 1][j - 1];
					}
				}
			}

			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					//White pieces
					if (newBoard[i + 1][j + 1] == W_PIECE && W_PIECE) {
						if (newBoard[i + 2][j] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i + 1;
							validMoves[movesFound][3] = j - 1;
							movesFound++;
						}
						if (newBoard[i + 2][j + 2] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i + 1;
							validMoves[movesFound][3] = j + 1;
							movesFound++;
						}
					}
					//Black pieces
					else if (newBoard[i + 1][j + 1] == B_PIECE && player == B_PIECE) {
						if (newBoard[i][j] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i - 1;
							validMoves[movesFound][3] = j - 1;
							movesFound++;
						}
						if (newBoard[i][j + 2] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i - 1;
							validMoves[movesFound][3] = j + 1;
							movesFound++;
						}
					}
					//White kings
					else if (newBoard[i + 1][j + 1] == W_KING && player == W_PIECE) {
						if (newBoard[i + 2][j] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i + 1;
							validMoves[movesFound][3] = j - 1;
							movesFound++;
						}
						if (newBoard[i + 2][j + 2] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i + 1;
							validMoves[movesFound][3] = j + 1;
							movesFound++;
						}
						if (newBoard[i][j] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i - 1;
							validMoves[movesFound][3] = j - 1;
							movesFound++;
						}
						if (newBoard[i][j + 2] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i - 1;
							validMoves[movesFound][3] = j + 1;
							movesFound++;
						}
					}
					//Black kings
					else if (newBoard[i + 1][j + 1] == B_KING && player == B_PIECE) {
						if (newBoard[i + 2][j] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i + 1;
							validMoves[movesFound][3] = j - 1;
							movesFound++;
						}
						if (newBoard[i + 2][j + 2] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i + 1;
							validMoves[movesFound][3] = j + 1;
							movesFound++;
						}
						if (newBoard[i][j] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i - 1;
							validMoves[movesFound][3] = j - 1;
							movesFound++;
						}
						if (newBoard[i][j + 2] == NO_PIECE) {
							validMoves[movesFound][0] = i;
							validMoves[movesFound][1] = j;
							validMoves[movesFound][2] = i - 1;
							validMoves[movesFound][3] = j + 1;
							movesFound++;
						}
					}
				}
			}
			//-1 to signal end of validMoves
			validMoves[movesFound][0] = -1;
			validMoves[movesFound][1] = -1;
			validMoves[movesFound][2] = -1;
			validMoves[movesFound][3] = -1;

			return movesFound;
		}

		//displayMoves displays the contents of validMoves to the consol.
		void displayMoves(int player) {

			for (int i = 0; validMoves[i][0] != -1; i++) {
				if (board[validMoves[i][0]][validMoves[i][1]] == player || board[validMoves[i][0]][validMoves[i][1]] == player + 2) {
					std::cout << i << "-  " << validMoves[i][0] << ", " << validMoves[i][1] << " -> " << validMoves[i][2] << ", " << validMoves[i][3] << "\n";
				}
			}
		}

		//performMove performs a move from the array of valid moves
		void performMove(int player, int chosenMove = -1 ) {

			//Either can have user choose move in function fo pass chosenMove to function on input (useful for AI).
			if (chosenMove == -1) {
				int validChoice = 0;

				while (validChoice == 0) {
					std::cout << "Choose a valid move.\n";
					std::cin >> chosenMove;
					if (board[validMoves[chosenMove][0]][validMoves[chosenMove][1]] != player && board[validMoves[chosenMove][0]][validMoves[chosenMove][1]] != player + 2) {
						std::cout << "Invalid selection.\n";
					}
					else {
						validChoice = 1;
					}
				}
			}
			//Copy old piece to new position.
			board[validMoves[chosenMove][2]][validMoves[chosenMove][3]] = board[validMoves[chosenMove][0]][validMoves[chosenMove][1]];
			//Delete opd piece.
			board[validMoves[chosenMove][0]][validMoves[chosenMove][1]] = NO_PIECE;

			//Check for king
			checkKing();
		}

		//clearValidJumps sets the validJumps array to all 0's.
		void clearValidJumps() {
			for (int i = 0; i < VALID_MOVES_SIZE; i++) {
				for (int j = 0; j < 4; j++) {
					validJumps[i][j] = 0;
				}
			}
		}

		//calculateJumps determines valid jumps for the whole board and stores them in the validJumps array, returns number of valid jumps found for the given player.
		int calculateJumps(int player) {

			clearValidJumps();

			int jumpsFound = 0;

			/*Make a copy of board with edges to prevent overrunning arrays later*/
			int newBoard[BOARD_SIZE + 2][BOARD_SIZE + 2];

			for (int i = 0; i < BOARD_SIZE + 2; i++) {
				for (int j = 0; j < BOARD_SIZE + 2; j++) {
					if (i == 0 || j == 0 || i == BOARD_SIZE + 1 || j == BOARD_SIZE + 1) {
						newBoard[i][j] = BOARD_EDGE;
					}
					else {
						newBoard[i][j] = board[i - 1][j - 1];
					}
				}
			}

			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					//White pieces
					if (newBoard[i + 1][j + 1] == W_PIECE) {
						if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == B_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
						if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == B_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
						if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == B_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
						if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == B_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
					}
					//Black pieces
					else if (newBoard[i + 1][j + 1] == B_PIECE) {
						if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == W_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == W_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == W_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == W_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
					}
					//White kings
					else if (newBoard[i + 1][j + 1] == W_KING) {
						if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == B_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
						if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == B_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
						if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == B_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
						if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == B_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == B_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == B_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == B_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == B_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 1) {
								jumpsFound++;
							}
						}
					}
					//Black kings
					else if (newBoard[i + 1][j + 1] == B_KING) {
						if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == W_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
						if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == W_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
						if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == W_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
						if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == W_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i + 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == W_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == W_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j - 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == W_PIECE) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
						if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == W_KING) {
							validJumps[jumpsFound][0] = i;
							validJumps[jumpsFound][1] = j;
							validJumps[jumpsFound][2] = i - 2;
							validJumps[jumpsFound][3] = j + 2;
							if (player == 2) {
								jumpsFound++;
							}
						}
					}
				}
			}
			//-1 to signal end of validJumps
			validJumps[jumpsFound][0] = -1;
			validJumps[jumpsFound][1] = -1;
			validJumps[jumpsFound][2] = -1;
			validJumps[jumpsFound][3] = -1;

			return jumpsFound;
		}

		//overloaded version of calculateJumps but this one is to be used if the player has just jumped, with prevJump being a validJumps element of the last jump.
		int calculateJumps(int player, int prevJump[4]) {

			int jumpsFound = 0;

			/*Make a copy of board with edges to prevent overrunning arrays later*/
			int newBoard[BOARD_SIZE + 2][BOARD_SIZE + 2];

			for (int i = 0; i < BOARD_SIZE + 2; i++) {
				for (int j = 0; j < BOARD_SIZE + 2; j++) {
					if (i == 0 || j == 0 || i == BOARD_SIZE + 1 || j == BOARD_SIZE + 1) {
						newBoard[i][j] = BOARD_EDGE;
					}
					else {
						newBoard[i][j] = board[i - 1][j - 1];
					}
				}
			}

			int i = prevJump[2];
			int j = prevJump[3];

			clearValidJumps();
			//White pieces
			if (newBoard[i + 1][j + 1] == W_PIECE) {
				if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == B_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
				if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == B_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
				if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == B_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
				if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == B_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
			}
			//Black pieces
			else if (newBoard[i + 1][j + 1] == B_PIECE) {
				if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == W_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == W_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == W_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == W_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
			}
			//White kings
			else if (newBoard[i + 1][j + 1] == W_KING) {
				if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == B_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
				if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == B_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
				if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == B_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
				if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == B_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == B_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == B_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == B_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == B_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 1) {
						jumpsFound++;
					}
				}
			}
			//Black kings
			else if (newBoard[i + 1][j + 1] == B_KING) {
				if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == W_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
				if (newBoard[i + 3][j - 1] == NO_PIECE && newBoard[i + 2][j] == W_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
				if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == W_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
				if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == W_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i + 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == W_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j - 1] == NO_PIECE && newBoard[i][j] == W_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j - 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == W_PIECE) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
				if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == W_KING) {
					validJumps[jumpsFound][0] = i;
					validJumps[jumpsFound][1] = j;
					validJumps[jumpsFound][2] = i - 2;
					validJumps[jumpsFound][3] = j + 2;
					if (player == 2) {
						jumpsFound++;
					}
				}
			}
			//-1 to signal end of validJumps
			validJumps[jumpsFound][0] = -1;
			validJumps[jumpsFound][1] = -1;
			validJumps[jumpsFound][2] = -1;
			validJumps[jumpsFound][3] = -1;

			return jumpsFound;
		}

		//displayJumps displays the contents of validJumps to the consol.
		void displayJumps(int player) {
			for (int i = 0; validJumps[i][0] != -1; i++) {
				if (board[validJumps[i][0]][validJumps[i][1]] == player || board[validJumps[i][0]][validJumps[i][1]] == player + 2) {
					std::cout << i << "-	" << validJumps[i][0] << ", " << validJumps[i][1] << " -> " << validJumps[i][2] << ", " << validJumps[i][3] << "\n";
				}
			}
		}

		//performJump performs a move from the array of validJumps
		int performJump(int player, int chosenJump = -1) {

			//Either can have user choose move in function fo pass chosenMove to function on input (useful for AI).
			if (chosenJump == -1) {
				int validChoice = 0;

				while (validChoice == 0) {
					std::cout << "Choose a valid move.\n";
					std::cin >> chosenJump;
					if (board[validJumps[chosenJump][0]][validJumps[chosenJump][1]] != player && board[validJumps[chosenJump][0]][validJumps[chosenJump][1]] != player + 2) {
						std::cout << "Invalid selection.\n";
					}
					else {
						validChoice = 1;
					}
				}
			}
			//Copy old piece to new position.
			board[validJumps[chosenJump][2]][validJumps[chosenJump][3]] = board[validJumps[chosenJump][0]][validJumps[chosenJump][1]];
			//Delete old piece.
			board[validJumps[chosenJump][0]][validJumps[chosenJump][1]] = NO_PIECE;
			//Delete jumped piece.
			board[validJumps[chosenJump][0] + ((validJumps[chosenJump][2] - validJumps[chosenJump][0]) / 2)][validJumps[chosenJump][1] + ((validJumps[chosenJump][3] - validJumps[chosenJump][1]) / 2)] = NO_PIECE;

			//Check for king
			checkKing();

			return chosenJump;
		}

		//checkKing check to see if any pieces are eligable to become king.
		void checkKing() {

			for (int i = 0; i < BOARD_SIZE; i++) {
				if (board[0][i] == B_PIECE) {
					board[0][i] = B_KING;
				}
				if (board[BOARD_SIZE - 1][i] == W_PIECE) {
					board[BOARD_SIZE - 1][i] = W_KING;
				}
			}
		}

		//displayBoard prints board to the consol.
		void displayBoard() {
			
			std::cout << "-----------------\n";
			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					std::cout << "|";
					if (board[i][j] == W_PIECE) {
						std::cout << "x";
					}
					else if (board[i][j] == B_PIECE) {
						std::cout << "o";
					}
					else if (board[i][j] == W_KING) {
						std::cout << "X";
					}
					else if (board[i][j] == B_KING) {
						std::cout << "O";
					}
					else {
						std::cout << " ";
					}
				}
				std::cout << "|\n";
			}
			std::cout << "-----------------\n";
		}

		//winCheck checks if either player has won the game.
		int winCheck() {

			int bPieces = 0,
				wPieces = 0;
			//count number of pieces for each player
			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					if (board[i][j] == W_PIECE || board[i][j] == W_KING) {
						wPieces++;
					}
					if (board[i][j] == B_PIECE || board[i][j] == B_KING) {
						bPieces++;
					}
				}
			}
			//Player wins if other has 0.
			if (bPieces == 0) {
				return 1;
			}
			else if (wPieces == 0) {
				return 2;
			}
			else return 0;
		}
};

/*AI class*/
class AI {

	//Access specifier
	private:

		int prevBoard[N_PREV_BOARDS][BOARD_SIZE][BOARD_SIZE];	//boards to store previous board states to prevent AI from repeating itself.
		int oldestBoard;

	//Access specifier
	public:

		//default constructor
		AI() {
			oldestBoard = 0;
			for (int i = 0; i < N_PREV_BOARDS; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					for (int k = 0; k < BOARD_SIZE; k++) {
						prevBoard[i][j][k] = NO_PIECE;
					}
				}
			}
		}

		//Saves given board state to prevBoard.
		void saveBoard(Board b) {

			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					prevBoard[oldestBoard][i][j] = b.board[i][j];
				}
			}
			oldestBoard++;
			if (oldestBoard >= N_PREV_BOARDS) {
				oldestBoard = 0;
			}
		}

		//Compairs 2 boards, returns 0 if all are different, 1 if any are the same
		int compairBoards(Board b) {

			int same = 0;

			for (int i = 0; i < N_PREV_BOARDS; i++) {
				same = 1;
				for (int j = 0; j < BOARD_SIZE; j++) {
					for (int k = 0; k < BOARD_SIZE; k++) {

						if (b.board[j][k] != prevBoard[i][j][k]) {
							same = 0;
						}
					}
				}
				if (same == 1) {
					return same;
				}
			}
			return same;
		}

		//sets relMoves array to all -1.
		void initRelMoves(int relMoves[RELEVANT_MOVES]) {

			for (int i = 0; i < RELEVANT_MOVES; i++) {
				relMoves[i] = -1;
			}
		}

		//Chooses move to look ahead for (calculation is based on distance of opponents pieces from players pieces).
		void pickRelevantMoves(Board b, int relMoves[RELEVANT_MOVES], int player) {

			double relMovesValues[RELEVANT_MOVES];
			int relMovesBuffer[RELEVANT_MOVES];

			//initialise relMovesValues
			for (int i = 0; i < RELEVANT_MOVES; i++) {
				relMovesValues[i] = 0;
			}
			//initialise relMovesBuffer
			for (int i = 0; i < RELEVANT_MOVES; i++) {
				relMovesBuffer[i] = -1;
			}
	
			for (int i = 0; b.validMoves[i][0] != -1; i++) {

				//check that piece belongs to player
				if (b.board[b.validMoves[i][0]][b.validMoves[i][1]] == player || b.board[b.validMoves[i][0]][b.validMoves[i][1]] == player + 2) {
					
					double relevance = 0;
					//calculate relevance
					for (int j = 0; j < BOARD_SIZE; j++) {
						for (int k = 0; k < BOARD_SIZE; k++) {
							if (b.board[j][k] != player || b.board[j][k] != player + 2 || b.board[j][k] != NO_PIECE) {	//check for other player piece
								relevance += (sqrt(2) * BOARD_SIZE) - sqrt((j - b.validMoves[i][0]) * (j - b.validMoves[i][0]) + (k - b.validMoves[i][1]) * (k - b.validMoves[i][1]));
							}
						}
					}
					//look for lowest element in relMovesValues.
					int lowest = 0;
					for (int j = 1; j < RELEVANT_MOVES; j++) {
						if (relMovesValues[j] < relMovesValues[j - 1]) {
							lowest = j;
						}
					}
					if (relevance > relMovesValues[lowest]) {
						relMovesBuffer[lowest] = i;
						relMovesValues[lowest] = relevance;
					}
				}
			}
			int j = 0;
			for (int i = 0; i < RELEVANT_MOVES; i++) {	//loop to copy buffer contents to relMoves (excluding -1's except from ones a end).
				if (relMovesBuffer[i] != -1) {
					relMoves[j] = relMovesBuffer[i];
					j++;
				}
			}
		}

		//Calculates a rating for the given board state.
		double calculateRating(Board b, int player) {

			double rating = 0;

			//count number of jumps player can make
			int playerJumps = b.calculateJumps(player);
			if (playerJumps > 0) {	//if player can jump- positive rating, more jumps = higher rating.
				rating += playerJumps * RATING_PARAM_JUMP_1;
				rating += pow(playerJumps * RATING_PARAM_JUMP_2, 2);
			}
			//count number of kings for player
			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					if (b.board[i][j] == player + 2) {
						rating += RATING_PARAM_KING;
					}
				}
			}

			/*calculate rating based on number of pieces which have even number of squares between us and them as this is likely
			to eventually lead to us jumping them.*/
			for (int i = 0; i < BOARD_SIZE; i++) {	//loop over player pieces
				for (int j = 0; j < BOARD_SIZE; j++) {

					if (b.board[i][j] == player || b.board[i][j] == player + 2) {
						for (int n = 0; n < BOARD_SIZE; n++) {	//loop over opponent pieces
							for (int m = 0; m < BOARD_SIZE; m++) {
								if (b.board[n][m] == player % 2 + 1 || b.board[n][m] == player % 2 + 3) {
									//calculate distance between two points
									double distance = sqrt((i - n)*(i - n) + (j - m)*(j - m));
									
									if (((int)distance + 1) % 2 == 0) {	//truncate and add 1 to round up.	
										rating += (1 / distance) * RATING_PARAM_DIST_1;	//value closer pieces.
										rating += pow((1 / distance) * RATING_PARAM_DIST_2, 2);
									}
								}
							}
						}
					}
				}
			}
			return rating;
		}

		/*Loop to look ahead multiple turns, called by lookAhead*/
		double lookAheadLoop(Board b, int ahead, int player) {
		
			double rating = 0;

			int canJump = b.calculateJumps(player);
			if (canJump > 0) {	//if can jump, dont loop again but rate board state after jumps as jumps can get very complicated due to another turn after jump.
				for (int i = 0; i < canJump; i++) {

					Board nextBoard(b.board);
					nextBoard.calculateJumps(player);
					nextBoard.performJump(player, i);

					//Rate board after jump to determine if it was a good jump.
					if (ahead % 2 != 0) {//our jump
						rating += pow(calculateRating(nextBoard, player), MOVES_AHEAD - ahead + 1);	//give significant priority to our jumps that happen sooner.
					}
					else {//other players jump, so we subtract the rating of their move.
						rating = rating - pow(calculateRating(nextBoard, player % 2 + 1), (MOVES_AHEAD - ahead + 1));	//give priority to lasting longer without getting jumped.
					}

				}
			}

			if (ahead < MOVES_AHEAD && canJump == 0) {	//Loop again

				int relMoves[RELEVANT_MOVES];
				initRelMoves(relMoves);
				b.calculateMoves(player);
				pickRelevantMoves(b, relMoves, player);

				double loopRating = 0;

				for (int i = 0; i < RELEVANT_MOVES && relMoves[i] != -1; i++) {
					Board nextBoard(b.board);
					nextBoard.calculateMoves(player);
					nextBoard.performMove(player, relMoves[i]);

					//Rate board state after we have performed move.
					if (ahead % 2 != 0) {	//If its our turn add the rating of the board state onto total rating of move.
						loopRating += (calculateRating(b, player));
					}
					else {	//If its opponents turn subtract rating of board (from their perspective) from total move rating.
						loopRating = loopRating - calculateRating(b, player % 2 + 1);
					}

					loopRating = lookAheadLoop(nextBoard, ahead + 1, player % 2 + 1);

					if (loopRating > rating) {
						rating = loopRating;
					}
				}
			}
			else if (canJump == 0 && ahead >= MOVES_AHEAD) {	//cant jump and at end of loop so just calculate value of board.
				if (ahead % 2 != 0) {	//loop ends on out move
					rating += calculateRating(b, player);
				}
				else {	//loop ends on opponents move.
					rating += calculateRating(b, player%2 + 1);
				}
			}
			/*else- end of loop*/
			return rating;
		}

		/*Looks ahead at next seriese to turns to work out which move is best, returns index to best move*/
		int lookAhead(Board b, int player) {

			double rating = 0,
				maxRating = 0;		//Big negative number
			int chosenMove = -1;

			int canJump = b.calculateJumps(player);

			if (canJump == 0) {

				int relMoves[RELEVANT_MOVES];
				initRelMoves(relMoves);
				pickRelevantMoves(b, relMoves, player);

				for (int i = 0; i < RELEVANT_MOVES && relMoves[i] != -1; i++) {

					Board tempBoard(b.board);	//creat copy of board so we dont change it.
					tempBoard.calculateMoves(player);
					tempBoard.performMove(player, relMoves[i]);

					//check to see if game is stuck in a loop.
					int isRepeat = 0;
					if (compairBoards(tempBoard) == 1) {
						isRepeat++;
					}
					if (isRepeat > 0) {
						std::cout << "Board matches previous\n";
						rating = -1e200;	//If board is same as a previous board, try not to perform this move.
					}
					else {
						rating = lookAheadLoop(tempBoard, 0, player % 2 + 1);
					}
					
				
					std::cout << "rating for move " << relMoves[i] << " = " << rating << "\n";
					
					if (rating > maxRating || i == 0) {	//make sure to allow first move to become maxRating.
						maxRating = rating;
						chosenMove = relMoves[i];
					}
				}
			}
			else {
				for (int i = 0; i < canJump; i++) {

					Board tempBoard(b.board);	//creat copy of board to we dont change it.
					tempBoard.calculateJumps(player);
					tempBoard.performJump(player, i);
					//check to see if we can jump again
					if (tempBoard.calculateJumps(player) == 0) {
						rating = lookAheadLoop(tempBoard, 0, player%2 + 1);	//If we cant jump again then its opponents turn.
					}
					else {
						rating = lookAheadLoop(tempBoard, 1, player);	//If we can jump again then its our turn.
					}

					std::cout << "rating for jump " << i << " = " << rating << "\n";

					if (rating > maxRating || i == 0) {
						maxRating = rating;
						chosenMove = i;
					}
				}
			}
			if (chosenMove == -1) {
				std::cout << "Warning: no moves suitable enough.\n";
			}
			return chosenMove;
		}
};

/*gameLoop function performs main loop to play game with 2 players*/
void gameLoop(Board b) {

	int canJump,
		hasJump,
		chosenJump;

PlayerOneTurn:
	if (b.winCheck() == 0) {
		hasJump = 0;
		b.displayBoard();

	POneJumpAgain:
		std::cout << "Player one's turn:\n\n";
		if (hasJump == 0) {
			canJump = b.calculateJumps(1);
		}
		else {
			canJump = b.calculateJumps(1, b.validJumps[chosenJump]);
		}

		if (canJump != 0) {
			if (hasJump == 1) {
				b.displayBoard();
			}
			b.displayJumps(1);
			chosenJump = b.performJump(1);
			hasJump = 1;
			goto POneJumpAgain;
		}
		else if(hasJump == 0){
			b.calculateMoves(1);
			b.displayMoves(1);
			b.performMove(1);
		}

		goto PlayerTwoTurn;
	}
	else {
		goto Win;
	}

PlayerTwoTurn:
	if (b.winCheck() == 0) {
		hasJump = 0;
		b.displayBoard();

	PTwoJumpAgain:
		std::cout << "Player two's turn:\n\n";
		if (hasJump == 0) {
			canJump = b.calculateJumps(2);
		}
		else {
			canJump = b.calculateJumps(2, b.validJumps[chosenJump]);
		}

		if (canJump != 0) {
			if (hasJump == 1) {
				b.displayBoard();
			}
			b.displayJumps(2);
			chosenJump = b.performJump(2);
			hasJump = 1;
			goto PTwoJumpAgain;
		}
		else if(hasJump == 0){
			b.calculateMoves(2);
			b.displayMoves(2);
			b.performMove(2);
		}

		goto PlayerOneTurn;
	}
	else {
	Win:
		b.displayBoard();
		std::cout << "player " << b.winCheck() << " wins!\n";
	}
}

/*gameLoop function performs main loop to play game with 1 AI and 1 player*/
void gameLoop(Board b, AI a) {

	int canJump,
		hasJump,
		chosenJump;

PlayerOneTurn:
	if (b.winCheck() == 0) {
		hasJump = 0;
		b.displayBoard();

	POneJumpAgain:
		std::cout << "Player one's turn:\n\n";
		if (hasJump == 0) {
			canJump = b.calculateJumps(1);
		}
		else {
			canJump = b.calculateJumps(1, b.validJumps[chosenJump]);
		}

		if (canJump != 0) {
			if (hasJump == 1) {
				b.displayBoard();
			}
			b.displayJumps(1);
			chosenJump = b.performJump(1);
			hasJump = 1;
			goto POneJumpAgain;
		}
		else if (hasJump == 0) {
			b.calculateMoves(1);	//stalemate shouldnt occur if player is playing.
			b.displayMoves(1);
			b.performMove(1);
		}

		goto PlayerTwoTurn;
	}
	else {
		goto Win;
	}

PlayerTwoTurn:
	if (b.winCheck() == 0) {
		hasJump = 0;
		b.displayBoard();

	PTwoJumpAgain:
		std::cout << "Player two's turn:\n\n";
		if (hasJump == 0) {
			canJump = b.calculateJumps(2);
		}
		else {
			canJump = b.calculateJumps(2, b.validJumps[chosenJump]);
		}

		if (canJump != 0) {
			if (hasJump == 1) {
				b.displayBoard();
			}
			b.displayJumps(1);
			std::cout << "Thinking...\n";
			int move = a.lookAhead(b, 2);
			std::cout << "I have chosen " << move << "\n";
			chosenJump = b.performJump(2, move);
			hasJump = 1;
			goto PTwoJumpAgain;
		}
		else if (hasJump == 0) {
			b.calculateMoves(2);
			b.displayMoves(2);
			std::cout << "Thinking...\n";
			int move = a.lookAhead(b, 2);
			std::cout << "I have chosen " << move << "\n";
			b.performMove(2, move);
		}
		a.saveBoard(b);
		goto PlayerOneTurn;
	}
	else {
	Win:
		b.displayBoard();
		std::cout << "player " << b.winCheck() << " wins!\n";
	}
}

/*gameLoop function performs main loop to play game with 2 AI's*/
void gameLoop(Board b, AI a1, AI a2) {

	int canJump,
		hasJump,
		chosenJump,
		winner = 0;

PlayerOneTurn:
	winner = b.winCheck();
	if (winner == 0) {
		hasJump = 0;
		b.displayBoard();

	POneJumpAgain:
		std::cout << "Player one's turn:\n\n";
		if (hasJump == 0) {
			canJump = b.calculateJumps(1);
		}
		else {
			canJump = b.calculateJumps(1, b.validJumps[chosenJump]);
		}

		if (canJump != 0) {
			if (hasJump == 1) {
				b.displayBoard();
			}
			b.displayJumps(1);
			std::cout << "Thinking...\n";
			int move = a1.lookAhead(b, 1);
			std::cout << "I have chosen " << move << "\n";
			chosenJump = b.performJump(1, move);
			hasJump = 1;
			goto POneJumpAgain;
		}
		else if (hasJump == 0) {
			b.calculateMoves(1);
			b.displayMoves(1);
			std::cout << "Thinking...\n";
			int move = a1.lookAhead(b, 1);

			if (move == -1) {	//move will be -1 in event of stalemate (current player cant move).
				winner = 2;
				goto Win;
			}
			else {
				std::cout << "I have chosen " << move << "\n";
				b.performMove(1, move);
			}
			
		}
		a1.saveBoard(b);
		goto PlayerTwoTurn;
	}
	else {
		goto Win;
	}

PlayerTwoTurn:
	winner = b.winCheck();
	if (winner == 0) {
		hasJump = 0;
		b.displayBoard();

	PTwoJumpAgain:
		std::cout << "Player two's turn:\n\n";
		if (hasJump == 0) {
			canJump = b.calculateJumps(2);
		}
		else {
			canJump = b.calculateJumps(2, b.validJumps[chosenJump]);
		}

		if (canJump != 0) {
			if (hasJump == 1) {
				b.displayBoard();
			}
			b.displayJumps(2);
			std::cout << "Thinking...\n";
			int move = a2.lookAhead(b, 2);
			std::cout << "I have chosen " << move << "\n";
			chosenJump = b.performJump(2, move);
			hasJump = 1;
			goto PTwoJumpAgain;
		}
		else if (hasJump == 0) {
			b.calculateMoves(2);
			b.displayMoves(2);
			std::cout << "Thinking...\n";
			int move = a2.lookAhead(b, 2);

			if (move == -1) {	//move will be -1 in event of stalemate (current player cant move).
				winner = 1;
				goto Win;
			}
			else {
				std::cout << "I have chosen " << move << "\n";
				b.performMove(2, move);
			}
		}
		a2.saveBoard(b);
		goto PlayerOneTurn;
	}
	else {
	Win:
		b.displayBoard();
		std::cout << "player " << winner << " wins!\n";
	}
}

int main()
{

	Board b;

	std::cout << "Choose a game mode\n";
	std::cout << "1- player vs player\n";
	std::cout << "2- player vs AI\n";
	std::cout << "3- AI vs AI\n";

	char choice;
Again:
	std::cin >> choice;

	switch (choice) {
	case '1':
		gameLoop(b);
		break;
	case '2':
	{AI a;
	gameLoop(b, a);
	break; }
	case '3':
	{AI a1, a2;
	gameLoop(b, a1, a2);
	break; }
	default:
		std::cout << "Invalid selection, try again.\n";
		goto Again;
	}

	std::cin.clear();
	std::cin.ignore(32767, '\n');
	std::cin.get();	//Persist consol
    return 0;
}

