#include <iostream>
#include <string>

#include "lexer.h"
#include "parser.h"
#include "token.h"

int main(){
	// Create the lexer, and generate the tokens from the file
	Lexer* lexer = new Lexer("sample.sxl");
	// lexer->setVerbose(true);
	lexer->generateTokens();

	// Create the parser
	Parser parser(lexer);
	//parser.setVerbose(true);
	try {
		cout << parser.parseSXL()->toString() << endl;
		cout << "\nDONE!!!!" << endl; 
	} catch( ParseException &e ) {
		cout << "\n" << e.what() << "\n" << endl;
	}

	return 0;
}