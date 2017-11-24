//
//  CVRP.cpp
//  ProjetoFinal
//
//  Created by Pedro Figueirêdo on 14/11/2017.
//  Copyright © 2017 UFPB. All rights reserved.
//

#include "../Headers/CVRP.hpp"

CVRP::CVRP(const std::string& filePath){
    std::vector<Node*> nodes;
    Solution* s;
    
    //Building nodes vector
    this->buildNodes(filePath, nodes);
    
    // Building adjMatrix
    this->buildAdjMatrix(nodes);
    
    s = new Solution(this->adjMatrix_, this->demands_, this->nVehicles_, this->capacity_, Solution::RelaxationLevel::None);
    
    std::cout << "Custo: " << s->getCost() << std::endl;
    s->printRoutes();
    
    s = VND(s);
    
    std::cout << "Custo: " << s->getCost() << std::endl;
    s->printRoutes();
    
}

CVRP::~CVRP(){}

Solution* CVRP::VND(Solution* solution){
    Solution newSolution = solution;
    int k = 1;
    while (k <= 2) {
        switch (k) {
            case 1:
                newSolution.descida1opt();
                break;
            case 2:
                newSolution.descida2opt();
                break;
        }
        
        if (newSolution.getCost() < solution->getCost()) {
            *solution = newSolution;
            k = 1;
        }else
            k += 1;
    }
    return solution;
}

void CVRP::buildAdjMatrix(const std::vector<Node*>& nodes){
    int n = (int) nodes.size();
    // Resizing adjMatrix
    this->adjMatrix_.resize(n);
    for (auto& column : this->adjMatrix_)
        column.resize(n);
    
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            this->adjMatrix_[i][j] = this->euclidianDistance(nodes[i], nodes[j]);
}

float CVRP::euclidianDistance(const Node* n1, const Node* n2){
    return std::sqrt(pow(n1->x_ - n2->x_, 2) + pow(n1->y_ - n2->y_, 2));
}

void CVRP::buildNodes(const std::string& filePath, std::vector<Node*>& nodes){
    std::ifstream file(filePath);
    
    if (file.is_open()) {
        std::string line, aux;
        int n = 0;
        
        // Saving nVehicles
        std::getline(file, line);
        auto position = std::find(line.begin(), line.end(), 'k');
        aux = line.substr(position - line.begin() + 1, line.end() - line.begin());
        this->nVehicles_ = std::stoi(aux);
        
        while (std::getline(file, line)) {
            if (line.find("DIMENSION") == std::string::npos)
                continue;
            position = std::find(line.begin(), line.end(), ':');
            aux = line.substr(position + 2 - line.begin(), line.end() - line.begin());
            n = std::stoi(aux);
            break;
        }
        
        while (std::getline(file, line)) {
            if (line.find("CAPACITY") == std::string::npos)
                continue;
            position = std::find(line.begin(), line.end(), ':');
            aux = line.substr(position + 2 - line.begin(), line.end() - line.begin());
            this->capacity_ = std::stoi(aux);
            break;
        }
        
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::stringstream test;
        int id,x,y,demand;
        
        for (int i = 0; i < n; i++) {
            std::getline(file, line);
            test << line;
            test >> id >> x >> y;
            nodes.push_back(new Node(id,x,y));
            test.clear();
        }
        
        this->demands_.resize(n);
        
        for (int i = -1; i < n; i++) {
            std::getline(file, line);
            if (line.find("DEMAND") != std::string::npos)
                continue;
            test << line;
            test >> id >> demand;
            this->demands_[id - 1] = demand;
            test.clear();
        }
        
    }else
        std::cerr << "Não foi possível abrir o arquivo." << std::endl;
    
    file.close();
}
