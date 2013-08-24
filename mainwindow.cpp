#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_velocity.setX(0);
    m_velocity.setY(0);
    m_velocity.setZ(1);
    m_thread_cam=new QThread();
    m_thread_algo=new QThread();
    TrackModel model;
    Q_ASSERT(track_library::io::populateTrackFromFile(model,"track/track.xml"));
    m_camera=new CameraSimulator(model);
    m_camera->moveToThread(m_thread_cam);
    connect(m_camera,SIGNAL(cameraResponse(QByteArray)),this,SLOT(onCameraResponse(QByteArray)));
    connect(m_thread_cam,SIGNAL(started()),m_camera,SLOT(init()));
    connect(m_camera,SIGNAL(unloaded()),m_thread_cam,SLOT(quit()));
    connect(m_camera,SIGNAL(unloaded()),m_camera,SLOT(deleteLater()));
    connect(this,SIGNAL(simulatorResponse(QMap<QString,QVariant>)),m_camera,SLOT(onSimulatorResponse(QMap<QString,QVariant>)));
    connect(m_thread_cam,SIGNAL(finished()),m_thread_cam,SLOT(deleteLater()));
    m_lua=new LuaControlAlgorithm();
    m_lua->moveToThread(m_thread_algo);
    connect(m_camera,SIGNAL(cameraResponse(QByteArray)),m_lua,SLOT(onCameraResponse(QByteArray)));
    connect(m_thread_algo,SIGNAL(started()),m_lua,SLOT(init()));
    connect(m_lua,SIGNAL(unloaded()),m_thread_algo,SLOT(quit()));
    connect(m_lua,SIGNAL(unloaded()),m_lua,SLOT(deleteLater()));
    connect(m_thread_algo,SIGNAL(finished()),m_thread_algo,SLOT(deleteLater()));
    connect(m_lua,SIGNAL(controlSignal(DataSet)),this,SLOT(onControlSignal(DataSet)));
    m_thread_algo->start();
    m_thread_cam->start();

    foreach (QObject *widget, ui->groupBox->children()) {
        foreach (QObject * child, widget->children()) {
            if(QString(child->metaObject()->className())=="QDoubleSpinBox")
            {
                QDoubleSpinBox * spinBox=reinterpret_cast<QDoubleSpinBox *>(child);
                if(spinBox->objectName().contains("rot"))
                    spinBox->setSingleStep(1);
                else
                    spinBox->setSingleStep(0.01);
                //controls[spinBox->objectName()]=spinBox;
                //oldvalues[spinBox->objectName()]=spinBox->value();
                //qDebug()<<spinBox->objectName();
                connect(spinBox,SIGNAL(valueChanged(double)),this,SLOT(onCameraParamsChanged()));
                //connect(spinBox,SIGNAL(editingFinished()),this,SLOT(onCameraParamsChanged()));
            }
        }
    }
    //Q_DECLARE_METATYPE(core::vector3df);
    int metatype_id = qRegisterMetaType<core::vector3df>("core::vector3df");
    metatype_id = qRegisterMetaType<DataSet>("DataSet");

    btDbvtBroadphase * broadphase=new btDbvtBroadphase();
    btDefaultCollisionConfiguration * collision_config=new btDefaultCollisionConfiguration();
    btCollisionDispatcher * dispatcher=new btCollisionDispatcher(collision_config);
    btSequentialImpulseConstraintSolver * solver=new btSequentialImpulseConstraintSolver();
    m_world=new btDiscreteDynamicsWorld(dispatcher, broadphase, solver,
                                   collision_config);

    m_world->setGravity(btVector3(0, -10, 0));

    // первый параметр -- нормаль к плоскости; второй вроде бы сдвиг по Y
    btStaticPlaneShape * ground_shape =new btStaticPlaneShape(btVector3(0, 0.5, 0), 0);
    // первый параметр -- как-то описывает поворот, как -- не знаю; второй -- смещение
    btDefaultMotionState * ground_motion_state=new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),
                                                         btVector3(0, -0.5, 0)));
    // первый и последний параметры -- масса и инерция, для статической плоскости нулевые
    btRigidBody::btRigidBodyConstructionInfo ground_ci (0, ground_motion_state,
                                                        ground_shape,
                                                        btVector3(0, 0, 0));
    btRigidBody * ground =new btRigidBody(ground_ci);
    m_world->addRigidBody(ground);

    btSphereShape * sphere_shape =new btSphereShape(5);
    btScalar mass = 1;
    btVector3 inertia (0, 0, 0);

    btDefaultMotionState * sphere_motion_state=new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),
                                                          btVector3(0,2.5, 0)));
    sphere_shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo sphere_ci (mass, sphere_motion_state,
                                                        sphere_shape, inertia);

    m_object=new btRigidBody(sphere_ci);
    m_world->addRigidBody(m_object);
    ui->poseX->setValue(0.6);
    ui->poseY->setValue(0.6);
    ui->poseZ->setValue(0.5);
    ui->rotX->setValue(80);
    ui->rotY->setValue(90);
}

