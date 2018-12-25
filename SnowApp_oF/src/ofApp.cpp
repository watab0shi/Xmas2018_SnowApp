#include "ofApp.h"


// setup
//----------------------------------------
void ofApp::setup()
{
  // basic variables
  w          = ofGetWidth();
  h          = ofGetHeight();
  frameRate  = 60;
  time       = 0.0;
  
  // sounds
  ofDirectory dir( "sounds" );
  dir.allowExt( "mp3" );
  dir.listDir();
  
  for( auto& f : dir.getFiles() )
  {
    sounds.emplace_back();
    sounds.back().load( f.getAbsolutePath() );
    sounds.back().setMultiPlay( true );
    sounds.back().setLoop( false );
    sounds.back().setSpeed( 1.5 );
  }
  playTime     = 0.0;
  lastPlayTime = 0.0;
  playInterval = 1.0 / 8;
  
  
  // serial
  readStr      = "";
  writeStr     = "";
  lastWriteStr = "";
  
  serial.listDevices();
  serial.setup( 0, 9600 );
  
  // websocket
  ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
  options.port    = 9092;
  options.bUseSSL = false;
  server.setup( options );
  
  server.addListener(this);
  
  // gui
  setupGui();
  
  // ust
  pxW = pxH = 0;
  ust.setDirection( ofxUST::DIRECTION_UP );
  ust.setMirror( bMirror );
  ust.setScanningParameterByAngles( -135, 135, skip );
  
  if( ust.isConnected() ) ust.startMeasurement();
  else                    ofLog() << "Connection failed!";
  
  
  // application settings
  ofSetVerticalSync( true );
  ofDisableAntiAliasing();
  ofShowCursor();
  
//  ofSetFrameRate( frameRate );
  ofBackground( 0 );
}


// update
//----------------------------------------
void ofApp::update()
{
  time        += ofGetLastFrameTime();
  fps          = floor( ofGetFrameRate() * 10 ) / 10;
  drawingScale = floor( drawingScale * 100 ) / 100;
  
  // read serial message
  while( true )
  {
    int c = serial.readByte();
    
    if( c == OF_SERIAL_NO_DATA || c == OF_SERIAL_ERROR || c == 0 ) break;
    
    if( c == '\n' )
    {
      vector< string > valStr = ofSplitString( readStr, "," );
      if( valStr.size() == 3 )
      {
        int r = ofToInt( valStr.at( 0 ) );
        int g = ofToInt( valStr.at( 1 ) );
        int b = ofToInt( valStr.at( 2 ) );
        receivedColor.set( r, g, b );
        
        ofLog() << "[ -> ] " << r << ", " << g << ", " << b;
      }
      readStr = "";
    }
    else
    {
      readStr.push_back( c );
    }
  }
  
  // sound
  if( ( time - lastPlayTime ) > playInterval && !queue.empty() )
  {
    lastPlayTime = time;
//    playTime     = time;
    for( auto& q : queue ) q->operator()();
    queue.clear();
  }
  
  // ust
  outerRect.setFromCenter( 0, -screenHeight * 0.5, screenWidth, screenHeight-2 );
  innerRect.setFromCenter( 0, -screenHeight * 0.5, screenWidth - screenPadding * 2, screenHeight - screenPadding * 2 );
  
  ust.setMirror( bMirror );
  ust.update();
  
  points.clear();
  for( auto p : ust.coordinates )
  {
    ofVec2f p_  = p + ofVec2f( 0, -sensorOffsetY );
    if( innerRect.inside( p_ ) ) points.push_back( p_ );
  }
  
  if( !points.empty() && pxW > 0 && pxH > 0 )
  {
    sendPoints();
  }
}


// sendPoints
//----------------------------------------
void ofApp::sendPoints()
{
  Json::Value      json;
  Json::FastWriter writer;
  json["message"] = "touched";
  
  string str = "[";
  int i = 0;
  for( auto& p : points )
  {
    int x = ( int )ofMap( p.x, innerRect.getLeft(), innerRect.getRight(), 0, pxW, true );
    int y = ( int )ofMap( p.y, innerRect.getTop(), innerRect.getBottom(), 0, pxH, true );
    
    if( i > 0 ) str += ",";
    str += ofToString( x );
    str += ",";
    str += ofToString( y );
    ++i;
  }
  str += "]";
  json["value"] = str;
  
  server.send( writer.write( json ) );
}

