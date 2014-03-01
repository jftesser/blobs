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
#include "ofxSTL.h"

class edgeCurve;

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

struct part_and_index {
    msa::physics::Particle3D *part;
    int index;
    int array_ind;
    part_and_index(msa::physics::Particle3D *_part, int _index, int _arr_ind) {
        part = _part;
        index = _index;
        array_ind = _arr_ind;
    }
};

class form {
public:
    form(edgeCurve *_master, edgeCurve *_slave);
    ~form();
    
    void updateWorld();
    void draw();
    void update();
    
    void zipNext();
    void unzipLast();
    
    void releaseCorners();
    
    void exportModel();
    
private:
    void trySubdivide(face _f);
    void subdivide(face _f); 
    int findVert(ofVec3f _v);
    bool unmade(msa::physics::Particle3D *_a, msa::physics::Particle3D *_b);
    float area(float _a, float _b, float _c);
    void setIndex(msa::physics::Particle3D *_p, ofPolyline &_master, ofPolyline &_slave, int _arr_ind);
    void orderEdge();
    bool uniqueFace(face _f);
    bool isIn(msa::physics::Particle3D *_p, vector<part_and_index *> _pais);
    msa::physics::Spring3D *addSpring(msa::physics::Particle3D *_p1, msa::physics::Particle3D *_p2, float _strength, float _len = -1);
    void addMesh(ofVboMesh &_mesh);
    
private:
    // mesh grabbing and processing
    edgeCurve *mMasterCurve;
    edgeCurve *mSlaveCurve;
    ofVboMesh mRawMesh;
    ofVboMesh mMesh;
    ofVboMesh mOuterMesh;
    ofVboMesh mSideMesh;
    vector <ofVec3f> mVerts;
    vector <face> mRawFaces;
    vector <face> mFaces;
    float mMaxEdgeLen;
    float mTol;
    
    // simulation
	float mBoundsSz;
	msa::physics::World3D mWorld;
    vector <msa::physics::Spring3D *> mSprings;
    vector <msa::physics::Particle3D *> mParticles;
    vector <msa::physics::Spring3D *> mBottomSprings;
    vector <msa::physics::Particle3D *> mBottomParticles;
    vector <msa::physics::Spring3D *> mMiddleSprings;
    vector <part_and_index *> mMasterEdgeParticles;
    vector <part_and_index *> mSlaveEdgeParticles;
    vector <msa::physics::Spring3D *> mZipperSprings;
    vector <msa::physics::Spring3D *> mOuterZipperSprings;
    int mAtZip;
    
    // appearance
    ofEasyCam mCamera;
	ofLight mLight;
    vector <ofBoxPrimitive*> mBounds;
    
    // export
    ofxSTLExporter mExporter;
    
};

#endif /* defined(__blobs__form__) */
