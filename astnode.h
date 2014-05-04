#ifndef __AST_NODE_H__
#define __AST_NODE_H__

#define quote(x) #x

// INCLUSIONS
#include <vector>
#include <string>
#include <sstream>
#include <cxxabi.h>

// Namespace
using namespace std;

class ASTNode {
	protected:
		vector <ASTNode*> children;
		string name;
		string text;
	public:
		// Constructors
		ASTNode(string name): name(name) {}
		ASTNode(string name, string text): name(name), text(text) {}


		void addChild(ASTNode* node) {
			this->children.push_back(node);
		}

		string toString() {
			return this->toString("");
		}
		string toString(string prefix) {
			// Prepare the string
			stringstream ss;
			ss << prefix << "<" << this->name << ">";

			// If the node has no text, print a new line char
			if ( text == "" ) ss << "\n";

			// Print the text is present
			if ( text != "" ) {
				ss << text;
			}

			// Print the children
			for(vector<ASTNode*>::iterator it = children.begin(); it != children.end(); it++) {
				ss << (*it)->toString(prefix + "\t");
			}

			// Close the XML node
			if ( text == "" ) ss << prefix;
			ss << "</" << this->name << ">\n";

			// Return the string
			return ss.str();
		}
};



// TYPE NODE
class TypeNode : public ASTNode {
	public: TypeNode(string type) : ASTNode("Type", type) {}
};
// IDENTIFIER NODE
class IdentifierNode : public ASTNode {
	public: IdentifierNode(string iden) : ASTNode("Identifier", iden) {}
};


// FUNC DECL
class FuncDeclNode : public ASTNode {
	public: FuncDeclNode() : ASTNode("FunctionDecl"){}
};
// PARAM
class ParamNode : public ASTNode {
	public: ParamNode() : ASTNode("Param"){}
};
// PARAMS
class ParamsNode : public ASTNode {
	public: ParamsNode() : ASTNode("Params"){}
};





// EXPRESSION
class ExprNode : public ASTNode {
	public: ExprNode() : ASTNode("Expression"){}
};

// TYPE CAST
class TypeCastNode : public ASTNode {
	public: TypeCastNode() : ASTNode("Params") {}
};


// LITERALS
class IntegerLiteralNode : public ASTNode {
	public: IntegerLiteralNode(string value) : ASTNode("IntegerLiteral", value) {}
};
class RealLiteralNode : public ASTNode {
	public: RealLiteralNode(string value) : ASTNode("RealLiteral", value) {}
};
class CharLiteralNode : public ASTNode {
	public: CharLiteralNode(string value) : ASTNode("CharLiteral", value) {}
};
class StringLiteralNode : public ASTNode {
	public: StringLiteralNode(string value) : ASTNode("StringLiteral", value) {}
};
class BooleanLiteralNode : public ASTNode {
	public: BooleanLiteralNode(string value) : ASTNode("BooleanLiteral", value) {}
};
class UnitLiteralNode : public ASTNode {
	public: UnitLiteralNode(string value) : ASTNode("UnitLiteral", value) {}
};


// FUNCTION CALL
class FuncCallNode : public ASTNode {
	public: FuncCallNode() : ASTNode("FunctionCall") {}
};
class FuncParamsNode : public ASTNode {
	public: FuncParamsNode() : ASTNode("Params") {}
};



class UnaryNode : public ASTNode {
	public: UnaryNode() : ASTNode("Unary") {}
};
class UnaryOpNode : public ASTNode {
	public: UnaryOpNode(string op) : ASTNode("UnaryOp", op) {}
};



// ARITHMETIC OPERATOR NODES
class PlusNode : public ASTNode {
	public: PlusNode() : ASTNode("Add") {}
};
class MinusNode : public ASTNode {
	public: MinusNode() : ASTNode("Subt") {}
};
class MultiplyNode : public ASTNode {
	public: MultiplyNode() : ASTNode("Mult") {}
};
class DivideNode : public ASTNode {
	public: DivideNode() : ASTNode("Div") {}
};

// BINARY OPERATOR NODES
class OrNode : public ASTNode {
	public: OrNode() : ASTNode("Or") {}
};
class AndNode : public ASTNode {
	public: AndNode() : ASTNode("And") {}
};

// RELATIONAL OPERATOR NODES
class GreaterNode : public ASTNode {
	public: GreaterNode() : ASTNode("GreaterNode") {}
};
class LesserNode : public ASTNode {
	public: LesserNode() : ASTNode("LesserNode") {}
};
class EqualsNode : public ASTNode {
	public: EqualsNode() : ASTNode("EqualsNode") {}
};
class NotEqualsNode : public ASTNode {
	public: NotEqualsNode() : ASTNode("NotEqualsNode") {}
};
class GreaterEqualsNode : public ASTNode {
	public: GreaterEqualsNode() : ASTNode("GreaterEqualsNode") {}
};
class LesserEqualsNode : public ASTNode {
	public: LesserEqualsNode() : ASTNode("LesserEqualsNode") {}
};



// ASSINGMENT NODE
class AssignNode : public ASTNode {
	public: AssignNode() : ASTNode("Assignment") {}
};


// VARIABLE DECLARATION NODE
class VariableDeclNode : public ASTNode {
	public: VariableDeclNode() : ASTNode("VariableDecl") {}
};




// READ/WRITE NODE
class ReadNode : public ASTNode {
	public: ReadNode() : ASTNode("Read") {}
};
class WriteNode : public ASTNode {
	public: WriteNode() : ASTNode("Write") {}
};



// HALT NODE
class HaltNode : public ASTNode {
	public: HaltNode() : ASTNode("Halt") {}
};




// STATEMENT NODE
class StatementNode : public ASTNode {
	public: StatementNode() : ASTNode("Statement") {}
};


// IF NODE
class IfNode : public ASTNode {
	public: IfNode() : ASTNode("If") {}
};
class ThenNode : public ASTNode {
	public: ThenNode() : ASTNode("Then") {}
};
class ElseNode : public ASTNode {
	public: ElseNode() : ASTNode("Else") {}
};

// WHILE NODE
class WhileNode : public ASTNode {
	public: WhileNode() : ASTNode("While") {}
};

// BLOCK NODE
class BlockNode : public ASTNode {
	public: BlockNode() : ASTNode("Block") {}
};

// SXL NODE
class SXLNode : public ASTNode {
	public: SXLNode() : ASTNode("SXL") {}
};

#endif