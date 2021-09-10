/*
At this moment this file is mainly copied sample from:
https://github.com/alpqr/imgui-qt3d
to ensure that everything works as expected
*/

#include <QCuboidMesh>
#include <QCylinderMesh>
#include <QEntity>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QObjectPicker>
#include <QOpenGLContext>
#include <QPainter>
#include <QPhongMaterial>
#include <QPickEvent>
#include <QPropertyAnimation>
#include <QSphereMesh>
#include <QSurfaceFormat>
#include <QText2DEntity>
#include <QTorusMesh>
#include <QTransform>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QPaintedTextureImage>
#include <Qt3DRender/QTexture>

#include "imgui.h"
#include "imguimanager.h"
#include "imguiqt3dwindow.h"

// to test that we can include headers from libuot
#include "galaxygenerator.h"
#include "libmain.h"

class ImguiManager;

Galaxy galaxy;
QQuaternion sceneRotation;
std::vector<std::pair<Qt3DCore::QEntity*, std::shared_ptr<Sector>>> entities;
std::vector<std::pair<Qt3DCore::QEntity*, std::pair<std::shared_ptr<Sector>, std::shared_ptr<Sector>>>> edges;
// TODO Uzale¿niæ to od czegoœ zasadnego, a nie magic number
float posMultiplier = 10.f;

namespace Qt3DCore
{
class QEntity;
}

namespace Qt3DRender
{
class QTexture2D;
}

class Gui
{
   public:
    void setManager(ImguiManager* mgr) { m_manager = mgr; }
    void frame(Qt3DCore::QEntity* rootEntity);

   private:
    ImguiManager* m_manager = nullptr;
    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    Qt3DRender::QTexture2D* m_customTexture = nullptr;
};

class TextureContents : public Qt3DRender::QPaintedTextureImage
{
   public:
    void paint(QPainter* painter) override
    {
        const QRect r = QRect(QPoint(0, 0), size());
        painter->fillRect(r, Qt::white);
        painter->setBrush(Qt::green);
        painter->drawEllipse(r.center(), 100, 50);
        painter->setPen(Qt::red);
        painter->drawText(r, Qt::AlignCenter, QLatin1String("Hello from QPainter"));
    }
};

void Gui::frame(Qt3DCore::QEntity* rootEntity)
{
    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in
    // a window automatically called "Debug"
    {
        static float f = 0.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        if (ImGui::Button("Test Window"))
            show_test_window ^= 1;
        if (ImGui::Button("Another Window"))
            show_another_window ^= 1;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        // Add some extra content to exercise our Qt3D integration a bit.
        if (ImGui::Button("Scale up"))
            m_manager->setScale(m_manager->scale() + 0.2f);
        if (ImGui::Button("Scale down"))
            m_manager->setScale(m_manager->scale() - 0.2f);

        const QSize customImageSize(320, 200);
        if (!m_customTexture)
        {
            // rootEntity's only purpose is to serve as a parent for resources
            // like textures
            m_customTexture = new Qt3DRender::QTexture2D(rootEntity);
            Qt3DRender::QPaintedTextureImage* customTextureContents = new TextureContents;
            customTextureContents->setWidth(customImageSize.width());
            customTextureContents->setHeight(customImageSize.height());
            m_customTexture->addTextureImage(customTextureContents);
        }
        ImGui::ImageButton(m_customTexture, ImVec2(customImageSize.width(), customImageSize.height()));
    }

    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello from another window!");
        ImGui::End();
    }

    // 3. Show the ImGui test window. Most of the sample code is in
    // ImGui::ShowTestWindow()
    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_test_window);
    }
}

QVector3D GetPos(QVector3D vec)
{
    QVector3D xAxis, yAxis, zAxis;
    sceneRotation.getAxes(&xAxis, &yAxis, &zAxis);
    QVector3D res = posMultiplier * (vec.x() * xAxis + vec.y() * yAxis + vec.z() * zAxis) + QVector3D(0.0f, 0.0f, -7.0f);
    return res;
}

