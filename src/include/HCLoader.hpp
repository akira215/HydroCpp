/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

#pragma once
// ===== External Includes ===== //
#include <string>
#include <vector>
#include <map>
// ===== HydroCpp Includes ===== //
#include "HCPoint.hpp"




namespace HydroCpp
{
    class HCLoader
    {
    public:
        /**
         * @brief constructor
         * @param filename of the excel filename containing the data
         * @param loc
         */
        HCLoader(const std::string& filename);

        /**
         * @brief
         */
        ~HCLoader();
    private:
        std::string m_filename;
        std::map<double,std::vector<HCPoint>> m_hull;
    };

}  // namespace std