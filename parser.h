#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <vector>
#include "lexer.h"
#include "token.h"
#include "parse-exception.h"
#include "astnode.h"

class Parser {

private:
	Lexer* lexer;
	string tree = "";
	bool verbose;

	ParseException error(string msg) {
		ParseException e(msg);
		return e;
	}

public:
	Parser(Lexer* l) {
		this->lexer = l;
	}
	void out(string s) {
		if ( verbose ) {
			cout << s << endl;
		}
	}
	void setVerbose(bool v) {
		verbose = v;
	}
	Token* nextToken() {
		Token* token = lexer->nextToken();
		out( "> NEXT: " + token->toString() );
		return token;
	}
	Token* previousToken() {
		Token* token = lexer->previousToken();
		//out( "< PREV: " + token->toString() );
		return token;
	}







	/**
	 * <RelationalOp> ::= '<' | '>' | '==' | '!=' | '<=' | '>='
	 */
	ASTNode* parseRelOp() {
		out("Parsing relational operator");

		Token* token = nextToken();

		// Check if the token is a relational operator
		if ( token->getType() != TK_REL_OP ) {
			previousToken();
			throw error("a relational operator");
		}
		
		if ( token->getImage() == ">" ) {	return new GreaterNode(); }
		if ( token->getImage() == "<" ) {	return new LesserNode(); }
		if ( token->getImage() == "==" ) {	return new EqualsNode(); }
		if ( token->getImage() == "!=" ) {	return new NotEqualsNode(); }
		if ( token->getImage() == ">=" ) {	return new GreaterEqualsNode(); }
		if ( token->getImage() == "<-" ) {	return new LesserEqualsNode(); }
		
		// If nothing match, throw an error. Should not, since lexer should successfully
		// parse the correct relational operator image to this token type
		previousToken();
		throw error( "Relational operator token has incorrect image. Possible bug in the lexer.");
	}


	/**
	 * <AddOp> ::= '+' | '-' | "or"
	 */
	ASTNode* parseAddOp() {
		out("Parsing additive operator");

		Token* token = nextToken();

		// Check if the token is an additive operator.
		if ( token->getType() != TK_ADD_OP ) {
			previousToken();
			throw error("an additive operator");
		}
		
		if ( token->getImage() == "+" ) {	return new PlusNode(); }
		if ( token->getImage() == "-" ) {	return new MinusNode(); }
		if ( token->getImage() == "or" ) {	return new OrNode(); }
		
		// If nothing match, throw an error. Should not, since lexer should successfully
		// parse the correct additive operator image to this token type
		previousToken();
		throw error( "Additive operator token has incorrect image. Possible bug in the lexer.");
	}


	/**
	 * <MultOp> ::= '*' | '/' | "and"
	 */
	ASTNode* parseMultOp() {
		out("Parsing multiplicative operator");

		Token* token = nextToken();

		// Check if the token is an multiplicative operator.
		if ( token->getType() != TK_MULT_OP ) {
			previousToken();
			throw error("an multiplicative operator");
		}
		
		if ( token->getImage() == "*" ) {	return new MultiplyNode(); }
		if ( token->getImage() == "/" ) {	return new DivideNode(); }
		if ( token->getImage() == "and" ) {	return new AndNode(); }
		
		// If nothing match, throw an error. Should not, since lexer should successfully
		// parse the correct multiplicative operator image to this token type
		previousToken();
		throw error( "Multiplicative operator token has incorrect image. Possible bug in the lexer.");
	}







	/**
	 * <Identifier> ::= <TK_IDENTIFIER>
	 */
	ASTNode* parseIdentifier() {
		Token* token = nextToken();

		// If not an identifier, go back 1 token and throw an error
		if ( token->getType() != TK_IDENTIFIER ) {
			previousToken();
			throw error( "Expected an identifier, found " + token->toString() );
		}
		// Return the node
		return new IdentifierNode( token->getImage() );
	}



	/**
	 * <Type> ::= 'int' | 'real' | 'bool' | 'char' | 'string' | 'unit'
	 */
	ASTNode* parseType() {
		Token* token = nextToken();

		// If a keyword
		if ( token->getType() == TK_KEYWORD ) {
			// Prepare known types
			std::vector<string> types {"int", "real", "bool", "char", "string", "unit"};
			// Check if token is a type keyword
			if ( find( types.begin(), types.end(), token->getImage() ) != types.end() ) {
				return new TypeNode( token->getImage() );
			}
		}

		// Go back 1 token and throw error if not a type keyword
		previousToken();
		throw error( "Unknown type '" + token->getImage() + "', at " + token->getPosition() );
	}





