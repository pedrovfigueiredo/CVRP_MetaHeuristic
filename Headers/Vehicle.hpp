//
//  Vehicle.hpp
//  ProjetoFinal
//
//  Created by Pedro Figueirêdo on 15/11/2017.
//  Copyright © 2017 UFPB. All rights reserved.
//

#ifndef Vehicle_hpp
#define Vehicle_hpp
#include <vector>

struct Vehicle{
    Vehicle(){};
    ~Vehicle(){};
    
    float getCost(const std::vector< std::vector<float> >& adjMatrix){
        float solution = 0;
        int i = 0;
        for (auto city : this->route){
            solution += adjMatrix[i][city];
            i = city;
        }
        solution += adjMatrix[i][0];
        return  solution;
    }
    int currLoad = 0;
    std::vector<int> route;
};

#endif /* Vehicle_hpp */
