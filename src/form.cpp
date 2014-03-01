//
//  form.cpp
//  blobs
//
//  Created by Jenna Fizel on 2/23/14.
//
//

#include "form.h"
#include "edgeCurve.h"

form::form(edgeCurve *_master, edgeCurve *_slave) :
mMaxEdgeLen(50),
mTol(0.3),
mMasterCurve(_master),
mSlaveCurve(_slave)
{
    
    mCamera.setPosition(ofVec3f(0, 15.f, -ofGetHeight()*1.25));
	mCamera.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));
    mCamera.enableMouseInput();
    
	ofVec3f startLoc;
	ofPoint dimens;
	mBoundsSz = ofGetHeight()*2.0;
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
	
    mWorld.setGravity(ofVec3f(0, 0, 0));
	
	mWorld.setWorldSize(ofVec3f(-mBoundsSz/2, -mBoundsSz/2, -mBoundsSz/2), ofVec3f(mBoundsSz/2, mBoundsSz/2, mBoundsSz/2));
	mWorld.setSectorCount(1);
    mWorld.setDrag(0.97);
    
	//mWorld.enableCollision();
}

form::~form() {
    
}

void form::draw() {
    
    for (int i=0; i<mParticles.size();i++) {
        mMesh.setVertex(i, mParticles[i]->getPosition());
        mOuterMesh.setVertex(i, mBottomParticles[i]->getPosition());
    }

    glEnable( GL_DEPTH_TEST );
	mCamera.begin(ofRectangle(ofGetHeight(), 0, ofGetHeight(), ofGetHeight()));
    
    
	ofSetColor(255,0,128);
	
	ofEnableLighting();
    
	ofSetColor(255, 255, 255);
	mLight.enable();
	mLight.setPosition(ofVec3f(30, -15+5, -1));
    mLight.setAttenuation(0.75,0,0);
	
	ofSetColor(185, 185, 185);
	
    for(int i=0; i<mBounds.size();i++) {
        mBounds[i]->draw();
    }
	
	ofDisableAlphaBlending();
	ofDisableBlendMode();
    
    ofSetColor(255, 0, 128);
    ofMaterial meshmat;
    meshmat.setColors(ofColor(255,0,125), ofColor(255,0,125), ofColor(255,255,255), ofColor(0,0,0));
    meshmat.setShininess(1.0);
    meshmat.begin();
    mMesh.ofMesh::draw(OF_MESH_FILL);
    mOuterMesh.ofMesh::draw(OF_MESH_FILL);
    for (int i=1; i<mMasterEdgeParticles.size();i++) {
        ofBeginShape();
        ofVertex((ofPoint &)mBottomParticles[mMasterEdgeParticles[i-1]->array_ind]->getPosition());
        ofVertex((ofPoint &)mBottomParticles[mMasterEdgeParticles[i]->array_ind]->getPosition());
        ofVertex((ofPoint &)mParticles[mMasterEdgeParticles[i]->array_ind]->getPosition());
        ofVertex((ofPoint &)mParticles[mMasterEdgeParticles[i-1]->array_ind]->getPosition());
        ofEndShape();
    }
    for (int i=1; i<mSlaveEdgeParticles.size();i++) {
        ofBeginShape();
        ofVertex((ofPoint &)mBottomParticles[mSlaveEdgeParticles[i-1]->array_ind]->getPosition());
        ofVertex((ofPoint &)mBottomParticles[mSlaveEdgeParticles[i]->array_ind]->getPosition());
        ofVertex((ofPoint &)mParticles[mSlaveEdgeParticles[i]->array_ind]->getPosition());
        ofVertex((ofPoint &)mParticles[mSlaveEdgeParticles[i-1]->array_ind]->getPosition());
        ofEndShape();
    }
    meshmat.end();
	mLight.disable();
	ofDisableLighting();
	
    if (mMasterEdgeParticles.size()>0) {
        part_and_index *pai = mMasterEdgeParticles[0];
        //for (auto pai : mMasterEdgeParticles) {
        ofPushMatrix();
        ofTranslate(pai->part->getPosition());
        ofSpherePrimitive(3, 5).draw();
        ofPopMatrix();
        //}
        pai = mSlaveEdgeParticles[0];
        //for (auto pai : mSlaveEdgeParticles) {
        ofPushMatrix();
        ofTranslate(pai->part->getPosition());
        ofSpherePrimitive(3, 5).draw();
        ofPopMatrix();
        //}
    }
    
    ofSetColor(255,255,255);
    for (int i=0; i<mAtZip;i++) {
        if (i < mZipperSprings.size()) {
            ofLine(mZipperSprings[i]->getOneEnd()->getPosition(),mZipperSprings[i]->getTheOtherEnd()->getPosition());
        }
        if (i < mOuterZipperSprings.size()) {
            ofLine(mOuterZipperSprings[i]->getOneEnd()->getPosition(),mOuterZipperSprings[i]->getTheOtherEnd()->getPosition());
        }
    }
    
    
	mCamera.end();
	glDisable(GL_DEPTH_TEST);
    
}

