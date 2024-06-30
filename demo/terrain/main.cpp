
//#define ENABLE_TEXTURE_CACHE
//#define CACHE_LOCATION "C:\\Users\\mateu\\source\\repos\\Renderella\\cache\\"

#include "WindowManager.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"
#include "TextureData.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "ObjLoader.hpp"
#include "Camera.hpp"
#include "PhongMat.hpp"
#include "Light.hpp"
#include "DirectionalLight.hpp"
#include "Framebuffer.hpp"
#include "Primitives.hpp"
#include "RenderQueue.hpp"

#include "WorldGen.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
	WindowManager window(1280, 720, "Renderella-terrain");

	RenderQueue renderQueue;

	auto phongMat = std::make_shared<PhongMat>("terrainMat");
	phongMat->setSpecular(glm::vec3(0.1f, 0.1f, 0.1f));
	phongMat->setDiffuseMap(std::make_shared<Texture>("assets/patchy-meadow1-bl/patchy-meadow1_albedo.png"));
	phongMat->setNormalMap(std::make_shared<Texture>("assets/patchy-meadow1-bl/patchy-meadow1_normal-ogl.png"));
	TextureData roughnessData("assets/patchy-meadow1-bl/patchy-meadow1_roughness.png");
	roughnessData.invert();
	phongMat->setShininessMap(std::make_shared<Texture>(roughnessData));

	WorldGen worldGen(8888, 50, 4, phongMat);
	auto chunks = worldGen.getChunks();
	for (auto& chunk : chunks) {
		renderQueue.add(chunk.second);
	}
	worldGen.generateWorld(200);


	Camera camera(Camera::ProjectionType::PERSPECTIVE, window.getAspectRatio(), true, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 65.0f, 100.0f, 0.2f);

	Light light(Light::Type::POINT, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.0f), 0.5f);
	DirectionalLight sun(glm::vec3(0.0f, 18.0f, 0.0f), glm::vec3(4.8f, -2.2f, 0.4f), glm::vec3(0.5f));

	Framebuffer lightFramebuffer(1024, 1024, 0, false, true, DirectionalLight::cascadeCount);
	Framebuffer mainFramebuffer(window.getWidth(), window.getHeight(), 3);
	Framebuffer defferedFramebuffer(window.getWidth(), window.getHeight(), 2);
	Framebuffer screenSpaceFramebuffer(window.getWidth() / 2, window.getHeight() / 2, 1);
	Framebuffer composeFramebuffer(window.getWidth(), window.getHeight(), 2);

	Mesh deferredPlane = mainFramebuffer.produceFbPlane("assets/shaders/screen.vert", "assets/shaders/deferred.frag");
	deferredPlane.getSubmeshes()[0].material->setTexture("lightDepthShadowSampler", lightFramebuffer.getDepthTex());
	deferredPlane.getSubmeshes()[0].material->setTexture("lightDepth", lightFramebuffer.getDepthTex());
	deferredPlane.getSubmeshes()[0].material->setIncludeLightsUniforms(true);
	deferredPlane.getSubmeshes()[0].material->setIncludeCameraUniform(true);
	auto sky = std::make_shared<Texture>("assets/san_giuseppe_bridge_4k.hdr");
	deferredPlane.getSubmeshes()[0].material->setTexture("sky", sky);

	Mesh screenSpacePlane = mainFramebuffer.produceFbPlane("assets/shaders/screen.vert", "assets/shaders/screen.frag");
	screenSpacePlane.getSubmeshes()[0].material->setIncludeCameraUniform(true);
	screenSpacePlane.getSubmeshes()[0].material->setIncludeFrameCounterUniform(true);
	screenSpacePlane.getSubmeshes()[0].material->setTexture("prevFrame", composeFramebuffer.getColorTexs()[0]);
	screenSpacePlane.getSubmeshes()[0].material->setTexture("prevSSR", composeFramebuffer.getColorTexs()[1]);
	screenSpacePlane.getSubmeshes()[0].material->setTexture("prevDepth", composeFramebuffer.getDepthTex());
	screenSpacePlane.getSubmeshes()[0].material->setMat4("prevProjectionView", glm::mat4(1.0f));

	Mesh composePlane = Framebuffer::produceEmptyFbPlane("assets/shaders/screen.vert", "assets/shaders/compose.frag");
	composePlane.getSubmeshes()[0].material->setTexture("deferredLight", defferedFramebuffer.getColorTexs()[0]);
	composePlane.getSubmeshes()[0].material->setTexture("deferredReflection", defferedFramebuffer.getColorTexs()[1]);
	composePlane.getSubmeshes()[0].material->setTexture("ssrTexture", screenSpaceFramebuffer.getColorTexs()[0]);
	composePlane.getSubmeshes()[0].material->setTexture("colorTexture", mainFramebuffer.getColorTexs()[0]);
	composePlane.getSubmeshes()[0].material->setTexture("depthTexture", mainFramebuffer.getDepthTex());

	Shader<GL_VERTEX_SHADER> shadowVertexShader("assets/shaders/shadow.vert", true);
	Shader<GL_FRAGMENT_SHADER> shadowFragmentShader("assets/shaders/shadow.frag", true);
	Shader<GL_GEOMETRY_SHADER> shadowGeometryShader("assets/shaders/shadow.geom", true);
	auto shadowProgram = std::make_shared<ShaderProgram>(shadowVertexShader, shadowFragmentShader, shadowGeometryShader);
	auto shadowMat = std::make_shared<Material>(shadowProgram);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	while (window.frame(false, true)) {
		glm::mat4 prevCamera = camera.getCameraMatrix();
		camera.update(window);

		light.setColor(glm::vec3(0.5f + sin(glfwGetTime() * 1.1f), 0.5f + sin(glfwGetTime()), 0.5f + sin(glfwGetTime() * 0.5f)));
		sun.setPosition(glm::vec3(sin(glfwGetTime()) * 4.0f - 2.0f, 2.0f, 0.0f));

		// directional light
		lightFramebuffer.bind(true);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_FRONT);

		glm::vec3 newLightPos = camera.getPosition() - sun.getDirection() * 30.0f;
		sun.setPosition(newLightPos);

		sun.use();
		Material::setOverrideMaterial(shadowMat);
		renderQueue.render();
		Material::setOverrideMaterial(nullptr);

		// player camera
		mainFramebuffer.bind(true);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		camera.use();
		renderQueue.render();

		// deferred shading
		defferedFramebuffer.bind();
		glDisable(GL_DEPTH_TEST);
		deferredPlane.draw();

		// screen space
		screenSpaceFramebuffer.bind(true);
		glDisable(GL_DEPTH_TEST);
		screenSpacePlane.getSubmeshes()[0].material->setMat4("prevProjectionView", prevCamera);
		screenSpacePlane.draw();

		// compose
		composeFramebuffer.bind(true);
		glDisable(GL_DEPTH_TEST);
		composePlane.draw();

		// copy depth for next frame
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFramebuffer.getFbo());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, composeFramebuffer.getFbo());
		glBlitFramebuffer(
			0, 0, mainFramebuffer.getWidth(), mainFramebuffer.getHeight(), 
			0, 0, composeFramebuffer.getWidth(), composeFramebuffer.getHeight(), 
			GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		// display - copy to default framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, composeFramebuffer.getFbo());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, composeFramebuffer.getWidth(), composeFramebuffer.getHeight(), 0, 0, window.getWidth(), window.getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	return 0;
}
