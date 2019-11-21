// Basé sur :
// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <QDebug>
#include <QSurfaceFormat>
#include <QMatrix4x4>



GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{
    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    sf.setSamples(16);
    setFormat(sf);

    setEnabled(true);                   // événements clavier et souris
    setFocusPolicy(Qt::StrongFocus);    // accepte focus
    setFocus();                         // donne le focus

    timer = new QTimer(this);
    timer->setInterval(20);           // msec
    connect (timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start();
    elapsedTimer.start();
}


GLArea::~GLArea()
{
    delete timer;

    // Contrairement aux méthodes virtuelles initializeGL, resizeGL et repaintGL,
    // dans le destructeur le contexte GL n'est pas automatiquement rendu courant.
    makeCurrent();
    tearGLObjects();
    doneCurrent();
}


void GLArea::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.2f,0.4f,1.0f,1.0f);
    glEnable(GL_DEPTH_TEST);

    makeGLObjects();

    // shader du sol
    program_sol = new QOpenGLShaderProgram(this);
    program_sol->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vsh");
    program_sol->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.fsh");
    if (! program_sol->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_sol->log();
    }
    program_sol->setUniformValue("texture", 0);

    // shader de billboard
    program_poisson = new QOpenGLShaderProgram(this);
    program_poisson->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/poisson.vsh");
    program_poisson->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/poisson.fsh");
    if (! program_poisson->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_poisson->log();
    }
    program_poisson->setUniformValue("texture", 0);
}


void GLArea::makeGLObjects()
{
    // Création du sol
    float tailleSol = 20.0f;
    float larg=banc.largeurAquarium/2.0f;
    float haut=banc.hauteurAquarium/2.0f;
    float prof=banc.profondeurAquarium/2.0f;
    GLfloat vertices_box[] = {
       -larg,-haut,-prof,
       +larg,-haut,-prof,
       -larg,-haut,+prof,
       +larg,-haut,+prof,
       -larg,+haut,-prof,
       +larg,+haut,-prof,
       -larg,+haut,+prof,
       +larg,+haut,+prof,

       -larg,-haut,-prof,
       -larg,-haut,+prof,
       -larg,+haut,-prof,
       -larg,+haut,+prof,
       +larg,-haut,-prof,
       +larg,-haut,+prof,
       +larg,+haut,-prof,
       +larg,+haut,+prof,

       -larg,-haut,-prof,
       -larg,+haut,-prof,
       +larg,-haut,-prof,
       +larg,+haut,-prof,
       -larg,-haut,+prof,
       -larg,+haut,+prof,
       +larg,-haut,+prof,
       +larg,+haut,+prof
    };

    GLfloat rgb_box[] = {
            200/255.0f, 0/255.0f, 0/255.0f,
            200/255.0f, 0/255.0f, 0/255.0f,
            200/255.0f, 0/255.0f, 0/255.0f,
            200/255.0f, 0/255.0f, 0/255.0f,
            255/255.0f, 0/255.0f, 0/255.0f,
            255/255.0f, 0/255.0f, 0/255.0f,
            255/255.0f, 0/255.0f, 0/255.0f,
            255/255.0f, 0/255.0f, 0/255.0f,
            0/255.0f, 200/255.0f, 0/255.0f,
            0/255.0f, 200/255.0f, 0/255.0f,
            0/255.0f, 200/255.0f, 0/255.0f,
            0/255.0f, 200/255.0f, 0/255.0f,
            0/255.0f, 255/255.0f, 0/255.0f,
            0/255.0f, 255/255.0f, 0/255.0f,
            0/255.0f, 255/255.0f, 0/255.0f,
            0/255.0f, 255/255.0f, 0/255.0f,
            0/255.0f, 0/255.0f, 200/255.0f,
            0/255.0f, 0/255.0f, 200/255.0f,
            0/255.0f, 0/255.0f, 200/255.0f,
            0/255.0f, 0/255.0f, 200/255.0f,
            0/255.0f, 0/255.0f, 255/255.0f,
            0/255.0f, 0/255.0f, 255/255.0f,
            0/255.0f, 0/255.0f, 255/255.0f,
            0/255.0f, 0/255.0f, 255/255.0f
        };

    QVector<GLfloat> vertData_sol;
    for (int i = 0; i < 24; ++i) {
        // coordonnées sommets
        for (int j = 0; j < 3; j++)
            vertData_sol.append(vertices_box[i*3+j]);
        // coordonnées texture
        for (int j = 0; j < 3; j++)
            vertData_sol.append(rgb_box[i*3+j]);
    }

    vbo_sol.create();
    vbo_sol.bind();
    vbo_sol.allocate(vertData_sol.constData(), vertData_sol.count() * int(sizeof(GLfloat)));


    // Création d'un poisson
    GLfloat vertices_poisson[] = {
       -0.5f, 0.0f, 0.0f,
        0.5f,-0.5f, 0.0f,
        0.5f, 0.5f, 0.0f

    };

    GLfloat texCoords_poisson[] = {
        0.0f, 0.5f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    QVector<GLfloat> vertData_poisson;
    for (int i = 0; i < 3; ++i) {
        // coordonnées sommets
        for (int j = 0; j < 3; j++)
            vertData_poisson.append(vertices_poisson[i*3+j]);
        // coordonnées texture
        for (int j = 0; j < 2; j++)
            vertData_poisson.append(texCoords_poisson[i*2+j]);
    }

    vbo_poisson.create();
    vbo_poisson.bind();
    vbo_poisson.allocate(vertData_poisson.constData(), vertData_poisson.count() * int(sizeof(GLfloat)));


    // Création de textures
    QImage image_sol(":/textures/ground.jpg");
    if (image_sol.isNull())
        qDebug() << "load image ground.jpg failed";
    textures[0] = new QOpenGLTexture(image_sol);

    QImage image_poisson(":/textures/poisson1.png");
    if (image_poisson.isNull())
        qDebug() << "load image poisson1.png failed";
    textures[1] = new QOpenGLTexture(image_poisson);
}


void GLArea::tearGLObjects()
{
    vbo_sol.destroy();
    vbo_poisson.destroy();
    for (int i = 0; i < 2; i++)
        delete textures[i];
}


void GLArea::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    windowRatio = float(w) / h;
}


