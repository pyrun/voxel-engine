#include "opengl.h"

Opengl::Opengl( QWidget *parent ) : QGLWidget( parent )
{

}

Opengl::~Opengl()
{
    delete m_shader;
    delete m_camera;
}

void Opengl::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    if (event->orientation() == Qt::Horizontal) {

    } else {
        //m_camera->MoveForward( -(float)numSteps*2);
        m_camera->GetPos().x += numSteps*0.5;
    }
    event->accept();
}

void Opengl::timerEvent(QTimerEvent *)
{
    update();
    //m_camera->GetPos().z = -1;
    //m_camera->MoveForward( -0.01);

    float l_cos = -0.5;

    //m_camera->GetPos().x = l_cos;
}

void Opengl::paintGL() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_obj->draw( m_shader, m_camera);
}

void Opengl::initializeGL()
{
    glewInit();

    // shader
    m_shader = new Shader( "shader/object");

    // camera
    m_camera = new Camera(glm::vec3( -1.0f, 0.0f, -1.0f), graphic_fov, (float)width()/(float)height(), graphic_znear, graphic_zfar);

    glClearColor(0.5, 0.5, 0.5, 1);

    initShaders();
    //initTextures();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    // Use QBasicTimer because its faster than QTimer
    m_timer.start(12, this);

    m_obj = new Object();
    m_obj->Init();
}


void Opengl::initShaders() {
    // Compile vertex shader
    /*if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "shader/object.vs"))
        close();

    // Compile fragment shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "shader/object.fs"))
        close();

    // Link shader pipeline
    if (!m_program.link())
        close();

    // Bind shader pipeline for use
    if (!m_program.bind())
        close();*/

}
