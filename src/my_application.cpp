#include <windows.h>
#include "oglrenderer.h"
#include "ase_serializer.h"
#include "model.h"
#include "application.h"
#include "gvector.h"
#include "point.h"
#include "input.h"
#include "frameratecontroller.h"
#include "pipeline.h"
#include "camera.h"

extern int client_area_width;
extern int client_area_height;

namespace core {

    /**
     * @brief Contains the logic that the application is about.
     * @remarks Needs to be instantiated.
     */
    class MyApplication: public Application
    {
    public:
        MyApplication(): firsttime(true), x(0), y(0), dx(0), dy(0), yanglelimit(0), mousex(0), mousey(0), oldmousex(-1), oldmousey(-1) {}
        virtual ~MyApplication() {}

        virtual void Initialize()
        {
            renderer = new OGLRenderer();
            renderer->Initialize();

            serializer = new ASESerializer();
            scene = serializer->LoadSceneFromFile(std::string("media\\test01.ASE"));
            renderer->LoadTextureMaps(scene->GetTexturesList());

            pipeline = new Pipeline();
            pipeline->SetViewport(0, 0, client_area_width, client_area_height);
            renderer->UpdateViewportProperties(pipeline);

            // "http://stackoverflow.com/questions/12943164/replacement-for-gluperspective-with-glfrustrum"
            float fh, fw, znear = 0.1f, zfar = 4000.f, aspect = (float)client_area_width/client_area_height;
            fh = tan(60.f / 2.f / 180.f * PI) * znear;
            fw = fh * aspect;
            pipeline->frustrum(-fw, fw, -fh, fh, znear, zfar);
            renderer->UpdateProjectionProperties(pipeline);

            camera = new Camera();

            utils::FrameRateController::Initialize();
            utils::FrameRateController::LockFrameRateAt(60);
        }

        virtual void Update(void)
        {
            utils::FrameRateController::Start();

            utils::Keyboard::Update();
            utils::Mouse::Update();

            // Exit the application is escape was triggered.
            if (utils::Keyboard::IsTriggered(VK_ESCAPE))
                PostQuitMessage(0);

            renderer->PreUpdate();

            // Set the camera transformation.
            UpdateCamera();
            pipeline->SetMatrixMode(core::Pipeline::MODELVIEW);
            pipeline->LoadIdentity();
            pipeline->PostMultiply(camera->GetViewTransformation());

            // Set the model transformation and render it.
            pipeline->PushMatrix();
            pipeline->PreTranslate(0, 0, -300);
            renderer->DrawModel(*scene);
            pipeline->PopMatrixEmpty();

            // Set the grid transformation and render it.
            pipeline->PushMatrix();
            pipeline->PreTranslate(0, -100, 0);
            renderer->DrawGrid();
            pipeline->PopMatrixEmpty();

            renderer->PostUpdate(utils::FrameRateController::GetFrameRate());

            utils::FrameRateController::End();
        }

        void UpdateCamera(void)
        {
            math::Matrix4D crossup, camerarotateY, camerarotatetemp, tmpmatrix;
            float dx, dy, d, tempangle;
            float speed = 10.f;
            float length;
            int mx, my;
            math::Vector3D tmp, tmp2;

            if (utils::Keyboard::IsPressed('C')) {
                yanglelimit = 0;
                camera->LookAt(math::Point3D(), math::Point3D(0, 0, -100), math::Vector3D(0, 1, 0));
            }

            // Handling the camera controls, first orientation, second translation.
            utils::Mouse::GetPosition(mx, my);
            mousex = mx;
            mousey = my;
            if (oldmousex == -1)
                oldmousex = mx;
            if (oldmousey == -1)
                oldmousey = my;
            dx = (float)mousex - oldmousex;
            dy = (float)mousey - oldmousey;
            utils::Mouse::SetPosition(oldmousex, oldmousey);

            // Crossing the camera up vector with the opposite of the look at direction.
            crossup = math::Matrix4D::CrossProduct(camera->upvector);
            tmp = -camera->lookatdirection;
            // 'tmp' is now orthogonal to the up and lookat vector.
            tmp = crossup * tmp;

            // Orthogonalizing the camera up and direction vector (to avoid floating point
            // imprecision). Discard the y and the w components (preserver x and z, making it
            // parallel to the movement plane).
            tmp.y = tmp.w = 0;
            length = sqrtf(tmp.x * tmp.x + tmp.z * tmp.z);
            if (length != 0) {
                // Normalizes tmp and return cross product matrix.
                crossup = math::Matrix4D::CrossProduct(tmp);
                tmp2 = camera->upvector;
                // 'tmp2' is now the oppposite of the direction vector.
                tmp2 = crossup * tmp2;
                camera->lookatdirection = -tmp2;
            }

            // Create a rotation matrix on the y relative the movement of the mouse on x.
            camerarotateY = math::Matrix4D::RotationY(-dx / 1000.f);
            // Limit yanglelimit to a certain range (to control x range of movement).
            tempangle = yanglelimit;
            tempangle -= dy / 1000;
            d = -dy / 1000;
            if (tempangle > 0.925f)
                d = 0.925f - yanglelimit;
            else if (tempangle < -0.925f)
                d = -0.925f - yanglelimit;
            yanglelimit += d;
            // Find the rotation matrix along perpendicular axis.
            camerarotatetemp = math::Matrix4D::AxisAngle(tmp, d / PI * 180);
            // Switching the rotations here makes no difference, why???, it seems geometrically the
            // result is the same. Just simulate it using your thumb and index.
            tmpmatrix = camerarotateY * camerarotatetemp;
            camera->lookatdirection = tmpmatrix * camera->lookatdirection;
            camera->upvector = tmpmatrix * camera->upvector;

            // Handling translations.
            if (utils::Keyboard::IsPressed('A')) {
                camera->position.x -= tmp.x * speed;
                camera->position.z -= tmp.z * speed;
            }

            if (utils::Keyboard::IsPressed('D')) {
                camera->position.x += tmp.x * speed;
                camera->position.z += tmp.z * speed;
            }

            if (utils::Keyboard::IsPressed('W')) {
                math::Vector3D vecxz;
                vecxz.x = camera->lookatdirection.x;
                vecxz.z = camera->lookatdirection.z;
                length = sqrtf(vecxz.x * vecxz.x + vecxz.z * vecxz.z);
                if (length != 0) {
                    vecxz.x /= length;
                    vecxz.z /= length;
                    camera->position.x += vecxz.x * speed;
                    camera->position.z += vecxz.z * speed;
                }
            }

            if (utils::Keyboard::IsPressed('S')) {
                math::Vector3D vecxz;
                vecxz.x = camera->lookatdirection.x;
                vecxz.z = camera->lookatdirection.z;
                length = sqrtf(vecxz.x * vecxz.x + vecxz.z * vecxz.z);
                if (length != 0) {
                    vecxz.x /= length;
                    vecxz.z /= length;
                    camera->position.x -= vecxz.x * speed;
                    camera->position.z -= vecxz.z * speed;
                }
            }
        }

        virtual void Cleanup()
        {
            utils::FrameRateController::Cleanup();
            renderer->Cleanup();
            delete renderer;
            delete scene;
            delete serializer;
            delete pipeline;
            delete camera;
        }

    private:
         Model *scene;
         OGLRenderer *renderer;
         ASESerializer *serializer;
         Pipeline *pipeline;
         Camera *camera;
         bool firsttime;
         int x, y, dx, dy, mousex, mousey, oldmousex, oldmousey;
         float yanglelimit;
    } my_application;
}
