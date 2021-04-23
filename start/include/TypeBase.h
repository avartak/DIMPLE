#ifndef TYPEBASE_H
#define TYPEBASE_H

#include <cstdint>

namespace avl {

    enum Type_ID {

        TYPE_UNKNOWN,
        TYPE_VOID,
        TYPE_BOOL,
        TYPE_INT8,
        TYPE_INT16,
        TYPE_INT32,
        TYPE_INT64,
        TYPE_UINT8,
        TYPE_UINT16,
        TYPE_UINT32,
        TYPE_UINT64,
        TYPE_REAL32,
        TYPE_REAL64,
        TYPE_POINTER,
        TYPE_ARRAY,
        TYPE_STRUCT,
        TYPE_UNION,
        TYPE_FUNCTION

    };

    struct TypeBase {

        uint16_t is;
        uint16_t attr;

        TypeBase(uint16_t);

        virtual ~TypeBase() = default;

        inline bool isUnknown() const {
            return is == TYPE_UNKNOWN;
        }
        
        inline bool isVoid() const {
            return is == TYPE_VOID;
        }
        
        inline bool isArray() const {
            return is == TYPE_ARRAY;
        }
        
        inline bool isStruct() const {
            return is == TYPE_STRUCT;
        }
        
        inline bool isUnion() const {
            return is == TYPE_UNION;
        }
        
        inline bool isPtr() const {
            return is == TYPE_POINTER;
        }
        
        inline bool isPrimitive() const {
            return isInt() || isReal() || isBool();
        }
        
        inline bool isBool() const {
            return is == TYPE_BOOL;
        }
        
        inline bool isInt() const {
            return isSignedInt() || isUnsignedInt();
        }
        
        inline bool isSignedInt() const {
            return is == TYPE_INT8   || is == TYPE_INT16  || is == TYPE_INT32  || is == TYPE_INT64;
        }
        
        inline bool isUnsignedInt() const {
            return is == TYPE_UINT8  || is == TYPE_UINT16 || is == TYPE_UINT32 || is == TYPE_UINT64;
        }
        
        inline bool isMaxSizeInt() const {
            return is == TYPE_INT64 || is == TYPE_UINT64;
        }
        
        inline bool isReal() const {
            return is == TYPE_REAL32 || is == TYPE_REAL64;
        }
        
        inline bool isSigned() const {
            return is == TYPE_INT8 || is == TYPE_INT16  || is == TYPE_INT32  || is == TYPE_INT64 || is == TYPE_REAL32 || is == TYPE_REAL64;
        }
        
        inline bool isCompound() const {
            return isStruct() || isUnion() || isArray();
        }
        
        inline bool isFunction() const {
            return is == TYPE_FUNCTION;
        }
        
    };

}

#endif