	/**
	 * <Literal> := <TK_INTEGER> | <TK_REAL> | <TK_BOOL> | <TK_CHAR> | <TK_STRING> | <TK_UNIT>
	 */
	ASTNode* parseLiteral() {
		Token* token = nextToken();

		// Prepare the type and value
		string value = token->getImage();

		// Check the token type
		if ( token->getType() == TK_INTEGER ) {
			return new IntegerLiteralNode(value);
		}
		if ( token->getType() == TK_REAL ) {
			return new RealLiteralNode(value);
		}
		if ( token->getType() == TK_BOOL ) {
			return new BooleanLiteralNode(value);
		}
		if ( token->getType() == TK_CHAR ) {
			return new CharLiteralNode(value);
		}
		if ( token->getType() == TK_STRING ) {
			return new StringLiteralNode(value);
		}
		if ( token->getType() == TK_UNIT ) {
			return new UnitLiteralNode(value);
		}
		
		// If no type was determined, throw an error
		previousToken();
		throw error( "Expected a valid type, found " + token->toString() );
	}



	

	/**
	 * <FormalParam> ::= <Identifier> ':' <Type>
	 */
	ASTNode* parseFormalParam() {
		// Prepare the node
		ParamNode* formalParamNode = new ParamNode();

		// Parse the identifier
		formalParamNode->addChild( parseIdentifier() );

		Token* token = nextToken();

		// If not a colon, error
		if ( token->getType() != TK_COLON ) {
			previousToken();
			throw error( "Expected a colon, found " + token->toString() );
		}

		// Parse the type
		formalParamNode->addChild( parseType() );

		// Return the param node
		return formalParamNode;
	}



	/**
	 * <FormalParams> ::= <FormalParam> { ',' <FormalParam> }
	 */
	ASTNode* parseFormalParams() {
		ParamsNode* params = new ParamsNode();

		// Parse the first param
		params->addChild( parseFormalParam() );

		// If a comma was found, continue adding params
		while ( nextToken()->getType() == TK_COMMA ) {
			// Attempt to parse another formal param
			params->addChild( parseFormalParam() );
		}
		// Move back one token. If a comma was not found, we will need to
		// process this non-comma token as something else
		previousToken();

		// Return params node
		return params;
	}


	/**
	 * <FunctionDecl> ::= 'function' <Identifier> '(' [<FormalParams>] ')' ':' <Type> <Block>
	 */
	ASTNode* parseFunctionDecl() {
		// Prepare the node
		FuncDeclNode* node = new FuncDeclNode();

		// Check for 'function' keyword
		Token* token = nextToken();
		if ( token->getType() != TK_KEYWORD || token->getImage() != "function" ) {
			previousToken();
			throw error( "Expected a 'function' keyword, found " + token->toString() );
		}

		node->addChild( parseIdentifier() );

		// Check for an opening parenthesis
		token = nextToken();
		if ( token->getType() != TK_OPEN_PAREN ) {
			previousToken();
			throw error( "Expected an opening parenthesis, found " + token->toString() );
		}

		// Parse the params (OPTIONAL)
		ASTNode* params = new ParamsNode();
		try {
			params = parseFormalParams();
		} catch( ParseException &e ) {}
		// Add Params
		node->addChild( params );

		// Check for <TK_CLOSE_PAREN>
		token = nextToken();
		if ( token->getType() != TK_CLOSE_PAREN ) {
			previousToken();
			throw error( "Expected a closing parenthesis, found " + token->toString() );
		}

		// Check for <TK_COLON>
		token = nextToken();
		if ( token->getType() != TK_COLON ) {
			previousToken();
			throw error( "Expected a colon, found " + token->toString() );
		}

		// Parse a type
		node->addChild( parseType() );

		// Parse  block
		node->addChild( parseBlock() );

		return node;
	}







	/**
	 * <ActualParams> ::= <Expression> { ',' <Expression> }
	 */
	ASTNode* parseActualParams() {
		// Prepare the node
		ASTNode* node = new FuncParamsNode();

		// Parse an expression
		node->addChild( parseExpression() );
		
		// If a comma was found, continue adding params
		while ( nextToken()->getType() == TK_COMMA ) {
			node->addChild( parseExpression() );
		}
		// Move back one token. If a comma was not found, we will need to
		// process this non-comma token as something else
		previousToken();

		// Return the node
		return node;
	}



