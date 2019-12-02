#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct TweeterTweets {
	/* This struct is the end struct of all unique tweeters and 
	their total amount of tweets */
    char* tweeter;
    int tweets;
} Tweeter_Tweets;

void CSVParseLines(FILE* file, char** lines_array);
void CSVParseName(char** lines_array, char** names_array, int lineCount);

bool HeaderErrorChecking(char* line);
bool CheckOneInstanceName(char* line);
int getNumColumns(char* line);

bool LineErrorChecking(char* line);
bool CheckMatchesWithHeaderQuotes(char* line);

void AddName(char* line, char* name, int nameIndex);
void CountTweeters(char** names_array, Tweeter_Tweets* tweeter_tweets_array, int lineCount);
void PrintTweeters(Tweeter_Tweets* tweeter_tweets_array, int tweetArrSize);
int getFileLineCount(FILE* file);

// GLOBAL
bool isQuotes = false;
int total_fields = 0;
int name_index = 0;

int main(int argc, char** argv) {

    //check if argc = 2, arg = file path
    if ((argc > 2) || (argc == 1)) {
        printf("Invalid Input Format\n");
        exit(0);
    }
    int filename_size = strlen(argv[1]);
    char* filename = (char*)malloc(filename_size * sizeof(char));
    strcpy(filename, argv[1]);

    FILE* file;
    file = fopen(filename, "r");

    int lineCount = getFileLineCount(file);
    rewind(file);

    if ((lineCount == 0) || (lineCount > 20000)) {
        printf("Invalid Input Format\n");
        exit(0);
    }

    //create an array of lines from the csv
    char** lines_array = (char**)malloc(lineCount * sizeof(char*));
    CSVParseLines(file, lines_array);
    
	//create an array of names from the lines_array
    char** names_array = (char**)malloc(lineCount * sizeof(char*));
    CSVParseName(lines_array, names_array, lineCount);

	//create an array of tweeters-tweet - allocated one for now.
    Tweeter_Tweets* tweeter_tweets_array = (Tweeter_Tweets*)malloc(1 * sizeof(Tweeter_Tweets));

	//count and Print the final result
    CountTweeters(names_array, tweeter_tweets_array, lineCount);

    free(filename);
    free(lines_array);
    free(names_array);
    free(tweeter_tweets_array);
    return 0;
}

int getFileLineCount(FILE* file) {
	/* this function gets the total number of 
	lines in a file and returns it */

	//counts the lines in the file
    int numLines = 0;
    char line[1025];
    while(!feof(file)) {
        if (!(fgets(line, 1025, file) == NULL)) {
            numLines++;
        }
    }
    return numLines;
}

void RemoveNewLines(char* line) {
	/* this function removes all newline delimters from every line*/

    int i = 0;
    char curChar = line[0];
	//loop through and change \n and \r to \0.
    while(curChar != '\0') {
        if((curChar == '\n') || (curChar == '\r')) {
            line[i] = '\0';
            line[i+1] = ' ';
        }
        i++;
        curChar = line[i];
    }
}

void CSVParseLines(FILE* file, char** lines_array) {
	/* This function parses each line from the file and stores it
	in a line array. This is a 2D array of strings. Each element of the
	array is a string, which is an entire line from the file */

    //check if file opened.
    if (file == NULL) {
        printf("Invalid Input Format\n");
        exit(0);
    }

    int lineCount = 0;
    char line[1025];
    while(!feof(file)) {  // while we haven't reached the end of the file
        //get a line
        if (!(fgets(line, 1025, file) == NULL)) {
            if ((strlen(line) == 1024) && (line[1023] != '\n')) {
                printf("Invalid Input Format\n");
                exit(0);
            }
			//remove the newline
            RemoveNewLines(line);

            //allocate space for the line string and fill it. 
            lines_array[lineCount] = (char*)malloc(1025 * sizeof(char));
            strcpy(lines_array[lineCount], line);

            lineCount++;
        }
    }
}

