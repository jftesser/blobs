//
//  form.cpp
//  blobs
//
//  Created by Jenna Fizel on 2/23/14.
//
//

#include "form.h"

form::form() :
mMaxEdgeLen(60)
{
}

form::~form() {
    
}

void form::draw() {
    ofSetColor(255, 0, 0);
    mMesh.draw(OF_MESH_FILL);
    ofSetColor(0, 0, 0);
    mMesh.draw(OF_MESH_WIREFRAME);
}

void form::update(ofVboMesh _mesh) {
    mRawMesh = _mesh;
    mRawMesh.enableIndices();
    mMesh.enableIndices();
    
    // clear existing
    mVerts.clear();
    mRawFaces.clear();
    mFaces.clear();
    
    // fill out with current state
    mVerts = mRawMesh.getVertices();
    vector <ofIndexType> raw_inds = mRawMesh.getIndices();
    
    for (int i=0; i<raw_inds.size(); i += 3) {
        mRawFaces.push_back(face(raw_inds[i],raw_inds[i+1],raw_inds[i+2]));
    }
    
    // subdivide
    for (auto f : mRawFaces) {
        trySubdivide(f);
    }
    
    // make new mesh
    mMesh.clear();
    mMesh.addVertices(mVerts);
    mMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    for (auto f : mFaces) {
        mMesh.addTriangle(f.a, f.b, f.c);
    }
    printf("made a mesh with %d vertices and %d faces",mVerts.size(),mFaces.size());
}

void form::trySubdivide(face &_f) {
    float dab, dbc, dca;
    dab = mVerts[_f.a].distance(mVerts[_f.b]);
    dbc = mVerts[_f.b].distance(mVerts[_f.c]);
    dca = mVerts[_f.c].distance(mVerts[_f.a]);
    
    if (dab > mMaxEdgeLen || dbc > mMaxEdgeLen || dca > mMaxEdgeLen) {
        subdivide(_f);
    } else {
        mFaces.push_back(_f);
    }
}

void form::subdivide(face &_f) {
    ofVec3f ab, bc, ca;
    ofVec3f a = mVerts[_f.a];
    ofVec3f b = mVerts[_f.b];
    ofVec3f c = mVerts[_f.c];
    
    ab = (a+b)*0.5;
    int abind = findVert(ab);
    if (abind < 0) {
        abind =mVerts.size();
        mVerts.push_back(ab);
    }
    bc = (b+c)*0.5;
    int bcind = findVert(bc);
    if (bcind < 0) {
        bcind =mVerts.size();
        mVerts.push_back(bc);
    }
    ca = (c+a)*0.5;
    int caind = findVert(ca);
    if (caind < 0) {
        caind =mVerts.size();
        mVerts.push_back(ca);
    }
    
    face A(_f.a,abind,caind);
    face B(_f.b,bcind,abind);
    face C(_f.c,caind,bcind);
    face D(abind,bcind,caind);
    
    trySubdivide(A);
    trySubdivide(B);
    trySubdivide(C);
    trySubdivide(D);
    
}

int form::findVert(ofVec3f _v) {
    int ind = -1;
    
    for (int i=0; i<mVerts.size();i++) {
        if (_v == mVerts[i]) return i;
    }
    
    return ind;
}