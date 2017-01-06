#include <iostream>
#include <string>
#include <fstream>
#include <limits>
using namespace std;

/* 
The structure is used to hold both a letter and a flag 
that indicates if the letter has already been found 
*/
struct Letter
{
	char letter;
	bool isFlagged;

	Letter()
	{
		letter = NULL;
		isFlagged = false;
	}
};
/****************************************************************************************************************************/

Letter** createArray(int& arrayDimension);
void fillArray(Letter** grid2D, int arrayDimension);
void getWordLength(int& desiredWordLength);
void solvePuzzle(Letter** grid2D, int arrayDimension, int desiredWordLength, bool changeDictionary);
bool checkPosition(int row, int column, int newRow, int newColumn);
bool checkFirstLetter(Letter** grid2D, int arrayDimension,string word, int wordLength);
bool checkEntireWord(Letter** grid2D, int arrayDimension,string word, int wordLength, int letter, int oldRow, int oldColumn);
void reset(Letter** grid2D, int arrayDimension);

/****************************************************************************************************************************/

int main()
{
	int arrayDimension = 0,
		desiredWordLength,
		userChoice;
	bool changeDictionary,
		isValid;

	Letter** grid2D = createArray(arrayDimension);

	fillArray(grid2D, arrayDimension);
	getWordLength(desiredWordLength);
	solvePuzzle(grid2D, arrayDimension, desiredWordLength, false);

	do
	{
		changeDictionary = false;
		isValid = true;

		cout << endl
			<< "1. Reset game."		<< endl
			<< "2. Change letters."			<< endl
			<< "3. Change word length."		<< endl
			<< "4. Change dictionary."		<< endl
			<< "5. Quit"					<< endl
			<< "Enter your choice: ";	
		cin >> userChoice;

		if(cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << endl << "Invalid input, try again." << endl;
			isValid = false;
		}

		if(isValid && (userChoice >= 1 && userChoice <= 5))
		{
			switch (userChoice)
			{
			case 1:
				grid2D = createArray(arrayDimension);
			case 2:
				fillArray(grid2D, arrayDimension);
			case 3:
				getWordLength(desiredWordLength);
				break;
			case 4:
				changeDictionary = true;
				break;
			case 5:
				cout << "Goodbye!" << endl << endl;
				return 0;
			default:
				break;
			}

			if(userChoice >= 1 && userChoice <= 4)
			{
				solvePuzzle(grid2D, arrayDimension, desiredWordLength, changeDictionary);
			}
		}
		else
		{
			cout << "Choose a number between 1-5" << endl << endl;
		}
	}
	while(userChoice != 5);
}

/****************************************************************************************************************************/

/*
This function asks the user for dimentsion input and creates
a dynamic 2D array.
*/
Letter** createArray(int& arrayDimension)
{
	bool isValid;

	do
	{
		isValid = true;

		cout << "Choose grid dimensions" << endl
			<< "1. Quit" << endl
			<< "2. 2X2"  << endl
			<< "3. 3X3"  << endl
			<< "4. 4X4"  << endl
			<< "5. 5X5"  << endl
			<< "6. 6X6"  << endl
			<< "7. 7X7"  << endl
			<< "8. 8X8"  << endl
			<< endl
			<< "Dimension: ";
		cin >> arrayDimension;

		// validation check - Making sure we are getting an int //
		if(cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input, try again." << endl << endl;
			isValid = false;
		}

		// validation check - Making sure the int is in range //
		if(isValid  && (arrayDimension > 8 || arrayDimension < 1) )
		{
			cout << "Please enter a number between 1-8" << endl << endl;
			isValid = false;
		}

		// If user wants to quit //
		if(arrayDimension == 1)
		{
			exit(1);
		}
	}
	while(!isValid);

	//create dynamic array of the struct//
	Letter** grid2D = new Letter*[arrayDimension]; 

	for (int i = 0; i < arrayDimension; i++)
	{
		grid2D[i] = new Letter[arrayDimension];
	}

	return grid2D;
}

/*
This function asks the user to fill in the array
from top left to bottom right
*/
void fillArray(Letter** grid2D, int arrayDimension)
{
	char s[100];
	int length;
	string w;

	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cout << "Enter rows from left to right top to bottom:" << endl;

	for (int row = 0; row < arrayDimension; row++)
	{
		cout << "Row number " << row + 1 << ":" << endl;

		for (int column = 0; column < arrayDimension; column++)
		{
			bool isValid;

			do 
			{
				isValid = true;

				cout << "Enter letter: ";
				cin.getline(s, 100);

				w = s;
				length = w.length();

				if(length > 1)
				{
					cout << "input must be a single letter" << endl;
					isValid = false;
				}
				else if(!isalpha(s[0]) && s[0] != '0')
				{
					cout << "input must be a letter" << endl;
					isValid = false;
				}
				else
				{
					grid2D[row][column].letter = s[0];
				}
			}
			while(!isValid);
		}
	}
}

/*
Pretty straight forward.
I seperated this function so that I can call it seperately upon user request 
to change the word length. It will come later.
*/
void getWordLength(int& desiredWordLength)
{
	bool isValid;

	do
	{
		isValid = true;

		cout << "What is the word length: ";
		cin >> desiredWordLength;

		if(cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input, try again." << endl << endl;
			isValid = false;
		}
		else if(desiredWordLength < 2 || desiredWordLength > 10)
		{
			cout << "Please enter a number between 2-10" << endl << endl;
			isValid = false;
		}
	}
	while(!isValid);
}