void CSVParseName(char** lines_array, char** names_array, int lineCount) {
	/* This line parses each individual line from the lines array, 
	and checks for any errors that would make the CSV invalid.
	It also looks for the name field and makes sure it is valid,
	and stores it in a 2D names array where each index is a name string */

    //header error checking
    if (!HeaderErrorChecking(lines_array[0])) {
        printf("Invalid Input Format\n");
        exit(0);
    }

    //line error checking
    for (int i = 1; i < lineCount; ++i)  {
        if(!LineErrorChecking(lines_array[i])) {
            printf("Invalid Input Format\n");
            exit(0);
        }
		//if no errors, we allocate memory for one of the spots in the names_array
        names_array[i] = (char*)malloc(1025 * sizeof(char));
        char name[1025];
		//call AddName to retrieve the value of name from the line
        AddName(lines_array[i], name, name_index);
		//copy the name into the names_array
        strncpy(names_array[i], name, 1025);
    }
}

bool HeaderErrorChecking(char* line) {
	/* Error checking for the header line */

    if(!CheckOneInstanceName(line)) {
        printf("Invalid Input Format\n");
        exit(0);
    } else {
		return true;
	}
}

bool CheckOneInstanceName(char* line) {
	/* This function makes sure there is only one instance 
	of a name field inside the header line of the file */
    
    int i = 0;
    int name_header = 0;
    int num_commas = 0;
    char currChar = line[i];

	// while we haven't reached the end of the line
    while(currChar != '\0') {
        char field[1025];
        if (currChar == ',' || i == 0) {
            int j;
            if(i == 0 && (currChar != ',')) { // conditions for the loop
                j = 0;
            } else { // if there is a comma, increment comma counter
                j = i + 1;
                ++num_commas;
            }
            char nameChar = line[j]; // the character after the comma
            int k = 0;
			// second while loop, when we want to actually read in the field
            while(nameChar != '\0' && nameChar != ',') {
                field[k] = nameChar;
                j++;
                nameChar = line[j];
                k++;
            }
            field[k] = '\0';
            currChar = nameChar;
			// if the name is valid, then assign the name_index
            if ((strcmp(field, "name") == 0) || (strcmp(field, "name\n") == 0) || (strcmp(field, "name\r") == 0)) {
                ++name_header;
                name_index = num_commas;
            } // otherwise it is the quotes version
            else if ((strcmp(field, "\"name\"") == 0) || (strcmp(field, "\"name\"\n") == 0) 
					|| (strcmp(field, "\"name\"\r") == 0)) {
                name_header++;
                isQuotes = true;
                name_index = num_commas;
            }
        }
        ++i;
        currChar = line[i];
    }
    total_fields = num_commas + 1;

    if (name_header == 1) {
        return true;
    } else {
        return false;
    }
}

int getNumColumns(char* line) {
	/* This function gets the total number of columns(fields) 
	for each linein the file */

    int num_commas = 0;
    for (int i = 0; i < strlen(line); ++i) {
        if (line[i] == ',') {
            ++num_commas;
        }
    }
    return num_commas + 1;
}

bool LineErrorChecking(char* line) {
	/* This function is for error checking for every line aside from
	the header. It involves checking the amount of columns and quotation placement */

    if (!(getNumColumns(line) == total_fields)) {
        printf("Invalid Input Format\n");
        exit(0);
    }

    if (!(CheckMatchesWithHeaderQuotes(line))) {
        printf("Invalid Input Format\n");
        exit(0);
    }
 
    return true;
}

bool CheckMatchesWithHeaderQuotes(char* line) {
	/* This function makes sure the name field has quotations
	in the correct placement compared to the header. Part of error checking
	for invalid CSVs */

    int i = 0;
    int num_commas = 0;
    char currChar = line[i];

	// while we haven't reached the end of the line
    while(currChar != '\0') {
        char field[1025];
        if (currChar == ',' || i == 0) { // conditions for the loop
            int j;
            if(i == 0 && (currChar != ',')) { // if there is a comma, increment comma counter
                j = 0;
            } else {
                j = i + 1;
                ++num_commas;
            }
            char nameChar = line[j]; // the character after the comma
            int k = 0;
			// second while loop, when we want to actually read in the field
            while(nameChar != '\0' && nameChar != ',') {
                field[k] = nameChar;
                j++;
                nameChar = line[j];
                k++;
            }
            field[k] = '\0';
            currChar = nameChar;
			// if it is the index we are interested in and it is valid, return true
            if (num_commas == name_index) {
                if (((field[0] == '\"') && (field[strlen(field) - 1] == '\"') && (isQuotes))
                    || ((field[0] != '\"') && (field[strlen(field) - 1] != '\"') && (!isQuotes))) {
                    return true;
                }
                else {
                    return false;
                }
            }
        }
        ++i;
        currChar = line[i];
    }
    return true;
}

