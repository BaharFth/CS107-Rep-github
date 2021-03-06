#include <vector>
#include <list>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>
#include "imdb.h"
#include "path.h"
using namespace std;

/**
 * Using the specified prompt, requests that the user supply
 * the name of an actor or actress.  The code returns
 * once the user has supplied a name for which some record within
 * the referenced imdb existsif (or if the user just hits return,
 * which is a signal that the empty string should just be returned.)
 *
 * @param prompt the text that should be used for the meaningful
 *               part of the user prompt.
 * @param db a reference to the imdb which can be used to confirm
 *           that a user's response is a legitimate one.
 * @return the name of the user-supplied actor or actress, or the
 *         empty string.
 */


//
static string promptForActor(const string& prompt, const imdb& db)
{
  string response;
  while (true) {
    cout << "please hit enter!";
    getline(cin, response);
    cout << prompt << " [or <enter> to quit]: ";
    getline(cin, response);

    if (response == "") return "";
    vector<film> credits;
    if (db.getCredits(response, credits)) return response;
    cout << "We couldn't find \"" << response << "\" in the movie database. "
	 << "Please try again." << endl;
  }
}

static void generateShortestPath(string source, string target, imdb& db) {

    list<path> partialPaths;                 //acts as a queue
    set<string> previouslySeenActors;        
    set<film> previouslySeenFilms;

    path partialPath(source);
    partialPaths.push_back(partialPath);

    while (!partialPaths.empty() && partialPaths.front().getLength() <= 5) {
        //pop the partial path at the front of the queue
        path front = partialPaths.front();
        partialPaths.pop_front();
        //get the movies of the last artist in this path
        vector<film> credits;
        db.getCredits(front.getLastPlayer(), credits);

        //for all the movies of this actor, do:
        for (unsigned i = 0; i < credits.size(); ++i) {
            film movie = credits[i];
            //cout << i<<" movie: " << movie.title << endl;
            //if the film is not previously visited:
            if (previouslySeenFilms.find(movie) == previouslySeenFilms.end()) {
                //insert the film to the priviously seen movies
                previouslySeenFilms.insert(movie);
                vector<string> players;
                //for all the cast in this movie:
                db.getCast(movie, players);
                for (unsigned j = 0; j < players.size(); ++j) {
                    string player = players[j];
                    //if the actor is not seen before...
                    if (previouslySeenActors.find(player) == previouslySeenActors.end()) {
                        //insert it in the visited actors,
                        previouslySeenActors.insert(player);
                        //clone the path,
                        path clone = front;
                        //if the player is the target, good, print the path
                        clone.addConnection(movie, player);
                        if (player == target) {
                            cout << clone;
                            return;
                        }
                        //otherwise, push it back to the end of the partial paths queue
                        else {
                            partialPaths.push_back(clone);
                        }
                    }
                }
            }
        }
    }
}


void printFilms(vector<film> films) {
    for (unsigned i = 0; i < films.size(); i++) {
        cout << films[i].title << endl;
    }
}
/**
 * Serves as the main entry point for the six-degrees executable.
 * There are no parameters to speak of.
 *
 * @param argc the number of tokens passed to the command line to
 *             invoke this executable.  It's completely ignored
 *             here, because we don't expect any arguments.
 * @param argv the C strings making up the full command line.
 *             We expect argv[0] to be logically equivalent to
 *             "six-degrees" (or whatever absolute path was used to
 *             invoke the program), but otherwise these are ignored
 *             as well.
 * @return 0 if the program ends normally, and undefined otherwise.
 */



int main(int argc, const char *argv[])
{
    //***BAHAR


    cout << "===============****6 Degrees****================" << endl;
    cout <<"----------------Building the imdb class---------"<<endl;
    
    //Initiate an imdb class instance
    imdb db("/home/ec2-user/Assignment_2/assn-2-six-degrees/updated-data/little-endian/");
    //ensure db is fine
  if (!db.good()) {
    cout << "Failed to properly initialize the imdb database." << endl;
    cout << "Please check to make sure the source files exist and that you have permission to read them." << endl;
    exit(1);
  }
  
  //printout the total number of actors and films in db
  db.printActorAndMovieCounts();
  cout << endl;


  /*
  cout << "--------------Print Actor and Movie Names-------------------" << endl;
  cout << "How many names would you like to see?" << endl;
  int count;
  cin >> count;
  bool printFirstCountNames = 1;

  if (printFirstCountNames) {

      db.printActorNames(count);
      cout << endl;
      db.printMovieNames(count);
      cout << endl;
  }
  */
  //test the getCredits function for "Meryl Streep"
  //cout << "--------------Testing getCredits function-------------------" << endl;
  /*
  //std::string name;
  std::cout << "Please, enter your full name: ";
  std::getline(std::cin, name);
  std::cout << "Hello, " << name << "!\n";
  cout << "Please enter an artis name:" << endl;
  //const 
  string player; //= "Meryl Streep";
  getline(cin, player);
  getline(cin, player);
  //cout << "player" << player;
  vector <film> films;
  db.getCredits(player, films);
  cout << "The number of films of " << player << " = " << films.size() << endl;
  //printFilms(films);
  cout << endl;
  cout << "--------------Testing getCast function-------------------" << endl;
  film movie;
  string movieTitle;
  cout << "Please enter a movie name:" << endl;
  getline(cin, movieTitle);
  getline(cin, movieTitle);
  movie.title = "Kosher";
  movie.year = 2002;
  vector <string> players;
  db.getCast(movie, players);
  cout << "The number of cast of " << movie.title << " = " << players.size() << endl;
  */
  
  //db.printActorNames(count);

 

  /*
  for (int i = 0; i < 400; i += 4)
  {
      cout << i<< "  "<<*(int*)db.Key2Address(i,db.getActorFileAddress()) << endl;
  }
  */
  
  //db.printActorName();
  //***
  cout << "Let's play!" << endl;
  while (true) {
    //Prompt for the first artist name
    string source = promptForActor("Actor or actress", db);
    cout << "source=" << source << endl; 
    if (source == "") break;
    //Prompt for the second artist name
    string target = promptForActor("Another actor or actress", db);
    if (target == "") break;
    if (source == target) {
      cout << "Good one.  This is only interesting if you specify two different people." << endl;
    } 
    //Call the function to find the path
    else {
        cout << "Calling generateShortestPath..." << endl;
      generateShortestPath(source, target, db);
    }
  }
  
  cout << "Thanks for playing!" << endl;
  return 0;
}

