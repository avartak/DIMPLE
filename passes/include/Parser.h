#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <memory>

#include <Pass.h>
#include <Token.h>
#include <AST.h>
#include <Identifier.h>
#include <Statement.h>
#include <BlockNode.h>

namespace avl {

    struct Parser : public Pass<Node> {

        std::vector<std::shared_ptr<Token> > tokens;
        std::size_t nParsed;

        Parser(const std::shared_ptr<AST>&);
   
        virtual void fail() override; 
        virtual bool run() override;

        bool success(std::size_t n);

        std::shared_ptr<Token> scan();

        bool isUnaryOp(std::size_t);
        bool isBinaryMathOp(std::size_t);
        bool isAssigner(std::size_t);
        bool isLiteral(std::size_t);
        bool isDefined(std::size_t);

        bool parseToken(std::size_t, int);
        bool parseTerm(std::size_t, bool);

        bool parseProg(std::size_t);
        bool parseEmpty(std::size_t);
        bool parseInclude(std::size_t);
        bool parseRepresentation(std::size_t);
        bool parseDeclaration(std::size_t);
        bool parseDefinition(std::size_t);
        bool parseStart(std::size_t);

        bool parseType(std::size_t);
        bool parseDataType(std::size_t);
        bool parsePrimitiveType(std::size_t);
        bool parsePointerType(std::size_t);
        bool parseArrayType(std::size_t);
        bool parseStructType(std::size_t);
        bool parseUnionType(std::size_t);
        bool parseFuncType(std::size_t);
        bool parseNameNodeSet(std::size_t);

        bool parseInit(std::size_t);
        bool parseRvalue(std::size_t);
        bool parseUntaggedInitSet(std::size_t);
        bool parseTaggedInitSet(std::size_t);

        bool parseExpr(std::size_t);
        bool parseUnary(std::size_t);
        bool parseLiteral(std::size_t);
        bool parsePreOpUnary(std::size_t);
        bool parsePostOpUnary(std::size_t);
        bool parseBinaryOperationRHS(std::size_t, int, const std::shared_ptr<Node>&);

        bool parseFunc(std::size_t);
        bool parseStatement(std::size_t, const std::shared_ptr<BlockNode>&);
        bool parseBlock(std::size_t, const std::shared_ptr<BlockNode>&);
        bool parseLocalVarDef(std::size_t, const std::shared_ptr<BlockNode>&);
        bool parseIf(std::size_t, const std::shared_ptr<BlockNode>&);
        bool parseLoop(std::size_t, const std::shared_ptr<BlockNode>&);

    };

}

#endif
