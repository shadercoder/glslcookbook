#include "scenenormalmap.h"

#include <cstdio>
#include <cstdlib>

#include "tgaio.h"

#include "glutils.h"
#include "defines.h"

#include <iostream>
using std::endl;
using std::cerr;

using glm::vec3;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

SceneNormalMap::SceneNormalMap() : angle(0.0f), tPrev(0.0f), rotSpeed(PI/2.0) { }

void SceneNormalMap::initScene()
{
    compileAndLinkShader();

    glEnable(GL_DEPTH_TEST);

    ogre = new VBOMesh("../media/bs_ears.obj",false,true,true);

    view = glm::lookAt(vec3(-1.0f,0.25f,2.0f), vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));
    projection = mat4(1.0f);

    angle = (float)( TO_RADIANS(100.0) );

    prog.setUniform("Light.Intensity", vec3(0.9f,0.9f,0.9f) );

    // Load diffuse texture
    glActiveTexture(GL_TEXTURE0);
    TGAIO::loadTex("../media/texture/ogre_diffuse.tga");

    // Load normal map
    glActiveTexture(GL_TEXTURE1);
    TGAIO::loadTex("../media/texture/ogre_normalmap.tga");
    
}

void SceneNormalMap::update( float t )
{
    float deltaT = t - tPrev;
    if(tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    
    if( this->m_animate) {
	angle += rotSpeed * deltaT;
	if( angle > TWOPI_F) angle -= TWOPI_F;
    }
}

void SceneNormalMap::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog.setUniform("Light.Position", view * vec4(10.0f * cos(angle),1.0f,10.0f * sin(angle),1.0f) );
    prog.setUniform("Material.Ks", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Shininess", 1.0f);

    model = mat4(1.0f);
    setMatrices();
    ogre->render();
}

void SceneNormalMap::setMatrices()
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
                    mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", projection * mv);
}

void SceneNormalMap::resize(int w, int h)
{
    glViewport(0,0,w,h);
    width = w;
    height = h;
    float c = 2.0f;
    projection = glm::ortho( -0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f);
}

void SceneNormalMap::compileAndLinkShader()
{
  try {
    prog.compileShader("shader/normalmap.vs");
    prog.compileShader("shader/normalmap.fs");
    prog.link();
    prog.use();
  } catch(GLSLProgramException & e) {
    cerr << e.what() << endl;
    exit( EXIT_FAILURE );
  }
}
