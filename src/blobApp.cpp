#include "blobApp.h"
#include "edgeCurve.h"
#include "form.h"


//--------------------------------------------------------------
void blobApp::setup(){
    ofBackground(255,255,255);
	ofSetFrameRate(60);
    ofEnableSmoothing();
    ofSetCircleResolution(60);
    
    // 2D Setup
#ifdef _MSC_VER 
	float pi = 3.14159;
#endif
    vector<ofVec3f> bverts;
    float ang = 0;
    float r = 200;
    float sweep = pi*1.35;
    float segs = 7.0;
    for (int i=0; i<=segs; i++) {
        bverts.push_back(ofVec3f(cos(ang)*r,sin(ang)*r,0));
        ang += sweep/segs;
    }
    mMasterCurve = new edgeCurve(bverts);
    mSlaveCurve = mMasterCurve->getSlave();
    
    mFlatOs = ofVec2f(ofGetHeight()*0.5, ofGetHeight()*0.5);
    
    // 3D Setup
    mForm = new form(mMasterCurve,mSlaveCurve);
    
    // FBOs Setup
    m2DFbo.allocate(ofGetHeight(), ofGetHeight(),GL_RGB);
    m2DFbo.begin();
    ofClear(255,255,255);
    m2DFbo.end();
    
    
    float stdsz = 20;
    // GUI Setup
    ofxUIColor pink = ofxUIColor(255,0,128);
    ofxUIColor pinktrans = ofxUIColor(255,0,128,150);
    ofxUIColor trans = ofxUIColor(255,255,255,0);
    ofxUIColor grey = ofxUIColor(90,90,90);
    
    mResolutionGUI = new ofxUICanvas();
    mResolutionGUI->setFont("GUI/Gotham-Bold.ttf");
    mResolutionGUI->setUIColors(grey, trans, trans, pinktrans, pink, pink, pink);
    mResolutionGUI->setDrawBack(false);
    mResSlider = new ofxUIIntSlider("resolution", 2, 11, 7, 200, stdsz/4.0, stdsz,ofGetHeight()-stdsz*2);
    mResolutionGUI->addWidget(mResSlider);
    mShowZipperToggle = new ofxUIToggle("show cut edge",false, stdsz,stdsz,stdsz,ofGetHeight()-stdsz*3.5);
    mResolutionGUI->addWidget(mShowZipperToggle);
    mExportCutFileButton = new ofxUIButton("export cut file",false,stdsz,stdsz,stdsz,ofGetHeight()-stdsz*5);
    mResolutionGUI->addWidget(mExportCutFileButton);
    mResolutionGUI->autoSizeToFitWidgets();
    ofAddListener(mResolutionGUI->newGUIEvent, this, &blobApp::guiEvent);
    
    float hx = ofGetHeight()-stdsz*2;
    float at = ofGetHeight()-stdsz*9.5;
    mHoleGUI = new ofxUICanvas();
    mHoleGUI->setFont("GUI/Gotham-Bold.ttf");
    mHoleGUI->setUIColors(grey, trans, trans, pinktrans, pink, pink, pink);
    mHoleGUI->setDrawBack(false);
    mHoleToggle = new ofxUIToggle("add holes", false, stdsz,stdsz,hx,at);
    mHoleGUI->addWidget(mHoleToggle);
    at += stdsz*1.5;
    mMirrorHolesToggle = new ofxUIToggle("mirror hole", true, stdsz,stdsz,hx,at);
    mHoleGUI->addWidget(mMirrorHolesToggle);
    /*at += stdsz*1.5;
    mFreeSlaveHoleToggle =new ofxUIToggle("free second hole",false, stdsz,stdsz,hx,at);
    mHoleGUI->addWidget(mFreeSlaveHoleToggle);*/
    at += stdsz*2;
    mHolePosSlider = new ofxUISlider("hole position", 0.001, 0.999, 0.501,200,stdsz/4.0,hx-200+stdsz,at);
    mHoleGUI->addWidget(mHolePosSlider);
    at += stdsz*2;
    mHoleSizeSlider = new ofxUISlider("hole size", 5, 100, 45, 200,stdsz/4.0,hx-200+stdsz,at);
    mHoleGUI->addWidget(mHoleSizeSlider);
    at += stdsz*2;
    mHoleOSSlider = new ofxUISlider("hole offset", 5, 100, 10,200,stdsz/4.0,hx-200+stdsz,at);
    mHoleGUI->addWidget(mHoleOSSlider);
    
    mHoleGUI->autoSizeToFitWidgets();
    ofAddListener(mHoleGUI->newGUIEvent, this, &blobApp::guiEvent);
    
    hx = ofGetWidth()*0.5+stdsz*2;
    at = ofGetHeight()-stdsz*6.5;
    m3DGUI = new ofxUICanvas();
    m3DGUI->setFont("GUI/Gotham-Bold.ttf");
    m3DGUI->setUIColors(grey, trans, trans, pinktrans, pink, pink, pink);
    m3DGUI->setDrawBack(false);
    mUpdateButton = new ofxUIButton("update", false, stdsz,stdsz,hx,at);
    m3DGUI->addWidget(mUpdateButton);
    at += stdsz*2;
    mSimulateToggle = new ofxUIToggle("simulate", false, stdsz,stdsz,hx,at);
    m3DGUI->addWidget(mSimulateToggle);
    at += stdsz*2;
    mExportSTLButton = new ofxUIButton("export stl", false, stdsz,stdsz,hx,at);
    m3DGUI->addWidget(mExportSTLButton);
    
    m3DGUI->autoSizeToFitWidgets();
    ofAddListener(m3DGUI->newGUIEvent, this, &blobApp::guiEvent);
    
    
    
}

