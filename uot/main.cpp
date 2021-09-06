#ifdef __linux__
#include "SDL2/SDL.h"
#elif _WIN32
#include "SDL.h"
#else
#error this compiles only on Windows/Linux
#endif

int main(int argc, char *argv[])
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
    bool show_new_game_window = false;
    bool show_options_window = false;
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

    {
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_Always);
        ImGui::Begin("Main Menu");
        if (ImGui::Button("New Game"))
            show_new_game_window ^= 1;
        if (ImGui::Button("Options"))
            show_options_window ^= 1;
        // Next navigation buttons goes here
        if (ImGui::Button("Exit"))
            // kill the app somehow, needs better way
            terminate();
        ImGui::End();
    }

    if (show_new_game_window)
    {
        ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_Always);
        ImGui::Begin("New Game Settings", &show_new_game_window);
        ImGui::Text("Placeholder for new game options!");
        if (ImGui::Button("Start New Game!!!"))
        {
            // start the game 
        }
        ImGui::End();
    }

     if (show_options_window)
    {
        ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_Always);
        ImGui::Begin("Options##window", &show_options_window);
        ImGui::Text("Placeholder for options!");
        if (ImGui::Button("Apply changes"))
        {
            show_options_window = false;
            // apply changes
        }
        ImGui::End();
    }


}

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

    w.setCreateSceneFunc(
        [&guiMgr](Qt3DCore::QEntity* parent)
        {
            guiMgr.initialize(parent);

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

    SDL_Window *window =
        SDL_CreateWindow("Universe of Technology", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Delay(3000);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