// draw
//----------------------------------------
void ofApp::draw()
{
  float s = drawingScale;
  ofPushMatrix();
  {
    ofTranslate( ofGetWidth() / 2, ofGetHeight() );
    ofScale( s, s );
    
    ofSetColor( 120 );
    glBegin( GL_LINES );
    for( auto& p : ust.coordinates )
    {
      glVertex2f( 0, -sensorOffsetY );
      glVertex2f( p.x, p.y - sensorOffsetY );
    }
    glEnd();
    
    // sensor circle
    ofNoFill();
    ofSetColor( 255, 160, 0 );
    ofDrawCircle( 0, -sensorOffsetY, 12 / s );
    ofDrawLine( -5 / s, -sensorOffsetY, 5 / s, -sensorOffsetY );
    ofDrawLine( 0, -sensorOffsetY - 5 / s, 00, -sensorOffsetY + 5 / s );
    
    // inside points
    ofPushStyle();
    {
      ofSetCircleResolution( 6 );
      ofFill();
      ofSetColor( 0, 255, 0 );
      for( auto& p : points ) ofDrawCircle( p, 2 / s );
    }
    ofPopStyle();
    
    // outer rect
    ofNoFill();
    ofSetColor( 255, 90 );
    ofDrawRectangle( outerRect );
    
    // inner rect
    ofSetColor( 0, 255, 0, 120 );
    ofDrawRectangle( innerRect );
  }
  ofPopMatrix();
  
  
  if( visibleGui ) gui.draw();
}


// keyPressed
//----------------------------------------
void ofApp::keyPressed( int _key )
{
  // gui
  if( _key == ' ' )
  {
    visibleGui = !visibleGui;
    ( visibleGui ) ? ofShowCursor(): ofHideCursor();
  }
  
  
  // fullscreen
  if( _key == 'f' ) ofToggleFullscreen();
}


// keyReleased
//----------------------------------------
void ofApp::keyReleased( int _key )
{
  
}


// mouseMoved
//----------------------------------------
void ofApp::mouseMoved( int _x, int _y )
{
//  Json::Value      json;
//  Json::FastWriter writer;
//  json["message"]    = "mouseMoved";
//  json["value"]["x"] = _x;
//  json["value"]["y"] = _y;
//  
//  server.send( writer.write( json ) );
}


// mouseDragged
//----------------------------------------
void ofApp::mouseDragged( int _x, int _y, int _button )
{
  
}


// mousePressed
//----------------------------------------
void ofApp::mousePressed( int _x, int _y, int _button )
{
//  Json::Value      json;
//  Json::FastWriter writer;
//  json["message"]    = "mousePressed";
//  json["value"]["x"] = _x;
//  json["value"]["y"] = _y;
//  
//  server.send( writer.write( json ) );
}


// mouseReleased
//----------------------------------------
void ofApp::mouseReleased( int _x, int _y, int _button )
{
  
}


// mouseEntered
//----------------------------------------
void ofApp::mouseEntered( int _x, int _y )
{
  
}


// mouseExited
//----------------------------------------
void ofApp::mouseExited( int _x, int _y )
{
  
}


// windowResized
//----------------------------------------
void ofApp::windowResized( int _w, int _h )
{
  w = _w;
  h = _h;
}


// dragEvent
//----------------------------------------
void ofApp::dragEvent( ofDragInfo _dragInfo )
{
  
}


// gotMessage
//----------------------------------------
void ofApp::gotMessage( ofMessage _msg )
{
  
}


// exit
//----------------------------------------
void ofApp::exit()
{
  // save gui settings
  gui.saveToFile( settingFile.getAbsolutePath() );
}


