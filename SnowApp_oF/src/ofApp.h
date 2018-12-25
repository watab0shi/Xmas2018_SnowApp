#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxLibwebsockets.h"
#include "ofxUST.h"


// ofApp
//----------------------------------------
class ofApp : public ofBaseApp
{
public:
  void setup();
  void update();
  void draw();
  
  void keyPressed( int _key );
  void keyReleased( int _key );
  void mouseMoved( int _x, int _y );
  void mouseDragged( int _x, int _y, int _button );
  void mousePressed( int _x, int _y, int _button );
  void mouseReleased( int _x, int _y, int _button );
  void mouseEntered( int _x, int _y );
  void mouseExited( int _x, int _y );
  void windowResized( int _w, int _h );
  void dragEvent( ofDragInfo _dragInfo );
  void gotMessage( ofMessage _msg );
  void exit();
  
  
  // basic variables
  float                w, h;
  int                  frameRate;
  double               time;
  
  // sounds
  vector< ofSoundPlayer > sounds;
  float                   playTime;
  float                   lastPlayTime;
  float                   playInterval;
  deque< ofPtr< function< void() > > > queue;
  
  // serial
  ofSerial               serial;
  string                 readStr;
  ofColor                receivedColor;
  string                 writeStr, lastWriteStr;
  
  // ust
  ofxUST                   ust;
  ofRectangle              outerRect;
  ofRectangle              innerRect;
  vector< ofVec2f >        points;
  
  
  // websocket
  ofxLibwebsockets::Server server;
  int pxW, pxH;
  
  void sendPoints();
  
  void onConnect( ofxLibwebsockets::Event& args );
  void onOpen( ofxLibwebsockets::Event& args );
  void onClose( ofxLibwebsockets::Event& args );
  void onIdle( ofxLibwebsockets::Event& args );
  void onMessage( ofxLibwebsockets::Event& args );
  void onBroadcast( ofxLibwebsockets::Event& args );
  
  // gui
  ofxPanel             gui;
  ofParameterGroup     params;
  ofParameter< float > fps;
  
  ofParameterGroup     sensorParams;
  ofParameter< int >   screenWidth;
  ofParameter< int >   screenHeight;
  ofParameter< int >   screenPadding;
  ofParameter< int >   sensorOffsetY;
  ofParameter< bool >  bMirror;
  ofParameter< int >   skip;
  ofParameter< float > drawingScale;
  void                 skipChanged( int& _skip );
  
  bool                 visibleGui;
  ofFile               settingFile;
  void                 setupGui();
};
