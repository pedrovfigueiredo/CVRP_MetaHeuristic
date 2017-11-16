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
#include <random>
#include "Vehicle.hpp"

class Solution{
public:
    // Defines how the first solution is created
    enum RelaxationLevel{
        None, Total
    };
    
    Solution(const std::vector< std::vector<float> >& adjMatrix, const std::vector<int>& demands, const unsigned nVehicles, const unsigned capacity, RelaxationLevel level);
    ~Solution();
    
    int getCost();
    void printRoutes();
private:
    void generateFirstSolution(RelaxationLevel level);
    
    // Neighborhood Moviments
    inline bool swapOneInRoute(Vehicle& vehicle);
    inline bool shiftNInRoute(Vehicle& vehicle, const int nElem);
    inline bool swapBetweenRoutes();
    
    // Constants
    const std::vector< std::vector<float> >& adjMatrix_;
    const std::vector<int>& demands_;
    const unsigned nVehicles_;
    const unsigned capacity_;
    
    // Variables
    std::vector< Vehicle > vehicles_;
    bool modifiedRoutes_;
    int currCost_;
};


#endif /* Solution_hpp */
