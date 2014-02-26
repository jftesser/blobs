//
//  edgeCurve.h
//  blobs
//
//  Created by Jenna Fizel on 2/22/14.
//
//

#ifndef __blobs__edgeCurve__
#define __blobs__edgeCurve__

#include <iostream>
#include "ofMain.h"

class hole;

struct draggableVertex {
    
	ofVec3f pos;
	bool 	bBeingDragged;
	bool 	bOver;
	float 	radius;
    int     ind;
    
    draggableVertex(ofVec3f _pos, int _ind) {
        pos.set(_pos);
        bBeingDragged = false;
        bOver = false;
        radius = 15.0;
        ind = _ind;
    }
	
};

class edgeCurve {
public:
    edgeCurve(vector <ofVec3f> _init_verts, bool _is_slave = false);
    ~edgeCurve();
    
    // call these from main app
    void draw();
    void update();
    void mouseMoved(int _x, int _y );
    void mouseDragged(int _x, int _y, int _button);
    void mousePressed(int _x, int _y, int _button);
    void mouseReleased(int _x, int _y, int _button);
    void forceUpdateVerts(vector <ofVec3f> _verts);
    
    // modifiers
    void addVertex();
    void removeVertex();
    void addHole(float _t, float _os, float _r, bool _both);
    void updateHole(float _t, float _os, float _r);
    void removeHole(bool _rmain, bool _rslave);
    void resample(int _vcnt);
    void calcZipper();
    
    // getters
    edgeCurve *getSlave() {return mSlavedCurve;};
    bool isLocked() {return mLocked;};
    bool isSlave() {return mIsSlave;};
    ofVboMesh getMesh();
    
private:
    vector <ofVec3f> calcSlaveVerts();
    void updatePath();
    void recenter();
    
private:
    vector <draggableVertex *> mVerts;
    ofPath *mPath;
    ofPolyline *mResampleLine;
    edgeCurve *mSlavedCurve;
    bool mIsSlave;
    bool mLocked;
    int mMinVertCnt;
    int mMaxVertCnt;
    int mVertCnt;
    hole *mHole;
    hole *mSlaveHole;
    ofPolyline *mZipperBase;
    ofPolyline *mZipper;
public:
    bool mHoleSlaved;
};



#endif /* defined(__blobs__edgeCurve__) */
