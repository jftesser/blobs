//
//  form.cpp
//  blobs
//
//  Created by Jenna Fizel on 2/23/14.
//
//

#include "form.h"

form::form() :
mMaxEdgeLen(40)
{
    
    mCamera.setPosition(ofVec3f(0, 15.f, -ofGetHeight()*1.25));
	mCamera.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));
    
	ofVec3f startLoc;
	ofPoint dimens;
	mBoundsSz = ofGetHeight();
	float hwidth = mBoundsSz*.5;
	float depth = 2.;
	float hdepth = depth*.5;
	
	for(int i = 0; i < 6; i++) {
        ofBoxPrimitive *bp = new ofBoxPrimitive();
		mBounds.push_back( bp );
		if(i == 0) { // ground //
			startLoc.set( 0., hwidth+hdepth, 0. );
			dimens.set(mBoundsSz, depth, mBoundsSz);
		} else if (i == 1) { // back wall //
			startLoc.set(0, 0, hwidth+hdepth);
			dimens.set(mBoundsSz, mBoundsSz, depth);
		} else if (i == 2) { // right wall //
			startLoc.set(hwidth+hdepth, 0, 0.);
			dimens.set(depth, mBoundsSz, mBoundsSz);
		} else if (i == 3) { // left wall //
			startLoc.set(-hwidth-hdepth, 0, 0.);
			dimens.set(depth, mBoundsSz, mBoundsSz);
		} else if (i == 4) { // ceiling //
			startLoc.set(0, -hwidth-hdepth, 0.);
			dimens.set(mBoundsSz, depth, mBoundsSz);
		} else if (i == 5) { // front wall //
			startLoc.set(0, 0, -hwidth-hdepth);
			dimens.set(mBoundsSz, mBoundsSz, depth);
		}
        bp->set(dimens.x, dimens.y, dimens.z);
        bp->setPosition(startLoc);
	}
	
}

form::~form() {
    
}

void form::draw() {
    
    //ofSetColor(0, 0, 0);
    //mMesh.draw(OF_MESH_WIREFRAME);
    
    glEnable( GL_DEPTH_TEST );
	
	mCamera.begin();
	ofSetColor(255,0,128);
    //mWorld.drawDebug();
	
	ofEnableLighting();
    
	ofSetColor(255, 255, 255);
	mLight.enable();
	mLight.setPosition(ofVec3f(30, -15+5, -5));
	
	ofSetColor(185, 185, 185);
	
    for(int i=0; i<mBounds.size()-1;i++) {
        mBounds[i]->draw();
    }
	
	ofDisableAlphaBlending();
	ofDisableBlendMode();
    
    ofSetColor(255, 0, 128);
        for (auto j : mJoints) {
            j->draw();
        }
 
	mLight.disable();
	ofDisableLighting();
	
	mCamera.end();
	glDisable(GL_DEPTH_TEST);

}

void form::updateWorld() {
    
    for (auto j : mJoints) {
        j->update();
    }
    
    for (auto n : mNodes) {
        n->update();
    }
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
    
    mJoints.clear();
    mNodes.clear();
    
    for (auto v : mVerts) {
        particle *p = new particle(v);
        mNodes.push_back(p);
    }
    
    for (auto f : mFaces) {
        spring *a = new spring(mNodes[f.a],mNodes[f.b]);
        mJoints.push_back(a);
        spring *b = new spring(mNodes[f.b],mNodes[f.c]);
        mJoints.push_back(b);
        spring *c = new spring(mNodes[f.c],mNodes[f.a]);
        mJoints.push_back(c);
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