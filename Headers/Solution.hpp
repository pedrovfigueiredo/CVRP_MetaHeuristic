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
    Solution(const Solution* s1);
    ~Solution();
    
    void descida1opt();
    void descida2opt();
    
    int getCost();
    void printRoutes();
private:
    void generateFirstSolution(RelaxationLevel level);
    
    
    // Neighborhood Moviments
    inline bool swapInRoute(Vehicle& vehicle);
    inline bool shiftNInRoute(Vehicle& vehicle, const int nElem);
    inline bool swapBetweenRoutes(Vehicle& v1, Vehicle& v2);
    inline bool dragAndDrop(Vehicle& v1, Vehicle& v2);
    inline bool twoOptSwap(Vehicle& vehicle);
    inline void twoOpt(std::vector<int>& newRoute, const std::vector<int>& route, const int i, const int j);
    
    // Constants
    const std::vector< std::vector<float> >& adjMatrix_;
    const std::vector<int>& demands_;
    const int nVehicles_;
    const int capacity_;
    
    // Variables
    std::vector< Vehicle > vehicles_;
    bool modifiedRoutes_;
    int currCost_;
};


#endif /* Solution_hpp */
