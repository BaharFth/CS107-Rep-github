using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}


const void* imdb::getActorFileAddress() { return this->actorFile; }

const void* imdb::getFilmFileAddress() { return this->movieFile; }

int imdb::getActorCount() { return *(int*) actorFile; }
int imdb::getMovieCount() { return *(int*) movieFile; }

void imdb::printActorAndMovieCounts() {
	cout << "Actor Count = " << this->getActorCount() << endl;
	cout << "Movie Count = " << this->getMovieCount() << endl;
}
/*
void* imdb::Key2Address(int key) {
	return (void*)((char*)actorFile+key);
}
*/

void* imdb::Key2Address(int key,const void* file) {
	return (void*)((char*)file + key);
}

//imdb::findActorAddress()
/*
void imdb::printActorName(int i) {
	char* actorAddress = 
		(char*) Key2Address(*(int*) Key2Address(i, actorFile), actorFile);
	string actorName;
	char letter;
	int ii = 0;
	do {
		letter = *(actorAddress + ii);
		actorName += letter;
		//cout << actorName << endl;
		ii++;
	} while (ii<800);
	cout << "Actor:"<<actorName << endl;
};
*/
void imdb::printActorNames(int count) {
    cout << "The first " << count << " actors are: " << endl;
    int numActors = *(int*)(actorFile);
    if (count > numActors) { count = numActors; };
    char* locPointer = (char*)actorFile + 4;
    for (int i = 0; i < count; i++) {
        locPointer += 4 * i;
        char* nameLetter = (char*)actorFile + (*(int*)locPointer);
        string name;
        while (*nameLetter != '\0') {
            name += *nameLetter;
            nameLetter++;
        }
        cout << i<<"- "<<name << endl;
    }
}

void imdb::printMovieNames(int count) {
    cout << "The first " << count << " Movies are: " << endl;
    int numMovies = *(int*)(actorFile);
    if (count > numMovies) { count = numMovies; };
    char* locPointer = (char*)movieFile + 4;
    for (int i = 0; i < count; i++) {
        locPointer += 4 * i;
        char* nameLetter = (char*)movieFile + (*(int*)locPointer);
        string name;
        while (*nameLetter != '\0') {
            name += *nameLetter;
            nameLetter++;
        }
        nameLetter++;
        int year = *nameLetter + 1900;

        cout << i <<"- "<<name<<"  ("<< year <<")"<< endl;
    }
}
//char* actorAddress = (char*)actorFile + *(int*)found;
	//void getActorOtherAddresses(actorP,actorMovieCountP, actorMoviesP);
	//void getActorMoviesKeys(vector<int>& movieKeys);
	//void	getMoviesByLocation(vector<int>&moviekeys, vector<film> &films>;
// you should be implementing these two methods right here... 
/*
bool imdb::getCredits(const string& player, vector<film>& films) const { 
	//void* findActorAddress(string& player);
	//void getActorOtherAddresses(actorP,actorMovieCountP, actorMoviesP);
	//void getActorMoviesKeys(vector<int>& movieKeys);
	//void	getMoviesByLocation(vector<int>&moviekeys, vector<film> &films>;
	//
	return false; }
*/
// s1: a struct key
// s2: an address in the file, where the offset to a string (actor or film name) is saved
int imdb::playercmp(const void * s1, const void * s2) {
    struct Key* pKey = (struct Key *) s1;
    char * toCompare = (char *) pKey->file + * (int *) s2;

    return strcmp( (const char *) pKey->pkey, toCompare );
}

int imdb::moviecmp(const void* s1, const void* s2) {
    struct Key* pKey = (struct Key*)s1;
    char* toCompare = (char*)pKey->file + *(int*)s2;

    struct film* pfilmKey = (struct film*)pKey->pkey;

    string title;
    while (*toCompare != '\0') {
        title += *toCompare;
        toCompare++;
    }
    //cout << "toCompare Title: " << title << endl;

    /* Skip \0 at end of string */
    toCompare++;

    /* year byte contains an offset from 1900 */
    int year = *toCompare + 1900;

    struct film filmToCompare;
    filmToCompare.title = title;
    filmToCompare.year = year;


    /* compare the two films using overlaoded operator */
    if (*pfilmKey < filmToCompare) return -1;
    else if (*pfilmKey == filmToCompare) return 0;
    else return 1;
}

int cmpfunc(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);   
}


