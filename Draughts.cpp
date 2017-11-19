/* Draughts.cpp : Defines the entry point for the console application. **
** Jamie Bull **
** 19/11/2017 */


#include "stdafx.h"
#include <iostream>

#define BOARD_SIZE 8	//8x8 square board
#define VALID_MOVES_SIZE 49	//size of validMoves array, 49 being theoretical maximum number of moves is board is 50% populated and every piece is a king.

/*pieces*/
#define NO_PIECE 0
#define W_PIECE 1
#define B_PIECE 2
#define W_KING 3
#define B_KING 4
#define BOARD_EDGE -1

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

		//clearValidMoves sets the valid moves array to all 0's.
		void clearValidMoves() {

			for (int i = 0; i < VALID_MOVES_SIZE; i++) {
				for (int j = 0; j < 4; j++) {
					validMoves[i][j] = 0;
				}
			}
		}

		//calculateMoves determines valid moves for the whole board and stores them in the validMoves array, returns number of valid moves found.
		int calculateMoves() {

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
					if (newBoard[i + 1][j + 1] == W_PIECE) {
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
					else if (newBoard[i + 1][j + 1] == B_PIECE) {
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
					//White kings and black kings
					else if (newBoard[i + 1][j + 1] == W_KING || newBoard[i + 1][j + 1] == B_KING) {
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
						if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == B_PIECE) {
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
						if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == W_PIECE) {
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
						if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == B_PIECE) {
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
						if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == B_PIECE) {
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
						if (newBoard[i + 3][j + 3] == NO_PIECE && newBoard[i + 2][j + 2] == W_PIECE) {
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
						if (newBoard[i - 1][j + 3] == NO_PIECE && newBoard[i][j + 2] == W_PIECE) {
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

		//displayJumps displays the contents of validJumps to the consol.
		void displayJumps(int player) {
			for (int i = 0; validJumps[i][0] != -1; i++) {
				if (board[validJumps[i][0]][validJumps[i][1]] == player || board[validJumps[i][0]][validJumps[i][1]] == player + 2) {
					std::cout << i << "-	" << validJumps[i][0] << ", " << validJumps[i][1] << " -> " << validJumps[i][2] << ", " << validJumps[i][3] << "\n";
				}
			}
		}

		//performJump performs a move from the array of validJumps
		void performJump(int player, int chosenJump = -1) {

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

/*gameLoop function performs main loop to play game*/
void gameLoop(Board b) {

	int canJump,
		hasJump;

PlayerOneTurn:
	if (b.winCheck() == 0) {
		hasJump = 0;
	POneJumpAgain:
		b.displayBoard();
		canJump = b.calculateJumps(1);
		if (canJump != 0) {
	
			b.displayJumps(1);
			b.performJump(1);
			hasJump = 1;
			goto POneJumpAgain;
		}
		else if(hasJump == 0){
			b.calculateMoves();
			b.displayMoves(1);
			b.performMove(1);
		}

		goto PlayerTwoTurn;
	}
	else {
		std::cout << "player " << b.winCheck() << "wins!\n";
	}

PlayerTwoTurn:
	if (b.winCheck() == 0) {
		hasJump = 0;
	PTwoJumpAgain:
		b.displayBoard();
		canJump = b.calculateJumps(2);
		if (canJump != 0) {
			
			b.displayJumps(2);
			b.performJump(2);
			hasJump = 1;
			goto PTwoJumpAgain;
		}
		else if(hasJump == 0){
			b.calculateMoves();
			b.displayMoves(2);
			b.performMove(2);
		}

		goto PlayerOneTurn;
	}
	else {
		std::cout << "player " << b.winCheck() << "wins!\n";
	}
}

int main()
{

	Board b;
	gameLoop(b);

	std::cin.clear();
	std::cin.ignore(32767, '\n');
	std::cin.get();	//Persist consol
    return 0;
}

