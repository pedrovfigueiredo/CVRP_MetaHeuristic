//
//  Node.hpp
//  ProjetoFinal
//
//  Created by Pedro Figueirêdo on 14/11/2017.
//  Copyright © 2017 UFPB. All rights reserved.
//

#ifndef Node_hpp
#define Node_hpp

class Node{
public:
    Node(const int id, const int x, const int y );
    ~Node();
    
    int demand_;
    int id_;
    int x_;
    int y_;
};

#endif /* Node_hpp */
