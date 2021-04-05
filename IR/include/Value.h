#ifndef VALUE_H
#define VALUE_H

#include <map>
#include <Entity.h>
#include <Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>

namespace avl {

    enum Value_ID {

        VALUE_INT,
        VALUE_BOOL,
        VALUE_CHAR,
        VALUE_REAL,
        VALUE_STRING,
        VALUE_VAL,
        VALUE_VAR,
        VALUE_FUNC,

    };

    struct Value : public Entity {

        uint16_t is;
        std::shared_ptr<Type> type;
        llvm::Value* llvm_value;

        Value(uint16_t, const std::shared_ptr<Type>&, llvm::Value*);
        Value(const std::shared_ptr<Type>&, llvm::Value*);

        virtual ~Value() = default;

        virtual llvm::Value* val() const;

        void setType(const std::shared_ptr<Type>&);

        inline bool isLiteralInt() const {
            return is == VALUE_INT;
        }

        inline bool isLiteralBool() const {
            return is == VALUE_BOOL;
        }

        inline bool isLiteralChar() const {
            return is == VALUE_CHAR;
        }

        inline bool isLiteralReal() const {
            return is == VALUE_REAL;
        }

        inline bool isLiteralString() const {
            return is == VALUE_STRING;
        }

        inline bool isLiteral() const {
            return (isLiteralInt()  || isLiteralBool() || 
                    isLiteralChar() || isLiteralReal() ||
                    isLiteralString());
        }

        inline bool isConst() const {
            return (llvm_value != nullptr) && llvm::isa<llvm::Constant>(llvm_value);
        }

        inline bool isConstInt() const {
            return isConst() && type->isInt();
        }

        inline bool isConstBool() const {
            return isConst() && type->isBool();
        }

        inline bool isConstNonNegativeInt() const {
            return isConstInt() && !llvm::cast<llvm::ConstantInt>(llvm_value)->isNegative();
        }

        inline bool isConstNoRelocation() const {
            return isConst() && !llvm::cast<llvm::Constant>(llvm_value)->needsRelocation();
        }

        inline bool isVar() const {
            return is == VALUE_VAR;
        }

        inline bool isFunction() const {
            return is == VALUE_FUNC;
        }

        inline bool isInstance() const {
            return isVar() || isFunction();
        }

        inline uint64_t getUInt64ValueOrZero() const {
            return ( (isConstInt() || isConstBool()) ? llvm::cast<llvm::ConstantInt>(llvm_value)->getZExtValue() : 0);
        }

        inline int64_t getInt64ValueOrZero() const {
            return ( (isConstInt() || isConstBool()) ? llvm::cast<llvm::ConstantInt>(llvm_value)->getSExtValue() : 0);
        }

    };

}

#endif
