#include <AST/NameNode.h>

namespace dmp {

    NameNode::NameNode():
        Node(NODE_NAMENODE),
        attr(0)
    {
    }
    
    NameNode::NameNode(const std::shared_ptr<Node>& nd):
        Node(NODE_NAMENODE),
        node(nd),
        attr(0)
    {
        loc = nd->loc;
    }
    
    NameNode::NameNode(const std::shared_ptr<Node>& nd, uint64_t a):
        Node(NODE_NAMENODE),
        node(nd),
        attr(a)
    {
        loc = nd->loc;
    }
    
    NameNode::NameNode(const std::shared_ptr<Identifier>& nm, const std::shared_ptr<Node>& nd):
        Node(NODE_NAMENODE),
        name(nm),
        node(nd),
        attr(0)
    {
        loc = nm->loc;
        loc.end = nd->loc.end;
    }

    NameNode::NameNode(const std::shared_ptr<Identifier>& nm, const std::shared_ptr<Node>& nd, uint64_t a):
        Node(NODE_NAMENODE),
        name(nm),
        node(nd),
        attr(a)
    {
        loc = nm->loc;
        loc.end = nd->loc.end;
    }

    NameNodeSet::NameNodeSet():
        Node(NODE_NAMENODESET)
    {
    }

}
