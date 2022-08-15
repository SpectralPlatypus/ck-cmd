#pragma once

#include "DockWidget.h"

#include <QTimer>

class hkgWindow;
class hkgDisplayHandler;
class hkgSceneDataConverter;
class hkgDisplayContext;
class hkgDisplayWorld;
class hkTextDisplay;
class hkgLight;
class hkgAabb;

class hkLoader;
class hkgGeometry;
class hkgFaceSet;
class hkaAnimationContainer;
class hkMatrix4;
class hkaSkeleton;
class hkgShaderEffectCollection;

class HavokWidget : public ::ads::CDockWidget
{
    hkgWindow* m_window = nullptr;

    // Default viewport, or for any viewport in which m_viewport has null data
    hkgDisplayHandler* m_displayHandler = nullptr;
    hkgSceneDataConverter* m_sceneConverter = nullptr;
    hkgDisplayWorld* m_displayWorld = nullptr;
    hkgDisplayWorld* m_skeletalWorld = nullptr;
    hkTextDisplay* m_textDisplay = nullptr;
    hkLoader* m_loader = nullptr;
    hkaAnimationContainer* animContainer = nullptr;
    hkgShaderEffectCollection* skeletalShader = nullptr;

    bool m_showWorldAxis = true;
    bool m_showGrid = true;

    QTimer m_qTimer;

    void setupLights();
    void setupFixedShadowFrustum(const hkgLight& light, const hkgAabb& areaOfInterest, float extraNear = 0, float extraFar = 0, int numSplits = 0, int preferedUpAxis = -1);
    void showAxis();
    void tickFrame(bool justCamera);
    void renderFrame();
    void clearFrameData();
    void setupScene();
    void drawSkeletal();
    void drawSkeletalTriangleThingy(hkaSkeleton* skeletal, const std::vector<hkMatrix4>& boneAbsTransform);
    hkgGeometry* createDoublePyramid(float* vTarget, float* vOrigin);
    hkgFaceSet* createDoublePyramid(float* vA, float* vB, hkgDisplayContext* context);
    void drawGrid();
    void cameraSetFront();

public:
    explicit HavokWidget(QWidget* parent);



protected:
    void initialize();

    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event);

    void paint();

};