/*
 *Method: getCredits
 *input: an artist name in form of string
 *output: (through pass by reference), a vector containing all
 *the films of the artist
*/
bool imdb::getCredits(const string& player, vector <film>& films) const {
    int numActors = *(int*)(actorFile);     //the number of actors is saved in the first 4 bytes
    void* cplayer = (void*)player.c_str();  //an adress to the player's name string

    /*struct Key { void* file; void* pkey; };*/
    //is a struct that saves a file adress and a data address
    struct Key playerKey;

    
    playerKey.file = (void*)actorFile;      //Adress of the actorFile
    playerKey.pkey = cplayer;               //the adress of plater name string


    //Use bsearch to see if the actor name exists in this file
    void* found = bsearch(
        &playerKey,                // pointer to the string we are looking for
        (int*)actorFile + 1,       // "array" base of offsets
        numActors,                 // array size (number of actors)
        sizeof(int),               // size of array element
        imdb::playercmp            // comparison function
    );

    //if actor not found
    if (found == NULL) return false;



    /* If the actor is found, procede to the next steps.
     * 1- set a pointer to the actor location in file

     * In the actor file, if a player has an even
     * number of characters, it's followed by a extra '\0'. Then a short
     * is there to depict the number of movies the player has acted
     * in. If sum of bytes of actor name and two bytes of number of
     * movies is not divisible by 4, then assume a 2 byte pad and go
     * next appropriate location.
     */
     
     /* locPointer: a pointer to our current location */
    char* locPointer = (char*)actorFile + *(int*)found;

    
    /*
    Print Actor Name (only for test):
    string name;
    char* nameLetter = (char*)locPointer;
    while (*nameLetter != '\0') {
        name += *nameLetter;
        nameLetter++;
    }
    cout << "Actor Name in the file is: " << name << "at Location "<< *(int*)found<< endl;
    */
   
    /* An integer to count the number of bytes from  the start of 
     * the record to the short containing number of movies
     */
    int nBytesFromStart = 0;

    /* move to the short location pointing to the number of movies */
    if (player.size() % 2 == 0) {
        locPointer += (player.size() + 2);
        nBytesFromStart += (player.size() + 2);
    }
    else {
        locPointer += player.size() + 1;
        nBytesFromStart += player.size() + 1;
    }


    short nMovies = *(short*)locPointer;
    nBytesFromStart += 2;
    locPointer += 2;

    /* if the number of bytes containing the name and movie count is not 
     * divisible by two, pad it up with extra 00 so it is
    */
    if (nBytesFromStart % 4 != 0) locPointer += 2;

    /* for all movies of this artist:
     * 1- save the movie address in the pointer movielocPointer
     * 2- from the movie file, get the movie name and year and save them
     *    in a new movie object
     * 3- push this movie object to the films vector
    
    */
    for (short i = 0; i < nMovies; i++) {
        struct film _film;

        /* calculate offset into movie file in memory */
        char* movielocPointer = (char*)movieFile + *(int*)locPointer;
        locPointer += 4;

        string title;
        while (*movielocPointer != '\0') {
            title += *movielocPointer;
            movielocPointer++;
        }

        /* Skip \0 at end of string */
        movielocPointer++;

        /* year byte contains an offset from 1900 */
        int year = *movielocPointer + 1900;

        _film.title = title;
        _film.year = year;

        films.push_back(_film);
    }
    return true;
}



/*Method: getCast
 *input: a fim& movie
 *output: (through pass by reference), a vector containing all
 *that movie's cast
*/
bool imdb::getCast(const film& movie, vector <string>& players) const {

    //cout << "Movie is " << movie.title << endl;


    /* The total number of movies in the file is saved in the first four
     * digits of the movieFile
    */
    int nElements = *(int*)movieFile;

    /*struct Key { void* file; void* pkey; };*/
    //is a struct that saves a file adress and a data address
    struct Key movieKey;
    movieKey.file = (void*)movieFile;
    movieKey.pkey = (void*)&movie;

    /*
     * use bsearch and the imdb::moviecmp to see if the requested movie exists
     * in the database
     */
    void* found = bsearch(
        &movieKey,                  // pointer to key
        (int*)movieFile + 1,        // pointer to base
        nElements,                  // number of elements
        sizeof(int),                // size of element
        imdb::moviecmp              // compare two pointer to pointer to c string
    );

    //If the movie was not found...
    if (found == NULL) { 
        cout << "Not Found!" << endl;
        return false;
    }



    /* a pointer to movie record location in memory 
     * this pointer move with us along the movie record
    */
    char* locPointer = (char*)movieFile + *(int*)found;

    {
        char* nameLetter = locPointer;
        string name;
        while (*nameLetter != '\0') {
            name += *nameLetter;
            nameLetter++;
        }
        //cout << "The film name from file is: " << name << endl;
    }

    int numBytesFromStart = 0;
    /* skip the number of bytes for movie name and year, if the total
     * number of bytes is odd, skip one more byte
     */

    numBytesFromStart += movie.title.size() + 2;
    locPointer += (movie.title.size() + 2);
    if (numBytesFromStart % 2 != 0) {
        numBytesFromStart++;
        locPointer++;
    }

    /* get the number of players and number of bytes from 
     * movie record start
     */
    short nPlayers = *(short*)locPointer;
    numBytesFromStart += 2;
    locPointer += 2;

    /* if the numBytesFrom start is not divisible by 4,
       move 2 more bytes ahead
    */
    if (numBytesFromStart % 4 != 0) locPointer += 2;


     /* for all the cast of this movie:
     * 1- save the  artist in the pointer actorlocPointer
     * 2- from the actor file, get the actor name 
     * 3- push this actor name to the actors vector

    */

    for (short i = 0; i < nPlayers; i++) {
        /* calculate offset into movie file in memory */
        char* actorlocPointer = (char*)actorFile + *(int*)locPointer;
        locPointer += 4;

        string name;
        while (*actorlocPointer != '\0') {
            name += *actorlocPointer;
            actorlocPointer++;
        }

        players.push_back(name);
    }

    return true;
}
//***
imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
