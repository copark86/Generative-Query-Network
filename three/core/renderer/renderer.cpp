#include "renderer.h"
#include "opengl/functions.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace three {
namespace renderer {
    void DebugCallbackFunc(GLenum source, GLenum type, GLuint eid, GLenum severity, GLsizei length, const GLchar* message, const void* user_param)
    {
        std::cout << message << std::endl;
    }
    void Renderer::initialize(int width, int height)
    {
        glfwSetErrorCallback([](int error, const char* description) {
            fprintf(stderr, "Error %d: %s\n", error, description);
        });
        if (!!glfwInit() == false) {
            throw std::runtime_error("Failed to initialize GLFW.");
        }

        _width = width;
        _height = height;
        _depth_pixels = std::make_unique<GLfloat[]>(width * height);
        _color_pixels = std::make_unique<GLubyte[]>(width * height * 3);

        // // SSAO
        // std::random_device seed;
        // std::default_random_engine engine(seed());
        // std::normal_distribution<> normal_dist(0.0, 1.0);
        // _ssao_texture_data = std::make_unique<GLfloat[]>(192 * 3);
        // for (int i = 0; i < 192; i++) {
        //     _ssao_texture_data[i * 3 + 0] = normal_dist(engine);
        //     _ssao_texture_data[i * 3 + 1] = normal_dist(engine);
        //     _ssao_texture_data[i * 3 + 2] = 0;
        // }

        _prev_num_objects = -1;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
#if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        _window = glfwCreateWindow(width, height, "Renderer", NULL, NULL);
        glfwMakeContextCurrent(_window);
        gl3wInit();

        _vao = std::make_unique<opengl::VertexArrayObject>();
        _depth_program = std::make_unique<multipass::Depth>();
        _main_program = std::make_unique<multipass::Main>();

        glCreateFramebuffers(1, &_frame_buffer);

        glCreateRenderbuffers(1, &_color_render_buffer);
        glNamedRenderbufferStorage(_color_render_buffer, GL_RGB, _width, _height);

        glCreateRenderbuffers(1, &_depth_render_buffer);
        glNamedRenderbufferStorage(_depth_render_buffer, GL_DEPTH_COMPONENT, _width, _height);

        // _depth_texture_data = std::make_unique<GLfloat[]>(width * height);
        // glCreateTextures(GL_TEXTURE_2D, 1, &_depth_texture);
        // glTextureStorage2D(_depth_texture, 1, GL_DEPTH_COMPONENT, width, height);
        // glTextureSubImage2D(_depth_texture, 0, 0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, _depth_texture_data.get());
        // glTextureParameteri(_depth_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTextureParameteri(_depth_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTextureParameteri(_depth_texture, GL_TEXTURE_WRAP_S, GL_NEAREST);
        // glTextureParameteri(_depth_texture, GL_TEXTURE_WRAP_T, GL_NEAREST);
        // glBindTextureUnit(0, _depth_texture);

        // glGenTextures(1, &_depth_texture);
        // glBindTexture(GL_TEXTURE_2D, _depth_texture);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glBindTexture(GL_TEXTURE_2D, 0);

        // glGenTextures(1, &_color_texture);
        // glBindTexture(GL_TEXTURE_2D, _color_texture);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glBindTexture(GL_TEXTURE_2D, 0);

        // glGenTextures(1, &_ssao_texture);
        // glBindTexture(GL_TEXTURE_2D, _ssao_texture);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 192, 1, 0, GL_RGB, GL_FLOAT, _ssao_texture_data.get());
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glBindTexture(GL_TEXTURE_2D, 0);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_1D);
        glDepthMask(GL_TRUE);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        glDebugMessageCallback(DebugCallbackFunc, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
    }
    Renderer::Renderer(int width, int height)
    {
        initialize(width, height);
    }
    Renderer::Renderer(scene::Scene* scene, int width, int height)
    {
        initialize(width, height);
        set_scene(scene);
    }
    Renderer::~Renderer()
    {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }
    void Renderer::set_scene(scene::Scene* scene)
    {
        glfwMakeContextCurrent(_window);
        _scene = scene;
        _vao->build(scene);
    }
    void Renderer::draw_objects(camera::PerspectiveCamera* camera)
    {
        glm::mat4& view_mat = camera->_view_matrix;
        glm::mat4& projection_mat = camera->_projection_matrix;
        std::vector<std::shared_ptr<base::Object>>& objects = _scene->_objects;
        for (int object_index = 0; object_index < objects.size(); object_index++) {
            std::shared_ptr<base::Object> object = objects[object_index];
            float smoothness = object->_smoothness ? 1.0 : 0.0;
            glm::mat4& model_mat = object->_model_matrix;

            _vao->bind_object(object_index);
            _main_program->uniform_matrix(_uniform_model_mat, glm::value_ptr(model_mat));
            _main_program->uniform_matrix(_uniform_view_mat, glm::value_ptr(view_mat));
            _main_program->uniform_matrix(_uniform_projection_mat, glm::value_ptr(projection_mat));
            _main_program->uniform_float(_uniform_soothness, smoothness);

            glDrawArrays(GL_TRIANGLES, 0, 3 * object->_num_faces);
        }
    }
    void Renderer::render_depth_map(
        camera::PerspectiveCamera* camera,
        py::array_t<GLfloat, py::array::c_style> np_depth_map)
    {
        if (glfwWindowShouldClose(_window)) {
            return;
        }

        // OpenGL commands are executed in global context (per thread).
        glfwMakeContextCurrent(_window);
        glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);

