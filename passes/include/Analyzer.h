#ifndef ANALYZER_H
#define ANALYZER_H

#include <map>
#include <string>
#include <memory>

#include <Pass.h>
#include <Node.h>
#include <Type.h>
#include <TypeNode.h>
#include <ExprNode.h>
#include <Value.h>
#include <Variable.h>
#include <Function.h>

namespace avl {

    struct Analyzer : public Pass<Entity> {

        std::map<std::string, std::shared_ptr<Type> > types;
        std::map<std::string, std::shared_ptr<Value> > constants;
        std::map<std::string, std::shared_ptr<Variable> > variables;
        std::map<std::string, std::shared_ptr<Function> > functions;
        std::shared_ptr<Function> currentFunction;

        Analyzer(const std::shared_ptr<AST>&);

        virtual void fail() override; 
        virtual bool run() override;

        bool createTypeReps();
        bool createConstReps();
        bool createVariables();
        bool createFunctions();

        bool getType(const std::shared_ptr<Node>&, bool includeOpaquePtr);
        bool getPtrType(const std::shared_ptr<PointerTypeNode>&, bool includeOpaquePtr);
        bool getArrayType(const std::shared_ptr<ArrayTypeNode>&, bool includeOpaquePtr);
        bool getStructType(const std::shared_ptr<StructTypeNode>&, bool includeOpaquePtr);
        bool getUnionType(const std::shared_ptr<UnionTypeNode>&, bool includeOpaquePtr);
        bool getFunctionType(const std::shared_ptr<FunctionTypeNode>&, bool includeOpaquePtr);

        bool getValue(const std::shared_ptr<Node>&);
        bool getLiteral(const std::shared_ptr<ExprNode>&);
        bool getAssignExpr(const std::shared_ptr<AssignExprNode>&);
        bool getCallExpr(const std::shared_ptr<CallExprNode>&);
        bool getUnaryExpr(const std::shared_ptr<UnaryExprNode>&);
        bool getBinaryExpr(const std::shared_ptr<BinaryExprNode>&);

        bool getGlobalVar(const std::shared_ptr<Identifier>&);
        bool getFunction(const std::shared_ptr<Identifier>&);
    };

}

#endif
