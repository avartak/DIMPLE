#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <map>
#include <string>
#include <memory>

#include <Common/Pass.h>
#include <AST/Node.h>
#include <AST/TypeNode.h>
#include <AST/ExprNode.h>
#include <AST/Initializer.h>
#include <AST/Statement.h>
#include <IR/Value.h>
#include <IR/Variable.h>
#include <IR/Function.h>
#include <IR/PrimitiveType.h>
#include <IR/PointerType.h>
#include <IR/ArrayType.h>
#include <IR/StructType.h>
#include <IR/UnionType.h>

namespace dmp {

    struct Translator : public Pass<Entity> {

        std::shared_ptr<Function> currentFunction;

        Translator(InputManager*, AST*, GST*);

        virtual void fail() override; 
        virtual bool run() override;

        bool createRepresentations();
        bool createGlobals();

        bool getType(const std::shared_ptr<Node>&, bool includeOpaquePtr = false);
        bool getTypeRep(const std::shared_ptr<Identifier>&, bool includeOpaquePtr = false);
        bool getPtrType(const std::shared_ptr<PointerTypeNode>&, bool includeOpaquePtr);
        bool getArrayType(const std::shared_ptr<ArrayTypeNode>&, bool includeOpaquePtr);
        bool getStructType(const std::shared_ptr<StructTypeNode>&, bool includeOpaquePtr);
        bool getUnionType(const std::shared_ptr<UnionTypeNode>&, bool includeOpaquePtr);
        bool getFunctionType(const std::shared_ptr<FunctionTypeNode>&, bool includeOpaquePtr);
        bool checkDuplicateNames(const std::shared_ptr<NameNodeSet>&);

        bool getValue(const std::shared_ptr<Node>&);
        bool getConstRep(const std::shared_ptr<Identifier>&);
        bool getGlobalInstance(const std::shared_ptr<Identifier>&);
        bool getGlobalRef(const std::shared_ptr<Identifier>&);
        bool literal(const std::shared_ptr<ExprNode>&);
        bool unary(const std::shared_ptr<UnaryExprNode>&);
        bool recast(const std::shared_ptr<BinaryExprNode>&);
        bool member(const std::shared_ptr<BinaryExprNode>&);
        bool element(const std::shared_ptr<BinaryExprNode>&);
        bool binary(const std::shared_ptr<ExprNode>&);
        bool assign(const std::shared_ptr<Variable>&, const std::shared_ptr<Node>&);

        bool getGlobalVar(const std::shared_ptr<Identifier>&, uint16_t, const std::shared_ptr<Type>&);
        bool initGlobal(const std::shared_ptr<Variable>&, const std::shared_ptr<Node>&);
        bool initLocal(const std::shared_ptr<Variable>&, const std::shared_ptr<Node>&);
        bool initLocalArray(const std::shared_ptr<Variable>&, const std::shared_ptr<Initializer>&);
        bool initLocalStruct(const std::shared_ptr<Variable>&, const std::shared_ptr<Initializer>&);
        bool initLocalUnion(const std::shared_ptr<Variable>&, const std::shared_ptr<Initializer>&);
        bool initConst(const std::shared_ptr<Type>&, const std::shared_ptr<Node>&);
        bool initSimpleConst(const std::shared_ptr<Type>&, const std::shared_ptr<Node>&);
        bool initArrayConst(const std::shared_ptr<ArrayType>&, const std::shared_ptr<Initializer>&);
        bool initStructConst(const std::shared_ptr<StructType>&, const std::shared_ptr<Initializer>&);
        bool initUnionConst(const std::shared_ptr<UnionType>&, const std::shared_ptr<Initializer>&);
        bool getArrayTypeIndex(const std::shared_ptr<ArrayType>&, const InitElement&, std::size_t&);
        bool getStructTypeIndex(const std::shared_ptr<StructType>&, const InitElement&, std::size_t&);
        bool getUnionTypeIndex(const std::shared_ptr<UnionType>&, const InitElement&, std::size_t&);

        bool getFunction(const std::shared_ptr<Identifier>&, uint16_t, const std::shared_ptr<Type>&);
        bool defineFunction(const std::shared_ptr<DefineStatement>&);
        bool defineLocalVar(const std::shared_ptr<DefineStatement>&);
        bool defineLocalRef(const std::shared_ptr<DefineStatement>&);
        bool defineBlock(const std::shared_ptr<BlockNode>&, std::shared_ptr<CodeBlock> = nullptr, std::shared_ptr<CodeBlock> = nullptr);
        bool defineIfBlock(const std::shared_ptr<BlockNode>&, std::shared_ptr<CodeBlock>, std::shared_ptr<CodeBlock>);
        bool defineLoopBlock(const std::shared_ptr<BlockNode>&);
        bool call(const std::shared_ptr<CallExprNode>&, const std::shared_ptr<Variable>& = nullptr);
        bool ret(const std::shared_ptr<ReturnStatement>&);
    };

}

#endif