void form::updateWorld() {
    mWorld.update();
}

void form::update() {
    // we need to get an actually even tesselation!
    float edgeres = 0.25;
    vector <ofPolyline> rawpolys = mMasterCurve->getPath()->getOutline();
    ofPath path;
    ofPolyline master = mMasterCurve->getBasePoly().getResampledByCount(mMasterCurve->getBasePoly().getPerimeter()/(mMaxEdgeLen*edgeres));
    vector<ofVec3f> mvs = master.getVertices();
    for (auto v : mvs) {
        path.lineTo(v);
    }
    
    ofPolyline slave = mSlaveCurve->getBasePoly().getResampledByCount(mSlaveCurve->getBasePoly().getPerimeter()/(mMaxEdgeLen*edgeres));
    vector<ofVec3f> svs =  slave.getVertices();
    for (auto v : svs) {
        path.lineTo(v);
    }
    
    path.close();
    
    for (int i=1;i<rawpolys.size();i++) {
        ofPolyline p = rawpolys[i];
        float len = p.getPerimeter();
        int segs = ceil(len/(mMaxEdgeLen*edgeres));
        if (segs % 2 == 0) segs++;
        ofPolyline resamp = p.getResampledByCount(segs);
        vector<ofPoint> vs = resamp.getVertices();
        path.moveTo(vs[0]);
        for (auto v : vs) {
            path.lineTo(v);
        }
        path.close();
    }
    path.setCurveResolution(1);
    
    
    
    
    mRawMesh = path.getTessellation();
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
    mMesh.enableIndices();
    mMesh.enableNormals();
    mMesh.addVertices(mVerts);
    mMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    for (auto f : mFaces) {
        mMesh.addTriangle(f.a, f.b, f.c);
    }
    
    mOuterMesh.clear();
    mOuterMesh.enableIndices();
    mOuterMesh.enableNormals();
    mOuterMesh.addVertices(mVerts);
    mOuterMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    for (auto f : mFaces) {
        mOuterMesh.addTriangle(f.a, f.b, f.c);
    }
    
    //mMesh = mRawMesh;

    for(auto p : mSprings) p->release();
    mSprings.clear();
    for(auto p : mParticles) p->release();
    mParticles.clear();
    for(auto p : mMasterEdgeParticles) delete p;
    mMasterEdgeParticles.clear();
    for(auto p : mSlaveEdgeParticles) delete p;
    mSlaveEdgeParticles.clear();
    for(auto p : mZipperSprings) p->release();
    mZipperSprings.clear();
    for(auto p : mOuterZipperSprings) p->release();
    mOuterZipperSprings.clear();
    for(auto p : mBottomSprings) p->release();
    mBottomSprings.clear();
    for(auto p : mBottomParticles) p->release();
    mBottomParticles.clear();
    for(auto p : mMiddleSprings) p->release();
    mMiddleSprings.clear();

    mWorld.clear();
    
    float thickness = 10;
    
    int ind = 0;
    for (auto v : mVerts) {
        msa::physics::Particle3D *p = new msa::physics::Particle3D(v,1,0.01); // pos, mass, diam
        setIndex(p,master,slave,ind);
        
        mParticles.push_back(p);
        mWorld.addParticle(p);
        
        msa::physics::Particle3D *bp = new msa::physics::Particle3D(v+ofVec3f(0,0,thickness),1,0.01); // pos, mass, diam
        
        mBottomParticles.push_back(bp);
        mWorld.addParticle(bp);
        
        ind++;
    }
    
    float strength = 0.99;
    for (auto f : mFaces) {
        if (unmade(mParticles[f.a],mParticles[f.b])) {
            float alen = mParticles[f.a]->getPosition().distance(mParticles[f.b]->getPosition());
            
            mSprings.push_back(addSpring(mParticles[f.a],mParticles[f.b],strength,alen));
            
            mBottomSprings.push_back(addSpring(mBottomParticles[f.a],mBottomParticles[f.b],strength,alen));
            
            // add the four middle springs
            mMiddleSprings.push_back(addSpring(mParticles[f.a],mBottomParticles[f.a],strength,thickness));
            //mMiddleSprings.push_back(addSpring(mParticles[f.b],mBottomParticles[f.b],strength,thickness));
            float crosslen = mParticles[f.a]->getPosition().distance(mBottomParticles[f.b]->getPosition());
            mMiddleSprings.push_back(addSpring(mParticles[f.a],mBottomParticles[f.b],strength,crosslen));
            mMiddleSprings.push_back(addSpring(mParticles[f.b],mBottomParticles[f.a],strength,crosslen));
            
        }
        if (unmade(mParticles[f.b],mParticles[f.c])) {
            float blen = mParticles[f.b]->getPosition().distance(mParticles[f.c]->getPosition());
            mSprings.push_back(addSpring(mParticles[f.b],mParticles[f.c],strength,blen));
            
            mBottomSprings.push_back(addSpring(mBottomParticles[f.b],mBottomParticles[f.c],strength,blen));
            
            // add the four middle springs
            mMiddleSprings.push_back(addSpring(mParticles[f.b],mBottomParticles[f.b],strength,thickness));
            //mMiddleSprings.push_back(addSpring(mParticles[f.c],mBottomParticles[f.c],strength,thickness));
            float crosslen = mParticles[f.b]->getPosition().distance(mBottomParticles[f.c]->getPosition());
            mMiddleSprings.push_back(addSpring(mParticles[f.b],mBottomParticles[f.c],strength,crosslen));
            mMiddleSprings.push_back(addSpring(mParticles[f.c],mBottomParticles[f.b],strength,crosslen));
        }
        
        if (unmade(mParticles[f.c],mParticles[f.a])) {
            float clen = mParticles[f.c]->getPosition().distance(mParticles[f.a]->getPosition());
            mSprings.push_back(addSpring(mParticles[f.c],mParticles[f.a],strength,clen));
            
            mBottomSprings.push_back(addSpring(mBottomParticles[f.c],mBottomParticles[f.a],strength,clen));
            
            // add the four middle springs
            mMiddleSprings.push_back(addSpring(mParticles[f.c],mBottomParticles[f.c],strength,thickness));
            //mMiddleSprings.push_back(addSpring(mParticles[f.a],mBottomParticles[f.a],strength,thickness));
            
            float crosslen = mParticles[f.c]->getPosition().distance(mBottomParticles[f.a]->getPosition());
            mMiddleSprings.push_back(addSpring(mParticles[f.c],mBottomParticles[f.a],strength,crosslen));
            mMiddleSprings.push_back(addSpring(mParticles[f.a],mBottomParticles[f.c],strength,crosslen));
        }
    }
    
    orderEdge();
    
    mMasterEdgeParticles[0]->part->makeFixed();
    mSlaveEdgeParticles[0]->part->makeFixed();
    
    // modify all vertices to be partially rotated into place
    ofVec3f pivot = mMasterEdgeParticles[0]->part->getPosition();
    ofVec3f axis = pivot-mSlaveEdgeParticles[0]->part->getPosition();
    axis.normalize();
    
    for (int i=0; i<mParticles.size();i++) {
        msa::physics::Particle3D *p = mParticles[i];
        msa::physics::Particle3D *bp = mBottomParticles[i];
        int sign = 1;
        if (mMasterCurve->getBasePoly().inside(p->getPosition()) || isIn(p,mMasterEdgeParticles)) {
            sign = -1;
        }
        ofVec3f pos = p->getPosition();
        pos.rotate(45*sign, pivot, axis);
        p->moveTo(pos);
        
        pos = bp->getPosition();
        pos.rotate(45*sign, pivot, axis);
        bp->moveTo(pos);
    }
    
    for (int i=0; i<mParticles.size();i++) {
        mMesh.setVertex(i, mParticles[i]->getPosition());
    }
    
    for (int i=0;i<mMasterEdgeParticles.size();i++) {
        msa::physics::Particle3D *pm = mMasterEdgeParticles[i]->part;
        int sind = (1.0-(float)(i)/(float)(mMasterEdgeParticles.size()-1))*(mSlaveEdgeParticles.size()-1);
        msa::physics::Particle3D *sm = mSlaveEdgeParticles[sind]->part;
        
        msa::physics::Particle3D *bpm = mBottomParticles[mMasterEdgeParticles[i]->array_ind];
        msa::physics::Particle3D *bsm = mBottomParticles[mSlaveEdgeParticles[sind]->array_ind];
        
        if (sm->getPosition().distance(pm->getPosition()) > mTol) {
            pm->retain();
            sm->retain();
            msa::physics::Spring3D *spring = new msa::physics::Spring3D(pm,sm,strength*0.25,0.0);
            mZipperSprings.push_back(spring);
            
            bpm->retain();
            bsm->retain();
            msa::physics::Spring3D *bspring = new msa::physics::Spring3D(bpm,bsm,strength*0.25,0.0);
            mOuterZipperSprings.push_back(bspring);
        }
        
    }
    
    /*msa::physics::Particle3D *stabalizer = new msa::physics::Particle3D(mMesh.getCentroid(),1,1);
    mWorld.addParticle(stabalizer);
    
    for (auto p : mParticles) {
        mWorld.makeSpring(stabalizer, p, strength*0.1, stabalizer->getPosition().distance(p->getPosition()));
    }*/
    
    mAtZip = 0;
    
    printf("made a mesh with %d vertices and %d faces",mVerts.size(),mFaces.size());
}