/*
This function opens the dictionary file,
choosing relevant words from it to try and build from the 2D array.
If we managed to build the word, we print it,
else, we choose the next suitable one. There is an option to switch dictionaries.
There are two dictionaries: The first is very small and will work most of the time,
but doesn't cover 100% of the words. 
The second dictionary )(words2.txt), is massive and is overkill, but is useful to get words 
that the first dictionary doesn't contain.
*/
void solvePuzzle(Letter** grid2D, int arrayDimension, int desiredWordLength, bool changeDictionary)
{
	string word;
	bool isValidWord;
	int wordLength;
	ifstream in;

	//First time the program runs, changeDictionary = false by default//
	if(!changeDictionary)
	{
		in.open("words.txt", ios::beg);
	}
	else
	{
		in.open("words2.txt", ios::beg);
	}

	if(!in.is_open())
	{
		cout << "Error opening file!" << endl;
		exit(99);
	}

	//Getting info from file - there are three strange chars ("ï»¿") at the begining of the file, so I'm ignoring them//
	in.ignore();
	in.ignore();
	in.ignore();

	in >> word;

	while(!in.eof())
	{
		isValidWord = false; // Assuming all words are bad until proven otherwise//
		wordLength = word.length();

		//making sure the word in the txt file meets demends//
		if(wordLength == desiredWordLength)
		{
			isValidWord =true; // We found a word, let's assume it's good for now //

			for(int i = 0; i < wordLength; i++) // checking all the letters are A-Z or a-z not simbols or digits. Game rules //
			{
				if(!isalpha(word[i]))
				{
					isValidWord = false; // If we get a "bad" char we want to move to next word, not waste time //
					break;
				}
			}
		}

		if(isValidWord && checkFirstLetter(grid2D, arrayDimension, word, wordLength)) // If word is not bad, we continue to check if it can be created using 2D array //
		{
			cout << word << endl; // If all is good we display the word //
		}

		reset(grid2D, arrayDimension); // If not, we reset the flags on the board // 
		in >> word;
	}

	in.close();
}

/*
This function get two sets of coordinates: for the letter we are checking,
and for the letter we are checking against. If they are "touching" each other

*/
bool checkPosition(int baseRow, int baseColumn, int newRow, int newColumn)
{
	if(
		(baseRow - 1 == newRow && baseColumn == newColumn)		|| // Check up //
		(baseRow - 1 == newRow && baseColumn + 1 == newColumn)	|| // Check up right //
		(baseRow - 1 == newRow && baseColumn - 1 == newColumn)	|| // Check up left //
		(baseRow == newRow && baseColumn + 1 == newColumn)		|| // Check right //
		(baseRow == newRow && baseColumn - 1 == newColumn)		|| // Check left //
		(baseRow + 1 == newRow && baseColumn == newColumn)		|| // Check bottom //
		(baseRow + 1 == newRow && baseColumn + 1 == newColumn)	|| // Check bottom right //
		(baseRow + 1 == newRow && baseColumn - 1 == newColumn)	   // Check bottom left //
		)
	{
		return true;
	}

	return false;
}

/*
This function checks the first letter. If the letter is not in the 2D array we skip it, no point in continuing.
If the first letter is found, we initiate the iterative function 'checkEntireWord' to try and build the word.
*/
bool checkFirstLetter(Letter** grid2D, int arrayDimension,string word, int wordLength)
{
	bool isValid = false;
	int oldRow,
		oldColumn,
		letter = 0; //'letter' is the counter//

	for (int row = 0; row < arrayDimension; row++)
	{
		for (int column = 0; column < arrayDimension; column++)
		{
			if(grid2D[row][column].letter == word[0])
			{
				//mark the letter as used//
				grid2D[row][column].isFlagged = true;

				//save the coordinates of the letter//
				oldRow = row;
				oldColumn = column;

				//call to iterative function//
				isValid = checkEntireWord(grid2D, arrayDimension, word, wordLength, letter + 1, oldRow, oldColumn);
				grid2D[row][column].isFlagged = false;

				if(isValid)
				{
					return true;
				}
			}
		}
	}

	return isValid;
}

/*
This function gets the coordinates of the letter that called it so it can call the checkPosition function. 
If the next letter exists, not flagged as used AND position right, the function will call itself again.
If the position is not valid or the next letter is not valid we reset the flag on the letter.
*/
bool checkEntireWord(Letter** grid2D, int arrayDimension,string word, int wordLength, int letter, int oldRow, int oldColumn)
{
	//This is one of the conditions that break the iteration//
	if (letter == wordLength)
	{
		return true;
	}

	bool isValid = false;

	for (int row = 0; row < arrayDimension; row++)
	{
		for (int column = 0; column < arrayDimension; column++)
		{
			if(grid2D[row][column].letter == word[letter] && !grid2D[row][column].isFlagged)
			{
				grid2D[row][column].isFlagged = true;

				if (checkPosition(oldRow, oldColumn, row, column))
				{
					//there's no reason to continue the iteraion if the position is invalid//
					isValid = checkEntireWord(grid2D, arrayDimension, word, wordLength, letter + 1, row, column);

					if(isValid)
					{
						//the next letter works fine - break the iteration//
						return true;
					}
					else
					{
						//the next letter is a dud - reset the flag//
						grid2D[row][column].isFlagged = false;
					}
				}
				else
				{
					//the position is invalid - reset the flag//
					grid2D[row][column].isFlagged = false;
				}
			}
		}
	}

	return isValid;
}

/*
This function resets the flags of all the letters in the grid//
*/
void reset(Letter** grid2D, int arrayDimension)
{
	for(int row = 0; row < arrayDimension; row++)
	{
		for(int column = 0; column< arrayDimension; column++)
		{
			grid2D[row][column].isFlagged = false;
		}
	}
}