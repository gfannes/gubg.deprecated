#ifndef HEADER_meta_parsing_Composite_hpp_ALREADY_INCLUDED
#define HEADER_meta_parsing_Composite_hpp_ALREADY_INCLUDED

#include "parsing/Component.hpp"
#include "parsing/Token.hpp"
#include <string>

namespace meta
{
    //A Composite consisting of _only_ Tokens
    struct TokenComposite: Component
    {
        std::list<Token::Ptr> childs_;
        void add(Token::Ptr token){childs_.push_back(token);}
        virtual std::string toString() const;
    };
    //A Composite consists of multiple Components
    struct Composite: Component
    {
        Components childs_;
        void add(Component::Ptr component){childs_.push_back(component);}
        virtual std::string toString() const;
    };

    //The static method construct() tries to create a TokenComposite of a specific type.
    //If this fails, nullptr is returned.
    struct Comment: TokenComposite
    {
        typedef std::shared_ptr<Comment> Ptr;
        static Comment::Ptr construct(TokenRange &);
    };
    struct Include: TokenComposite
    {
        typedef std::shared_ptr<Include> Ptr;
        static Include::Ptr construct(TokenRange &);
    };
    struct Define: TokenComposite
    {
        typedef std::shared_ptr<Define> Ptr;
        static Define::Ptr construct(TokenRange &);
    };
    struct String: TokenComposite
    {
        typedef std::shared_ptr<String> Ptr;
        static String::Ptr construct(TokenRange &);
    };
    struct Character: TokenComposite
    {
        typedef std::shared_ptr<Character> Ptr;
        static Character::Ptr construct(TokenRange &);
    };

    struct Namespace: Composite
    {
        typedef std::shared_ptr<Namespace> Ptr;
        static Namespace::Ptr construct(ComponentRange &);

        std::string name_;
    };
}

#endif