// setupGui
//----------------------------------------
void ofApp::setupGui()
{
  int hue = 0;// gui color
  
  ofxGuiSetFont( "gui/Menlo-Italic.ttf", 10 );
  ofxGuiSetDefaultWidth( 255 );
  ofxGuiSetHeaderColor( ofColor::fromHsb( hue, 255, 180 ) );
  ofxGuiSetFillColor( ofColor::fromHsb( hue, 255, 180 ) );
  ofxGuiSetBackgroundColor( ofColor( 36 ) );
  
  sensorParams.setName( "Sensor" );
  sensorParams.add( screenWidth.set( "ScreenWidth",    5500, 0, 6000 ) );
  sensorParams.add( screenHeight.set( "ScreenHeight",  2470, 0, 6000 ) );
  sensorParams.add( screenPadding.set( "ScreenPadding",   0, 0,  500 ) );
  sensorParams.add( sensorOffsetY.set( "SensorY",         0, 0,  500 ) );
  sensorParams.add( bMirror.set( "Mirror", true ) );
  sensorParams.add( skip.set( "Skip", 1, 1, 5 ) );
  sensorParams.add( drawingScale.set( "DrawingScale", 0.15, 0.1, 1.0 ) );
  
  skip.addListener( this, &ofApp::skipChanged );
  
  params.setName( "Xmas2018_WS_Serial_UST_Sound" );
  params.add( fps.set( "FPS", 0, 0, 60 ) );
  params.add( sensorParams );
  
  gui.setup( params );
  gui.setUseTTF( true );
  gui.setSize( 255, gui.getShape().height + 10 );
  gui.setHeaderBackgroundColor( ofColor::fromHsb( hue, 255, 180 ) );
  
  // load gui settings
  settingFile = ofFile( ofToDataPath( "gui/settings.xml" ) );
  if( settingFile.exists() ) gui.loadFromFile( settingFile.getAbsolutePath() );
  
  visibleGui = true;
}


// skipChanged
//----------------------------------------
void ofApp::skipChanged( int& _skip )
{
  ust.stopMeasurement();
  ust.setScanningParameterBySteps( ust.getMinStep(), ust.getMaxStep(), skip );
  ust.startMeasurement();
}


// onConnect
//----------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event& args )
{
  ofLog() << "[ws:onConnect] ";
}

// onOpen
//----------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event& args )
{
  ofLog() << "[ws:onOpen] ";
}

// onClose
//----------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args )
{
  ofLog() << "[ws:onClose] ";
}

// onIdle
//----------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args )
{
//  ofLog() << "[ws:onIdle] ";
}

// onMessage
//----------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args )
{
  string       jsonStr = args.message;
  Json::Value  json;
  Json::Reader reader;
  
  reader.parse( jsonStr, json );
  ofLog() << "[ws:onMessage] " << jsonStr;
  
  if( json["message"] == "size" )
  {
    pxW = json["value"]["w"].asInt();
    pxH = json["value"]["h"].asInt();
  }
  
  if( json["message"] == "hit" )
  {
    float r = json["value"]["r"].asFloat();
    float g = json["value"]["g"].asFloat();
    float b = json["value"]["b"].asFloat();
    ofFloatColor fc( r, g, b );
    ofColor      c = ofColor::fromHsb( fc.getHue() * 255, 220, 255 );
    
    lastWriteStr = writeStr;
    
    writeStr  = ofToHex( c.r );
    writeStr += ofToHex( c.g );
    writeStr += ofToHex( c.b );
    writeStr += ";";
    
    if( writeStr != lastWriteStr )
    {
      ofLog() << "[ <- ] " << writeStr;
      serial.writeBytes( ( unsigned char* )writeStr.c_str(), writeStr.size() );
    }
    
    int idx = ( int )ofRandom( sounds.size() );
    
    queue.push_back( make_shared< function< void() > >( [ this, idx ](){
      sounds.at( idx ).play();
    } ) );
    ofLog() << queue.size();
  }
}

// onBroadcast
//----------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event& args )
{
//  ofLog() << "[ws:onBroadcast] ";
}