MainWindow::~MainWindow()
{
    delete ui;
}

constexpr int LINE_WIDTH = 5;
constexpr int BLACK_VALUE = 15;
constexpr int WHITE_VALUE = 75;
constexpr int FRAME_SIZE = 128;

void MainWindow::onCameraResponse(const QByteArray &frame)
{
    int width=0;
    QImage img(128,1,QImage::Format_RGB32);
    //QByteArray frame=frame["frame"].toByteArray();
    for(int i=0;i<frame.length();i++)
    {
        if((unsigned char)frame.at(i)<=BLACK_VALUE)
            width++;
        //qDebug()<<(unsigned char)frame.at(i);
        img.setPixel(i,0,qRgb(frame.at(i),frame.at(i),frame.at(i)));
    }
    ui->picture->setPixmap(QPixmap::fromImage(img));
    ui->lineWidth->setText(QString::number(width));
}

/*
 *-2,38
 *0,58
 *-0,49
 *-56,00
 */

void MainWindow::onCameraParamsChanged()
{
    map.clear();
    map["camX"]=QVariant(ui->poseX->value());
    map["camY"]=QVariant(ui->poseY->value());
    map["camZ"]=QVariant(ui->poseZ->value());
    map["rotateX"]=QVariant(ui->rotX->value());
    map["rotateY"]=QVariant(ui->rotY->value());
    map["rotateZ"]=QVariant(ui->rotZ->value());
    btTransform t;
    btTransform old=m_object->getWorldTransform();
    t.setOrigin(btVector3(ui->poseX->value(),old.getOrigin().getY(),ui->poseZ->value()));
    m_object->setWorldTransform(t);
    emit simulatorResponse(map);
}

void MainWindow::onControlSignal(const DataSet &data)
{
    qDebug() << "Have controlSignal with angle =" << data["angle"].toFloat()
                    << ", rspeed =" << data["rspeed"].toFloat();

    /*m_angle_container<<data["angle"].toFloat();
    bool allowRotate=true;
    if(m_angle_container.length()>1)
    {
        float delta=fabs(m_angle_container.last()-m_angle_container.first());
        if(delta>=1.0f)
        {
            qDebug()<<"Delata too big, slowing";
            m_velocity=m_velocity*(-1);
            allowRotate=false;
            m_object->applyImpulse(m_velocity,btVector3(0,3,3));
            m_angle_container.pop_back();
        }
        else
            allowRotate=true;
    }
    if(allowRotate)
        m_velocity=m_velocity.rotate(btVector3(0,1,0),data["angle"].toFloat()-90);
*/
    m_timer=startTimer(500);
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    killTimer(m_timer);

    /*if(m_object->getLinearVelocity().length()<0.5)
        m_object->applyImpulse(m_velocity,btVector3(0,3,3));*/
    m_object->setLinearVelocity(btVector3(1,0,0));
    m_world->stepSimulation(1/60.f, 10);
    btTransform t;
    m_object->getMotionState()->getWorldTransform(t);
    btVector3 p = t.getOrigin();
    btQuaternion r=t.getRotation();
    map.clear();
    qDebug()<<r.y();
    map["camX"]=p.getX();
    map["camY"]=ui->poseY->value();
    map["camZ"]=p.getZ();
    map["rotateX"]=QVariant(ui->rotX->value());
    map["rotateY"]=QVariant(-r.y()+90);
    map["rotateZ"]=QVariant(ui->rotZ->value());
    emit simulatorResponse(map);
    e->accept();  
}

void MainWindow::on_pushButton_clicked()
{
    startTimer(1000);
    /*map.clear();
    map["camX"]=QVariant(ui->poseX->value());
    map["camY"]=QVariant(ui->poseY->value());
    map["camZ"]=QVariant(ui->poseZ->value());
    map["rotateX"]=QVariant(ui->rotX->value());
    map["rotateY"]=QVariant(ui->rotY->value());
    map["rotateZ"]=QVariant(ui->rotZ->value());*/
    /*core::vector3df targetPos=m_camera->getTargetPosition();
    targetPos.Y=0;
    core::vector3df camPos(ui->poseX->value(),0,ui->poseZ->value());
    core::vector3df targetVector(targetPos.X-camPos.X,0,targetPos.Z-camPos.Z);
    qDebug()<<targetVector.getLength();
    qDebug()<<targetVector.X<<targetVector.Y<<targetVector.Z;
    targetVector.setLength(0.5);
    qDebug()<<targetVector.X<<targetVector.Y<<targetVector.Z;*/
    //emit simulatorResponse(map);
}
