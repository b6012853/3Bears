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
//defining the command letters to move the bear on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game

const string playerFileLocation = "players\\";
const string playerFileType = ".txt";

struct Item {
	int x, y;
	char symbol;
	bool visible;
};

struct Bomb {
	Item item;
	bool active;
};

struct Bear {
	int x, y;
	char symbol;
	bool visible;
	bool moved;
};

struct Player{
	string name;
	int score;
	bool cheated;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Bear>& bear, vector<Bomb>& bombs);
	void paintGame(const char g[][SIZEX], string mess, int noOfBears, int noOfMoves, Player player);
	bool wantsToQuit(const int key);
	bool isArrowKey(const int k);
	int  getKeyPress();
	bool updateGameData(const char g[][SIZEX], vector<Bear>& bear, vector<Bomb>& bombs, const int key, string& mess, int& numberOfMoves);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const vector<Bear> bear, const vector<Bomb> bombs);
	void endProgram();
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	string paintEntryScreen();
	Player player;
	Player loadPlayer(const string playerName);
	void savePlayer(const Player& player);

	//local variable declarations 
	char grid[SIZEY][SIZEX];	//grid for display
	char maze[SIZEY][SIZEX];	//structure of the maze
	string message("LET'S START...");	//current message to player
	vector<Bear> bears;
	bears.push_back(Bear());
	bears.push_back(Bear());
	bears.push_back(Bear());

	vector<Bomb> bombs;
	for (int i = 0; i < 6; i++)
	{
		bombs.push_back(Bomb()); //Item 0 detenator, 1+ are bombs
	}

	bool forceQuit = false;
	int noOfMoves(0);

	//Entry screen
	player = loadPlayer(paintEntryScreen());
	//action...
	initialiseGame(grid, maze, bears, bombs);	//initialise grid (incl. walls & bear)
	paintGame(grid, message, bears.size(), noOfMoves, player);			//display game info, modified grid & messages
	int key(getKeyPress()); 			//read in  selected key: arrow or letter command
	while (!wantsToQuit(key) && !forceQuit)			//while user does not want to quit
	{
		if (isArrowKey(key))
		{
			forceQuit = updateGameData(grid, bears, bombs, key, message, noOfMoves);		//move bear in that direction
			if (bears.empty())
			{
				savePlayer(player);
			}
			updateGrid(grid, maze, bears, bombs);			//update grid information
		}
		else
			message = "INVALID KEY!";	//set 'Invalid key' message
		paintGame(grid, message, bears.size(), noOfMoves, player);		//display game info, modified grid & messages
		if( !forceQuit ) key = getKeyPress(); 			//display menu & read in next option
	}
	endProgram();						//display final message
	return 0;
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], vector<Bear>& bears, vector<Bomb>& bombs)
{ //initialise grid & place bear in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setInitialDataFromMaze(char maze[][SIZEX], vector<Bear>& bears, vector<Bomb>& bombs);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], vector<Bear> bears, vector<Bomb> bombs);

	setInitialMazeStructure(maze);		//initialise maze
	setInitialDataFromMaze(maze, bears, bombs);	//initialise bear's position
	updateGrid(grid, maze, bears, bombs);		//prepare grid
}

