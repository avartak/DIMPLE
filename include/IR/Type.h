#ifndef TYPE_H
#define TYPE_H

#include <memory>
#include <string>
#include <llvm/IR/Type.h>
#include <Common/TypeBase.h>
#include <IR/Entity.h>

namespace dmp {

    struct Type : public Entity, public TypeBase {

        static std::vector<std::pair<const Type*, const Type*> > being_compared;

        std::string name;
        llvm::Type* llvm_type;
        bool complete;

        Type(uint16_t);
        Type(uint16_t, const std::string&);

        virtual ~Type() = default;

        virtual bool operator==(const Type&) const = 0;
        virtual bool operator!=(const Type&) const;
        
        virtual bool construct(const std::shared_ptr<Type>&) = 0;
        virtual std::shared_ptr<Type> clone() const = 0;

        std::size_t size() const;
        std::size_t alignment() const;
        bool moveDirectly() const;
        bool passDirectly() const;
        bool retDirectly() const;

        inline bool isComplete() const {
            return complete;
        }
        
    };

}

#endif
