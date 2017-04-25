//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c – group assignment
//Author: Pascale Vacher
//Last updated: 24 February 2017
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>	
#include <iomanip> 
#include <conio.h> 
#include <cassert> 
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <direct.h>
#include <stdlib.h>
#include <algorithm>
using namespace std;

//include our own libraries
#include "ConsoleUtils.h"	//for Clrscr, Gotoxy, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

//defining the size of the grid
const int  SIZEX(16);    	//horizontal dimension
const int  SIZEY(11);		//vertical dimension
//defining symbols used for display of the grid and content
const char BEAR('@');   	//bear
const char TUNNEL(' ');    	//tunnel
const char WALL('#');    	//border
const char BOMB('0');
const char DETONATOR('T');
const char EXIT('X');
const char PILL('P');
const char LOCK('L');
const char KEY('K');
//defining the command letters to move the bear on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game
const char CHEAT('C');		//To activate cheat mode
const char UNLOCK('U');		//To unlock all levels
const char RULES('F');		//To display rules
//Add global key here for rules (Liam).

const string playerFileLocation = "players\\";
const string playerFileType = ".txt";

struct Item {
	int x, y;
	char symbol;
	bool visible;
	bool active;
};

struct Bear {
	int x, y;
	char symbol;
	bool visible;
	bool moved;
	bool invincible;
};

struct Player{
	string name;
	int score;
	bool cheated = false;
	bool cheating = false;
};

struct Pill
{
	Item item;
	int moves;
	bool taken;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void endProgram();
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	int paintMainMenu(int& unlockedLevels);
	int getKeyPress();
	void playGame(char grid[][SIZEX], char maze[][SIZEX], vector<Bear>& bears, vector<Item>& bombs, Item& detonator, string& message, int& noOfMoves, Player& player, vector<Player>& highscores, Pill& pill, Item& lock, Item& lKey, int& level, int& unlockedLevels);
	string paintEntryScreen();
	Player player;
	Player loadPlayer(const string playerName);
	vector<Player> loadHighscore();

	

	//local variable declarations 
	char grid[SIZEY][SIZEX];	//grid for display
	char maze[SIZEY][SIZEX];	//structure of the maze
	string message("LET'S START...");	//current message to player
	vector<Player> highscores = loadHighscore();
	vector<Bear> bears;
	

	srand(time(NULL));
	Pill pill;

	vector<Item> bombs;
	Item detonator, lock, lKey;
	const int noOfBombs = 5;
	for (int b = 0; b < noOfBombs; b++) //b for Bomb
	{
		bombs.push_back(Item());
	}

	int noOfMoves(0);
	int level;
	int unlockedLevels = 1;
	
	//Entry screen
	if (_mkdir(playerFileLocation.c_str()) == 0) //If the players folder doesn't exist create it.
	{
		cout << "Created new player save folder: " << playerFileLocation;
	}
	player = loadPlayer(paintEntryScreen()); //Load the details of the player (if any) from file.
	
	int selection;

	do {
		
		selection = paintMainMenu(unlockedLevels);

		switch (selection)
		{
		case 1:
			level = 1;
			for (int b = 0; b < 3; b++) //b for bears
				bears.push_back(Bear());
			playGame(grid, maze, bears, bombs, detonator, message, noOfMoves, player, highscores, pill, lock, lKey, level, unlockedLevels);
			break;
		case 2:
			if (unlockedLevels < 2)
			{
				showMessage(clBlack, clWhite, 8, 10, "That level has not been unlocked yet!");
			}
			else
			{
				level = 2;
				for (int b = 0; b < 3; b++) //b for bears
					bears.push_back(Bear());
				playGame(grid, maze, bears, bombs, detonator, message, noOfMoves, player, highscores, pill, lock, lKey, level, unlockedLevels);
			}
			break;
		case 3:
			void displayRules();
			displayRules();
			break;
		case 4:
			void displayHighscore(const Player& player, const vector<Player>& highscores);
			displayHighscore(player, highscores);
			break;
		default:
			break;
		}
	} while (selection != 5);
	
	endProgram();						//display final message
	return 0;
}

