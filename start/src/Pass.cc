#include <Pass.h>
#include <Entity.h>

namespace avl {

    template<typename T>
    Pass<T>::Pass(InputManager* in, AST* tree, GST* sym):
        input(in),
        ast(tree),
        gst(sym)
    {
    }

    template<typename T>
    void Pass<T>::fail() {
    }

    template<typename T>
    bool Pass<T>::success() {
        errors.clear();
        return true;
    }

    template<typename T>
    bool Pass<T>::error() {
        fail();
        return false;
    }

    template<typename T>
    bool Pass<T>::error(const std::string& msg) {
        errors.push_back(Error(msg));
        return error();
    }

    template<typename T>
    bool Pass<T>::error(const Node& node, const std::string& msg) {
        errors.push_back(Error(node.loc, msg));
        return error();
    }

    template<typename T>
    bool Pass<T>::error(const Node* node, const std::string& msg) {
        if (node != nullptr) {
            errors.push_back(Error(node->loc, msg));
        }
        else {
            errors.push_back(Error(msg));
        }
        return error();
    }

    template<typename T>
    bool Pass<T>::error(const std::shared_ptr<Node>& node, const std::string& msg) {
        if (node) {
            errors.push_back(Error(node->loc, msg));
        }
        else {
            errors.push_back(Error(msg));
        }
        return error();
    }

    template<typename T>
    bool Pass<T>::hasErrors() const {
        return errors.size() > 0;
    }

    template<typename T>
    std::string Pass<T>::errorPrintout() {
        std::string err;
        for (std::size_t i = errors.size(); i > 0; i--) {
            err += errors[i-1].print(input);
        }
        return err;
    }

    template struct Pass<Node>;
    template struct Pass<Entity>;
    template struct Pass<void>;
}
