//
//  Context.h
//  TheRecipe2015
//
//  Created by BVR on 26/02/15.
//  Copyright (c) 2015 Ranjit. All rights reserved.
//

#ifndef TheRecipe2015_Context_h
#define TheRecipe2015_Context_h

#include <queue>
#include <map>
#include <set>
#include <sstream>
#include <cerrno>
#include <typeinfo>

#include "UtilitiesAndExceptions.h"

using namespace std;

namespace Recipe
{
    
    /****************************************************************
     
                            Utilities
     
     ****************************************************************/
    
    
#define FINAL_RESULT_LENGTH 300
    
    static inline string &ltrim(string &s)
    {
        s.erase(s.begin(),find_if_not(s.begin(),s.end(),[](int c){return isspace(c);}));
        return s;
    }
    
    static inline string &rtrim(string &s)
    {
        s.erase(find_if_not(s.rbegin(),s.rend(),[](int c){return isspace(c);}).base(), s.end());
        return s;
    }
    
    static inline string trim(const string &s)
    {
        string t=s;
        return ltrim(rtrim(t));
    }
    
    std::map<std::string, Action > mapAction =
    {
        {"MIX", Action::MIX_ADD},
        {"CHOP",Action::CHOP_SUB},
        {"STRAIN",Action::STRAIN_DIV},
        {"SPRINKLE",Action::SPRINKLE_MUL},
        {"KICK",Action::KICK_BIT_AND},
        {"SMASH",Action::SMASH_BIT_OR},
        {"DESTROY",Action::DESTROY_REMAINDER},
        {"DEFAULT",Action::DEFAULT}
    };
    
}


/************************************************************
 
            Context Class Declaration
 
 ************************************************************/

namespace Recipe
{
    template<typename T>
    class Context
    {
        
    private:
        T                   mTResult = numeric_limits<T>::max();
        map<string, T>      mmapIngredientsValue;
        map<string, T>      msetIngredientsUsed;
        std::stringstream   mstrStream;
        string              mpInputSring;
        string              mstrResult = "";
        
        inline bool         isIngridient(string &ingredient);
        inline bool         isAction(string &action);
        
        Calci<T>            *pCalci=nullptr;
        T                   StoArithemetic( string &toConvert, string::size_type &sz );
        static char         finalResult[FINAL_RESULT_LENGTH];
        
        void                Sentence(string &strToEvaluate );   // tries to evaluate multiple sentences in the string
        void                Evaluate(string &strToEvaluate );   // evaluates single sentence
        string              ConstructIngredients();             // builds map with the given ingredients
   
    public:
        Context(const char* inputString );
        inline const char*         GetResult();
    };
}

/***************************************************************
 
                    Context Definition
 
 ****************************************************************/

namespace Recipe
{
    
    template<typename T>
    char Context<T>::finalResult[FINAL_RESULT_LENGTH];
    
    template<typename T>
    Context<T>::Context( const char* inputString ): mpInputSring(inputString)
    {
        try
        {
            if( !is_arithmetic<T>::value)
            {
                throw RecipeException("context will only deal with Arithemetic Data types");
            }
            mstrStream.str(inputString);
            string temp = ConstructIngredients( );
            Sentence(temp);
        }
        catch(RecipeException &re)
        {
            strncpy( finalResult, re.what(), FINAL_RESULT_LENGTH );
        }
    }
    
    template<typename T>
    string Context<T>::ConstructIngredients()
    {
        string          temp;
        T               val;
        string::size_type sz;
        try
        {
            while ( std::getline( mstrStream, temp ) )
            {
                temp = ltrim(temp);
                if(std::isdigit( temp[0] ) )
                {
                    val = StoArithemetic(temp, sz);
                    
                    temp =temp.substr(sz);
                    temp =trim(temp);
                    std::transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
                    
                    if( mmapIngredientsValue.find(temp)== mmapIngredientsValue.end())
                        mmapIngredientsValue.insert( pair<string, T>( temp, val) );
                    else
                        throw RecipeException("Error::Ingredients shouldn't have duplicates...");
                }
            }
            
            if(! mmapIngredientsValue.size() )
                throw RecipeException("Error:: Invalid/No Ingredients found, Grammar is empty!!");
        }
        catch (RecipeException &re)
        {
            throw re;
        }
        return move(temp);
    }
    
    
    template<typename T>
    T Context<T>::StoArithemetic(string &toConvert, string::size_type &sz)
    {
        T               returnVal =0;
        
        try
        {
            auto tHash =    typeid (T).hash_code();
            
            if( tHash == typeid (double).hash_code())
            {
                returnVal = stod(toConvert, &sz);
                
            }
            else if( tHash == typeid (float).hash_code())
            {
                returnVal = stof(toConvert, &sz);
            }
            else if( tHash == typeid (int).hash_code())
            {
                returnVal = stoi(toConvert, &sz);
            }
            else if( tHash == typeid (long double).hash_code())
            {
                returnVal = stold(toConvert, &sz);
            }
            else if( tHash == typeid (long long).hash_code())
            {
                returnVal = stoll(toConvert, &sz);
            }
            else if( tHash == typeid (unsigned long).hash_code())
            {
                returnVal = stoul(toConvert, &sz);
            }
            else if( tHash == typeid (unsigned long long).hash_code())
            {
                returnVal = stoull(toConvert, &sz);
            }
            else
            {
                string str = "unsupported ingredient conversion data type";
                str  = str + typeid(T).name();
                throw RecipeException(str);
            }
        }
        catch(std::logic_error & e)
        {
            string str = "Invalid arument or out of range While performing Arithemetic conversion....";
            throw RecipeException( str+toConvert );
        }
        catch(RecipeException & e)
        {
            throw e;
        }

        return move(returnVal);
    }
    
    
    
