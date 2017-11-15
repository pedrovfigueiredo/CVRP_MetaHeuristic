//
//  Solution.hpp
//  ProjetoFinal
//
//  Created by Pedro Figueirêdo on 14/11/2017.
//  Copyright © 2017 UFPB. All rights reserved.
//

#ifndef Solution_hpp
#define Solution_hpp
#include <vector>
#include <iostream>
#include "Vehicle.hpp"

class Solution{
public:
    Solution(const std::vector< std::vector<float> >& adjMatrix, const std::vector<int>& demands, const unsigned nVehicles, const unsigned capacity);
    ~Solution();
    
    int getCost();
    void printRoutes();
    void generateFirstSolution();
private:
    const std::vector< std::vector<float> >& adjMatrix_;
    const std::vector<int>& demands_;
    const unsigned nVehicles_;
    const unsigned capacity_;
    
    std::vector< Vehicle > vehicles_;
    bool modifiedRoutes_;
    int currCost_;
};


#endif /* Solution_hpp */