class SceneRotationEventFilter : public QObject
{
    bool rightButtonPressed = false;
    QPoint lastPos;
    bool eventFilter(QObject* object, QEvent* event) override
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            if (((QMouseEvent*)event)->button() == Qt::MouseButton::RightButton)
            {
                rightButtonPressed = true;
                lastPos = ((QMouseEvent*)event)->globalPos();
            }
        }
        if (event->type() == QEvent::MouseButtonRelease)
        {
            if (((QMouseEvent*)event)->button() == Qt::MouseButton::RightButton)
            {
                rightButtonPressed = false;
            }
        }
        if (event->type() == QEvent::MouseMove)
        {
            if (rightButtonPressed)
            {
                auto pos = ((QMouseEvent*)event)->globalPos();
                auto temp = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 0.1 * (lastPos.x() - pos.x()));
                sceneRotation *= temp;
                auto temp2 = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 0.1 * (lastPos.y() - pos.y()));
                sceneRotation *= temp2;
                for (auto pair : entities)
                {
                    auto sector = pair.second;
                    auto entity = pair.first;
                    Qt3DCore::QTransform* qtTrans = new Qt3DCore::QTransform;
                    qtTrans->setTranslation(GetPos(sector->position));
                    qtTrans->setRotation(sceneRotation);
                    auto transforms = entity->componentsOfType<Qt3DCore::QTransform>();
                    for (auto trans : transforms)
                        entity->removeComponent(trans);
                    entity->addComponent(qtTrans);
                }
                for (auto pair : edges)
                {
                    auto pair2 = pair.second;
                    auto entity = pair.first;
                    auto obj1 = pair2.first;
                    auto obj2 = pair2.second;
                    auto pos1 = GetPos(obj1->position);
                    auto pos2 = GetPos(obj2->position);
                    auto pos = (pos1 + pos2) / 2;

                    Qt3DCore::QTransform* qtEdgeTrans = new Qt3DCore::QTransform;
                    qtEdgeTrans->setTranslation(pos);
                    auto rot = QQuaternion::rotationTo({0.0f, 1.0f, 0.0f}, (pos1 - pos2).normalized());
                    qtEdgeTrans->setRotation(rot);
                    auto transforms = entity->componentsOfType<Qt3DCore::QTransform>();
                    for (auto trans : transforms)
                        entity->removeComponent(trans);
                    entity->addComponent(qtEdgeTrans);
                }
                lastPos = pos;
            }
        }
        return false;
    }
};

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        fmt.setVersion(4, 2);
        fmt.setProfile(QSurfaceFormat::CoreProfile);
    }
    QSurfaceFormat::setDefaultFormat(fmt);

    ImguiQt3DWindow w;
    w.setFormat(fmt);

    Gui gui;
    ImguiManager guiMgr;
    gui.setManager(&guiMgr);
    guiMgr.setFrameFunc(std::bind(&Gui::frame, &gui, std::placeholders::_1));
    guiMgr.setInputEventSource(&w);
    guiMgr.setOutputInfoFunc(
        [&w]()
        {
            ImguiManager::OutputInfo outputInfo;
            outputInfo.size = w.size();
            outputInfo.dpr = w.devicePixelRatio();
            outputInfo.guiTag = w.guiTag();
            outputInfo.activeGuiTag = w.activeGuiTag();
            outputInfo.guiTechniqueFilterKey = w.guiTechniqueFilterKey();
            return outputInfo;
        });
    // uncomment to start with gui hidden
    // guiMgr.setEnabled(false);

    galaxy = GenerateGalaxyTest({6, 0.5});
    SceneRotationEventFilter fo;
    w.installEventFilter(&fo);
    w.setCreateSceneFunc(
        [&guiMgr](Qt3DCore::QEntity* parent)
        {
            guiMgr.initialize(parent);

            for (auto sector : galaxy.sectors)
            {
                Qt3DCore::QEntity* qtSector = new Qt3DCore::QEntity(parent);
                entities.push_back(std::make_pair(qtSector, sector));
                // Da³em Cuboid ¿eby by³o widaæ ¿e siê ok obraca, docelowo pewnie QSphereMesh
                Qt3DExtras::QCuboidMesh* qtSectorGeom = new Qt3DExtras::QCuboidMesh;
                qtSectorGeom->setXExtent(2);
                Qt3DCore::QTransform* qtSectorTrans = new Qt3DCore::QTransform;

                qtSectorTrans->setTranslation(GetPos(sector->position));
                qtSectorTrans->setRotation(sceneRotation);
                Qt3DExtras::QPhongMaterial* qtSectorMat = new Qt3DExtras::QPhongMaterial;
                qtSector->addComponent(qtSectorGeom);
                qtSector->addComponent(qtSectorTrans);
                qtSector->addComponent(qtSectorMat);
                Qt3DRender::QObjectPicker* qtSectorPicker = new Qt3DRender::QObjectPicker;
                qtSector->addComponent(qtSectorPicker);
                QObject::connect(qtSectorPicker, &Qt3DRender::QObjectPicker::pressed,
                                 [qtSectorMat, qtSector](Qt3DRender::QPickEvent* ev)
                                 {
                                     if (ImGui::GetIO().WantCaptureMouse)
                                         return;
                                     // do smth
                                 });

                for (auto neighbor : sector->neighbors)
                {
                    if (sector->sector_id < neighbor->sector_id)
                    {
                        Qt3DCore::QEntity* qtEdge = new Qt3DCore::QEntity(parent);
                        edges.push_back(std::make_pair(qtEdge, std::make_pair(sector, neighbor)));
                        Qt3DExtras::QCylinderMesh* qtEdgeGeom = new Qt3DExtras::QCylinderMesh;

                        auto pos1 = GetPos(sector->position);
                        auto pos2 = GetPos(neighbor->position);

                        qtEdgeGeom->setRadius(0.05f);
                        qtEdgeGeom->setLength(pos1.distanceToPoint(pos2));

                        Qt3DCore::QTransform* qtEdgeTrans = new Qt3DCore::QTransform;

                        auto pos = (pos1 + pos2) / 2;
                        qtEdgeTrans->setTranslation(pos);

                        auto rot = QQuaternion::rotationTo(
                            {0.0f, 1.0f, 0.0f},
                            (pos1-pos2).normalized());
                        qtEdgeTrans->setRotation(rot);
                        Qt3DExtras::QPhongMaterial* qtEdgeMat = new Qt3DExtras::QPhongMaterial;
                        qtEdge->addComponent(qtEdgeGeom);
                        qtEdge->addComponent(qtEdgeTrans);
                        qtEdge->addComponent(qtEdgeMat);
                    }
                }
            }

            Qt3DCore::QEntity* cube = new Qt3DCore::QEntity(parent);
            Qt3DExtras::QCuboidMesh* cubeGeom = new Qt3DExtras::QCuboidMesh;
            cubeGeom->setXExtent(2);
            cubeGeom->setYExtent(2);
            cubeGeom->setZExtent(2);
            Qt3DCore::QTransform* cubeTrans = new Qt3DCore::QTransform;
            cubeTrans->setTranslation(QVector3D(18, 5, -10));
            Qt3DExtras::QPhongMaterial* cubeMat = new Qt3DExtras::QPhongMaterial;
            cube->addComponent(cubeGeom);
            cube->addComponent(cubeTrans);
            cube->addComponent(cubeMat);

            Qt3DCore::QEntity* cube2 = new Qt3DCore::QEntity(parent);
            // or, why have a cube when we can have a torus instead
            Qt3DExtras::QTorusMesh* cubeGeom2 = new Qt3DExtras::QTorusMesh;
            cubeGeom2->setRadius(5);
            cubeGeom2->setMinorRadius(1);
            cubeGeom2->setRings(100);
            cubeGeom2->setSlices(20);
            Qt3DCore::QTransform* cubeTrans2 = new Qt3DCore::QTransform;
            cubeTrans2->setTranslation(QVector3D(-15, -4, -20));
            cube2->addComponent(cubeGeom2);
            cube2->addComponent(cubeTrans2);
            cube2->addComponent(cubeMat);

            auto rotAnim = [](QObject* obj, const QByteArray& name, float start, float end, int duration)
            {
                QPropertyAnimation* anim = new QPropertyAnimation(obj, name);
                anim->setStartValue(start);
                anim->setEndValue(end);
                anim->setDuration(duration);
                anim->setLoopCount(-1);
                anim->start();
            };
            rotAnim(cubeTrans, "rotationX", 0.0f, 360.0f, 5000);
            rotAnim(cubeTrans2, "rotationY", 0.0f, 360.0f, 5000);

            // an entity that toggles the gui when pressed.
            // replace with a QText2DEntity some day when it actually works. in
            // the meantime a sphere will do.
            Qt3DCore::QEntity* toggleText = new Qt3DCore::QEntity(parent);
            Qt3DExtras::QSphereMesh* toggleTextGeom = new Qt3DExtras::QSphereMesh;
            Qt3DCore::QTransform* toggleTextTrans = new Qt3DCore::QTransform;
            toggleTextTrans->setTranslation(QVector3D(-14, 7, -5));
            toggleTextTrans->setScale(0.5f);
            Qt3DExtras::QPhongMaterial* toggleTextMat = new Qt3DExtras::QPhongMaterial;
            toggleTextMat->setDiffuse(guiMgr.isEnabled() ? Qt::green : Qt::red);
            toggleText->addComponent(toggleTextGeom);
            toggleText->addComponent(toggleTextTrans);
            toggleText->addComponent(toggleTextMat);
            Qt3DRender::QObjectPicker* toggleTextPicker = new Qt3DRender::QObjectPicker;
            QObject::connect(toggleTextPicker, &Qt3DRender::QObjectPicker::pressed,
                             [toggleTextMat, &guiMgr](Qt3DRender::QPickEvent*)
                             {
                                 if (ImGui::GetIO().WantCaptureMouse)
                                     return;
                                 guiMgr.setEnabled(!guiMgr.isEnabled());
                                 toggleTextMat->setDiffuse(guiMgr.isEnabled() ? Qt::green : Qt::red);
                             });
            toggleText->addComponent(toggleTextPicker);

            Qt3DExtras::QText2DEntity* bbb = new Qt3DExtras::QText2DEntity(nullptr);
            bbb->setParent(parent);
            bbb->setText("ABCDEFGH");
            bbb->setWidth(200);
            bbb->setHeight(20);
            bbb->setColor(QColor(255, 0, 0));
            bbb->setFont(QFont("monospace"));
            Qt3DCore::QTransform* ccc = new Qt3DCore::QTransform;
            // ccc->setTranslation(QVector3D(5, 2, 0));
            ccc->setScale(0.05f);
            ccc->setRotationX(0.1f);
            bbb->addComponent(ccc);
        });

    w.resize(1280, 720);
    w.show();

    return app.exec();
}