	/**
	 * <FunctionCall> ::= <Identifier> '(' [<ActualParams>] ')'
	 */
	ASTNode* parseFunctionCall() {
		// Prepare the node
		ASTNode* node = new FuncCallNode();

		// Parse the identifier (function name)
		node->addChild( parseIdentifier() );

		// Check for an opening parenthesis
		Token* token = nextToken();
		if ( token->getType() != TK_OPEN_PAREN ) {
			previousToken();
			throw error( "Expected an opening parenthesis, found " + token->toString() );
		}

		// Parse the params
		node->addChild( parseActualParams() );

		// Check for a closing parenthises
		token = nextToken();
		if ( token->getType() != TK_CLOSE_PAREN ) {
			previousToken();
			throw error( "Exptecred a closing parenthesis, found " + token->toString() );
		}

		// Return the node
		return  node;
	}




	/**
	 * <UnaryOp> ::= '+' | '-' | 'not'
	 */
	ASTNode* parseUnaryOperator() {
		Token* token = nextToken();

		// If the token is not a additive operator or a keyword, throw an error
		if ( token->getType() != TK_ADD_OP && token->getType() != TK_KEYWORD ) {
			previousToken();
			throw error( "Expected an additive operator or a 'not', found " + token->toString() );
		}
		// If the additive operator or the keyword is not a "+", "-" or "not", throw an error
		if ( token->getImage() != "+" && token->getImage() != "-" && token->getImage() != "not" ) {
			previousToken();
			throw error( "Invalid unary operator " + token->toString() );
		}

		// Return the node
		return new UnaryOpNode( token->getImage() );
	}


	/**
	 * <Unary> ::= <UnaryOp> <Expression>
	 */
	ASTNode* parseUnary() {
		// Prepare the node
		ASTNode* node = new UnaryNode();
		// Parse the unary operator
		node->addChild( parseUnaryOperator() );
		// Parse expression
		node->addChild( parseExpression() );

		// Return the node
		return node;
	}



	/**
	 * <TypeCase> ::= '(' <Type> ')' <Expression>
	 */
	ASTNode* parseTypeCast() {
		// Prepare the node
		ASTNode* node = new TypeCastNode();

		// Check for an opening parenthesis
		Token* token = nextToken();
		if ( token->getType() != TK_OPEN_PAREN ) {
			previousToken();
			throw error( "Expected an opening parenthesis, found " + token->toString() );
		}

		// Parse a type
		try {
			node->addChild( parseType() );
		} catch( ParseException &e ) {
			// Move back to the TK_OPEN_PAREN token
			previousToken();
			throw e;
		}

		// Check for a closing parenthesis
		token = nextToken();
		if ( token->getType() != TK_CLOSE_PAREN ) {
			previousToken();
			throw error( "Expected a closing parenthesis, found " + token->toString() );
		}

		// Parse an expression
		node->addChild( parseExpression() );

		// Return the node
		return node;
	}








	/**
	 * <Factor> ::= <Literal> | <Identifier> | <FunctionCall> | <TypeCast> | <SubExpression> | <Unary>
	 */
	ASTNode* parseFactor() {

		// Try parsing a literal
		try {
			return parseLiteral();
		} catch( ParseException &e ) {}

		// Try parsing an identifier
		try {
			return parseIdentifier();
		} catch( ParseException &e ) {}

		// Try parsing a function call
		try {
			return parseFunctionCall();
		} catch( ParseException &e ) {}
			
		// Try parsing a type cast
		try {
			return parseTypeCast();
		} catch( ParseException &e ) {}

		// Try parsing a subexpression
		try {
			return parseSubExpression();
		} catch( ParseException &e ) {}

		// Try parsing a unary
		try {
			return parseUnary();
		} catch( ParseException &e ) {}

		// Throw an error if non of the above returned a node
		throw error( "Expected a valid expression factor, found " + lexer->getToken()->toString() );
	}


	/**
	 * <Term> ::= <Factor> { <MultOp> <Factor> }
	 */
	ASTNode* parseTerm() {
		out("Parsing Term");

		// Parse a factor
		ASTNode* fact1 = parseFactor();

		try {
			// Parse the mult op
			ASTNode* opNode = parseMultOp();
			// Parse the second factor
			ASTNode* fact2 = parseTerm();
			// Add the factor to the operator node
			opNode->addChild( fact1 );
			opNode->addChild( fact2 );
			// Return the operator ndoe
			return opNode;
		}
		catch( ParseException &e ){
			// If no mult operator is present after the first factor,
			// Return the first factor
			return fact1;
		}
	}


