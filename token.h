#ifndef __H_TOKEN__
#define __H_TOKEN__

#include <string>
#include <sstream>
#include "tokentype.h"

using namespace std;

/** 
 * The Token class.
 * Represents a single token, with a type and a string of matched characters (image).
 */
class Token {
	private:
		/** Token type */
		string type;
		/** Token image */
		string image;
		/** Row where matched string was found in file */
		int row;
		/** Column where matched string was found in file */
		int col;

	public:
		/** Constructor */
		Token(string type, string image, int row, int col) {
			this->type = type;
			this->image = image;
			this->row = row;
			this->col = col;
		}
		Token ( Token* t ) {
			this->type = t->type;
			this->image = t->image;
			this->row = t->row;
			this->col = t->col;
		}

		/** Returns the token type. */
		string getType() {
			return this->type;
		}

		/** Returns the token image. */
		string getImage() {
			return this->image;
		}

		/** Returns the row position. */
		int getRow() {
			return this->row;
		}

		/** Returns the column position. */
		int getCol() {
			return this->col;
		}

		/** Checks if the token is a null token (indicated by an empty type) */
		bool isNullToken() {
			return this->type.length() == 0;
		}

		/** Checks if the token is an EOF token */
		bool isEOF() {
			return this->type == TK_EOF;
		}

		string getPosition() { 
			stringstream ss;
			ss << "line#" << this->row << ":" << this->col;
			return ss.str();
		}

		/** Used internally to print the token. */
		string toString() {
			stringstream ss;
			ss << "<" << this->type << "> " << this->image << " at " << getPosition();
			return ss.str();
		}

		static Token* nullToken() {
			return Token::nullToken(0,0);
		}
		static Token* nullToken(int row, int col) {
			return new Token("","",row,col);
		}
};


#endif