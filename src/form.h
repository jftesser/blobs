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
#include "ofxBullet.h"

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

class form {
public:
    form();
    ~form();
    
    void updateWorld();
    void draw();
    void update(ofVboMesh _mesh);
private:
    void trySubdivide(face &_f);
    void subdivide(face &_f);
    int findVert(ofVec3f _v);
private:
    // mesh grabbing and processing
    ofVboMesh mRawMesh;
    ofVboMesh mMesh;
    vector <ofVec3f> mVerts;
    vector <face> mRawFaces;
    vector <face> mFaces;
    float mMaxEdgeLen;
    
    // simulation
    ofxBulletWorldRigid mWorld;
    vector <ofxBulletBox*> mBounds;
	ofxBulletCustomShape* mBoundsShape;
	ofMaterial mBoundsMat;
	float mBoundsSz;
	
	ofxBulletCustomShape *mShape;
	ofMaterial mShapeMat;
    
    ofCamera mCamera;
	ofLight mLight;
};

#endif /* defined(__blobs__form__) */
