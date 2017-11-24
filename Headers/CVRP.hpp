//
//  CVRP.hpp
//  ProjetoFinal
//
//  Created by Pedro Figueirêdo on 14/11/2017.
//  Copyright © 2017 UFPB. All rights reserved.
//

#ifndef CVRP_hpp
#define CVRP_hpp
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include "Node.hpp"
#include "Solution.hpp"

class CVRP {
public:
    CVRP(const std::string& filePath);
    ~CVRP();
    
    //Solution* VNS();
    
private:
    void buildNodes(const std::string& filePath, std::vector<Node*>& nodes);
    void buildAdjMatrix(const std::vector<Node*>& nodes);
    Solution* VND(Solution* solution);
    
    inline float euclidianDistance(const Node* n1, const Node* n2);
    
    unsigned nVehicles_;
    unsigned capacity_;
    std::vector<int> demands_;
    std::vector< std::vector<float> > adjMatrix_;
};

#endif /* CVRP_hpp */

