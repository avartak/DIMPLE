#ifndef ANALYZER_H
#define ANALYZER_H

#include <map>
#include <string>
#include <memory>

#include <Pass.h>
#include <Node.h>
#include <TypeNode.h>
#include <ExprNode.h>
#include <Initializer.h>
#include <BlockNode.h>
#include <GST.h>
#include <Value.h>
#include <Variable.h>
#include <Function.h>
#include <PrimitiveType.h>
#include <PointerType.h>
#include <ArrayType.h>
#include <StructType.h>
#include <UnionType.h>

namespace avl {

    struct Analyzer : public Pass<Entity> {

        std::shared_ptr<Function> currentFunction;

        Analyzer(const std::shared_ptr<AST>&, const std::shared_ptr<GST>&);

        virtual void fail() override; 
        virtual bool run() override;

        bool createRepresentations();
        bool createGlobals();

        bool getType(const std::shared_ptr<Node>&, bool includeOpaquePtr = false);
        bool getPtrType(const std::shared_ptr<PointerTypeNode>&, bool includeOpaquePtr);
        bool getArrayType(const std::shared_ptr<ArrayTypeNode>&, bool includeOpaquePtr);
        bool getStructType(const std::shared_ptr<StructTypeNode>&, bool includeOpaquePtr);
        bool getUnionType(const std::shared_ptr<UnionTypeNode>&, bool includeOpaquePtr);
        bool getFunctionType(const std::shared_ptr<FunctionTypeNode>&, bool includeOpaquePtr);
        bool useInArraySizeExpr(const std::shared_ptr<Node>&);

        bool getValue(const std::shared_ptr<Node>&);
        bool getConst(const std::shared_ptr<Identifier>&);
        bool getGlobalInstance(const std::shared_ptr<Identifier>&);
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
        bool initPrimitiveConst(const std::shared_ptr<PrimitiveType>&, const std::shared_ptr<Node>&);
        bool initPtrConst(const std::shared_ptr<PointerType>&, const std::shared_ptr<Node>&);
        bool initArrayConst(const std::shared_ptr<ArrayType>&, const std::shared_ptr<Node>&);
        bool initStructConst(const std::shared_ptr<StructType>&, const std::shared_ptr<Node>&);
        bool initUnionConst(const std::shared_ptr<UnionType>&, const std::shared_ptr<Node>&);
        bool getArrayTypeIndex(const std::shared_ptr<ArrayType>&, const InitElement&, std::size_t&);
        bool getStructTypeIndex(const std::shared_ptr<StructType>&, const InitElement&, std::size_t&);
        bool getUnionTypeIndex(const std::shared_ptr<UnionType>&, const InitElement&, std::size_t&);

        bool getFunction(const std::shared_ptr<Identifier>&, uint16_t, const std::shared_ptr<Type>&);
        bool defineCurrentFunction(const std::shared_ptr<DefineStatement>&);
        bool defineLocalVar(const std::shared_ptr<DefineStatement>&);
        bool defineBlock(const std::shared_ptr<BlockNode>&, std::shared_ptr<CodeBlock> = nullptr, std::shared_ptr<CodeBlock> = nullptr);
        bool defineIfBlock(const std::shared_ptr<IfBlockNode>&, std::shared_ptr<CodeBlock>, std::shared_ptr<CodeBlock>);
        bool defineLoopBlock(const std::shared_ptr<LoopBlockNode>&);
        bool call(const std::shared_ptr<CallExprNode>&, const std::shared_ptr<Variable>& = nullptr);
        bool ret(const std::shared_ptr<ReturnStatement>&);
    };

}

#endif
