#pragma once

#include "ofMain.h"
#include "ofxUI.h"

class edgeCurve;
class form;

class blobApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void exit();
    void guiEvent(ofxUIEventArgs &e);
    
private:
    // 2d
    edgeCurve *mMasterCurve;
    edgeCurve *mSlaveCurve;
    ofVec2f mFlatOs;
    
    ofxUICanvas *mResolutionGUI;
    ofxUIToggle *mShowZipperToggle;
    ofxUIButton *mExportCutFileButton;
    ofxUIIntSlider *mResSlider;
    
    ofxUICanvas *mHoleGUI;
    ofxUIToggle *mHoleToggle;
    ofxUIToggle *mMirrorHolesToggle;
    ofxUIToggle *mFreeHolesToggle;
    ofxUISlider *mHoleSizeSlider;
    ofxUISlider *mHoleOSSlider;
    ofxUISlider *mHolePosSlider;
    
    ofxUICanvas *m3DGUI;
    ofxUIButton *mUpdateButton;
    ofxUIToggle *mSimulateToggle;
    ofxUIButton *mExportSTLButton;
    
    // 3d
    form *mForm;
    
    // bg
    ofFbo m2DFbo;
    float mBorder;
    
    // util
    bool mSaveFrame;
};
