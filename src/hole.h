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
    void addToPath(ofPath *_path);
    void update(float _os, float _r, float _t);
    void free();
    void lock();
    
    // call these from edge curve
    void mouseMoved(int _x, int _y );
    void mouseDragged(int _x, int _y, int _button);
    void mousePressed(int _x, int _y, int _button);
    void mouseReleased(int _x, int _y, int _button);
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
    bool mFreed;
};

#endif /* defined(__blobs__hole__) */
