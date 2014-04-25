// HEADER GUARDS
#ifndef __LEXER_H__
#define __LEXER_H__

// INCLUSIONS
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "tokentype.h"
#include "token.h"
#include "keywords.h"

// NAMESPACE
using namespace std;


/**
 * The Lexer class.
 */
class Lexer {

	private:
		// The file path of the source file
		string filepath;
		// The source file input stream
		ifstream* in;
		// The row of the current character being read in the file
		int row;
		// The column of the current character being read in the file
		int col;
		// The buffer: all characters read so far that haven't yet matched a token.
		// This buffer will be reset to an empty string, when a token has been matched.
		string buffer;
		// The storage string.
		// Any extra characters read and pushed to the buffer, that do not match the
		// current token being generated. They should be popped and used next, in place
		// of the next character in the file.
		string storage;

		string error(){
			return string("Lexer: Unrecognized input. ");
		}

		string filePos() {
			stringstream ss;
			ss << ", at " << this->filepath << ":" << this->row << ":" << this->col;
			return ss.str();
		}

	public:


		/**
		 * Constructor
		 */
		Lexer(string filepath) {
			// Save the filepath
			this->filepath = filepath;
			// Create an input file stream
			this->in = new ifstream(this->filepath);
			// Initialize the row and col
			this->row = 1;
			this->col = 0;
			// Initialize the buffer
			this->buffer = string("");
			// Initialie the storage string
			this->storage = string("");
		}
		



		/**
		 * Reads the next character.
		 * Increments row and col appropriately.
		 */
		char next() {
			// Get the next character
			// noskipws: Do not skip whitespace
			char c;
			*this->in >> noskipws >> c;
			// If a new line
			if ( c == '\n' ) {
				// Increment row, and reset col
				this->row++;
				this->col = 0;
			}
			// If not new line, increment col
			else if ( c == '\t' ) {
				this->col += 4;
			} else {
				this->col++;
			}
			// Return the character
			return c;
		}
		/**
		 * Peeks for the next character.
		 * Does not increments row and col or move on to the next character.
		 * Can be used to check what the next character is, without advancing in
		 * the source file.
		 */
		char peek() {
			return this->in->peek();
		}




		/**
		 * Returns the source file path
		 */
		string getFilePath() {
			return this->filepath;
		}
		/**
		 * Returns the row
		 */
		int getRow() {
			return this->row;
		}
		/**
		 * Returns the col
		 */
		int getCol() {
			return this->col;
		}



		/**
		 * Adds the given character to the buffer.
		 */
		void pushToBuffer(char c) {
			this->buffer.push_back(c);
		}
		/**
		 * Returns the contents of the buffer.
		 */
		string getBuffer() {
			return this->buffer;
		}
		/**
		 * Flushes the buffer and returns the flushed contents.
		 */
		string flushBuffer() {
			string b = this->buffer;
			this->buffer = "";
			return b;
		}




		/**
		 * Pushed the character to the store stack.		
		 */
		void pushStore(char c) {
			this->storage.push_back(c);
		}

		/**
		 * Pops the first character from the store stack.
		 */
		char popStore() {
			char c = this->storage[0];
			this->storage = this->storage.substr(1);
			return c;
		}
		/**
		 * Removes and stores the last character in the buffer
		 */
		void storeFromBuffer() {
			// Get the last char in the buffer
			char c = this->buffer.back();
			// Push it to the storage
			this->pushStore(c);
			// Set the buffer to its own substring, excluding the last character
			this->buffer = this->buffer.substr(0, this->buffer.length() - 1);
		}
		/**
		 * Returns whether or not there are characters in storage.
		 * Can be used to check if the next character should come from the file or
		 * from the storage.
		 */
		bool hasStore() {
			return this->storage.length() > 0;
		}


		

		/**
		 * Returns true if the end of the input has been reached.
		 */
		bool done() {
			return this->in->eof() || this->peek() == EOF;
		}
		




