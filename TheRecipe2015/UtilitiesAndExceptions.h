//
//  UtilitiesAndExceptions.h
//  TheRecipe2015
//
//  Created by BVR on 26/02/15.
//  Copyright (c) 2015 Ranjit. All rights reserved.
//

#ifndef TheRecipe2015_UtilitiesAndExceptions_h
#define TheRecipe2015_UtilitiesAndExceptions_h

#include <exception>
#include <string>
#include <iostream>
#include <cmath>
#include <typeinfo>

using namespace std;

namespace Recipe
{
    enum class Action : int {
        MIX_ADD,
        CHOP_SUB,
        STRAIN_DIV,
        SPRINKLE_MUL,
        KICK_BIT_AND,
        SMASH_BIT_OR,
        DESTROY_REMAINDER,
        DEFAULT
    };
    
    class RecipeException : public exception
    {
    public:
        RecipeException( string msg )
        {
            mMessageOnException = msg;
        }
        
        const char * what () const throw ()
        {
            return mMessageOnException.c_str();
        }
        void DisplayMsgAndExit()
        {
            cerr<<what();
            exit(1);
        }
        
    private:
        string mMessageOnException;
    };
    
    //inteface for all terminal expressions
    
    template<typename T>
    class TerminalExpression
    {
        
    public:
        T virtual Interpret(T &a, T &b)=0;
        
        T round( T value )
        {
            return floor( value*100000 + 0.5 )/100000;
        }
        
        virtual ~TerminalExpression () { }
    };
    
    //Addition
    template<typename T>
    class Addition: public TerminalExpression<T>
    {
        
    public:
        T Interpret(T &a, T &b) override
        {
            T result = a+b;
            try
            {
                if (a > 0 && b > numeric_limits<T>::max() - a)
                {
                    
                    if( result < a || result < b)
                    {
                        string str= "Error:: overflow while performing addition on" ;
                        str = str + to_string(b);
                        throw RecipeException(str);
                    }
                }
                else if (a < 0 && b < numeric_limits<T>::min() - a)
                {
                    if( result > a || result > b)
                    {
                        string str= "Error:: underflow while performing addition on" ;
                        str = str + to_string(b);
                        throw RecipeException(str);
                    }
                }
            }
            catch (RecipeException &re)
            {
                throw re;
            }
            
            return result;
            
        }
    };
    //Substraction
    template<typename T>
    class Substraction: public TerminalExpression<T>
    {
        
    public:
        T Interpret(T &a, T &b) override
        {
            try
            {
                if( (a<0 && b>0) || (a>0 && b<0) )
                {
                    T compb = -b;
                    return Addition<T>().Interpret(a, compb);
                }
            }
            catch(RecipeException &re)
            {
                throw re;
            }
            return a-b;
        }
    };
    
    //Multiplication
    template<typename T>
    class Multiplication: public TerminalExpression<T>
    {
        
    public:
        //how about "Interpret(T &a,T &b)-> decltype(a*b)" ?
        //best approach to declare this type of functions auto Interpret(T &a,T &b)-> decltype(a*b)
        // but as we are overriding base class interpret method this wont be a viable approach
        
        T Interpret(T &a, T &b) override
        {
            return a*b;             //overflow and underflow conditions not handled ....
        }
    };
    
    
    //Divison
    template<typename T>
    class Divison: public TerminalExpression<T>
    {
        
    public:
        T Interpret(T &a, T &b) override
        {
            decltype(a/b) result = a/b;
            try
            {
                if( !b  )
                    throw RecipeException( "Error::division with 0 is Illegal.. " );
                    
            }
        
            catch(std::logic_error & e)
            {
                throw RecipeException("Error::Invalid argument or out of range While performing Arithemetic operation....");
            }
            catch(RecipeException & e)
            {
                throw e;
            }
            
            return result;     //overflow and underflow conditions not handled ....[if b is less than zero]
        }
    };
    
    //Modulo
    template<typename T>
    class Modulo: public TerminalExpression<T>
    {
        
    public:
        
        T Interpret(T &a, T &b) override
        {
            try
            {
                if( !b )
                    throw RecipeException( "Error::division with 0 is Illegal.. " );
            }
            catch (RecipeException &re)
            {
                throw re;
            }
            
            return remainder(a,b);
        }
    };
    
    //Bitwise Or
    template<typename T>
    class BitWiseOR: public TerminalExpression<T>
    {
        
