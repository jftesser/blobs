//
//  hole.h
//  blobs
//
//  Created by Jenna Fizel on 2/23/14.
//
//

#ifndef __blobs__hole__
#define __blobs__hole__

#include <iostream>
#include "ofMain.h"
class edgeCurve;
struct draggableVertex;

class hole {
public:
    hole(float _os, float _r, float _t, edgeCurve *_edge, ofPath *_addpath);
    ~hole();
    
    void draw();
    void addToPath();
    void update(float _os, float _r, float _t);
private:
    vector <draggableVertex *> mVerts;
    ofPath *mPath;
    ofPath *mAddPath;
    edgeCurve *mEdge;
    float mT;
    float mOS;
    float mR;
    ofPoint mEdgePt;
    ofVec3f mEdgeNormal;
};

#endif /* defined(__blobs__hole__) */