msa::physics::Spring3D *form::addSpring(msa::physics::Particle3D *_p1, msa::physics::Particle3D *_p2, float _strength, float _len) {
    float len = _len;
    if (len < 0) len = _p1->getPosition().distance(_p2->getPosition());
    msa::physics::Spring3D *spring = new msa::physics::Spring3D(_p1,_p2,_strength,_len);
    mWorld.addConstraint(spring);
    
    return spring;
}
void form::zipNext() {
    if (mAtZip < mZipperSprings.size()) {
        mZipperSprings[mAtZip]->setStrength(0.5);
        mWorld.addConstraint(mZipperSprings[mAtZip]);
        mZipperSprings[mAtZip]->getA()->release(); // for correct reference counting on destruct
        mZipperSprings[mAtZip]->getB()->release();
        
        mOuterZipperSprings[mAtZip]->setStrength(0.5);
        mWorld.addConstraint(mOuterZipperSprings[mAtZip]);
        mOuterZipperSprings[mAtZip]->getA()->release(); // for correct reference counting on destruct
        mOuterZipperSprings[mAtZip]->getB()->release();
        
        mAtZip++;
    }
}

void form::unzipLast() {
    if (mAtZip < mZipperSprings.size() && mAtZip > 0) {
        --mAtZip;
        mZipperSprings[mAtZip]->setStrength(0);
        mOuterZipperSprings[mAtZip]->setStrength(0);
    }
}