        glViewport(0, 0, _width, _height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        _main_program->use();

        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _color_render_buffer);

        draw_objects(camera);

        glReadPixels(0, 0, _width, _height, GL_DEPTH_COMPONENT, GL_FLOAT, _depth_pixels.get());
        auto depth_map = np_depth_map.mutable_unchecked<2>();
        for (int h = 0; h < _height; h++) {
            for (int w = 0; w < _width; w++) {
                depth_map(h, w) = _depth_pixels[(_height - h - 1) * _width + w];
            }
        }

        glUseProgram(0);
        glBindVertexArray(0);
    }
    void Renderer::render(
        camera::PerspectiveCamera* camera,
        py::array_t<GLuint, py::array::c_style> np_rgb_map)
    {
        if (glfwWindowShouldClose(_window)) {
            return;
        }

        // OpenGL commands are executed in global context (per thread).
        glfwMakeContextCurrent(_window);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("glCheckFramebufferStatus() != GL_FRAMEBUFFER_COMPLETE");
        }

        // First pass
        {
            glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);
            _main_program->attach_depth_texture();
            // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _color_render_buffer);
            // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depth_render_buffer);
            // glNamedFramebufferRenderbuffer(_frame_buffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _color_render_buffer);
            // glNamedFramebufferRenderbuffer(_frame_buffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depth_render_buffer);
            // glNamedFramebufferTexture(_frame_buffer, GL_DEPTH_ATTACHMENT, _depth_texture, 0);
            // glDrawBuffer(GL_NONE);
            // glReadBuffer(GL_NONE);
            glNamedFramebufferDrawBuffer(_frame_buffer, GL_NONE);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, _width, _height);
            // GLenum buf = GL_DEPTH_ATTACHMENT;
            // glDrawBuffers(1, &buf);

            _depth_program->use();

            glm::mat4& view_mat = camera->_view_matrix;
            glm::mat4& projection_mat = camera->_projection_matrix;
            std::vector<std::shared_ptr<base::Object>>& objects = _scene->_objects;
            for (int object_index = 0; object_index < objects.size(); object_index++) {
                std::shared_ptr<base::Object> object = objects[object_index];
                glm::mat4& model_mat = object->_model_matrix;
                _vao->bind_object(object_index);
                _depth_program->uniform_matrix(_uniform_model_mat, glm::value_ptr(model_mat));
                _depth_program->uniform_matrix(_uniform_view_mat, glm::value_ptr(view_mat));
                _depth_program->uniform_matrix(_uniform_projection_mat, glm::value_ptr(projection_mat));

                glDrawArrays(GL_TRIANGLES, 0, 3 * object->_num_faces);
            }

            // glBindTexture(GL_TEXTURE_2D, _depth_texture);

            // std::unique_ptr<GLfloat[]> buffer = std::make_unique<GLfloat[]>(_width * _height);
            // glReadPixels(0, 0, _width, _height, GL_DEPTH_COMPONENT, GL_FLOAT, buffer.get());
            // auto rgb_map = np_rgb_map.mutable_unchecked<3>();

            // GLfloat min_value = 1.0;
            // for (int h = 0; h < _height; h++) {
            //     for (int w = 0; w < _width; w++) {
            //         GLfloat depth = buffer[(_height - h - 1) * _width + w];
            //         if(depth < min_value){
            //             min_value = depth;
            //         }
            //     }
            // }
            // std::cout << min_value << std::endl;

            // for (int h = 0; h < _height; h++) {
            //     for (int w = 0; w < _width; w++) {
            //         rgb_map(h, w, 0) = (buffer[(_height - h - 1) * _width + w] - min_value) / (1.0 - min_value) * 255.0;
            //         rgb_map(h, w, 1) = (buffer[(_height - h - 1) * _width + w] - min_value) / (1.0 - min_value) * 255.0;
            //         rgb_map(h, w, 2) = (buffer[(_height - h - 1) * _width + w] - min_value) / (1.0 - min_value) * 255.0;
            //     }
            // }

            // glReadPixels(0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, _color_pixels.get());
            // for (int h = 0; h < _height; h++) {
            //     for (int w = 0; w < _width; w++) {
            //         rgb_map(h, w, 0) = _color_pixels[(_height - h - 1) * _width * 3 + w * 3 + 0];
            //         rgb_map(h, w, 1) = _color_pixels[(_height - h - 1) * _width * 3 + w * 3 + 1];
            //         rgb_map(h, w, 2) = _color_pixels[(_height - h - 1) * _width * 3 + w * 3 + 2];
            //     }
            // }

            // glFlush();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glUseProgram(0);
            glBindVertexArray(0);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("glCheckFramebufferStatus() != GL_FRAMEBUFFER_COMPLETE");
        }

        // Second pass
        {
            glViewport(0, 0, _width, _height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            _main_program->use();
            // glActiveTexture(GL_TEXTURE0);
            // glBindTexture(GL_TEXTURE_2D, _depth_texture);
            _main_program->bind_textures();
            draw_objects(camera);

            glReadPixels(0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, _color_pixels.get());
            auto rgb_map = np_rgb_map.mutable_unchecked<3>();
            for (int h = 0; h < _height; h++) {
                for (int w = 0; w < _width; w++) {
                    rgb_map(h, w, 0) = _color_pixels[(_height - h - 1) * _width * 3 + w * 3 + 0];
                    rgb_map(h, w, 1) = _color_pixels[(_height - h - 1) * _width * 3 + w * 3 + 1];
                    rgb_map(h, w, 2) = _color_pixels[(_height - h - 1) * _width * 3 + w * 3 + 2];
                }
            }
        }

        glUseProgram(0);
        glBindVertexArray(0);
    }
    void Renderer::render_depth_map(scene::Scene* scene, camera::PerspectiveCamera* camera,
        py::array_t<GLfloat, py::array::c_style> np_depth_map)
    {
        set_scene(scene);
        render_depth_map(camera, np_depth_map);
    }
    void Renderer::render(scene::Scene* scene, camera::PerspectiveCamera* camera,
        py::array_t<GLuint, py::array::c_style> np_rgb_map)
    {
        set_scene(scene);
        render(camera, np_rgb_map);
    }
}
}