	/**
	 * <SimpleExpression> ::= <Term> { <AddOp> <Term> }
	 */
	ASTNode* parseSimpleExpression() {
		out("Parsing Simple Expression");

		// Parse a term
		ASTNode* term1 = parseTerm();

		try {
			// Parse the additive op
			ASTNode* opNode = parseAddOp();
			// Parse the second term
			ASTNode* term2 = parseTerm();
			// Add the terms to the operator node
			opNode->addChild( term1 );
			opNode->addChild( term2 );
			// Return the operator ndoe
			return opNode;
		}
		catch( ParseException &e ){
			// If no additive operator is present after the first term,
			// Return the term
			return term1;
		}
	}


	/**
	 * <Expression> ::= <SimpleExpression> { <RelOp> <SimpleExpression> }
	 */
	ASTNode* parseExpression() {
		out("Parsing Expression");

		// Prepare the node
		ASTNode* node = new ExprNode();
		// Parse the first simple expression
		ASTNode* expr1 = parseSimpleExpression();

		try {
			// Parse the relational op
			ASTNode* opNode = parseRelOp();
			// Parse the second simple expression
			ASTNode* expr2 = parseSimpleExpression();
			// Add the expressions to the operator node, and the operator
			// node to the ExprNode
			opNode->addChild( expr1 );
			opNode->addChild( expr2 );
			node->addChild( opNode );
		}
		catch( ParseException &e ){
			// Ignore no relational operator is present after the first
			// simple expression
			node->addChild( expr1 );
		}

		// Return the node
		return node;
	}


	/**
	 * <SubExpression> ::= '(' <Expression> ')'
	 */
	ASTNode* parseSubExpression() {
		out("Parsing SubExpression");

		Token* token = nextToken();

		// Check for opening parenthesis
		if ( token->getType() != TK_OPEN_PAREN ) {
			previousToken();
			throw error( "Expected an opening parenthesis, found " + token->toString() );
		}

		// Attempt to parse as expression
		ASTNode* node = parseExpression();

		// Check for closing parenthesis
		token = nextToken();
		if ( token->getType() != TK_CLOSE_PAREN ) {
			previousToken();
			throw error( "Expected a closing parenthesis, found " + token->toString() );
		}

		// Return the node
		return node;
	}



	/**
	 * <AssignStatement> ::= 'set' <Identifier> '<-' <Expression>
	 */
	ASTNode* parseAssignStatement() {
		out("Parsing Assignment statement");

		// Prepare node
		ASTNode* node = new AssignNode();

		// Check for "set"
		Token* token = nextToken();
		if ( token->getType() != TK_KEYWORD || token->getImage() != "set" ) {
			previousToken();
			throw error( "Expected 'set' keyword, found " + token->toString() );
		}

		// Parse identifier
		node->addChild( parseIdentifier() );

		// Check for "<-"
		token = nextToken();
		if ( token->getType() != TK_ASSIGN_OP ) {
			previousToken();
			throw error( "Expected '<-' operator, found " + token->toString() );
		}

		// Parse Expression
		node->addChild( parseExpression() );

		// Check for ';'
		token = nextToken();
		if ( token->getType() != TK_SEMICOLON ) {
			previousToken();
			throw error( "Expected semicolon ';', found " + token->toString() );
		}

		return node;
	}



	/**
	 * <VariableDecl> ::= 'let' <Identifier> ':' <Type> '=' <Expression> ( ';' | ('in' <Block>) )
	 * @todo ('in' <Block>)
	 */
	ASTNode* parseVariableDecl() {
		out("Parsing Assignment statement");

		// Prepare the node
		ASTNode* node = new VariableDeclNode();

		// Check for "let"
		Token* token = nextToken();
		if ( token->getType() != TK_KEYWORD || token->getImage() != "let" ) {
			previousToken();
			throw error( "Expected 'let' keyword, found " + token->toString() );
		}

		// Parse Identifier
		node->addChild( parseIdentifier() );

		// Check for colon
		token = nextToken();
		if ( token->getType() != TK_COLON ) {
			previousToken();
			throw error( "Expected ':', found " + token->toString() );
		}

		// Parse type
		node->addChild( parseType() );

		// Check for equals
		token = nextToken();
		if ( token->getType() != TK_EQUALS_OP ) {
			previousToken();
			throw error( "Expected '=', found " + token->toString() );
		}

		// Parse Expression
		node->addChild( parseExpression() );

		// Check for ';'
		token = nextToken();
		if ( token->getType() != TK_SEMICOLON ) {
			// Check for 'in' keyword
			if ( token->getType() == TK_KEYWORD && token->getImage() == "in" ) {
				node->addChild( parseBlock() );
			}
			// If not ';' or 'in', error
			else {
				previousToken();
				throw error( "Expected semicolon ';', found " + token->toString() );
			}
		}

		// Return the node
		return node;
	}

	