void playGame(char grid[][SIZEX], char maze[][SIZEX], vector<Bear>& bears, vector<Item>& bombs, Item& detonator, string& message, int& noOfMoves, Player& player, vector<Player>& highscores, Pill& pill, Item& lock, Item& lKey, int& level, int& unlockedLevels)
{
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Bear>& bear, vector<Item>& bombs, Item& detonator, Pill& pill, Item& lock, Item& lKey, int level);
	void paintGame(const char g[][SIZEX], string mess, int noOfBears, int noOfMoves, Player player, bool showRules, const vector<Bear>& bear);
	int getKeyPress();
	bool wantsToQuit(const int key);
	bool isCheatKey(const char key);
	bool isArrowKey(const int k);
	bool updateGameData(const char g[][SIZEX], vector<Bear>& bear, vector<Item>& bombs, Item& detonator, const int key, string& mess, int& numberOfMoves, const Player& player, Pill& pill, Item& lock, Item& lKey, int& level);
	void savePlayer(const Player& player);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const vector<Bear> bear, const vector<Item> bombs, const Item detonator, Pill& pill, Item& lock, Item& lKey);
	void saveHighscores(vector<Player>& highscores);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);

	bool gameEnd = false;
	bool showRules = false;
	
	//action...
	initialiseGame(grid, maze, bears, bombs, detonator, pill, lock, lKey, level);		//initialise grid (incl. walls & bear)
	paintGame(grid, message, bears.size(), noOfMoves, player, showRules, bears);	//display game info, modified grid & messages
	int key(getKeyPress()); 									//read in selected key: arrow or letter command
	while (!wantsToQuit(key) && !(gameEnd && (level == 3)))						//while user does not want to quit
	{
		if (gameEnd)
		{
			gameEnd = false;
			level += 1;
			if (unlockedLevels < 2)
			{
				unlockedLevels = 2;
			}
			if (level <= 2)
			{
				for (int b = 0; b < 3; b++) //b for bears
					bears.push_back(Bear());
				initialiseGame(grid, maze, bears, bombs, detonator, pill, lock, lKey, level);	//initialise grid (incl. walls & bear)
				paintGame(grid, message, bears.size(), noOfMoves, player, showRules, bears);			//display game info, modified grid & messages
				int key(getKeyPress()); 			//read in  selected key: arrow or letter command
			}
		}
		if (key != RULES && !showRules)
		{
			if (isCheatKey(key))
			{
				player.cheated = true;
				player.cheating = !player.cheating;
				noOfMoves = 500;
				cout << "\a";
				cout << "\a";
				cout << "\a";
			}
			else if (isArrowKey(key))
			{
				gameEnd = updateGameData(grid, bears, bombs, detonator, key, message, noOfMoves, player, pill, lock, lKey, level);		//move bear in that direction
				
				if (bears.empty() && level == 3)
				{
					if (player.score > noOfMoves && !player.cheated) //If the new score is lower and they haven't cheated
					{
						player.score = noOfMoves; //Change their player record
						savePlayer(player);		  //And update their file.
					}

					if (player.score < highscores.at(5).score)
					{
						highscores.at(5) = player;
						saveHighscores(highscores);
					}

				}
				updateGrid(grid, maze, bears, bombs, detonator, pill, lock, lKey);			//update grid information
			}
		}
		else if (key == RULES)
		{
			if (!showRules)
				showRules = true;
			else
				showRules = false;
		}
		else
			message = "INVALID KEY!";  //set 'Invalid key' message

		paintGame(grid, message, bears.size(), noOfMoves, player, showRules, bears);	//display game info, modified grid & messages

		if (!gameEnd)
		{
			key = getKeyPress(); 		   //display menu & read in next option
		}
		else
		{
			getKeyPress();
			showMessage(clBlack, clWhite, 40, 9, "                                      ");
		}
	}
		
	
	Clrscr();
}

void displayHighscore(const Player& player, const vector<Player>& highscores)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	string makeLength(string s, int length);
	int getKeyPress();
	
	showMessage(clDarkGrey, clYellow, 8, 1, " CURRENT PLAYER: " + makeLength(player.name, 24));
	showMessage(clDarkGrey, clYellow, 8, 2, " PREVIOUS SCORE: " + makeLength(to_string(player.score), 24));
	showMessage(clDarkGrey, clYellow, 8, 4, makeLength(" HIGHSCORES ", 24));
	showMessage(clDarkGrey, clYellow, 8, 5, makeLength(" 1:  " + highscores.at(0).name + " - " + to_string(highscores.at(0).score), 24));
	showMessage(clDarkGrey, clYellow, 8, 6, makeLength(" 2:  " + highscores.at(1).name + " - " + to_string(highscores.at(1).score), 24));
	showMessage(clDarkGrey, clYellow, 8, 7, makeLength(" 3:  " + highscores.at(2).name + " - " + to_string(highscores.at(2).score), 24));
	showMessage(clDarkGrey, clYellow, 8, 8, makeLength(" 4:  " + highscores.at(3).name + " - " + to_string(highscores.at(3).score), 24));
	showMessage(clDarkGrey, clYellow, 8, 9, makeLength(" 5:  " + highscores.at(4).name + " - " + to_string(highscores.at(4).score), 24));
	showMessage(clDarkGrey, clYellow, 8, 10, makeLength(" 6:  " + highscores.at(5).name + " - " + to_string(highscores.at(5).score), 24));
	showMessage(clBlack, clWhite, 8, 12, " Press any key to return.");
	Gotoxy(0, 0);

	getKeyPress();
	Clrscr();
}

