//
//  main.cpp
//  ProjetoFinal
//
//  Created by Pedro Figueirêdo on 14/11/2017.
//  Copyright © 2017 UFPB. All rights reserved.
//

#include <iostream>
#include "../Headers/CVRP.hpp"

int main(int argc, const char * argv[]) {
    
    const std::string filePath = "/Users/PedroFigueiredo/Google Drive/5 Período/Análise e Projeto de Algoritmos/Projeto Final/ProjetoFinal/ProjetoFinal/Instances/P/P-n70-k10.vrp";
    
    CVRP cvrp(filePath);
    
    std::cout << "Custo da primeira solução: " << cvrp.solution_->getCost() << std::endl << std::endl;
    cvrp.solution_->printRoutes();
    
    cvrp.VNS(5000);
    
    std::cout << "Custo depois do VNS: " << cvrp.solution_->getCost() << std::endl << std::endl;
    cvrp.solution_->printRoutes();
    
    return 0;
}