    public:
        T Interpret(T &a, T &b)
        {
            T result;
            if(is_integral<T>::value)
                result = (long long)a | (long long)b;
            else
            {
                constexpr unsigned u = sizeof(T);
                unsigned char buffer[u];
                const unsigned char *pa = reinterpret_cast<unsigned char *>(&a);
                const unsigned char *pb = reinterpret_cast<unsigned char *>(&b);
                for( auto i = 0; i<u; i++ )
                    buffer[i] = pa[i] | pb[i];
                
                result = *(reinterpret_cast<T*>(buffer));
            }
            return result;
        }
        
    };
    //Bitwise And
    template<typename T>
    class BitWiseAND: public TerminalExpression<T>
    {
        
    public:
        T Interpret(T &a, T &b)
        {
            T result;
            if(is_integral<T>::value)
                result = (long long)a & (long long)b;
            else
            {
                constexpr unsigned u = sizeof(T);
                unsigned char buffer[u];
                const char *pa = reinterpret_cast< char *>(&a);
                const char *pb = reinterpret_cast< char *>(&b);
                for( auto i = 0; i<u; i++ )
                    buffer[i] = pa[i] & pb[i];
                
                 result = *(reinterpret_cast<T*>(buffer));
            }
            return result;
        }
        
    };
    //Bitwise Xor
    template<typename T>
    class BitWiseXOR: public TerminalExpression<T>
    {
        
    public:
        T Interpret(T &a, T &b)
        {
            T result;
            if(is_integral<T>::value)
                result = (long long)a ^ (long long)b;
            else
            {
                constexpr unsigned u = sizeof(T);
                unsigned char buffer[u];
                const unsigned char *pa = reinterpret_cast<unsigned char *>(&a);
                const unsigned char *pb = reinterpret_cast<unsigned char *>(&b);
                for( auto i = 0; i<u; i++ )
                    buffer[i] = pa[i]^pb[i];
                
                result = *(reinterpret_cast<T*>(buffer));
            }
            return result;
        }
        
    };
    
    //container of all operators
    template<typename T>
    class Calci
    {
    public:
        TerminalExpression<T> *pAdd =nullptr, *pSub = nullptr, *pMul = nullptr, *pDiv = nullptr, *pMod = nullptr;
        TerminalExpression<T> *pOr = nullptr, *pAnd = nullptr, *pXor = nullptr;
        static Calci<T>* GetInstance( );
        ~ Calci();
        T Operate( T a, T b, Action action);
    private:
        static Calci<T> * ptrObjCalci;
        Calci();
        
    };

   }



/************************************************
        IMPLEMENTATION OF RECIPE
 ************************************************/

namespace  Recipe
{
    
    
    template<typename T>
    Recipe::Calci<T>* Recipe::Calci<T>::ptrObjCalci = nullptr;
    
    template<typename T>
    Calci<T>* Calci<T>::GetInstance()
    {
        if (ptrObjCalci == nullptr )
        {
            ptrObjCalci = new Calci<T>();
        }
        return ptrObjCalci;
    }
    
    template<typename T>
    Calci<T>::Calci( )
    {
        pAdd    = new Addition<T>;
        pSub    = new Substraction<T>;
        pMul    = new Multiplication<T>;
        pDiv    = new Divison<T>;
        pMod    = new Modulo<T>;
        pOr     = new BitWiseOR<T>;
        pAnd    = new BitWiseAND<T>;
        pXor    = new BitWiseXOR<T>;
    }
    
    template<typename T>
    Calci<T>::~Calci()
    {
        delete pAdd;        //no need of virtual destructors as we are not allocating any data members
        delete pSub;
        delete pDiv;
        delete pMod;
        delete pMul;
        delete pXor;
        delete pOr;
        delete pAnd;
    }
    
    template<typename T>
    T Calci<T>::Operate( T a, T b, Action action)
    {
        T result = 0.0f;
        
        switch (action)
        {
            case Action::MIX_ADD:
                result = pAdd->Interpret(a, b);
                break;
                
            case Action::CHOP_SUB:
                result = pSub->Interpret(a, b);
                break;
                
            case Action::STRAIN_DIV:
                result = pDiv->Interpret(a, b);
                break;
                
            case Action::SPRINKLE_MUL:
                result = pMul->Interpret(a, b);
                break;
                
            case Action::KICK_BIT_AND:
                result =  pAnd->Interpret(a, b);
                break;
                
            case Action::SMASH_BIT_OR:
                result =  pOr->Interpret(a, b);
                break;
                
            case Action::DESTROY_REMAINDER:
                result = pMod->Interpret(a, b);
                break;
                
            case Action::DEFAULT:
                break;
            default:
                std::cerr<<"Error:Invalind Operand";
        }
        return result;
    }
    
}

#endif