void form::releaseCorners() {
    if (mMasterEdgeParticles.size() > 0) {
        mMasterEdgeParticles[0]->part->makeFree();
        mSlaveEdgeParticles[0]->part->makeFree();
    }
}

bool form::isIn(msa::physics::Particle3D *_p, vector<part_and_index *> _pais) {
    for (auto pai : _pais) {
        if (_p == pai->part) return true;
    }
    return false;
}

void form::orderEdge() {
    // bubble sort
    part_and_index *temp = NULL;
    bool finished = false;
    if (mMasterEdgeParticles.size() > 1) {
        while (!finished)    {
            finished = true;
            for (int i = 0; i < mMasterEdgeParticles.size()-1; i++) {
                if (mMasterEdgeParticles[i]->index > mMasterEdgeParticles[i+1]->index) {
                    temp = mMasterEdgeParticles[i];
                    mMasterEdgeParticles[i] = mMasterEdgeParticles[i+1];
                    mMasterEdgeParticles[i+1] = temp;
                    finished=false;
                }
            }
        }
    }
    
    if (mSlaveEdgeParticles.size() > 1) {
        finished = false;
        while (!finished)    {
            finished = true;
            for (int i = 0; i < mSlaveEdgeParticles.size()-1; i++) {
                if (mSlaveEdgeParticles[i]->index > mSlaveEdgeParticles[i+1]->index) {
                    temp = mSlaveEdgeParticles[i];
                    mSlaveEdgeParticles[i] = mSlaveEdgeParticles[i+1];
                    mSlaveEdgeParticles[i+1] = temp;
                    finished=false;
                }
            }
        }
    }
}

