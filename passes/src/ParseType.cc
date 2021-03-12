#include <Parser.h>
#include <TokenID.h>
#include <UnknownTypeNode.h>
#include <VoidTypeNode.h>
#include <PrimitiveTypeNode.h>
#include <PointerTypeNode.h>
#include <ArrayTypeNode.h>
#include <StructTypeNode.h>
#include <UnionTypeNode.h>
#include <FunctionTypeNode.h>
#include <NameNodeSet.h>

namespace avl {

    bool Parser::parseType(std::size_t it) {

        if (parseDataType(it) ||
            parseFuncType(it))
        {
            return true;
        }

        return error();

    }

    bool Parser::parseDataType(std::size_t it) {

        if (parsePrimitiveType(it) ||
            parsePointerType(it) || 
            parseArrayType(it) || 
            parseStructType(it) || 
            parseUnionType(it))
        {
            return true;
        }

        return error();

    }

    bool Parser::parsePrimitiveType(std::size_t it) {

        if (parseToken(it, TOKEN_UINT8)   || parseToken(it, TOKEN_UINT16)  || parseToken(it, TOKEN_UINT32) || parseToken(it, TOKEN_UINT64) ||
            parseToken(it, TOKEN_INT8 )   || parseToken(it, TOKEN_INT16 )  || parseToken(it, TOKEN_INT32 ) || parseToken(it, TOKEN_INT64 ) ||
            parseToken(it, TOKEN_BOOL )   || parseToken(it, TOKEN_REAL32)  || parseToken(it, TOKEN_REAL64))
        {
            std::map<int, int> tymap = {
                {TOKEN_BOOL   , TYPE_BOOL  },
                {TOKEN_UINT8  , TYPE_UINT8 },
                {TOKEN_UINT16 , TYPE_UINT16},
                {TOKEN_UINT32 , TYPE_UINT32},
                {TOKEN_UINT64 , TYPE_UINT64},
                {TOKEN_INT8   , TYPE_INT8  },
                {TOKEN_INT16  , TYPE_INT16 },
                {TOKEN_INT32  , TYPE_INT32 },
                {TOKEN_INT64  , TYPE_INT64 },
                {TOKEN_REAL32 , TYPE_REAL32},
                {TOKEN_REAL64 , TYPE_REAL64}
            };

            result = std::make_shared<PrimitiveTypeNode>(tymap[tokens[it]->is]);
            result->loc = tokens[it]->loc;
            return success(1);
        }

        return error();
    }

    bool Parser::parsePointerType(std::size_t it) {

        std::size_t n = 0;

        std::shared_ptr<Node> pointee;

        if (parseToken(it, TOKEN_GENERIC_POINTER)) {
            pointee = std::make_shared<UnknownTypeNode>();
            result = std::make_shared<PointerTypeNode>(pointee);
            result->loc = tokens[it]->loc;
            return success(1);
        }

        if (!parseToken(it, TOKEN_POINTER)) {
            return error();
        }
        n++;

        if (parseToken(it+n, TOKEN_IDENT)) {
            pointee = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
            n++;
        }
        else if (parseType(it+n)) {
            pointee = result;
            n += nParsed;
        }
        else {
            return error(tokens[it+n], "Failed to parse pointee type");
        }

        auto loc = tokens[it]->loc;
        loc.end = pointee->loc.end;
        result = std::make_shared<PointerTypeNode>(pointee);
        result->loc = loc;
        return success(n);
    }

    bool Parser::parseArrayType(std::size_t it) {

        std::size_t n = 0;

        std::shared_ptr<Node> nelem;
        std::shared_ptr<Node> array_of;

        if (!parseToken(it, TOKEN_SQUARE_OPEN)) {
            return error();
        }
        n++;
        if (!parseExpr(it+n)) {
            return error(tokens[it+n], "Failed to parse array size after \'[\'");
        }
        n += nParsed;
        nelem = result;
        if (!parseToken(it+n, TOKEN_SQUARE_CLOSE)) {
            return error(tokens[it+n], "Expect \']\'");
        }
        n++;


        if (parseToken(it+n, TOKEN_IDENT)) {
            array_of = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
            n++;
        }
        else if (parseDataType(it+n)) {
            n += nParsed;
            array_of = result; 
        }
        else {
            return error(tokens[it+n], "Failed to parse array type");
        }

        auto loc = tokens[it]->loc;
        loc.end = array_of->loc.end;
        result = std::make_shared<ArrayTypeNode>(array_of, nelem);
        result->loc = loc;
        return success(n);
    }

