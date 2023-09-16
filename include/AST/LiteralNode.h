#ifndef LITERALNODE_H
#define LITERALNODE_H

#include <memory>
#include <string>
#include <AST/ExprNode.h>
#include <AST/Token.h>

namespace dmp {

    struct BoolNode : public ExprNode {

        bool literal;

        BoolNode(const std::shared_ptr<Token>&, bool);

        static std::shared_ptr<BoolNode> construct(const std::shared_ptr<Token>&);

    };

    struct IntNode : public ExprNode {

        uint64_t literal;
        std::string str;

        IntNode(const std::shared_ptr<Token>&, uint64_t);

        static std::shared_ptr<IntNode> construct(const std::shared_ptr<Token>&);

    };

    struct RealNode : public ExprNode {

        double literal;
        std::string str;

        RealNode(const std::shared_ptr<Token>&, double);

        static std::shared_ptr<RealNode> construct(const std::shared_ptr<Token>&);

    };

    struct CharNode : public ExprNode {

        char literal;
        std::string str;

        CharNode(const std::shared_ptr<Token>&, char);

        static std::shared_ptr<CharNode> construct(const std::shared_ptr<Token>&);

    };

    struct StringNode : public ExprNode {

        std::string literal;
        std::string str;

        StringNode(const std::shared_ptr<Token>&, const std::string&);

        static std::shared_ptr<StringNode> construct(const std::shared_ptr<Token>&);

    };

}

#endif