		/** 
		 * Returns the next token.
		 *
		 * The method will read a single character from the file, and using a lookahead of 1, it will determine
		 * the type of the next token to be read.
		 *
		 * Once the type is determined, it will continue consuming tokens, until the token is complete.
		 * 
		 * Returns the next token, or a null token if either:
		 *  i)	End of input has been reached.
		 * ii)	No token could by determined from the input.
		 */
		Token* nextToken() {
			Token* tk = new Token("","",0,0);

			// Flag to check if a token has been matched
			bool matched = false;

			while ( ( !this->done() || this->hasStore() ) && !matched ) {

				// Read the next character
				// If there are characters in the store, pop the next one, otherwise read from file
				char ch = ( this->hasStore() )? this->popStore() : this->next();

				// WHITESPACE SKIP
				if ( ch == '\n' || ch == ' ' || ch == '\t' ) {
					continue;
				}

				// COMMENTS
				if ( ch == '/' ) {
					// Peek the next character
					char p = this->peek();
					// Check if another forward slash (line comment)
					if ( p == '/' ) {
						do {
							// Keep reading characters
							ch = this->next();
							this->pushToBuffer(ch);
							// until an end of line is found
						} while ( ch != '\n' );
						// Flush the buffer
						this->flushBuffer();
						continue;
					}
					// Otherwise, if the peek char is an asterisk (block comment)
					else if ( p == '*' ) {
						do {
							// Get the next character
							ch = this->next();
							this->pushToBuffer(ch);
							// Peek the one after that
							p = this->peek();
						} while ( !(ch == '*' && p == '/') );
						// Keep reading characters until the read character and the peeked
						// one indicate a closing block comment
						// Read the next character (the peeked '/')
						this->next();
						// Flush the buffer
						this->flushBuffer();
						continue;
					}
				}


				// STRING LITERALS
				if ( ch == '"' ) {
					// Push the double quote to the buffer
					this->pushToBuffer(ch);
					// Loop until we don't find anymore printable characters
					bool ignoreNextQuote = false;
					do {
						ch = this->next();
						this->pushToBuffer(ch);
						// If a backslash is found, ignore the next dbl quote - treat is as a printable
						if ( ch == '\\' && this->peek() == '"' ) {
							ignoreNextQuote = true;
						}
						// If a dbl quote is found
						if ( ch == '"' ) {
							// check if ignoring the next quote,
							if ( ignoreNextQuote ) {
								// If yes, reset ignoreNextQuote to false
								ignoreNextQuote = false;
							} else {
								// Otherwise, stop. We've reached the end of the string literal
								break;
							}
						}
					} while( Lexer::isPrintable(ch) );

					// The last, extra character should be another double quote
					if ( ch == '"' ) {
						tk = new Token( TK_STRING, this->flushBuffer(), this->getRow(), this->getCol() );
						matched = true;
					}
				}


				// CHARACTER LITERALS
				if ( ch == '\'' ) {
					// Push the single quote to the buffer
					this->pushToBuffer(ch);
					// Get hte next character
					ch = this->next();
					this->pushToBuffer(ch);
					// It should be a printable
					if ( Lexer::isPrintable(ch) ) {
						// If the character is a backslash, accept the next character, even if it is a single quote
						if ( ch == '\\' ) {
							// Get the next character
							ch = this->next();
							this->pushToBuffer(ch);
						}
						// Get the next character
						ch = this->next();
						this->pushToBuffer(ch);
						// It should be a single quote
						if ( ch == '\'' ) {
							// Create the token
							tk = new Token( TK_CHAR, this->flushBuffer(), this->getRow(), this->getCol() );
							matched = true;
						} else {
							cout << this->error() << "Expected \"'\", found '"  << ch << "'" << this->filePos();
							return Token::nullToken();
						}
					} else {
						cout << this->error() << "Expected a printable character, found '"  << ch << "'" << this->filePos();
						return Token::nullToken();
					}
				}


				// IDENTIFIERS / KEYWORDS
				// If character is an alpha char or underscore ...
				if ( Lexer::isAlpha(ch) || Lexer::isUnderscore(ch) ) {
					// Push character to buffer
					this->pushToBuffer(ch);
					// Loop until we don't find any more identifier characters
					while ( Lexer::isIdentifierChar(ch) ) {
						ch = this->next();
						this->pushToBuffer(ch);
					}

					// Store the last character read (extra)
					this->storeFromBuffer();

					// Create the token
					string tk_image = this->flushBuffer();
					string tk_type = ( isKeyword(tk_image) )? TK_KEYWORD : TK_IDENTIFIER;
					tk = new Token( tk_type, tk_image, this->getRow(), this->getCol() );
					matched = true;
				}


				// INTEGERS AND REALS
				if ( Lexer::isDigit(ch) ) {
					// Push character to buffer
					this->pushToBuffer(ch);
					// Loop until we don't find any more digits
					while( Lexer::isDigit(ch) ) {
						ch = this->next();
						this->pushToBuffer(ch);
					}
					// Set the type to an integer
					string tk_type = TK_INTEGER;
					// Check for a dot, i.e. reading a real, not an integer
					if ( ch == '.' ) {
						// Loop until we don't find any more digits
						do {
							ch = this->next();
							this->pushToBuffer(ch);
						}
						while( Lexer::isDigit(ch) );
						// End of reading the real number. Set the type to real
						tk_type = TK_REAL;
					}
					// Store the last character read (extra)
					this->storeFromBuffer();
					// Create token
					tk = new Token( tk_type, this->flushBuffer(), this->getRow(), this->getCol() );
					matched = true;
				}


				// ASSIGNMENT OPERATOR "<-"
				// If the character is a '<' (less than) character, and the peeked char is a '-' (minus)
				if ( ch == '<' && this->peek() == '-' ) {
					// Push the read character (<)
					this->pushToBuffer(ch);
					// read the peeked char
					ch = this->next();
					// push the peeked character (-)
					this->pushToBuffer(ch);

					// Create the token
					tk = new Token( TK_ASSIGN_OP, this->flushBuffer(), this->getRow(), this->getCol() );
					matched = true;
				}


				// SYNTAX SYMBOLS
				if ( !matched ) {

					string tk_type("");
					switch( ch ) {
						case ':':
							tk_type = TK_COLON;
							break;
						case ';':
							tk_type = TK_SEMICOLON;
							break;
						case ',':
							tk_type = TK_COMMA;
							break;

						case '(':
							tk_type = TK_OPEN_BRACE;
							break;
						case ')':
							tk_type = TK_CLOSE_BRACE;
							break;
						case '{':
							tk_type = TK_OPEN_BLOCK;
							break;
						case '}':
							tk_type = TK_CLOSE_BLOCK;
							break;
						
						case '+':
						case '-':
							tk_type = TK_ADD_OP;
							break;

						case '*':
						case '/':
							tk_type = TK_MULT_OP;
							break;

						case '=':
						case '>':
						case '<':
							// Relational operator
							tk_type = TK_REL_OP;
							// If we peek and find an '=' symbol, add it to the rel op
							if ( this->peek() == '=' ) {
								// Push the initial character
								this->pushToBuffer(ch);
								// Get the following equals (pushed later after end of switch)
								ch = this->next();
							} else {
								// If no peeked equals, and inital char was an equals,
								if ( ch == '=' ) {
									// Set the type to an equals op
									tk_type = TK_EQUALS_OP;
								}
							}

					}
					// Push the last character read to the buffer
					this->pushToBuffer(ch);

					if ( tk_type.length() > 0 ) {
						// Create the token
						tk = new Token( tk_type, this->flushBuffer(), this->getRow(), this->getCol() );
					}
				}

				// Check if a token was matched
				if ( tk->isNullToken() ) {
					// Show an error if not
					cout << "Lexer: Unrecognized input '" << ch << "' at " << this->getFilePath() << ":" << this->getRow() << ":" << this->getCol() << endl;
					break;
				} else {
					matched = true;
				}
			}

			return tk;
		}






		/** 
		 * Returns whether or not the given character parameter is an alphabet letter.
		 */
		static bool isAlpha(char c) {
			return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
		}
		/** 
		 * Returns whether or not the given character parameter is an underscore character.
		 */
		static bool isUnderscore(char c) {
			return c == '_';
		}
		/** 
		 * Returns whether or not the given character parameter is a digit.
		 */
		static bool isDigit(char c) {
			return c >= '0' && c <= '9';
		}
		/** 
		 * Returns whether or not the given character parameter is an identifier valid character.
		 */
		static bool isIdentifierChar(char c) {
			return Lexer::isAlpha(c) || Lexer::isDigit(c) || Lexer::isUnderscore(c);
		}
		/**
		 * Returns whether or not the given character parameter is a printable character.
		 */
		static bool isPrintable(char c) {
			return c >= '\x20' && c <= '\x7E';
		}


};


#endif