void displayRules()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	int getKeyPress();

	showMessage(clDarkGrey, clYellow, 8, 1, "        THREE BEARS GAME        ");
	showMessage(clDarkGrey, clYellow, 8, 2, "              Rules             ");

	showMessage(clDarkGrey, clWhite, 8, 4, " Rescue all bears '@' through       ");
	showMessage(clDarkGrey, clWhite, 8, 5, " exit 'X' avoiding bombs 'O'        ");
	showMessage(clDarkGrey, clWhite, 8, 6, " To disable bombs use detonator 'T' ");
	showMessage(clDarkGrey, clWhite, 8, 7, "                                    ");
	showMessage(clDarkGrey, clWhite, 8, 8, " TO MOVE USE ARROW KEYS             ");
	showMessage(clDarkGrey, clWhite, 8, 9, " TO QUIT ENTER 'Q'                  ");
	showMessage(clBlack, clWhite, 0, 0, ""); //Reset position and colour (otherwise the console would have a grey background colour).

	getKeyPress();
	Clrscr();
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], vector<Bear>& bears, vector<Item>& bombs, Item& detonator, Pill& pill, Item& lock, Item& key, int level)
{ //initialise grid & place bear in middle
	void loadLevel(int LevelNo, char maze[][SIZEX]);
	void setInitialDataFromMaze(char maze[][SIZEX], vector<Bear>& bears, vector<Item>& bombs, Item& detonator, Item& lock, Item& key);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], vector<Bear> bears, vector<Item> bombs, const Item detonator, Pill& pill, Item& lock, Item& key);

	pill.item.symbol = PILL;
	pill.moves = 0;
	pill.taken = false;
	pill.item.visible = false;
	pill.item.x = 0;
	pill.item.y = 0;

	detonator.visible = false;
	lock.visible = false;

	loadLevel(level, maze); //initialsie maze
	setInitialDataFromMaze(maze, bears, bombs, detonator, lock, key);	//initialise bear's position
	updateGrid(grid, maze, bears, bombs, detonator, pill, lock, key);		//prepare grid
}
void setInitialDataFromMaze(char maze[][SIZEX], vector<Bear>& bears, vector<Item>& bombs, Item& detonator, Item& lock, Item& key)
{ //extract bear's coordinates from initial maze info
	int noOfBears = 0;
	int noOfBombs = 0;
	detonator.visible = false;
	detonator.x = -1;
	detonator.y = -1;
	lock.visible = false;
	key.visible = false;
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col)
			switch (maze[row][col])
		{
			case BEAR:
			{
				bears[noOfBears].x = col;
				bears[noOfBears].y = row;
				bears[noOfBears].symbol = BEAR;
				bears[noOfBears].visible = true;
				bears[noOfBears].invincible = false;
				noOfBears++;
				maze[row][col] = TUNNEL;
				break;
			}
			case DETONATOR:
			{
				detonator.x = col;
				detonator.y = row;
				detonator.symbol = DETONATOR;
				detonator.visible = true;
				detonator.active = false;
				maze[row][col] = TUNNEL;
				break;
			}
			case BOMB:
			{
				bombs[noOfBombs].x = col;
				bombs[noOfBombs].y = row;
				bombs[noOfBombs].symbol = BOMB;
				bombs[noOfBombs].visible = true;
				noOfBombs++;
				maze[row][col] = TUNNEL;
				break;
			}
			case LOCK:
				lock.x = col;
				lock.y = row;
				lock.symbol = LOCK;
				lock.visible = true;
				lock.active = true;
				break;
			case KEY:
				key.x = col;
				key.y = row;
				key.symbol = KEY;
				key.visible = true;
				break;
		}
	}
}
void loadLevel(int LevelNo, char maze[][SIZEX])
{
	ifstream level;
	string line;
	level.open("levels\\level" + to_string(LevelNo) + ".txt");
	if (level.is_open())
	{
		for (int row(0); row < SIZEY; ++row)
		{
			getline(level, line);
			for (int col(0); col < SIZEX; ++col)
			{
				int current = line[col] - '0';
				switch (current)
				{
				case 0: maze[row][col] = TUNNEL; break;
				case 1: maze[row][col] = WALL; break;
				case 2: maze[row][col] = BEAR; break;
				case 3: maze[row][col] = BOMB; break;
				case 4: maze[row][col] = DETONATOR; break;
				case 5: maze[row][col] = EXIT; break;
				case 6: maze[row][col] = LOCK; break;
				case 7: maze[row][col] = KEY; break;
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], const vector<Bear> bears, const vector<Item> bombs, const Item detonator, Pill& pill, Item& lock, Item& key)
{ //update grid configuration after each move
	void setMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeBear(char g[][SIZEX], const Bear bear);
	void placeBomb(char g[][SIZEX], const Item bomb);
	void placePill(char g[][SIZEX], Pill& pill);

	setMaze(grid, maze);	//reset the empty maze configuration into grid
	for (auto bomb : bombs)
	{
		placeBomb(grid, bomb);
	}

	if (detonator.visible)
	{
		grid[detonator.y][detonator.x] = detonator.symbol;
	}
	if (lock.visible)
	{
		grid[lock.y][lock.x] = lock.symbol;
	}
	else if (!lock.active)
	{
		grid[lock.y][lock.x] = TUNNEL;
		grid[key.y][key.x] = TUNNEL;
	}
	if (key.visible)
	{
		grid[key.y][key.x] = key.symbol;
	}
	placePill(grid, pill);
	for (auto bear : bears)
	{
		placeBear(grid, bear);	//set bear in grid
	}
}

void setMaze(char grid[][SIZEX], const char maze[][SIZEX])
{ //reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row)	
		for (int col(0); col < SIZEX; ++col)	
			grid[row][col] = maze[row][col];
}

void placeBear(char g[][SIZEX], const Bear bear)
{ //place bear at its new position in grid
	if (bear.visible)
		g[bear.y][bear.x] = bear.symbol;
}

void placeBomb(char g[][SIZEX], const Item bomb)
{
	if (bomb.visible)
		g[bomb.y][bomb.x] = bomb.symbol;
}

void placePill(char g[][SIZEX], Pill& pill)
{
	if (pill.moves >= 10 && !pill.taken)
	{
		pill.moves = 0;
		pill.item.visible = true;
		bool placed = false;
		while (placed == false)
		{
			int x = rand() % SIZEX;
			int y = rand() % SIZEY;
			if (g[y][x] == TUNNEL)
			{
				pill.item.x = x;
				pill.item.y = y;
				placed = true;
			}
		}
	}
	if (pill.item.visible)
	{
		g[pill.item.y][pill.item.x] = pill.item.symbol;
	}
	pill.moves++;
}
//---------------------------------------------------------------------------
//----- move the bear
//---------------------------------------------------------------------------
bool updateGameData(const char g[][SIZEX], vector<Bear>& bears, vector<Item>& bombs, Item& detonator, const int key, string& mess, int& numberOfMoves, const Player& player, Pill& pill, Item& lock, Item& lKey, int& level)
{ //move bear in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	void setMaze(char grid[][SIZEX], const char maze[][SIZEX]);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void endProgram();
	bool gameEnd = false;
	assert(isArrowKey(key));
	
	char maze[SIZEY][SIZEX];
	setMaze(maze, g);

	void removeBombs(vector<Item>& bombs);
	mess = "                                         ";		//reset message to blank
	const int noOfBombs = bombs.size();

	//calculate direction of movement for given key
	int dx(0), dy(0), moved(0), deleteIndex(-1);
	setKeyDirection(key, dx, dy); 
	while (moved < bears.size())
	{

		for (auto &bear : bears){
			//check new target position in grid and update game data (incl. bear coordinates) if move is possible
			switch (maze[bear.y + dy][bear.x + dx])
			{			//...depending on what's on the target position in grid...
			case TUNNEL:		//can move
				if (player.cheating || bear.invincible)
				{
					if (detonator.x == bear.x && detonator.y == bear.y) //Reset so the detonator is visible when the bear moves off it.
						detonator.visible = true;
					else
						if (!bear.moved)
							for (int b = 0; b < noOfBombs; b++)
							{
								if (bear.y == bombs[b].y && bear.x == bombs[b].x && (player.cheating || bear.invincible) && !detonator.active) //Reset so the bombs are visible when the bear moves off it.
									bombs[b].visible = true;
								else
									if (!bear.moved)
										maze[bear.y][bear.x] = TUNNEL;
							}
				}
				else
				{
					if (detonator.x == bear.x && detonator.y == bear.y) // redraw detonator after moving off it
					{
						maze[bear.y][bear.x] = DETONATOR;
						detonator.visible = true;
					}
					for (int b = 0; b < noOfBombs; b++)
					{
						if (bear.y == bombs[b].y && bear.x == bombs[b].x && (player.cheating || bear.invincible) && !detonator.active) //Reset so the bombs are visible when the bear moves off it.
							bombs[b].visible = true;
						else
							if (!bear.moved)
								maze[bear.y][bear.x] = TUNNEL;
					}
				}
				if (!bear.moved)
				{
					bear.y += dy;	//go in that Y direction
					bear.x += dx;	//go in that X direction
					moved++;
					bear.moved = true;
				}
				break;
			case BEAR:
				for (auto &bear2 : bears)
				{
					if (bear2.x == bear.x + dx && bear2.y == bear.y + dy)
					{
						if (bear2.moved)
						{
							bear.moved = true;
							moved++;
						}
					}
				}
				break;
			case WALL:  		//hit a wall and stay there
				if (!bear.moved)
				{
					bear.moved = true;
					moved++;
					cout << "\a";
				}
				break;
			case DETONATOR:
				if (!bear.moved)
				{
					detonator.active = true;
				detonator.visible = false;
				bear.y += dy;	//move the bear onto the detonator
				bear.x += dx;
				bear.moved = true;
				moved++;
				if (!player.cheating)
					removeBombs(bombs);
				}
				break;
			case BOMB:
				if (!(player.cheating || bear.invincible))
				{
					gameEnd = true;
					level = 3;
					mess = "You just killed a bear, you sad person!  ";
					showMessage(clBlack, clWhite, 40, 9, "Press any key to return to the main menu.");
					maze[bear.y][bear.x] = TUNNEL;
					bear.y += dy;	//move the bear onto the detonator
					bear.x += dx;
					maze[bear.y][bear.x] = BEAR;
					bear.moved = true;
					moved++;
				}
				else
				{
					if (!bear.moved)
					{
						bear.y += dy;	//go in that Y direction
						bear.x += dx;	//go in that X direction
						moved++;
						bear.moved = true;
					}
					for (int b = 0; b < noOfBombs; b++)
					{
						if (!(bear.y == bombs[b].y && bear.x == bombs[b].x))
						{
							if (!bear.moved)
							{
								maze[bear.y][bear.x] = TUNNEL; bombs[b].visible = false;
							}
						}
					}
				}
				break;
			case PILL:
				if (!bear.moved){
					pill.item.visible = false;
					maze[bear.y][bear.x] = TUNNEL;
					bear.y += dy;	//move the bear onto the detonator
					bear.x += dx;
					maze[bear.y][bear.x] = BEAR;
					bear.moved = true;
					bear.invincible = true;
					pill.taken = true;
					moved++;
				}
				break;
			case EXIT:
				bear.y += dy;	//go in that Y direction
				bear.x += dx;	//go in that X direction
				bear.moved = true;
				moved++;
				if (bear.invincible)
				{
					pill.taken = false;
				}
				for (unsigned int c = 0; c < bears.size(); c++)
				{
					if (maze[bears[c].y][bears[c].x] == EXIT)
					{
						deleteIndex = c; // assign index of a bear to be deleted after remaing bears move
					}
				}
				break;
			case LOCK:
				if (!bear.moved)
				{
					if (lock.active)
					{
						bear.moved = true;
						moved++;
					}
					else
					{
						bear.y += dy;	//go in that Y direction
						bear.x += dx;	//go in that X direction
						bear.moved = true;
						moved++;
						maze[bear.y][bear.x] = BEAR;
					}
				}
				break;
			case KEY:
				if (!bear.moved)
				{
					bear.y += dy;	//go in that Y direction
					bear.x += dx;	//go in that X direction
					bear.moved = true;
					moved++;
					lock.active = false;
					maze[bear.y][bear.x] = BEAR;
					maze[lock.y][lock.x] = TUNNEL;
					lKey.visible = false;
					lock.visible = false;
				}
				break;
			}
		}
	}
	if (deleteIndex >= 0) //delete bear
	{
		bears.erase(bears.begin() + deleteIndex);
	}
	for (auto &bear : bears){ // reset moved variable for the next move
		bear.moved = false;
	}
	if (bears.empty())
	{
		gameEnd = true;
		showMessage(clBlack, clWhite, 40, 9, "LEVEL COMPLETE! Press any key.");
	}
	if (!player.cheated)
	{
		numberOfMoves++;
	}
	return gameEnd;
}

void removeBombs(vector<Item>& bombs)
{
	//The detonator was walked on
	const int noOfBombs = bombs.size();
	for (int b = 0; b < noOfBombs; b++)
	{
		bombs[b].visible = false; //Change the visibility of each one
	}
}
//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy)
{ //calculate direction indicated by key
	bool isArrowKey(const int k);
	assert(isArrowKey(key));
	switch (key)	//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		dx = -1;	//decrease the X coordinate
		dy = 0;
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		dx = +1;	//increase the X coordinate
		dy = 0;
		break;
	case UP: 		//when UP arrow pressed...
		dx = 0;
		dy = -1;	//decrease the Y coordinate
		break;
	case DOWN: 		//when DOWN arrow pressed...
		dx = 0;
		dy = +1;	//increase the Y coordinate
		break;
	}
}

int getKeyPress()
{ //get key or command (in uppercase) selected by user
	//KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = _getch();			//read in the selected arrow key or command letter
	while (keyPressed == 224) 		//ignore symbol following cursor key
		keyPressed = _getch();
	return toupper(keyPressed);		//return it in uppercase 
}

bool isArrowKey(const int key)
{	//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN);
}
bool isCheatKey(const char key)
{	//Check if the user has entered the cheat key (C)
	return (toupper(key) == CHEAT);
}
bool wantsToQuit(const int key)
{	//check if the user wants to quit (when key is 'Q' or 'q')
	return toupper(key) == QUIT;
}

//---------------------------------------------------------------------------
//----- File access
//---------------------------------------------------------------------------

Player loadPlayer(const string playerName)
{
	const string fileName = playerFileLocation + playerName + playerFileType;
	Player p;
	ifstream fin(fileName, ios::in); //Open the file
	if (fin.fail())	//Check if the open was successful.
	{
		p.name = playerName; //File must not exist, so default values are assigned.
		p.score = 500;
	}
	else 
	{
		//File open successfully, load the values from the file.
		fin >> p.name; fin.get();
		p.score = 500;
		fin.close();
	}
	return p;
}

void savePlayer(const Player& player)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	const string fileName = playerFileLocation + player.name + playerFileType;
	ofstream fout(fileName, ios::out);
	
	if (fout.fail())	//Check if the open was successful.
	{
		void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
		showMessage(clRed, clYellow, 0, 20, "Failed to save file: " + fileName);
	}
	else 
	{
		//Save the required data in the file.
		fout << player.name << "\n" << player.score;
		fout.close();
	}
}

vector<Player> loadHighscore()
{
	bool compareScore(const Player player1, const Player player2);
	const string fileName = playerFileLocation + "bestScores" + playerFileType;
	vector<Player> highscores;
	Player tempPlayer;
	ifstream fin(fileName, ios::in); //Open the file
	const int maxHighscores = 6;		//The maximum number of highscores that can be stored
	if (fin.fail())	//Check if the open was successful.
	{
		void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
		showMessage(clRed, clYellow, 0, 20, "Failed to open file: " + fileName);
	}
	else
	{
		//File open successfully, load the values from the file.
		for (int scores = 0; scores < maxHighscores; scores++)
		{
			if (fin)
			{
				fin >> tempPlayer.name;
				fin >> tempPlayer.score;
			}
			else
			{
				tempPlayer.name = "anonymous";
				tempPlayer.score = 500;
			}
			highscores.push_back(tempPlayer);
		}
		fin.close();
	}
	sort(highscores.begin(), highscores.end(), compareScore);
	return highscores;
}

void saveHighscores(vector<Player>& highscores)
{
	bool compareScore(const Player player1, const Player player2);
	const string fileName = playerFileLocation + "bestScores" + playerFileType;
	ofstream fout(fileName, ios::out);
	const int highscoresSize = highscores.size();
	int player = 0;

	if (fout.fail())	//Check if the open was successful.
	{
		void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
		showMessage(clRed, clYellow, 0, 20, "Failed to save file: " + fileName);
	}
	else
	{
		//Save the required data in the file.
		for (player; player < highscoresSize; player++)
		{
			fout << highscores.at(player).name << endl;
			fout << highscores.at(player).score << endl;
		}
		fout.close();
	}
	sort(highscores.begin(), highscores.end(), compareScore);
}


//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

string tostring(char x) {
    std::ostringstream os;
    os << x;
    return os.str();
}
void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message)
{
	Gotoxy(x, y);
	SelectBackColour(backColour);
	SelectTextColour(textColour);
	cout << message;
}
void paintGame(const char g[][SIZEX], string mess, int noOfBears, int noOfMoves, Player player, bool showRules, const vector<Bear>& bear)
{ //display game title, messages, maze, bear and other Bears on screen
	string tostring(char x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void paintGrid(const char g[][SIZEX], const vector<Bear>& bears);
	string calcTime();
	string makeLength(string s, int length);

	//display score
	showMessage(clYellow, clBlack, 0, 0, "THREE BEARS GAME");
	showMessage(clDarkGrey, clYellow, 39, 0, " CURRENT PLAYER: " + makeLength(player.name, 24));
	showMessage(clDarkGrey, clYellow, 39, 1, " PREVIOUS SCORE: " + makeLength(to_string(player.score), 24));
	showMessage(clDarkGrey, clYellow, 39, 2, " DATE AND TIME : " + calcTime());
	if (player.cheating)
	{
		showMessage(clBlack, clWhite, 39, 3, " CHEAT MODE ACTIVATED ");
	}
	else
	{
		showMessage(clBlack, clWhite, 39, 3, "                           ");
	}
	//Rescued
	string bearString = "";
	int bears(0);
	for (int i = noOfBears; i < 3; i++)
	{
		bearString += "@";
		bears++;
	}

	bearString = makeLength(bearString, 8);

	showMessage(clDarkGrey, clYellow, 0, 2, "RESCUED " + bearString);

	//Key drawn under the maze.
	showMessage(clDarkGrey, clWhite, 0, SIZEY + 5, makeLength("BEAR", 15));
	SelectTextColour(clGreen);
	cout << BEAR;
	showMessage(clDarkGrey, clWhite, 0, SIZEY + 6, makeLength("BOMB", 15));
	SelectTextColour(clRed);
	cout << BOMB;
	showMessage(clDarkGrey, clWhite, 0, SIZEY + 7, makeLength("DETONATOR", 15));
	SelectTextColour(clYellow);
	cout << DETONATOR;
	showMessage(clDarkGrey, clWhite, 0, SIZEY + 8, makeLength("LOCK", 15));
	cout << LOCK;
	showMessage(clDarkGrey, clWhite, 0, SIZEY + 9, makeLength("KEY", 15));
	cout << KEY;
	showMessage(clDarkGrey, clWhite, 0, SIZEY + 10, makeLength("POWER UP PILL", 15));
	SelectTextColour(clMagenta);
	cout << PILL;
	showMessage(clDarkGrey, clWhite, 0, SIZEY + 11, makeLength("EXIT", 15));
	SelectBackColour(clGrey);
	SelectTextColour(clBlack);
	cout << EXIT;

	//display menu options available
	showMessage(clBlack, clWhite, 40, 5, "NUMBER OF MOVES: " + to_string(noOfMoves));
	showMessage(clBlack, clWhite, 40, 6, "BEARS ESCAPED:   " + to_string(bears));

	if (showRules)
	{
	showMessage(clDarkGrey, clWhite, 40, 13, " GAME RULES:                        ");
	showMessage(clDarkGrey, clWhite, 40, 14, " Rescue all bears '@' through       ");
	showMessage(clDarkGrey, clWhite, 40, 15, " exit 'X' avoiding bombs 'O'        ");
	showMessage(clDarkGrey, clWhite, 40, 16, " To disable bombs use detonator 'T' ");
	showMessage(clDarkGrey, clWhite, 40, 17, " To open locks 'L' use key 'K'      ");
	showMessage(clDarkGrey, clWhite, 40, 18, "                                    ");
	showMessage(clDarkGrey, clWhite, 40, 19, " TO MOVE USE ARROW KEYS             ");
	showMessage(clDarkGrey, clWhite, 40, 20, " TO QUIT ENTER 'Q'                  ");
	showMessage(clDarkGrey, clWhite, 40, 21, " TO ACTIVATE CHEATMODE ENTER 'C'    ");
	   showMessage(clBlack, clWhite, 40, 23, " Game paused... press 'F' to resume ");
	}
	else
	{
		showMessage(clDarkGrey, clWhite, 40, 13, " Press F to display the rules.      ");
		showMessage(clBlack, clWhite, 40, 14, "                                    ");
		showMessage(clBlack, clWhite, 40, 15, "                                    ");
		showMessage(clBlack, clWhite, 40, 16, "                                    ");
		showMessage(clBlack, clWhite, 40, 17, "                                    ");
		showMessage(clBlack, clWhite, 40, 18, "                                    ");
		showMessage(clBlack, clWhite, 40, 19, "                                    ");
		showMessage(clBlack, clWhite, 40, 20, "                                    ");
		showMessage(clBlack, clWhite, 40, 21, "                                    ");
		showMessage(clBlack, clWhite, 40, 23, "                                    ");
	}

	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 8, mess);	//display current message
	
	// display grid contents
	paintGrid(g, bear);
}
string paintEntryScreen()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	int  getKeyPress();
	int x(22);
	const int y(10);
	const int size(20);
	char name[] = "____________________";
	string finalName = "";
	int index(0);
	int key;
	do
	{
		//check if the character is backspace
		showMessage(clDarkGrey, clYellow, 10, 5, "        THREE BEARS GAME        ");
		showMessage(clDarkGrey, clYellow, 10, 6, "      Bart, James and Liam      ");
		showMessage(clDarkGrey, clYellow, 10, 7, "  BSc Computer Science 2016-17  ");
		showMessage(clBlack, clWhite, 10, 10, "Enter name: ");
		showMessage(clBlack, clWhite, 22, 10, name);
		Gotoxy(x, y);
		key = getKeyPress();
		if (key != 13)
			name[index] = char(key);
		if (key != 8)
		{
			index++;
			x++;
		}
		else
		{
			name[index] = '_';
			if (x>22)
				x--;
			if (index > 0)
				index--;
		}
	} while (key != 13 && x <= size+22);
	//for 0 to index return new name
	for (int i = 0; i < index - 1; i++)
	{
		finalName = finalName + name[i];
	}
	Clrscr();
	return finalName;
}

