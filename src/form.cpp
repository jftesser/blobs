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
    
    
    glEnable( GL_DEPTH_TEST );
	
	mCamera.begin();
	ofSetColor(255,0,128);
	
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
    mMesh.ofMesh::draw(OF_MESH_WIREFRAME);
	mLight.disable();
	ofDisableLighting();
	
	mCamera.end();
	glDisable(GL_DEPTH_TEST);
    
}

void form::updateWorld() {
    for (auto s : mSprings) {
        s->update();
    }
    
    for (auto p : mParticles) {
        p->update();
    }
    
    for (int i=0; i<mParticles.size();i++) {
        mMesh.setVertex(i, mParticles[i]->mPos);
    }
}

void form::update(ofPath *_path) {
	ofPath *lowres = new ofPath(*_path);
    lowres->setCurveResolution(2);
    mRawMesh = lowres->getTessellation();
    //_path->setCurveResolution(15);
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
    
    // snap to
    snap(_path->getOutline(),lowres->getOutline());
    
    // make new mesh
    mMesh.clear();
    mMesh.addVertices(mVerts);
    mMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    for (auto f : mFaces) {
        mMesh.addTriangle(f.a, f.b, f.c);
    }
    
    mSprings.clear();
    mParticles.clear();
    
    for (auto v : mVerts) {
        particle *p = new particle(v);
        mParticles.push_back(p);
    }
    
    for (auto f : mFaces) {
        if (unmade(mParticles[f.a],mParticles[f.b])) {
            spring *a = new spring(mParticles[f.a],mParticles[f.b]);
            mSprings.push_back(a);
        }
        if (unmade(mParticles[f.b],mParticles[f.c])) {
            spring *b = new spring(mParticles[f.b],mParticles[f.c]);
            mSprings.push_back(b);
        }
        
        if (unmade(mParticles[f.c],mParticles[f.a])) {
            spring *c = new spring(mParticles[f.c],mParticles[f.a]);
            mSprings.push_back(c);
        }
    }
    
	delete lowres;
    
    printf("made a mesh with %d vertices and %d faces",mVerts.size(),mFaces.size());
}

void form::snap(vector<ofPolyline> _polys,vector<ofPolyline> _low_polys) {
    for (int i=0; i<mVerts.size();i++) {
        float mind = -1;
        float minind = -1;
        ofPoint pt;
		ofPoint lpt;
        int ind = 0;
        for (auto p : _polys) {
            ofPoint tpt = p.getClosestPoint( mVerts[i]);
            float d = tpt.distance(mVerts[i]);
            if (mind > d || mind < 0) {
                mind = d;
                minind = ind;
                pt = tpt;
            }
            ind++;
        }

		lpt =  _low_polys[minind].getClosestPoint(mVerts[i]);

		ofPoint avg = (mVerts[i]+pt)*0.5;
        
        if (_low_polys[0].inside(avg) == false) {
            mVerts[i].set(pt);
        }
    }
}
 
bool form::unmade(particle *_a, particle *_b) {
    for (auto s : mSprings) {
        if ((_a == s->mA && _b == s->mB) || (_a == s->mB && _b == s->mA)) {
            return false;
        }
    }
    return true;
}

void form::trySubdivide(face &_f) {
    float dab, dbc, dca;
    dab = mVerts[_f.a].distance(mVerts[_f.b]);
    dbc = mVerts[_f.b].distance(mVerts[_f.c]);
    dca = mVerts[_f.c].distance(mVerts[_f.a]);
    
    if (dab > mMaxEdgeLen || dbc > mMaxEdgeLen || dca > mMaxEdgeLen || area(dab, dbc, dca) > area(mMaxEdgeLen*0.5,mMaxEdgeLen*0.5,mMaxEdgeLen*0.5) ) {
        subdivide(_f);
    } else {
        mFaces.push_back(_f);
    }
}

float form::area(float _a, float _b, float _c) {
    float s = (_a+_b+_c)/2.0;
    
    return sqrt(s*(s-_a)*(s-_b)*(s-_c));
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