void setInitialMazeStructure(char maze[][SIZEX])
{ //set the position of the walls in the maze
	//initialise maze configuration
	int initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
		= { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		    { 1, 2, 3, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 1},
			{ 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1},
			{ 1, 2, 1, 0, 1, 5, 0, 0, 0, 3, 0, 1, 0, 1, 0, 1},
			{ 1, 0, 1, 0, 1, 3, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
			{ 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
			{ 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
			{ 1, 0, 1, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
			{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
	// with 1 for wall, 0 for tunnel, etc. 
	//copy into maze structure
	for (int row(0); row < SIZEY; ++row)	
		for (int col(0); col < SIZEX; ++col)
			switch (initialMaze[row][col])
			{
				case 0: maze[row][col] = TUNNEL; break;
				case 1: maze[row][col] = WALL; break;
				case 2: maze[row][col] = BEAR; break;
				case 3: maze[row][col] = BOMB; break;
				case 4: maze[row][col] = DETONATOR; break;
				case 5: maze[row][col] = EXIT; break;
			}
}
void setInitialDataFromMaze(char maze[][SIZEX], vector<Bear>& bears, vector<Bomb>& bombs)
{ //extract bear's coordinates from initial maze info
	int noOfBears = 0;
	int noOfBombs = 1; //Start from 1 as 0 is the detonator.
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			switch (maze[row][col])
			{
				case BEAR:
				{
					bears[noOfBears].x = col;
					bears[noOfBears].y = row;
					bears[noOfBears].symbol = BEAR;
					bears[noOfBears].visible = true;
					noOfBears++;
					maze[row][col] = TUNNEL;
					break;
				}
				case DETONATOR:
				{		
					bombs[0].item.x = col;
					bombs[0].item.y = row;
					bombs[0].item.symbol = DETONATOR;
					bombs[0].item.visible = true;
					bombs[0].active = false;
					maze[row][col] = TUNNEL;
					break;
				}
				case BOMB:
				{
					bombs[noOfBombs].item.x = col;
					bombs[noOfBombs].item.y = row;
					bombs[noOfBombs].item.symbol = BOMB;
					bombs[noOfBombs].item.visible = true;
					bombs[noOfBombs].active = true;
					noOfBombs++;
					maze[row][col] = TUNNEL;
					break;
				}
			}
}

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], const vector<Bear> bears, const vector<Bomb> bombs)
{ //update grid configuration after each move
	void setMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeBear(char g[][SIZEX], const Bear bear);
	void placeBomb(char g[][SIZEX], const Bomb bomb);

	setMaze(grid, maze);	//reset the empty maze configuration into grid
	for (auto bear : bears)
	{
		placeBear(grid, bear);	//set bear in grid
	}

	for (auto bomb : bombs)
	{
		placeBomb(grid, bomb);
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

void placeBomb(char g[][SIZEX], const Bomb bomb)
{
	if (bomb.item.visible)
		g[bomb.item.y][bomb.item.x] = bomb.item.symbol;
}

//---------------------------------------------------------------------------
//----- move the bear
//---------------------------------------------------------------------------
bool updateGameData(const char g[][SIZEX], vector<Bear>& bears, vector<Bomb>& bombs, const int key, string& mess, int& numberOfMoves)
{ //move bear in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	void setMaze(char grid[][SIZEX], const char maze[][SIZEX]);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void endProgram();
	bool forceQuit = false;
	assert(isArrowKey(key));
	
	char maze[SIZEY][SIZEX];
	setMaze(maze, g);

	void removeBombs(vector<Bomb>& bombs);
	mess = "                                         ";		//reset message to blank

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
				if (bear.y == bombs[0].item.y && bear.x == bombs[0].item.x) //Reset so the detonator is visible when the bear moves off it.
				{
					bombs[0].active = false;	//Deactivate the detonator.
					bombs[0].item.visible = true;
				}
				else
					if (!bear.moved)
						maze[bear.y][bear.x] = TUNNEL;
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
				}
				break;
			case DETONATOR:
				bombs[0].active = true;	//Activate the detonator
				bombs[0].item.visible = false;
				bear.y += dy;	//move the bear onto the detonator
				bear.x += dx;
				bear.moved = true;
				moved++;
				removeBombs(bombs);
				break;
			case BOMB:
				forceQuit = true;
				mess = "You just killed a bear, you sad person!  ";
				maze[bear.y][bear.x] = TUNNEL;
				bear.y += dy;	//move the bear onto the detonator
				bear.x += dx;
				maze[bear.y][bear.x] = BEAR;
				bear.moved = true;
				moved++;
				break;
			case EXIT:
				bear.y += dy;	//go in that Y direction
				bear.x += dx;	//go in that X direction
				bear.moved = true;
				moved++;
				for (unsigned int i = 0; i < bears.size(); i++)
				{
					if (maze[bears[i].y][bears[i].x] == EXIT)
					{
						deleteIndex = i; // assign index of a bear to be deleted after remaing bears move
					}
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
		forceQuit = true;
		showMessage(clBlack, clWhite, 40, 9, "FREEDOM!");
	}
	numberOfMoves++;
	return forceQuit;
}

void removeBombs(vector<Bomb>& bombs)
{
	//The detonator was walked on
	const int noOfBombs = bombs.size();
	for (int b = 0; b < noOfBombs; b++)
	{
		bombs[b].active = false;
		bombs[b].item.visible = false;
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
		p.name = playerName;
		p.score = 500;
		p.cheated = false;
	}
	else 
	{
		//  file open successfully: process the file
		fin >> p.name; fin.get();
		fin >> p.score; fin.get();
		fin >> p.cheated;
		fin.close();
	}
	return p;
}

void savePlayer(const Player& player)
{
	const string fileName = playerFileLocation + player.name + playerFileType;
	ofstream fout(fileName, ios::out);
	if (fout.fail())	//Check if the open was successful.
	{
		showMessage(clRed, clYellow, 0, 20, "Failed to save file: " + fileName);
	}
	else 
	{
		//  file open successfully: process the file
		fout << player.name << "\n" << player.score << "\n" << player.cheated;
		fout.close();
	}
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
void paintGame(const char g[][SIZEX], string mess, int noOfBears, int noOfMoves, Player player)
{ //display game title, messages, maze, bear and other Bears on screen
	string tostring(char x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void paintGrid(const char g[][SIZEX]);

	//display game title
	showMessage(clBlack, clYellow, 0, 0, "___GAME___");
	showMessage(clWhite, clRed, 40, 1, "FoP Task 1c: February 2017");

	//display score
	showMessage(clYellow, clBlack, 0, 0, "THREE BEARS GAME");
	showMessage(clDarkGrey, clYellow, 40, 0, " CURRENT PLAYER:");
	showMessage(clDarkGrey, clYellow, 40, 1, " PREVIOUS SCORE:");
	showMessage(clDarkGrey, clYellow, 40, 2, " DATE AND TIME :");
	//Rescued
	string bearString = "";
	int bears(0);
	for (int i = noOfBears; i < 3; i++)
	{
		bearString += "@";
		bears++;
	}

	while (bearString.length() < 8)
		bearString.append(" ");

	showMessage(clGrey, clYellow, 0, 2, "RESCUED " + bearString);

	//display menu options available
	showMessage(clBlack, clWhite, 40, 5, "NUMBER OF MOVES: " + to_string(noOfMoves));
	showMessage(clBlack, clWhite, 40, 6, "BEARS ESCAPED:   " + to_string(bears));
	showMessage(clDarkGrey, clWhite, 40, 13, " GAME LEVEL 1 RULES:                ");
	showMessage(clDarkGrey, clWhite, 40, 14, " Rescue all bears '@' through       ");
	showMessage(clDarkGrey, clWhite, 40, 15, " exit 'X' avoiding bombs 'O'        ");
	showMessage(clDarkGrey, clWhite, 40, 16, " To disable bombs use detonator 'T' ");
	showMessage(clDarkGrey, clWhite, 40, 17, "                                    ");
	showMessage(clDarkGrey, clWhite, 40, 18, " TO MOVE USE ARROW KEYS             ");
	showMessage(clDarkGrey, clWhite, 40, 19, " TO QUIT ENTER 'Q'                  ");
	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 8, mess);	//display current message
	
	// display grid contents
	paintGrid(g);
}
string paintEntryScreen()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	int  getKeyPress();
	int x(10);
	const int y(5);
	const int size(20);
	char name[] = "____________________";
	string finalName = "";
	int index(0);
	int key;
	do
	{
		//check if the character is backspace
		showMessage(clBlack, clWhite, 10, 1, "  THREE BEARS GAME");
		showMessage(clBlack, clWhite, 10, 2, "BART, JAMES AND LIAM");
		showMessage(clBlack, clWhite, 10, 3, "FoP Module - 2016-17");
		showMessage(clBlack, clWhite, 10, 5, name);
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
			if (x>10)
				x--;
			if (index > 0)
				index--;
		}
	} while (key != 13 && x <= size+10);
	//for 0 to index return new name
	for (int i = 0; i < index - 1; i++)
	{
		finalName = finalName + name[i];
	}
	Clrscr();
	return finalName;
}
void paintGrid(const char g[][SIZEX])
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
				case BEAR:
					SelectBackColour(clBlack);
					SelectTextColour(clGreen);
					break;
				case DETONATOR:
					SelectBackColour(clBlack);
					SelectTextColour(clYellow);
					break;
				case EXIT:
					SelectBackColour(clGrey);
					SelectTextColour(clBlack);
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

void endProgram()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	showMessage(clRed, clYellow, 40, 10, "");	//hold output screen until a keyboard key is hit
	system("pause");
}
