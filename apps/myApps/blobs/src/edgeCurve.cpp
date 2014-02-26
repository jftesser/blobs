//
//  edgeCurve.cpp
//  blobs
//
//  Created by Jenna Fizel on 2/22/14.
//
//

#include "edgeCurve.h"
#include "hole.h"

edgeCurve::edgeCurve(vector <ofVec3f> _init_verts, bool _is_slave) :
mSlavedCurve(NULL),
mIsSlave(_is_slave),
mLocked(_is_slave),
mPath(NULL),
mMaxVertCnt(13),
mMinVertCnt(4),
mVertCnt(_init_verts.size()),
mHole(NULL),
mSlaveHole(NULL),
mHoleSlaved(true),
mResampleLine(NULL)
{
    int ind = 0;
    for (auto v : _init_verts) {
        mVerts.push_back(new draggableVertex(v, ind));
        ind++;
    }
    
    if (mIsSlave == false) {
        mSlavedCurve = new edgeCurve(calcSlaveVerts(),true);
        
        mPath = new ofPath();
        updatePath();
        mPath->setFillColor(ofColor(185));
        mPath->setFilled(true);
        mPath->setStrokeColor(ofColor(90));
        mPath->setStrokeWidth(3);
        
        recenter();
    }
    
}

edgeCurve::~edgeCurve() {
    if (mPath) delete mPath;
    if (mHole) delete mHole;
    if (mSlaveHole) delete mSlaveHole;
    if (mResampleLine) delete mResampleLine;
}

void edgeCurve::updatePath() {
    int ind =0;
    mPath->clear();
    mPath->setCurveResolution(10);
    for (int ind =0; ind <mVerts.size(); ind++) {
        int nind = ind+1;
        if (nind < mVerts.size()) {
            int nnind = ind-1;
            if (nnind < 0 ) nnind = 0;
            
            draggableVertex *v = mVerts[ind];
            draggableVertex *nv = mVerts[nind];
            draggableVertex *nnv = mVerts[nnind];
            
            if (ind == 0) {
                mPath->moveTo(v->pos.x,v->pos.y,v->pos.z);
            }
            
            mPath->bezierTo((nnv->pos+v->pos)*0.5, v->pos, (v->pos+nv->pos)*0.5);
        }
    }
    
    if (mSlavedCurve) {
        for (int ind =0; ind <mSlavedCurve->mVerts.size(); ind++) {
            int nind = ind+1;
            if (nind < mSlavedCurve->mVerts.size()) {
                int nnind = ind-1;
                if (nnind < 0 ) nnind = 0;
                
                draggableVertex *v = mSlavedCurve->mVerts[ind];
                draggableVertex *nv = mSlavedCurve->mVerts[nind];
                draggableVertex *nnv = mSlavedCurve->mVerts[nnind];
                
                if (ind == 0) {
                    mPath->lineTo(v->pos.x,v->pos.y,v->pos.z);
                }
                
                mPath->bezierTo((nnv->pos+v->pos)*0.5, v->pos, (v->pos+nv->pos)*0.5);
            }
        }    }
    
    mPath->close();
    
    if (mHole) {
        mHole->addToPath();
    }
    
    if (mSlaveHole) {
        mSlaveHole->addToPath();
    }
    
}
void edgeCurve::update() {
    
    if (mSlavedCurve && mSlavedCurve->isLocked()) {
        mSlavedCurve->forceUpdateVerts(calcSlaveVerts());
    }
}

void edgeCurve::forceUpdateVerts(vector<ofVec3f> _verts) {
    int ind = 0;
    mVerts.clear();
    for (auto v : _verts) {
        mVerts.push_back(new draggableVertex(v,ind));
        ind++;
    }
}

vector<ofVec3f> edgeCurve::calcSlaveVerts() {
    ofVec3f rotcen = (mVerts[0]->pos+mVerts[mVerts.size()-1]->pos)*0.5;
    vector <ofVec3f> rotatedverts;
    for (auto v : mVerts) {
        ofVec3f pos;
        pos.set(v->pos);
        ofVec3f rpos = pos.rotateRad(pi, rotcen, ofVec3f(0,0,1));
        rotatedverts.push_back(rpos);
    }
    
    return rotatedverts;
}

