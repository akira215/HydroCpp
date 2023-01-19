/*
  HydroCpp
  Repository: https://github.com/akira215/HydroCpp
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/


#pragma once
// ===== External Includes ===== //
#include <string>
#include <stdexcept>
#include <iostream>

// ===== HydroCpp Includes ===== //

namespace HydroCpp
{
    class HCLogError
    {
    public:
        inline explicit HCLogError(const std::string& err) 
        {
            std::cerr << err << std::endl;
        };
    };

    class HCLogInfo
    {
    public:
        inline explicit HCLogInfo(const std::string& err) 
        {
            std::cout << err << std::endl;
        };
    };
}