void form::setIndex(msa::physics::Particle3D *_p, ofPolyline &_master, ofPolyline &_slave, int _ind) {
    unsigned int mind = 0;
    unsigned int sind = 0;
    ofPoint mcpt = _master.getClosestPoint(_p->getPosition(), &mind);
    ofPoint scpt = _slave.getClosestPoint(_p->getPosition(), &sind);
    float mdist = mcpt.distance(_p->getPosition());
    float sdist = scpt.distance(_p->getPosition());
    
    //if (mdist < sdist) {
    if (mdist < mTol) {
        mMasterEdgeParticles.push_back(new part_and_index(_p,mind, _ind));
    }
    //} else {
    if (sdist < mTol) {
        mSlaveEdgeParticles.push_back(new part_and_index(_p,sind, _ind));
    }
    //}
    
}

bool form::unmade(msa::physics::Particle3D *_a, msa::physics::Particle3D *_b) {
    for (auto s : mSprings) {
        if ((_a == s->getA() && _b == s->getA()) || (_a == s->getB() && _b == s->getA())) {
            return false;
        }
    }
    return true;
}

void form::trySubdivide(face _f) {
    float dab, dbc, dca;
    dab = mVerts[_f.a].distance(mVerts[_f.b]);
    dbc = mVerts[_f.b].distance(mVerts[_f.c]);
    dca = mVerts[_f.c].distance(mVerts[_f.a]);
    
    if (dab > mMaxEdgeLen || dbc > mMaxEdgeLen || dca > mMaxEdgeLen  ) { //|| area(dab, dbc, dca) > area(mMaxEdgeLen*0.5,mMaxEdgeLen*0.5,mMaxEdgeLen*0.5)
        subdivide(_f);
    } else {
        mFaces.push_back(_f);
    }
}

float form::area(float _a, float _b, float _c) {
    float s = (_a+_b+_c)/2.0;
    
    return sqrt(s*(s-_a)*(s-_b)*(s-_c));
}

