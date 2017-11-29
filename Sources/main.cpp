//
//  main.cpp
//  ProjetoFinal
//
//  Created by Pedro Figueirêdo on 14/11/2017.
//  Copyright © 2017 UFPB. All rights reserved.
//

#include <iostream>
#include <thread>
#include <vector>
#include "../Headers/CVRP.hpp"

void grasp_VNS(CVRP* cvrp, const std::size_t executionCount ,const float alpha){
    cvrp->buildFirstSolution(alpha);
    cvrp->VNS(executionCount);
}

int main(int argc, const char * argv[]) {
    
    const std::string filePath = "/Users/PedroFigueiredo/Google Drive/5 Período/Análise e Projeto de Algoritmos/Projeto Final/ProjetoFinal/ProjetoFinal/Instances/M/M-n200-k16.vrp";
    const std::size_t executionCount = 100000;
    std::vector<CVRP*> cvrps;
    std::vector<std::thread> vCVRP;
    Solution* bestSolution;
    
    for (int i = 0; i < 5; i++){
        cvrps.push_back(new CVRP(filePath));
        vCVRP.push_back(std::thread(grasp_VNS, cvrps[i] , executionCount , 0.1f  * (i + 1)));
    }
    
    for (auto& thread : vCVRP)
        thread.join();
    
    bestSolution = cvrps[0]->solution_;
    for (int i = 0; i < cvrps.size(); i++)
        if (cvrps[i]->solution_->getCost() < bestSolution->getCost())
            bestSolution = cvrps[i]->solution_;
    
    
    std::cout << "Best Solution" << std::endl;
    std::cout << "Cost: " << bestSolution->getCost() << std::endl;
    bestSolution->printRoutes();
    
    return 0;
}