void AddName(char* line, char* name, int nameIndex) {
	/* This function adds a valid name to the names array.
	It copies each valid character over and does error checking as well.
	It also checks and stores meta data relating to the global vars */
    
    char tempName[1025];
    char currChar = line[0];
    int commaCount = 0;
    int i = 0;

    // loop through the line and count commas
    while(currChar != '\0') {
        if(currChar == ',') {
            commaCount++;
        }
    
        // if (currently a comma or first field), and if correct index, and if next char is not last
        if(((currChar == ',') || i == 0) && (commaCount == nameIndex) && (line[i + 1] != '\0')) {
            int j;
            if(i == 0 && (currChar != ',')) {
                j = 0;
            } else {
                j = i + 1;
            }
            char nameChar = line[j]; // the character after the comma
            int k = 0;
            while(nameChar != '\0' && nameChar != ',') {
                tempName[k] = nameChar;
                j++;
                nameChar = line[j];
                k++;
            }
            tempName[k] = '\0';
            currChar = nameChar;
        }

        i++;
        currChar = line[i];
    }
	//remove the quotes if the header had "name as well"
	//the quotes are guaranteed to be there due to error checking
    if (isQuotes) {
        for (int i = 1; i < strlen(tempName) - 1; ++i) {
            name[i-1] = tempName[i];
            name[i] = '\0';
        }
    } else {
        strcpy(name, tempName);
    }
}

void CountTweeters(char** names_array, Tweeter_Tweets* tweeter_tweets_array, int lineCount) { 
	/* This function counts the top 10 tweeters, then calls a different function
	to print them out. Modifies the arrays in the process. */

    int k = 0;
    int tweetArrSize = 1;

	// outer loop, for every line, get a name
    for(int i = 1; i < lineCount; i++) {
        char currName[1025];
        if(names_array[i][0] != '\0') { // if the name is not already counted, copy it over
            strncpy(currName, names_array[i], 1025);

            int tweetCount = 0;
			// inner loop, count how many times that name appears in the rest of the array
            for(int j = 1; j < lineCount; j++) {
				// if the name is not counted, count it, then mark it as counted
                if(strcmp(currName, names_array[j]) == 0) {
                    tweetCount++;
                    names_array[j][0] = '\0';
                }
            }
			// increase the overall array size and make space for it
            tweetArrSize++;
            tweeter_tweets_array = (Tweeter_Tweets*)realloc(tweeter_tweets_array, 
                                            tweetArrSize * sizeof(Tweeter_Tweets));
            tweeter_tweets_array[k].tweeter = (char*)malloc(1025 * sizeof(char));

            strncpy(tweeter_tweets_array[k].tweeter, currName, 1025);
            tweeter_tweets_array[k].tweets = tweetCount;
            k++;
        }
    }
    PrintTweeters(tweeter_tweets_array, tweetArrSize - 1);
}

void PrintTweeters(Tweeter_Tweets* tweeter_tweets_array, int tweetArrSize) {
	/* This function prints out the top 10 names. Contains a list
	of unique tweeters and doesn't modify the arrays any further */

	//print max of (tweeters and 10) statements
    for (int i = 0; ((i < 10) && (i < tweetArrSize)); i++) {
        int max_index = 0;
		//locate the max index by finding the highest tweet count
        for (int j = 0; j < tweetArrSize; ++j) {
            if(tweeter_tweets_array[j].tweets > tweeter_tweets_array[max_index].tweets) {
                max_index = j;
            }
        }
        printf("%s: %d\n", tweeter_tweets_array[max_index].tweeter, tweeter_tweets_array[max_index].tweets);
		//set this tweet count to -1 so it doesn't get set as the max in the future
        tweeter_tweets_array[max_index].tweets = -1;
    }
}