void edgeCurve::recenter() {
    ofVec3f normcen = (mVerts[0]->pos+mVerts[mVerts.size()-1]->pos)*0.5;
    for (auto v : mVerts) {
        v->pos -= normcen;
    }
    
}

void edgeCurve::addVertex() {
    if (mVertCnt < mMaxVertCnt) {
        resample(mVertCnt+1);
    }
}

void edgeCurve::removeVertex() {
    if (mVertCnt > mMinVertCnt) {
        resample(mVertCnt-1);
    }
}

void edgeCurve::addHole(float _t, float _os, float _r, bool _both) {
    if (mHole) {
        delete mHole;
        mHole = NULL;
    }
    
    if (mSlaveHole) {
        delete mSlaveHole;
        mSlaveHole = NULL;
    }
    
    mHole = new hole(_os,_r,_t,this,mPath);
    if (_both)
        mSlaveHole = new hole(_os,_r,_t,mSlavedCurve,mPath);

}

void edgeCurve::updateHole(float _t, float _os, float _r) {
    if (mHole)
        mHole->update(_os, _r, _t);
    if (mSlaveHole)
        mSlaveHole->update(_os, _r, _t);
}

void edgeCurve::removeHole(bool _rmain, bool _rslave) {
    if (_rmain && mHole){
        delete mHole;
        mHole = NULL;
    }
    if (_rslave && mSlaveHole){
        delete mSlaveHole;
        mSlaveHole = NULL;
    }
}

void edgeCurve::resample(int _vcnt) {
    if (mResampleLine) {
        delete mResampleLine;
        mResampleLine = NULL;
    }
    mResampleLine = new ofPolyline();
    
    int ind =0;
    for (auto v : mVerts) {
        if (ind ==0) {
            mResampleLine->addVertex(v->pos);
            mResampleLine->curveTo(v->pos);
            mResampleLine->curveTo(v->pos);
        } else if (ind == mVerts.size()-1) {
            mResampleLine->curveTo(v->pos);
            mResampleLine->curveTo(v->pos);
        }
        else {
            mResampleLine->curveTo(v->pos);
        }
        
        ind++;
    }
    
    ofPolyline rpln = mResampleLine->getResampledBySpacing(mResampleLine->getPerimeter()/(_vcnt+0.001));
    
    mVerts.clear();
    vector <ofPoint> pts = rpln.getVertices();
    
    ind =0;
    for (auto pt : pts) {
        mVerts.push_back(new draggableVertex(pt,ind));
        ind++;
    }
    mVertCnt = _vcnt;
}

void edgeCurve::draw() {
    
    if (mIsSlave == false) {
        updatePath();
        mPath->draw();
    }
    
    if (mLocked == false) {
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
    
    if (mHole) {
        mHole->draw();
    }
    
    if (mSlaveHole) {
        mSlaveHole->draw();
    }
}

void edgeCurve::mouseMoved(int _x, int _y ){
    if (mLocked == false) {
        for (auto v : mVerts){
            float diffx = _x - v->pos.x;
            float diffy = _y - v->pos.y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < v->radius){
                v->bOver = true;
                printf("over vertex %i\n",v->ind);
            } else {
                v->bOver = false;
            }
        }
    }
}

void edgeCurve::mouseDragged(int _x, int _y, int button){
    if (mLocked == false) {
        for (auto v : mVerts){
            if (v->bBeingDragged == true){
                v->pos.x = _x;
                v->pos.y = _y;
                return; // only ever move one vertex
            }
        }
    }
}

void edgeCurve::mousePressed(int _x, int _y, int _button){
    if (mLocked == false) {
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

void edgeCurve::mouseReleased(int _x, int _y, int _button){
	for (auto v : mVerts){
		v->bBeingDragged = false;
	}
    if (mIsSlave == false) recenter();
}

ofVboMesh edgeCurve::getMesh() {
    ofVboMesh tess;
    if (mPath) {
        tess = ofVboMesh(mPath->getTessellation());
    }
    
    return tess;
}

