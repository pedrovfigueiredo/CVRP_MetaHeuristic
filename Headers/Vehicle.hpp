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
    
    int currLoad = 0;
    std::vector<int> route;
};

#endif /* Vehicle_hpp */
