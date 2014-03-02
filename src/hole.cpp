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
mAddPath(_addpath),
mFreed(false)
{
    update(_os, _r, _t);
}

hole::~hole() {
    
}

void hole::free() {
    mFreed = true;
}

void hole::lock() {
    mFreed = false;
    update(mOS, mR, mT);
}

void hole::update(float _os, float _r, float _t) {
    if (mFreed == false) {
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
        ofPoint cen =mEdgePt+mEdgeNormal*(mOS+mR);
        float segs = 6.0;
        int ind = 0;
        for( float theta = 0; theta < TWO_PI+1*TWO_PI/segs; theta += TWO_PI/segs)
        {
            mVerts.push_back(new draggableVertex(ofPoint(mR * cos(theta)+cen.x, mR * sin(theta)+cen.y,0),ind));
            ind++;
        }
    }
}

void hole::draw() {
   if (mFreed) {
            ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
            ofEnableSmoothing();
            ofSetLineWidth(1.25);
            
            ofNoFill();
            ofSetColor(255,0,128);
            ofBeginShape();
            for (auto v : mVerts){
                ofVertex(v->pos.x, v->pos.y);
            }
            ofEndShape();
            
            
            ofSetColor(255,0,128);
            for (auto v : mVerts){
                ofFill();
                if (v->bOver == true) {
                    ofCircle(v->pos.x, v->pos.y,12);
                } else {
                    ofCircle(v->pos.x, v->pos.y,8);
                }
            }
            
            ofDisableBlendMode();
        

  }
}

void hole::addToPath(ofPath *_path) {
        
        
        for (int i=0; i<mVerts.size()-2;i++) {
            if (i==0) _path->moveTo((mVerts[i]->pos+mVerts[i+1]->pos)*0.5);
            _path->bezierTo((mVerts[i]->pos+mVerts[i+1]->pos)*0.5, mVerts[i+1]->pos, (mVerts[i+1]->pos+mVerts[i+2]->pos)*0.5);
            
        }
        
        
        _path->close();
    
}

void hole::mouseMoved(int _x, int _y ){
    if (mFreed == true) {
        for (auto v : mVerts){
            float diffx = _x - v->pos.x;
            float diffy = _y - v->pos.y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < v->radius){
                v->bOver = true;
                printf("over hole vertex %i\n",v->ind);
            } else {
                v->bOver = false;
            }
        }
    }
}

void hole::mouseDragged(int _x, int _y, int button){
    if (mFreed == true) {
        for (auto v : mVerts){
            if (v->bBeingDragged == true){
                v->pos.x = _x;
                v->pos.y = _y;
            }
        }
    }
}

void hole::mousePressed(int _x, int _y, int _button){
    if (mFreed == true) {
        for (auto v : mVerts){
            float diffx = _x - v->pos.x;
            float diffy = _y - v->pos.y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < v->radius){
                v->bBeingDragged = true;
            } else {
                v->bBeingDragged = false;
            }
        }
    }
}

void hole::mouseReleased(int _x, int _y, int _button){
	for (auto v : mVerts){
		v->bBeingDragged = false;
	}
}
