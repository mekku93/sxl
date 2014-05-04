#ifndef __H_KEYWORDS__
#define __H_KEYWORDS__

#include <vector>

using namespace std;

// Keywords and reserved words
vector<string> KEYWORDS {

	"function",
	"if",
	"while",
	"halt",
	"in",

	"and",
	"or",
	"not",

	"read",
	"write",

	"set",
	"let",

	"int",
	"real",
	"char",
	"string",
	"bool",
	"unit",

	"true",
	"false",
	
};

/** 
 * Returns whether or not the given string parameter is a known keyword.
 */
bool isKeyword(string s) {
	return find( KEYWORDS.begin(), KEYWORDS.end(), s ) != KEYWORDS.end();
}


#endif