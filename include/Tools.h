#pragma once
#include <iostream>


class Tools {
public:
    static void printSuccess(){
        std::cout<<'1'<<std::endl;
    }
    static void printFailure(){
        std::cout<<'0'<<std::endl;
    }
};