	/**
	 * <IfStatement> ::= 'if' '(' <Expression ')' <Statement> [ 'else' <Statement> ]
	 */
	ASTNode* parseIfStatement() {
		out("Parsing If Statement");

		// Prepare node
		ASTNode* node = new IfNode();

		// Check for "if"
		Token* token = nextToken();
		if ( token->getType() != TK_KEYWORD || token->getImage() != "if" ) {
			previousToken();
			throw error( "Expected 'if' keyword, found " + token->toString() );
		}

		// Check for '('
		token = nextToken();
		if ( token->getType() != TK_OPEN_PAREN ) {
			previousToken();
			throw error( "Expected an opening parenthesis, found " + token->toString() );
		}

		// Parse expression
		node->addChild( parseExpression() );

		// Check for ')'
		token = nextToken();
		if ( token->getType() != TK_CLOSE_PAREN ) {
			previousToken();
			throw error( "Expected a closing parenthesis, found " + token->toString() );
		}

		// Parse statement
		node->addChild( parseStatement() );

		// Check for 'else'
		token = nextToken();
		if ( token->getType() == TK_KEYWORD && token->getImage() == "else" ) {
			node->addChild( parseStatement() );
		} else {
			previousToken();
		}

		// Return node
		return node;
	}



	/**
	 * <WhileStatement> ::= 'while' '(' <Expression> ')' <Statement>
	 */
	ASTNode* parseWhileStatement() {
		out("Parsing while statement");

		// Prepare the node
		ASTNode* node = new WhileNode();

		// Check for "while"
		Token* token = nextToken();
		if ( token->getType() != TK_KEYWORD || token->getImage() != "while" ) {
			previousToken();
			throw error( "Expected 'while' keyword, found " + token->toString() );
		}

		// Check for '('
		token = nextToken();
		if ( token->getType() != TK_OPEN_PAREN ) {
			previousToken();
			throw error( "Expected an opening parenthesis, found " + token->toString() );
		}

		// Parse expression
		node->addChild( parseExpression() );

		// Check for ')'
		token = nextToken();
		if ( token->getType() != TK_CLOSE_PAREN ) {
			previousToken();
			throw error( "Expected a closing parenthesis, found " + token->toString() );
		}

		// Parse statement
		node->addChild( parseStatement() );

		// Return the node
		return node;
	}



	ASTNode* parseBlock() {
		out("Parsing block");

		// Prepare node
		ASTNode* node = new BlockNode();

		// Check for '{'
		Token* token = nextToken();
		if ( token->getType() != TK_OPEN_BLOCK ) {
			previousToken();
			throw error( "Expected an opening brace, found " + token->toString() );
		}

		// Parse statements, until statements do not match
		do {
			try {
				node->addChild( parseStatement() );
			} catch ( ParseException &e ) {
				break;
			}
		} while( 1 == 1 );

		// Check for '}'
		token = nextToken();
		if ( token->getType() != TK_CLOSE_BLOCK ) {
			previousToken();
			throw error( "Expected a closing brace, found " + token->toString() );
		}

		// Return node
		return node;
	}