void GLArea::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrice de projection
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, windowRatio, 1.0f, 1000.0f);

    // Matrice de vue (caméra)
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(xPos, yPos, zPos);
    viewMatrix.rotate(xRot, 1, 0, 0);
    viewMatrix.rotate(yRot, 0, 1, 0);
    viewMatrix.rotate(zRot, 0, 0, 1);

    // Affichage du sol
    vbo_sol.bind();
    program_sol->bind(); // active le shader program du sol

    QMatrix4x4 modelMatrixSol;
    modelMatrixSol.translate(0.0f, 0.0f, 0.0f);
    program_sol->setUniformValue("projectionMatrix", projectionMatrix);
    program_sol->setUniformValue("viewMatrix", viewMatrix);
    program_sol->setUniformValue("modelMatrix", modelMatrixSol);

    program_sol->setAttributeBuffer("in_position", GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    program_sol->setAttributeBuffer("colAttr", GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
    program_sol->enableAttributeArray("in_position");
    program_sol->enableAttributeArray("colAttr");

    glLineWidth(3);
    glDrawArrays(GL_LINES, 0, 24);

    program_sol->disableAttributeArray("in_position");
    program_sol->disableAttributeArray("colAttr");
    program_sol->release();


    // Affichage des poissons
    glDepthMask(GL_FALSE);
    vbo_poisson.bind();
    program_poisson->bind(); // active le shader program des poissons

    program_poisson->setUniformValue("projectionMatrix", projectionMatrix);
    program_poisson->setUniformValue("viewMatrix", viewMatrix);

    program_poisson->setAttributeBuffer("in_position", GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program_poisson->setAttributeBuffer("in_uv", GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    program_poisson->enableAttributeArray("in_position");
    program_poisson->enableAttributeArray("in_uv");

    textures[1]->bind();
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    banc.affiche(program_poisson);

    glDisable(GL_BLEND);
    textures[1]->release();

    program_poisson->disableAttributeArray("in_position");
    program_poisson->disableAttributeArray("in_uv");
    program_poisson->release();
    glDepthMask(GL_TRUE);
}


void GLArea::keyPressEvent(QKeyEvent *ev)
{
    float da = 0.1f;

    switch(ev->key()) {
    case Qt::Key_A :
        xRot -= da;
        break;

    case Qt::Key_Q :
        xRot += da;
        break;

    case Qt::Key_Z :
        yRot -= da;
        break;

    case Qt::Key_S :
        yRot += da;
        break;

    case Qt::Key_E :
        zRot -= da;
        break;

    case Qt::Key_D :
        zRot += da;
        break;
        }
    update();
}


void GLArea::keyReleaseEvent(QKeyEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->text();
}


void GLArea::mousePressEvent(QMouseEvent *ev)
{
    lastPos = ev->pos();
}


void GLArea::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y() << ev->button();
}


void GLArea::mouseMoveEvent(QMouseEvent *ev)
{
    int dx = ev->x() - lastPos.x();
    int dy = ev->y() - lastPos.y();

    if (ev->buttons() & Qt::LeftButton) {
        xRot += dy;
        yRot += dx;
        update();
    } else if (ev->buttons() & Qt::RightButton) {
        xPos += dx/10.0f;
        yPos -= dy/10.0f;
        update();
    } else if (ev->buttons() & Qt::MidButton) {
        xPos += dx/10.0f;
        zPos += dy;
        update();
    }

    lastPos = ev->pos();
}


void GLArea::onTimeout()
{
    static qint64 old_chrono = elapsedTimer.elapsed(); // static : initialisation la première fois et conserve la dernière valeur
    qint64 chrono = elapsedTimer.elapsed();
    dt = (chrono - old_chrono) / 1000.0f;
    old_chrono = chrono;
    banc.anime(dt);


    update();
}
