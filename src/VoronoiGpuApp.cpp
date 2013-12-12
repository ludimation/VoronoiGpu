#include "cinder/app/AppBasic.h"
#include "cinder/ip/Hdr.h"
#include "cinder/gl/Texture.h"
#include "VoronoiGpu.h"

#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;

class VoronoiGpuApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings ) { settings->enableHighDensityDisplay( true ); }
	void setup();
	void calculateVoronoiTexture();
	
	void mouseDown( MouseEvent event );
    void mouseDrag( MouseEvent event);
	void keyDown( KeyEvent event );
	
	void update();
	void draw();

    int winH;
    int winW;
    Vec2f winC;
    float scale;
    Vec2f offset;
    Vec2f newLoc;

	vector<Vec2f>	mPoints;
	gl::Texture		mTexture;
	bool			mShowDistance;
};

void VoronoiGpuApp::setup()
{
    int winH = (float)getWindowHeight();
    int winW = (float)getWindowWidth();
    float scale = 0.5f;
    
    winC.x = winW / 2.0f;
    winC.y = winH / 2.0f;
    offset.x = newLoc.x = 0.0f;
    offset.y = newLoc.y = 0.0f;
    
    /*
     // debugging
     cout
     << "===="                   << endl
     << "setup(): executing"    << endl
     << "winH = " << winH        << endl
     << "winW = " << winW        << endl
     << "winC = " << winC        << endl
     << "scale = " << scale      << endl
     << "offset = " << offset    << endl
     << "newLoc = " << newLoc    << endl
     ;//*/

    
	mShowDistance = true;
	// register window changed display callback
	getWindow()->getSignalDisplayChange().connect( [this] { calculateVoronoiTexture(); } );
	
    mPoints.push_back( Vec2f( 100, 100 ) );
    mPoints.push_back( Vec2f( 200, 120 ) );
    mPoints.push_back( Vec2f( 130, 140 ) );	
    mPoints.push_back( Vec2f( 200, 200 ) );
    
    calculateVoronoiTexture();
}

void VoronoiGpuApp::calculateVoronoiTexture()
{
	if( mShowDistance ) {
		Channel32f rawDistanceMap = calcDistanceMapGpu( mPoints, toPixels( getWindowWidth() ), toPixels( getWindowHeight() ) );
		// we need to convert the raw distances into a normalized range of 0-1 so we can show them sensibly
		ip::hdrNormalize( &rawDistanceMap );
		mTexture = gl::Texture( rawDistanceMap );
	}
	else {
		Surface32f rawDistanceMap = calcDiscreteVoronoiGpu( mPoints, toPixels( getWindowWidth() ), toPixels( getWindowHeight() ) );
		// we need to convert the site locations into a normalized range of 0-1 so we can show them sensibly
		ip::hdrNormalize( &rawDistanceMap );
		mTexture = gl::Texture( rawDistanceMap );
	}
}

void VoronoiGpuApp::mouseDown( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
	calculateVoronoiTexture();
}

void VoronoiGpuApp::mouseDrag( MouseEvent event )
{
	//mouseDown( event );
}


void VoronoiGpuApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'd' ) {
		mShowDistance = ! mShowDistance;
		calculateVoronoiTexture();
	}
	else if( event.getChar() == 'x' ) {
		mPoints.clear();
		calculateVoronoiTexture();
	}
}

void VoronoiGpuApp::update()
{
    // move each point further away from the center of the screen
    for (int i=0; i < mPoints.size(); i++){
        // calculate new position based on scaled offset from center of screen
        offset.x = mPoints[i].x - winC.x;
        offset.y = mPoints[i].y - winC.y;
        offset.x *= scale;
        offset.y *= scale;
        newLoc.x = mPoints[i].x + offset.x;
        newLoc .y= mPoints[i].y + offset.y;
        
        // remove any points that are one screen away from the edge of the screen
        if (newLoc.x > winW * 2 || newLoc.x < -winW || newLoc.y > winH * 2 || newLoc.y < -winH ) {
            mPoints.erase (mPoints.begin() + i);
        } else {
            mPoints[i].x = newLoc.x;
            mPoints[i].y = newLoc.y;
        }

        /*
        // debugging
        cout
        << "--"                             << endl
        << "moving points"                  << endl
        << "i = " << i                      << endl
        << "mPoints[i] = " << mPoints[i]    << endl
        << "offset = " << offset            << endl
        << "newLoc = " << newLoc            << endl
        ;// */
    }
    
    // update texture
    calculateVoronoiTexture();
}

void VoronoiGpuApp::draw()
{
	gl::clear();
	gl::setMatricesWindow( getWindowSize() );
	
	gl::color( Color( 1, 1, 1 ) );
	if( mTexture ) {
		gl::draw( mTexture, toPoints( mTexture.getBounds() ) );
		mTexture.disable();
	}
	
	// draw the voronoi sites in yellow
	gl::color( Color( 1.0f, 1.0f, 0.0f ) );	
	for( vector<Vec2f>::const_iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		gl::drawSolidCircle( Vec2f( *ptIt ), 2.0f );
	
	gl::enableAlphaBlending();
	gl::drawStringRight( "Click to add a point", Vec2f( getWindowWidth() - toPixels( 10 ), getWindowHeight() - toPixels( 20 ) ), Color( 1, 0.3, 0 ), Font( "Arial", toPixels( 12 ) ) );
	gl::disableAlphaBlending();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( VoronoiGpuApp, RendererGl )