void form::subdivide(face _f) {
    ofVec3f a = mVerts[_f.a];
    ofVec3f b = mVerts[_f.b];
    ofVec3f c = mVerts[_f.c];
    float ablen = a.distance(b);
    float bclen = b.distance(c);
    float calen = c.distance(a);
    
    int abind = _f.a;
    if (ablen > mMaxEdgeLen) {
        ofVec3f ab = (a+b)*0.5;
        abind = findVert(ab);
        if (abind < 0) {
            abind =mVerts.size();
            mVerts.push_back(ab);
        }
    }
    
    int bcind = _f.b;
    if (bclen > mMaxEdgeLen) {
        ofVec3f bc = (b+c)*0.5;
        bcind = findVert(bc);
        if (bcind < 0) {
            bcind =mVerts.size();
            mVerts.push_back(bc);
        }
    }
    
    int caind = _f.c;
    if (calen > mMaxEdgeLen) {
        ofVec3f ca = (c+a)*0.5;
        caind = findVert(ca);
        if (caind < 0) {
            caind =mVerts.size();
            mVerts.push_back(ca);
        }
    }
    
    face A(_f.a,abind,caind);
    face B(_f.b,bcind,abind);
    face C(_f.c,caind,bcind);
    face D(abind,bcind,caind);
    
    if (uniqueFace(A)) trySubdivide(A);
    if (uniqueFace(B)) trySubdivide(B);
    if (uniqueFace(C)) trySubdivide(C);
    if (uniqueFace(D)) trySubdivide(D);
    
}

bool form::uniqueFace(face _f) {
    if (_f.a != _f.b && _f.a != _f.c && _f.b != _f.c) return true;
    return false;
}

int form::findVert(ofVec3f _v) {
    int ind = -1;
    
    for (int i=0; i<mVerts.size();i++) {
        if (mVerts[i].distance(_v) < mTol) return i;
    }
    
    return ind;
}

void form::exportModel() {
    mExporter.beginModel("blob sim");
    
    addMesh(mMesh);
    addMesh(mOuterMesh);
    
    // doing the edge by hand
    for (int i=1; i<mMasterEdgeParticles.size();i++) {
        ofPoint p0 = mBottomParticles[mMasterEdgeParticles[i-1]->array_ind]->getPosition();
        ofPoint p1 = mBottomParticles[mMasterEdgeParticles[i]->array_ind]->getPosition();
        ofPoint p2 = mParticles[mMasterEdgeParticles[i]->array_ind]->getPosition();
        ofPoint p3 = mParticles[mMasterEdgeParticles[i-1]->array_ind]->getPosition();
        
        mExporter.addTriangle(p0, p1, p2, ofPoint(0,0,0));
        mExporter.addTriangle(p0, p2, p3, ofPoint(0,0,0));
    }
    for (int i=1; i<mSlaveEdgeParticles.size();i++) {
        ofBeginShape();
        ofPoint p0 = mBottomParticles[mSlaveEdgeParticles[i-1]->array_ind]->getPosition();
        ofPoint p1 = mBottomParticles[mSlaveEdgeParticles[i]->array_ind]->getPosition();
        ofPoint p2 = mParticles[mSlaveEdgeParticles[i]->array_ind]->getPosition();
        ofPoint p3 = mParticles[mSlaveEdgeParticles[i-1]->array_ind]->getPosition();
        
        mExporter.addTriangle(p0, p1, p2, ofPoint(0,0,0));
        mExporter.addTriangle(p0, p2, p3, ofPoint(0,0,0));
    }

    
    mExporter.useASCIIFormat(false); //export as binary
    mExporter.saveModel(ofToDataPath("blob-to-print-"+ofGetTimestampString()+".stl"));
}

void form::addMesh(ofVboMesh &_mesh) {
    vector<ofVec3f> ns = _mesh.getNormals();
    vector<unsigned int> inds = _mesh.getIndices();
    vector<ofVec3f> vs = _mesh.getVertices();
    for (int i=0; i<inds.size();i+=3) {
        mExporter.addTriangle(vs[inds[i]], vs[inds[i+1]], vs[inds[i+2]], ofPoint(0,0,0));
    }
}