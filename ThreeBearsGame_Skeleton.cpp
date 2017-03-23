//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c – group assignment
//Author: Pascale Vacher
//Last updated: 24 February 2017
//---------------------------------------------------------------------------

/* TODO
	Let Bears go on bombs,detonator and exit
	Add colours
*/

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

struct Item {
	int x, y;
	char symbol;
	bool visible;
};

struct Bomb {
	Item item;
	int colour;
	bool active;
};

struct Bear {
	int x, y;
	char symbol;
	bool visible;
	bool moved;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Bear>& bear, vector<Bomb>& bombs);
	void paintGame(const char g[][SIZEX], string mess, int noOfBears);
	bool wantsToQuit(const int key);
	bool isArrowKey(const int k);
	int  getKeyPress();
	bool updateGameData(const char g[][SIZEX], vector<Bear>& bear, vector<Bomb>& bombs, const int key, string& mess);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const vector<Bear> bear, const vector<Bomb> bombs);
	void endProgram();

	//local variable declarations 
	char grid[SIZEY][SIZEX];	//grid for display
	char maze[SIZEY][SIZEX];	//structure of the maze
	//Item bear = {0, 0, BEAR, true}; 			//bear's position and symbol
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
	//action...
	initialiseGame(grid, maze, bears, bombs);	//initialise grid (incl. walls & bear)
	paintGame(grid, message, bears.size());			//display game info, modified grid & messages
	int key(getKeyPress()); 			//read in  selected key: arrow or letter command
	while (!wantsToQuit(key) && !forceQuit)			//while user does not want to quit
	{
		if (isArrowKey(key))
		{
			forceQuit = updateGameData(grid, bears, bombs, key, message);		//move bear in that direction
			updateGrid(grid, maze, bears, bombs);			//update grid information
		}
		else
			message = "INVALID KEY!";	//set 'Invalid key' message
		paintGame(grid, message, bears.size());		//display game info, modified grid & messages
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
					bombs[0].colour = clYellow;
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
					bombs[noOfBombs].colour = clRed;
					bombs[noOfBombs].active = true;
					noOfBombs++;
					maze[row][col] = TUNNEL;
					break;
				}
				//will work for other items too
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
bool updateGameData(const char g[][SIZEX], vector<Bear>& bears, vector<Bomb>& bombs, const int key, string& mess)
{ //move bear in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	void setMaze(char grid[][SIZEX], const char maze[][SIZEX]);
	bool forceQuit = false;
	assert(isArrowKey(key));
	
	char maze[SIZEY][SIZEX];
	setMaze(maze, g);

	void removeBombs(vector<Bomb>& bombs);

	//reset message to blank
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
					else if (bear2.x == bear.x + (dx * 2) && bear2.y == bear.y + (dy * 2))
					{
						bear.y += dy;	//go in that Y direction
						bear.x += dx;	//go in that X direction
						moved++;
						bear.moved = true;
					}
				}
				break;
			case WALL:  		//hit a wall and stay there
				//cout << '\a';	//beep the alarm
				//mess = "CANNOT GO THERE!";
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
				bear.y += dy;	//move the bear onto the detonator
				bear.x += dx;
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
					if (maze[bear.y][bear.x] == EXIT)
					{
						deleteIndex = i; // assign index of a bear to be deleted after remaing bears move
					}
				}
				//Add Rescued bar and increment rescued bears
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
	return forceQuit;
}

void removeBombs(vector<Bomb>& bombs)
{
	//The detonator was walked on 
	for (int b=0; b < bombs.size(); b++)
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
void paintGame(const char g[][SIZEX], string mess, int noOfBears)
{ //display game title, messages, maze, bear and other Bears on screen
	string tostring(char x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void paintGrid(const char g[][SIZEX]);

	//display game title
	showMessage(clBlack, clYellow, 0, 0, "___GAME___");
	showMessage(clWhite, clRed, 40, 0, "FoP Task 1c: February 2017");
	//Rescued
	string bearString = "";
	for (int i = noOfBears; i < 3; i++)
	{
		bearString += "@";
	}
	showMessage(clGrey, clYellow, 0, 1, "RESCUED " + bearString);

	//display menu options available
	showMessage(clRed, clYellow, 40, 3, "TO MOVE USE KEYBOARD ARROWS ");
	showMessage(clRed, clYellow, 40, 4, "TO QUIT ENTER 'Q'           ");

	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 8, mess);	//display current message
	
	// display grid contents
	paintGrid(g);
}

void paintGrid(const char g[][SIZEX])
{ //display grid content on screen
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col)
			cout << g[row][col];	//output cell content
		cout << endl;
	}
}

void endProgram()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	showMessage(clRed, clYellow, 40, 9, "");	//hold output screen until a keyboard key is hit
	system("pause");
}
