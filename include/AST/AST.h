#ifndef AST_H
#define AST_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <AST/NameNode.h>
#include <AST/Statement.h>
#include <AST/Identifier.h>

namespace dmp {

    struct AST {
        std::map<std::string, std::shared_ptr<NameNode> > representations;
        std::map<std::string, std::shared_ptr<NameNode> > declarations;
        std::map<std::string, std::shared_ptr<DefineStatement> > definitions;

        std::shared_ptr<Node> getNonSynonymRepNode(const std::shared_ptr<Identifier>&);
    };

}

#endif
