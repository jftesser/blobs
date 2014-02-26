//
//  hole.cpp
//  blobs
//
//  Created by Jenna Fizel on 2/23/14.
//
//

#include "hole.h"
#include "edgeCurve.h"

hole::hole(float _os, float _r, float _t, edgeCurve *_edge, ofPath *_addpath) :
mOS(_os),
mT(_t),
mEdge(_edge),
mPath(NULL),
mAddPath(_addpath)
{
    update(_os, _r, _t);
}

hole::~hole() {
    
}

void hole::update(float _os, float _r, float _t) {
    mOS = _os;
    mR = _r;
    mT = _t;
    ofPolyline pln = mAddPath->getOutline()[0];
    float sc_t = mT*0.5;
    if (mEdge->isSlave()) {
        sc_t = 1.0-mT*0.5;
    }
    
    mEdgePt = pln.getPointAtPercent(sc_t);
    mEdgeNormal = pln.getNormalAtIndex(pln.getIndexAtPercent(sc_t)).rescale(-1);
    
    mVerts.clear();
    
}

void hole::draw() {
    /*ofSetColor(255, 0, 0);
    ofSetLineWidth(4);
    ofLine(mEdgePt, mEdgePt+mEdgeNormal*mOS);
    ofCircle(mEdgePt+mEdgeNormal*(mOS+mR), mR);*/
}

void hole::addToPath() {
    if (mVerts.size() == 0) {
        //mAddPath->circle(mEdgePt+mEdgeNormal*(mOS+mR), mR);
        ofPoint cen =mEdgePt+mEdgeNormal*(mOS+mR);
        float segs = 6.0;
        vector<ofPoint> pts;
        
        for( float theta = 0; theta < TWO_PI+1*TWO_PI/segs; theta += TWO_PI/segs)
        {
            pts.push_back(ofPoint(mR * cos(theta)+cen.x, mR * sin(theta)+cen.y));
        }
        
        for (int i=0; i<pts.size()-2;i++) {
            if (i==0) mAddPath->moveTo((pts[i]+pts[i+1])*0.5);
            mAddPath->bezierTo((pts[i]+pts[i+1])*0.5, pts[i+1], (pts[i+1]+pts[i+2])*0.5);
            
        }
        
        
        mAddPath->close();
    }
}