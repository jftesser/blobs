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
#include "MSAPhysics3D.h"

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
    void update(ofPath *_path);
private:
    void trySubdivide(face &_f);
    void subdivide(face &_f); 
    int findVert(ofVec3f _v);
    bool unmade(msa::physics::Particle3D *_a, msa::physics::Particle3D *_b);
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
	
	msa::physics::World3D mWorld;
    vector <msa::physics::Spring3D *> mSprings;
    vector <msa::physics::Particle3D *> mParticles;
    
    ofCamera mCamera;
	ofLight mLight;
};

#endif /* defined(__blobs__form__) */