	/**
	 * <Statement> ::=	  <FunctionDecl>
	 *					| <Assignment>
	 *					| <Expression>
	 *					| <VariableDecl>
	 *					| <ReadStatement>
	 *					| <WriteStatement>
	 *					| <IfStatement>
	 *					| <WhileStatement>
	 *					| <HaltStatement>
	 *					| <Block>
	 */
	ASTNode* parseStatement() {
		out("Parsing Statement");
		
		// Try parsing a function declaration
		try {
			return parseFunctionDecl();
		} catch( ParseException &e ) {}

		// Try parsing an assignment statement
		try {
			return parseAssignStatement();
		} catch( ParseException &e ) {}

		// Try parsing an expression statement, followed by a ';'
		try {
			ASTNode* expr = parseExpression();
			// Check for semicolon
			Token* token = nextToken();
			if ( token->getType() == TK_SEMICOLON ) {
				return expr;
			}
			// If not a semicolon
			previousToken();
		} catch( ParseException &e ) {}

		// Try parsing a variable declaration
		try {
			return parseVariableDecl();
		} catch( ParseException &e ) {}

		// Try parsing a read statement
		try {
			return parseReadStatement();
		} catch( ParseException &e ) {}
			
		// Try parsing a write statement
		try {
			return parseWriteStatement();
		} catch( ParseException &e ) {}

		// Try parsing an if statement
		try {
			return parseIfStatement();
		} catch( ParseException &e ) {}

		// Try parsing a while statement
		try {
			return parseWhileStatement();
		} catch( ParseException &e ) {}

		// Try parsing a halt statement
		try {
			return parseHaltStatement();
		} catch( ParseException &e ) {}

		// Try parsing a block
		try {
			return parseBlock();
		} catch( ParseException &e ) {}

		// Throw an error if non of the above returned a node
		throw error( "Expected a statement, found " + lexer->getToken()->toString() );
	}




	/**
	 * <ReadStatement> ::= 'read' <Identifier> ';'
	 */
	ASTNode* parseReadStatement() {
		out("Parsing Read statement");

		// Prepare the node
		ASTNode* node = new ReadNode();

		// Check for "read"
		Token* token = nextToken();
		if ( token->getType() != TK_KEYWORD || token->getImage() != "read" ) {
			previousToken();
			throw error( "Expected 'read' keyword, found " + token->toString() );
		}

		// Parse Identifier
		node->addChild( parseIdentifier() );

		// Check for ';'
		token = nextToken();
		if ( token->getType() != TK_SEMICOLON ) {
			previousToken();
			throw error( "Expected semicolon ';', found " + token->toString() );
		}

		// Return node
		return node;
	}


	/**
	 * <ReadStatement> ::= 'write' <Identifier> ';'
	 */
	ASTNode* parseWriteStatement() {
		out("Parsing Write statement");

		// Prepare the node
		ASTNode* node = new WriteNode();

		// Check for "write"
		Token* token = nextToken();
		if ( token->getType() != TK_KEYWORD || token->getImage() != "write" ) {
			previousToken();
			throw error( "Expected 'write' keyword, found " + token->toString() );
		}

		// Parse Identifier
		node->addChild( parseIdentifier() );

		// Check for ';'
		token = nextToken();
		if ( token->getType() != TK_SEMICOLON ) {
			previousToken();
			throw error( "Expected semicolon ';', found " + token->toString() );
		}

		// Return node
		return node;
	}



	/**
	 * <HaltStatement> ::= 'halt' [ <Integer> | <Identifier>] ';'
	 */
	ASTNode* parseHaltStatement() {
		out("Parsing halt statement");

		// Prepare the node
		ASTNode* node = new HaltNode();

		// Check for "halt"
		Token* token = nextToken();
		if ( token->getType() != TK_KEYWORD || token->getImage() != "halt" ) {
			previousToken();
			throw error( "Expected 'halt' keyword, found " + token->toString() );
		}

		// Parse Identifier
		// Check for ';'
		token = nextToken();
		if ( token->getType() == TK_INTEGER ) {
			node->addChild( new IntegerLiteralNode( token->getImage() ) );
		}
		else {
			previousToken();
			try {
				node->addChild( parseIdentifier() );
			} catch( ParseException &e ) {
				previousToken();
				throw error( "Invalid exit code. Expected an integer literal or variable, found " + token->toString() );
			}
		}

		// Check for ';'
		token = nextToken();
		if ( token->getType() != TK_SEMICOLON ) {
			previousToken();
			throw error( "Expected semicolon ';', found " + token->toString() );
		}

		// Return node
		return node;
	}



	/**
	 * <Sxl> ::= { <Statement> }
	 */
	ASTNode* parseSXL() {
		out("Begin parsing SXL");

		// Prepare the node
		ASTNode* node = new SXLNode();

		Token* token;
		// Loop if next token is not EOF
		while ( ( token = nextToken() )->getType() != TK_EOF ) {
			// Token is not EOF - move back to allow parseStatement to process it
			previousToken();
			node->addChild( parseStatement() );
		}

		// Return the node
		return node;
	}


};


#endif