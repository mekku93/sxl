#include <iostream>
#include <string>

#include "lexer.h"
#include "token.h"

int main() {
	Lexer lexer("sample.sxl");

	Token* tk = new Token("","",0,0);

	while ( !(tk = lexer.nextToken())->isNullToken() ) {
		cout << tk->toString() << endl;
	}
	cout << "\n\n";

	return 0;
}