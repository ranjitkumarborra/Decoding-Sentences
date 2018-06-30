//
//  main.cpp
//  TheRecipe2015
//
//  Created by BVR on 26/02/15.
//  Copyright (c) 2015 Ranjit. All rights reserved.
//

#include <iostream>
#include "Context.h"
#include "UtilitiesAndExceptions.h"


template <typename T = double>

char *prepare_recipe ( string makeReipe )
{
    return prepare_recipe<T>(makeReipe.c_str());
}

template <typename T = double>
char *prepare_recipe ( const char*  makeRecipe )
{
    //best practise is to have calci as global obj, or calci to support smart pointer concept

    auto pObjCalci = Recipe::Calci<T>::GetInstance();
    Recipe::Context <double> objRC (makeRecipe);
    const char *pchRecipe = objRC.GetResult();
    char * pch  = new char[strlen(pchRecipe)];
    strcpy(pch, pchRecipe);
    delete pObjCalci;
    return pch;
}

int main(int argc, const char * argv[])
{
    auto pResult = prepare_recipe("1Yak\n5 Poster\n892.111 Electromagnetic\n1.8374568 Plastic\n Electromagnetic poster chop Plastic. Sprinkle yak");
    
    cout<< pResult<<endl;
    delete pResult;
    return 0;
}