    bool Parser::parseStructType(std::size_t it) {

        std::size_t n = 0;

        bool packed = false;

        if (!parseToken(it, TOKEN_STRUCT)) {
            return error();
        }
        n++;

        if (parseToken(it+n, TOKEN_PACKED)) {
            packed = true;
            n++;
        }

        if (!parseNameNodeSet(it+n)) {
            return error();
        }
        auto members = std::static_pointer_cast<NameNodeSet>(result);
        if (members->set.size() == 0) {  
            return error(tokens[it+n], "Empty struct is not allowed");
        }
        n += nParsed;

        auto loc = tokens[it]->loc;
        loc.end = members->loc.end;
        result = std::make_shared<StructTypeNode>(members, packed);
        result->loc = loc;
        return success(n);
    }

    bool Parser::parseUnionType(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_UNION)) {
            return error();
        }
        n++;

        if (!parseNameNodeSet(it+n)) {
            return error();
        }
        auto members = std::static_pointer_cast<NameNodeSet>(result);
        if (members->set.size() == 0) {  
            return error(tokens[it+n], "Empty union is not allowed");
        }
        n += nParsed;

        auto loc = tokens[it]->loc;
        loc.end = members->loc.end;
        result = std::make_shared<UnionTypeNode>(members);
        result->loc = loc;
        return success(n);
    }

    bool Parser::parseFuncType(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_FUNCTION)) {
            return error();
        }
        auto loc = tokens[it]->loc;
        n++;

        if (!parseNameNodeSet(it+n)) {
            return error();
        }
        auto args = std::static_pointer_cast<NameNodeSet>(result);
        for (std::size_t i = 0; i < args->set.size(); i++) {
            if (!args->set[i].name) {
                return error(args->set[i], "Unnamed function argument");
            }
        }
        n += nParsed;

        std::shared_ptr<Node> ret;
        if (!parseToken(it+n, TOKEN_RETURNS)) {
            ret = std::make_shared<VoidTypeNode>();
            loc.end = args->loc.end;
        }
        else {
            n++;
            if (parseDataType(it+n)) {
                n += nParsed;
                ret = result;
            }
            else if (parseToken(it+n, TOKEN_IDENT)) {
                ret = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
                n++;
            }
            else {
                return error(tokens[it+n], "Failed to parse function return type");
            }
            loc.end = ret->loc.end;
        }

        result = std::make_shared<FunctionTypeNode>(args, ret);
        result->loc = loc;
        return success(n);
    }

    bool Parser::parseNameNodeSet(std::size_t it) {

        std::size_t n = 0;
        auto nns = std::make_shared<NameNodeSet>();

        if (!parseToken(it, TOKEN_ROUND_OPEN)) {
            return error(tokens[it], "Expect \'(\' after " + tokens[it-1]->str);
        }
        n++;

        while (true) {
            std::shared_ptr<Identifier> name;
            std::shared_ptr<Node> type;
            if (parseToken(it+n, TOKEN_IDENT) && parseToken(it+n+1, ':') && parseToken(it+n+2, TOKEN_IDENT)) {
                name = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
                type = std::make_shared<Identifier>(tokens[it+n+2]->str, tokens[it+n+2]->loc);
                n += 3;
            }
            else if (parseToken(it+n, TOKEN_IDENT) && parseToken(it+n+1, ':') && parseDataType(it+n+2)) {
                name = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
                type = result;
                n += 2+nParsed;
            }
            else if (parseDataType(it+n)) {
                name = std::make_shared<Identifier>();
                type = result;
                n += nParsed;
            }
            else if (parseToken(it+n, TOKEN_IDENT)) {
                name = std::make_shared<Identifier>();
                type = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
                n++;
            }
            else {
                break;
            }

            nns->set.push_back(NameNode(name, type));
            if (parseToken(it+n, TOKEN_COMMA)) {
                n++;
            }
            else {
                break;
            }
        }

        if (parseToken(it+n, TOKEN_ROUND_CLOSE)) {
            auto loc = tokens[it]->loc;
            loc.end = tokens[it+n]->loc.end;
            result = nns;
            result->loc = loc;
            n++;
            return success(n);
        }
        if (nns->set.size() == 0) {
            return error(tokens[it+n], "Unexpected token \'" + tokens[it+n]->str + "\' after \'(\'");
        }
        return error(tokens[it+n], "Expect \',\' or \')\' after " + tokens[it+n-1]->str);
    }

}
