//
//  form.h
//  blobs
//
//  Created by Jenna Fizel on 2/23/14.
//
//

#ifndef __blobs__form__
#define __blobs__form__

#include <iostream>
#include "ofMain.h"

struct face {
    int a;
    int b;
    int c;
    face( int _a, int _b, int _c) {
        a = _a;
        b = _b;
        c = _c;
    }
};

class particle;
class spring;

class form {
public:
    form();
    ~form();
    
    void updateWorld();
    void draw();
    void update(ofPath *_path);
private:
    void trySubdivide(face &_f);
    void subdivide(face &_f); 
    int findVert(ofVec3f _v);
    bool unmade(particle *_a, particle *_b);
    void snap(vector<ofPolyline> _polys,vector<ofPolyline> _low_polys);
    float area(float _a, float _b, float _c);
private:
    // mesh grabbing and processing
    ofVboMesh mRawMesh;
    ofVboMesh mMesh;
    vector <ofVec3f> mVerts;
    vector <face> mRawFaces;
    vector <face> mFaces;
    float mMaxEdgeLen;
    
    // simulation
    vector <ofBoxPrimitive*> mBounds;
	float mBoundsSz;
	
	//ofxBulletCustomShape *mShape;
    vector <spring *> mSprings;
    vector <particle *> mParticles;
    
    ofCamera mCamera;
	ofLight mLight;
};

class particle {
public:
    particle(ofPoint _p) {
        mPos = _p;
    }
    ofVec3f mPos, mVelocity, mAcceleration;
    
    void update() {
        
        mVelocity += mAcceleration;
        
        mPos += mVelocity;
        mAcceleration.set(0,0);
        
        /*if (mPos.y< 0 ) {
            mVelocity.y=0;
            mPos.y=0;
        }*/
    }
    
};

class spring {
public:
    particle *mA, *mB;
    float mLength;
    
    spring(particle *_a, particle *_b) {
        mA = _a;
        mB = _b;
        mLength = mA->mPos.distance(mB->mPos);
    }
    
    void update() {
        float dist = mA->mPos.distance(mB->mPos);
        ofVec3f dir = mB->mPos - mA->mPos;
        dir.normalize();
        dir *= (dist-mLength)/100;
        mA->mVelocity += dir;
    }
    
    void draw() {
        ofLine(mA->mPos, mB->mPos);
    }
};

#endif /* defined(__blobs__form__) */