    template<typename T>
    void Context<T>::Sentence( string &stringToEvaluate )
    {
        try
        {
            stringstream sentenceStream;
            sentenceStream.str( stringToEvaluate );
            string temp;
            pCalci = Calci<T>::GetInstance();
            while (std::getline( sentenceStream, temp, '.' ) )
            {
                Evaluate(temp);
            }
            
            mstrResult = (to_string(mTResult)+mstrResult);
            
            strncpy(finalResult, mstrResult.c_str(), FINAL_RESULT_LENGTH);
            
            if (msetIngredientsUsed.size() not_eq mmapIngredientsValue.size() )
            {
                map<string,T> itr;
                string temp = "all the ingredients are not used in recipe..\n";
                set_difference( mmapIngredientsValue.begin(), mmapIngredientsValue.end(),msetIngredientsUsed.begin(), msetIngredientsUsed.end(), std::inserter(itr, itr.begin()));
                for_each(itr.begin(), itr.end(), [&]( pair<string, double> p){ temp = temp+p.first+"\t";});
                throw RecipeException(temp);
            }
        }
        catch (RecipeException &re)
        {
            throw re;
        }
       
    }
    
    
    
    template<typename T>
    void Context<T>::Evaluate( string &strToEvaluate )
    {
        stringstream istream;
        string       temp;
        
        queue<T>    qOperands;
        Action      currentAction= Action::DEFAULT;
        int         mnCharPosition   = 0;
        std::transform(strToEvaluate.begin(), strToEvaluate.end(), strToEvaluate.begin(), ::toupper);
        istream.str(strToEvaluate);
        try
        {
            while (istream >> temp )
            {
                if( isAction(temp))
                {
                    currentAction = mapAction[temp];
                }
                
                else if( isIngridient(temp) )
                {
                    //Check for appending the required character....

                    if (temp.length() > mnCharPosition )
                    {
                        mstrResult = mstrResult + temp[mnCharPosition++];
                    }
                    else
                    {
                        stringstream  tempStream ;
                        tempStream << "Error::In sentence:" << strToEvaluate.substr(0, istream.peek()) << "\t"<< temp <<"has only"<<temp.size()<<" but"<<mnCharPosition<<"positional character is required to evaluate the expression";
                       throw RecipeException( tempStream.str() );
                    }
                    
                    if(qOperands.empty() and numeric_limits<T>::max() == mTResult)
                        mTResult = mmapIngredientsValue[temp];
                    else
                        qOperands.push(mmapIngredientsValue[temp]);
                    
                    msetIngredientsUsed.insert(pair<string,T>(temp,mmapIngredientsValue[temp] ));
                }
               
                else
                {
                    string str = "Error:Invalid word In the Sentence::" + temp;
                    throw RecipeException(str);
                }
                if (currentAction != Action::DEFAULT)
                {
                    while ( !qOperands.empty() )
                    {
                        mTResult = pCalci->Operate(mTResult, qOperands.front(), currentAction);
                        qOperands.pop();
                    }
                }
                
            }
            if (currentAction == Action::DEFAULT )
            {
                throw RecipeException("Error:No Action[operator] is present in the sentence");
            }
        }
        catch( RecipeException &re )
        {
            throw re;
        }
    }
    
    template<typename T>
    bool Context<T>::isAction( string &action )
    {
        return mapAction.end() not_eq mapAction.find(action);
    }
    
    template<typename T>
    bool Context<T>::isIngridient( string &ingredient )
    {
        return mmapIngredientsValue.end() not_eq mmapIngredientsValue.find(ingredient);
    }
    
    template<typename T>
    const char* Context<T>::GetResult()
    {
        return finalResult;
    }
    
}

#endif
