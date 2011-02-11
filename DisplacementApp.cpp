#include "cinder/app/AppBasic.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
// #include "cinder/Capture.h"

#include "helper.h"
#include "ParticleController.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;

using std::vector;

#define VWIDTH 640
#define VHEIGHT 480

class TestApp : public AppBasic {
public:
    void prepareSettings( Settings *settings );
    void setup();
    void update();
    void draw();
    void keyDown( KeyEvent event );

    gl::Texture  mImage;
    CameraPersp  mCamera;
    gl::VboMesh  mVboMesh;
    gl::GlslProg mShader;
    // Capture      mCapture;
};

void TestApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize(1024, 768);
    settings->setFrameRate(60.0f);
}

void TestApp::setup()
{
    gl::enableDepthRead();
    gl::enableDepthWrite();

    try {
        mShader = gl::GlslProg( loadResource( RES_VERT ), loadResource( RES_FRAG ) );
    }
    catch( gl::GlslProgCompileExc &exc ) {
        std::cout << "Shader compile error: " << std::endl;
        std::cout << exc.what();
    }
    catch( ... ) {
        std::cout << "Unable to load shader" << std::endl;
    }

    int cVertexes = VWIDTH * VHEIGHT;
    int cQuads = ( VWIDTH - 1 ) * ( VHEIGHT - 1 );

    gl::VboMesh::Layout layout;
    layout.setStaticIndices();
    layout.setStaticPositions();
    layout.setStaticTexCoords2d();
    layout.setStaticNormals();

    mVboMesh = gl::VboMesh( cVertexes, cQuads * 4, layout, GL_QUADS );

    vector<uint32_t> indices;
    vector<Vec3f>    positions;
    vector<Vec2f>    texcoords;
    vector<Vec3f>    normals;

    for ( int x = 0; x < VWIDTH; ++x) {
        for ( int y = 0; y < VHEIGHT; ++y ) {
            if ( ( x < VWIDTH - 1 ) && ( y < VHEIGHT - 1 ) ) {
                indices.push_back( (x+0) * VHEIGHT + (y+0) );
                indices.push_back( (x+1) * VHEIGHT + (y+0) );
                indices.push_back( (x+1) * VHEIGHT + (y+1) );
                indices.push_back( (x+0) * VHEIGHT + (y+1) );
            }
            positions.push_back( Vec3f( (x / (float) VWIDTH) - 0.5f, (y / (float) VHEIGHT) - 0.5f, 0 ) );
            normals.push_back( Vec3f( 0, 0, 1.0f ) );
            texcoords.push_back( Vec2f( x / (float) VWIDTH, 1.0f - y / (float) VHEIGHT ) );
        }
    }

    mVboMesh.bufferIndices( indices );
    mVboMesh.bufferPositions( positions );
    mVboMesh.bufferTexCoords2d( 0, texcoords );
    mVboMesh.bufferNormals( normals );

    try {
        gl::Texture::Format format;
        format.setInternalFormat(GL_RGBA_FLOAT32_ATI);
        //  not required with newer cards
        format.setMagFilter(GL_NEAREST);
        format.setMinFilter(GL_NEAREST);
        //mImage = gl::Texture( loadImage( loadResource( RES_TEXTURE_PNG ) ), format );
    }
    catch( ... ) {
        std::cout << "unable to load the texture file!" << std::endl;
    }

    // mCapture = getAvailableCapture(VWIDTH, VHEIGHT);

	mCamera.lookAt( Vec3f( 0, 0, -3 ), Vec3f::zero() );
}

void TestApp::update()
{
    if (mCapture && mCapture.checkNewFrame()) {
        mImage = gl::Texture( mCapture.getSurface() );
    }

    gl::setMatrices(mCamera);
}

void TestApp::draw()
{	
    gl::clear( Color( 0.15f, 0.15f, 0.15f ) );

    mImage.bind(0);
    mShader.bind();
    mShader.uniform("tex", 0);
    gl::pushMatrices();
    gl::scale( Vec3f( 4, 4, 3 ) );
    // gl::color( Color( 1.0f, 1.0f, 0 ) );
    gl::draw( mVboMesh );
    gl::popMatrices();
    mShader.unbind();
    mImage.unbind();
}

void TestApp::keyDown( KeyEvent event )
{
    if (event.getCode() == app::KeyEvent::KEY_ESCAPE) {
        this->quit();
    }
}

CINDER_APP_BASIC( TestApp, RendererGl )