int paintMainMenu(int& unlockedLevels)
{
	const int noOfMenuItems = 5;
	string MenuItems[noOfMenuItems] =
	{ "Level 1",			//To add new menu items:
	  "Level 2",
	  "Rules",				//   -Adjust the noOfMenuItems accordingly
	  "Highscores",		//   -Add it to this array
	  "Quit" };				//The loops below will handle the render and selection. Change the switch in the caller to change behaviour.
	int selection = 1;

	int getKeyPress();
	int key=72;
	
	do {
		showMessage(clDarkGrey, clYellow, 8, 1, "        THREE BEARS GAME        "); //Menu Header
		showMessage(clDarkGrey, clYellow, 8, 2, "            Main Menu           ");

		SelectBackColour(clBlack);
		SelectTextColour(clWhite);
		
		for (int menuItem = 0; menuItem < noOfMenuItems; menuItem++)
		{
			Gotoxy(8, menuItem + 4);	 //X and Y pos of the menu item, Y has an offset added to account for the header.
			cout << MenuItems[menuItem]; //Output each menu option
		}

		if (key == UP && selection > 1)
			selection--;
		else if (key == DOWN && selection < noOfMenuItems)
			selection++;
		else if (key == UNLOCK)
		{
			if (key == UNLOCK)
			{
				unlockedLevels = 100;
				showMessage(clBlack, clWhite, 8, 10, "All levels unlocked!");
			}
		}

		//Output selector
		Gotoxy(6, selection + 3); //Menu options start from line 4, take into account selection starts counting from 1, so we must add 3 to render on the correct line.
		cout << ">";
		Gotoxy(0, 0);

		key = getKeyPress();
		Clrscr();
	} while (key != 13);
	return selection;
}

