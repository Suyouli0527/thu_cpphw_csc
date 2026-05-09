#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>

class Tools {
public:
    static void printSuccess(){
        std::cout<<'1'<<std::endl;
    }
    static void printFailure(){
        std::cout<<'0'<<std::endl;
    }
    static std::string formatAmount(double amount){
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << amount;
        return oss.str();
    }
};