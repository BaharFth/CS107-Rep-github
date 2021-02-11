/**
 * File: rsg.cc
 * ------------
 * Provides the implementation of the full RSG application, which
 * relies on the services of the built-in string, ifstream, vector,
 * and map classes as well as the custom Production and Definition
 * classes provided with the assignment.
 */
 
#include <map>
#include <fstream>
#include <iterator>
#include <assert.h>
#include "definition.h"
#include "production.h"
using namespace std;

/**
 * Takes a reference to a legitimate infile (one that's been set up
 * to layer over a file) and populates the grammar map with the
 * collection of definitions that are spelled out in the referenced
 * file.  The function is written under the assumption that the
 * referenced data file is really a grammar file that's properly
 * formatted.  You may assume that all grammars are in fact properly
 * formatted.
 *
 * @param infile a valid reference to a flat text file storing the grammar.
 * @param grammar a reference to the STL map, which maps nonterminal strings
 *                to their definitions.
 */

static void readGrammar(ifstream& infile, map<string, Definition>& grammar)
{
  while (true) {
    string uselessText;
    getline(infile, uselessText, '{');
    if (infile.eof()) return;  // true? we encountered EOF before we saw a '{': no more productions!
    infile.putback('{');
    Definition def(infile);
    grammar[def.getNonterminal()] = def;
  }
}


/**
* function: printStringVector
* -----------------------------
* Prints a vector of strings 
*/
void printStringVector(vector <string>  phrases) {
    for (unsigned i = 0; i < phrases.size(); i++) {
        cout << phrases[i] << " ";
    }
    cout << endl;
}
/**
* function: isNonterminal
* -----------------------------
* Determines whether or not a string is a nonterminal
*/
bool isNonterminal(string word) {
    if (word[0] == '<' && word[word.size() - 1] == '>' ) return 1;
    else return 0;
}

/**
* function: isInGrammar
* -----------------------------
* Determines whether or not a string is the grammar
*/

bool isInGrammar(string word, map<string, Definition>& grammar) {
    return(grammar.count(word));
}

/**
* function: expand
* -----------------------------
* Expands a nonterminal to a vector of strings based on the grammar
*/

void expand(string word, map<string, Definition>& grammar, vector<string> &expansion) 
{
    //cout << "*------expanding: " << word;
    if (isNonterminal(word)) { 
        assert(isInGrammar(word, grammar));
        //cout << ": nonterminal"<<endl; 
        Definition def = grammar[word];
        Production randProduct = def.getRandomProduction();
        vector<string> phrases = randProduct.getPhrases();
        //cout << "Expansion: ";
        //printStringVector(phrases);
        //cout << endl;
        for (unsigned i = 0; i < phrases.size(); i++) {
            expand(phrases[i], grammar, expansion);
        }
    }
    else {
        //cout << ": terminal" << endl;
        expansion.push_back(word);
    }
}
/**
 * Performs the rudimentary error checking needed to confirm that
 * the client provided a grammar file.  It then continues to
 * open the file, read the grammar into a map<string, Definition>,
 * and then print out the total number of Definitions that were read
 * in.  
 * It then runs a random expansion of <start> according to this grammar for 3 times.
 *
 * @param argc the number of tokens making up the command that invoked
 *             the RSG executable.  There must be at least two arguments,
 *             and only the first two are used.
 * @param argv the sequence of tokens making up the command, where each
 *             token is represented as a '\0'-terminated C string.
 */
int main(int argc, char *argv[])
{
  cout << endl << "------------------------rsg---------------------" << endl<<endl;
  if (argc == 1) {
    cerr << "You need to specify the name of a grammar file." << endl;
    cerr << "Usage: rsg <path to grammar text file>" << endl;
    return 1; // non-zero return value means something bad happened 
  }
  
  ifstream grammarFile(argv[1]);
  if (grammarFile.fail()) {
    cerr << "Failed to open the file named \"" << argv[1] << "\".  Check to ensure the file exists. " << endl;
    return 2; // each bad thing has its own bad return value
  }
  
  // things are looking good...

  //build the grammar as per the file
  map<string, Definition> grammar;
  readGrammar(grammarFile, grammar);

  
  cout << "The grammar file is called: "<< argv[1] << endl;
      //"\" contains"<< grammar.size() << " definitions." << endl;

  //make a vector and expand the <start> word in it for 3 times
  cout << "** 3 random Expansions: " << endl<<endl;
  vector<string> expansion = {};
  for (int i = 1; i < 4; i++) {
      cout << "* expansion "<<i <<":"<< endl;
      expansion.clear();
      expand("<start>", grammar, expansion);
      printStringVector(expansion);
      cout << endl;
  } 
  return 0;
}