void paintGrid(const char g[][SIZEX], const vector<Bear>& bears)
{ //display grid content on screen
	Gotoxy(0, 4);
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col)
		{
			switch (g[row][col])
			{
				case BOMB:
					SelectBackColour(clBlack);
					SelectTextColour(clRed);
					break;
				case DETONATOR:
					SelectBackColour(clBlack);
					SelectTextColour(clYellow);
					break;
				case BEAR:
					for (auto bear : bears)
					{
						if (bear.x == col && bear.y == row && bear.invincible)
						{
							SelectBackColour(clBlack);
							SelectTextColour(clMagenta);
							break;
						}
						else
						{
							SelectBackColour(clBlack);
							SelectTextColour(clGreen);
						}
					}
					break;
				case EXIT:
					SelectBackColour(clGrey);
					SelectTextColour(clBlack);
					break;
				case PILL:
					SelectBackColour(clBlack);
					SelectTextColour(clMagenta);
					break;
				default:
					SelectBackColour(clBlack);
					SelectTextColour(clWhite);
					break;
			}
			cout << g[row][col];	//output cell content
		}
		cout << endl;
	}
}

string calcTime()
{
	time_t now = time(0);
	char* dt = ctime(&now);
	return dt;
}

string makeLength(string s, int length)
{
	while (s.length() < length)
	{
		s.append(" ");
	}
	return s;
}

bool compareScore(const Player player1, const Player player2)
{
	return (player1.score < player2.score);
}

void endProgram()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	showMessage(clRed, clYellow, 4, 1, "");	//hold output screen until a keyboard key is hit
	system("pause");
}
