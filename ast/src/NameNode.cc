#include <NameNode.h>

namespace avl {

    NameNode::NameNode():
        Node(NODE_NAMENODE)
    {
    }
    
    NameNode::NameNode(const std::shared_ptr<Node>& nd):
        Node(NODE_NAMENODE),
        node(nd)
    {
        loc = nd->loc;
    }
    
    NameNode::NameNode(const std::shared_ptr<Identifier>& nm, const std::shared_ptr<Node>& nd):
        Node(NODE_NAMENODE),
        name(nm),
        node(nd)
    {
        loc.start = nm->loc.start;
        loc.end = nd->loc.end;
    }

    NameNodeSet::NameNodeSet():
        Node(NODE_NAMENODESET)
    {
    }

}