//--------------------------------------------------------------
void blobApp::update(){
    if (mMasterCurve) {
        mMasterCurve->update();
        mMasterCurve->updateHole(mHolePosSlider->getScaledValue(), mHoleOSSlider->getScaledValue(), mHoleSizeSlider->getScaledValue());
    }
    
    if (mSimulateToggle->getValue())
        mForm->updateWorld();
    
    m2DFbo.begin();
    ofEnableSmoothing();
    ofSetCircleResolution(60);
    ofBackground(220);
    ofPushMatrix();
    ofTranslate(mFlatOs.x, mFlatOs.y,0);
    if (mMasterCurve) mMasterCurve->draw();
    if (mSlaveCurve) mSlaveCurve->draw();
    ofPopMatrix();
    m2DFbo.end();
    
    
}

//--------------------------------------------------------------
void blobApp::draw(){
    if (ofGetFrameNum() == 0) {
        mHoleToggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
        mMirrorHolesToggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
        //mFreeSlaveHoleToggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    }
    
    
    ofBackground(185);
    
    ofSetColor(255);
    m2DFbo.draw(0,0);
    mForm->draw();
    
    // GUI
    ofSetColor(90);
    ofFill();
    float b = 12;
    ofRect(ofGetWidth()*0.5-b*0.5,0,b, ofGetHeight());
    
    if (mSimulateToggle->getValue()) {
    ofSetColor(255, 0, 128);
    ofTrueTypeFont font;
    font.loadFont("GUI/Gotham-Bold.ttf", 12);
    font.drawString("tap the 'z' key to zip the from up and 'x' to unzip.\n\n'c' releases the corners", ofGetHeight()+40,40);
    }
    
}

//--------------------------------------------------------------
void blobApp::keyPressed(int _key){
    if (_key == 'z') mForm->zipNext();
    
    if (_key == 'x') mForm->unzipLast();
    
    if (_key == 'c') mForm->releaseCorners();
}

//--------------------------------------------------------------
void blobApp::keyReleased(int _key){
    
}

//--------------------------------------------------------------
void blobApp::mouseMoved(int _x, int _y){
    if (mMasterCurve) mMasterCurve->mouseMoved(_x-mFlatOs.x, _y-mFlatOs.y);
    if (mSlaveCurve) mSlaveCurve->mouseMoved(_x-mFlatOs.x, _y-mFlatOs.y);

}

//--------------------------------------------------------------
void blobApp::mouseDragged(int _x, int _y, int _button){
    if (mMasterCurve) mMasterCurve->mouseDragged(_x-mFlatOs.x, _y-mFlatOs.y, _button);
    if (mSlaveCurve) mSlaveCurve->mouseDragged(_x-mFlatOs.x, _y-mFlatOs.y, _button);

}

//--------------------------------------------------------------
void blobApp::mousePressed(int _x, int _y, int _button){
    if (mMasterCurve) mMasterCurve->mousePressed(_x-mFlatOs.x, _y-mFlatOs.y, _button);
    if (mSlaveCurve) mSlaveCurve->mousePressed(_x-mFlatOs.x, _y-mFlatOs.y, _button);

}

//--------------------------------------------------------------
void blobApp::mouseReleased(int _x, int _y, int _button){
    if (mMasterCurve) mMasterCurve->mouseReleased(_x-mFlatOs.x, _y-mFlatOs.y, _button);
    if (mSlaveCurve) mSlaveCurve->mouseReleased(_x-mFlatOs.x, _y-mFlatOs.y, _button);

}

//--------------------------------------------------------------
void blobApp::windowResized(int _w, int _h){

}

//--------------------------------------------------------------
void blobApp::gotMessage(ofMessage _msg){

}

//--------------------------------------------------------------
void blobApp::dragEvent(ofDragInfo _dragInfo){

}

void blobApp::exit()
{
    if (mMasterCurve) delete mMasterCurve;
    if (mSlaveCurve) delete mSlaveCurve;
    delete mHoleGUI;
    delete mResolutionGUI;
}

void blobApp::guiEvent(ofxUIEventArgs &_e)
{
    if (_e.getName() == mResSlider->getName()) {
        int vcnt = mResSlider->getScaledValue();
        if (mMasterCurve) mMasterCurve->resample(vcnt);
    }
    else if (_e.getName() == mHoleToggle->getName()) {
        if (mMasterCurve) {
            mMasterCurve->removeHole(true, mMirrorHolesToggle->getValue());
            if (mHoleToggle->getValue()) {
                mMasterCurve->addHole(mHolePosSlider->getScaledValue(), mHoleOSSlider->getScaledValue(), mHoleSizeSlider->getScaledValue(), mMirrorHolesToggle->getValue());
            }
        }
    }
    else if (_e.getName() == mMirrorHolesToggle->getName()) {
        if (mMasterCurve) {
            mMasterCurve->removeHole(false,!mMirrorHolesToggle->getValue());
            if (mMirrorHolesToggle->getValue() == true) {
                mHoleToggle->setName("add holes");
                mMasterCurve->removeHole(true, true);
                mMasterCurve->addHole(mHolePosSlider->getScaledValue(), mHoleOSSlider->getScaledValue(), mHoleSizeSlider->getScaledValue(), mMirrorHolesToggle->getValue());
            } else {
                mHoleToggle->setName("add hole");
            }
        }
    }
    else if (_e.getName() == mUpdateButton->getName() && mUpdateButton->getValue()) {
        mForm->update();
    }
    else if (_e.getName() == mShowZipperToggle->getName()) {
        mMasterCurve->showZipper(mShowZipperToggle->getValue());
    }
    else if (_e.getName() == mExportCutFileButton->getName() && mExportCutFileButton->getValue()) {
        ofBeginSaveScreenAsPDF("blob-to-cut-"+ofGetTimestampString()+".pdf", false);
        ofClear(255,255,255);
        ofPushMatrix();
        ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
        mMasterCurve->mZipperPath->draw();
        ofPopMatrix();
        ofEndSaveScreenAsPDF();
    }
    else if (_e.getName() == mExportSTLButton->getName() && mExportSTLButton->getValue()) {
        mForm->exportModel();
    }
}
