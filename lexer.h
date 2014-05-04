// HEADER GUARDS
#ifndef __LEXER_H__
#define __LEXER_H__

// INCLUSIONS
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
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
		// Bool flag, for indicating if done or not
		// Used to prevent re-reading the last EOF character over and over again
		bool done;
		// Stream vector of tokens
		vector <Token> tokens;
		vector <Token>::iterator iterator;
		// Verbose output
		bool verbose = false;



		string error(){
			return string("Lexer: Unrecognized input. ");
		}

		string filePos() {
			stringstream ss;
			ss << ", at " << this->filepath << ":" << this->row << ":" << this->col;
			return ss.str();
		}

	public:


		Lexer() {
			this->init("");
		}

		/**
		 * Constructor
		 */
		Lexer(string filepath) {
			this->init(filepath);
		}
		

		void init(string filepath) {
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
			// Set the done lfag to false
			this->done = false;
		}

		// Sets verbose output on or off
		void setVerbose(bool v) {
			this->verbose = v;
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
		bool eof() {
			return this->in->eof() || this->peek() == EOF;
		}


		/**
		 * Returns true if the lexer has finished.
		 */
		bool isDone() {
			return this->done;
		}
		

		vector<Token>::iterator getPosition() {
			return this->iterator;
		}
		void setPosition(vector<Token>::iterator it) {
			this->iterator = it;
		}
		// Moves the iterator forward
		void forward() {
			this->iterator++;
		}
		// Moves the iterator backwards
		void backwards() {
			this->iterator--;
		}
		// Returns a pointer to the token, pointed to by the iterator
		Token* getToken() {
			return &(*this->iterator);
		}

		// Moves the iterator forward and returns the token
		Token* nextToken() {
			if ( this->iterator != this->tokens.end() ) {
				Token* tk = &*this->iterator;
				this->iterator++;
				return tk;
			} else {
				return Token::nullToken(this->row,this->col);
			}
		}
		Token* previousToken() {
			this->iterator--;
			if ( this->iterator != this->tokens.begin() ) {
				Token* tk = &*this->iterator;
				return tk;
			} else {
				return Token::nullToken(this->row, this->col);
			}
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
		void generateTokens() {

			// Loop if:
			// 		not EOF or not empty store
			// and	no match currently found
			while ( !done || this->hasStore() ) {

				// CURRENT TOKEN
				Token* tk = Token::nullToken(this->row, this->col);

				// Flag to check if a token has been matched
				bool matched = false;

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
							// until an end of line/file is found
						} while ( !(ch == '\n' || this->eof()) );
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
						} while ( !(ch == '*' && p == '/') && !this->eof() );
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
							return;
						}
					} else {
						cout << this->error() << "Expected a printable character, found '"  << ch << "'" << this->filePos();
						return;
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
					string tk_type = "";
					// If the image is a keyword
					if ( isKeyword(tk_image) ) {
						tk_type = TK_KEYWORD;
						// Set the type to TK_BOOL if "true" or "false"
						if ( tk_image == "false" || tk_image == "true" ) {
							tk_type = TK_BOOL;
						}
						// Set the type to TK_MULT_OP if "and"
						else if ( tk_image == "and" ) {
							tk_type = TK_MULT_OP;
						}
						// Set the type to TK_ADD_OP if "or"
						else if ( tk_image == "or" ) {
							tk_type = TK_ADD_OP;
						}
					} else {
						// If not a keyword, then an identifier
						tk_type = TK_IDENTIFIER;
					}
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

						// <REAL>
						// Loop until we don't find any more digits
						do {
							ch = this->next();
							this->pushToBuffer(ch);
						}
						while( Lexer::isDigit(ch) );
						// Check if last character was an 'E' or 'e'
						if ( ch == 'E' || ch == 'e' ) {
							char p = this->peek();
							// check if the peek is a plus or minus
							if ( p == '+' || p == '-' ) {
								// if it is, get it and push it to the bufer
								ch = this->next();
								this->pushToBuffer(ch);
								// Loop until we don't find any more digits
								do {
									ch = this->next();
									this->pushToBuffer(ch);
								}
								while( Lexer::isDigit(ch) );
							}
						}
						// End of reading the real number. Set the type to real
						tk_type = TK_REAL;

					} // End of <REAL> check

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

				// Equals and not equals comparison
				if ( (ch == '=' || ch == '!') && this->peek() == '=' ) {
					// Push the first equals
					this->pushToBuffer(ch);
					// read the peeked char
					ch = this->next();
					// push the peeked equals
					this->pushToBuffer(ch);

					// Create the token
					tk = new Token( TK_REL_OP, this->flushBuffer(), this->getRow(), this->getCol() );
					matched = true;
				}


				// IF NO MATCH FOUND SO FAR
				if ( !matched ) {
					// SYNTAX SYMBOLS
					string tk_type("");
					switch( ch ) {
						case '#':
							tk_type = TK_UNIT;
							break;
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
							tk_type = TK_OPEN_PAREN;
							break;
						case ')':
							tk_type = TK_CLOSE_PAREN;
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
							tk_type = TK_EQUALS_OP;
							break;

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
							}
							break;
						default:
							if ( this->eof() ) tk_type = TK_EOF;
							this->done = true;
							break;

					} // End of switch

					// Push the last character read to the buffer
					this->pushToBuffer(ch);
					// If a syntax symbol was found
					if ( tk_type.length() > 0 ) {
						// Create the token
						string image = (tk_type == TK_EOF)? "eof" : this->flushBuffer();
						tk = new Token( tk_type, image, this->getRow(), this->getCol() );
					}

				} // End of !matched check

				// If not token was matched
				if ( tk->isNullToken() ) {
					// If not end of file, then unrecognized character was read
					if ( !this->eof() ) {
						cout << "Lexer: Unrecognized input '" << ch << "' at " << this->getFilePath() << ":" << this->getRow() << ":" << this->getCol() << endl;
						return;
					} else {
						// Read EOF. We are done
						this->done = true;
					}
				}

				// If there was a match, push the token back
				this->tokens.push_back( *tk );
				if ( this->verbose == true ) {
					cout << tk->toString() << endl;
				}

			} // End of while loop
			this->iterator = tokens.begin();
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