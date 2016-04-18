////////////////////////////////////////////////////////////////////////////////
//
// Minimalistic Vulkan Triangle sample
//
// 

#define USE_GLSL
#define VERTEX_BUFFER_BIND_ID 0


// vulkan utilities.
#include "vku.hpp"

#include "../glm/gtc/matrix_transform.hpp"

inline float deg_to_rad(float deg) { return deg * (3.1415927f / 180); }


class triangle_example : public vku::window
{
public:
	struct {
		glm::mat4 projectionMatrix;
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
	} uniform_data;

  vku::buffer vertex_buffer;
  vku::buffer index_buffer;
  vku::buffer uniform_buffer;
  vku::vertexInputState vertexInputState;
  vku::descriptorPool descPool;
  vku::pipeline pipe;
  size_t num_indices;

	triangle_example() : vku::window(false) {
		width = 1280;
		height = 720;
		zoom = -2.5f;
		title = "Vulkan Example - Basic indexed triangle";
		// Values not set here are initialized in the base class constructor

    // Vertices
		struct Vertex { float pos[3]; float col[3]; };

		static const Vertex vertex_data[] = {
			{ { 1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
			{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
			{ { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f} }
		};

    vertex_buffer = vku::buffer(device, (void*)vertex_data, sizeof(vertex_data), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

		// Indices
		static const uint32_t index_data[] = { 0, 1, 2 };
    index_buffer = vku::buffer(device, (void*)index_data, sizeof(index_data), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		num_indices = 3;

    // Binding state
    vertexInputState.binding(VERTEX_BUFFER_BIND_ID, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
    vertexInputState.attrib(0, VERTEX_BUFFER_BIND_ID, VK_FORMAT_R32G32B32_SFLOAT, 0);
    vertexInputState.attrib(1, VERTEX_BUFFER_BIND_ID, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3);

    uniform_buffer = vku::buffer(device, (void*)nullptr, sizeof(uniform_data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		
		updateUniformBuffers();

    pipe = vku::pipeline(device, swapChain.renderPass(), vertexInputState.get(), pipelineCache);

    descPool = vku::descriptorPool(device);

    pipe.allocateDescriptorSets(descPool);
    pipe.updateDescriptorSets(uniform_buffer);

		for (int32_t i = 0; i < swapChain.imageCount(); ++i) {
      const vku::cmdBuffer &cmdbuf = drawCmdBuffers[i];
      cmdbuf.begin(swapChain.renderPass(), swapChain.frameBuffer(i), width, height);

      cmdbuf.bindPipeline(pipe);
      cmdbuf.bindVertexBuffer(vertex_buffer, VERTEX_BUFFER_BIND_ID);
      cmdbuf.bindIndexBuffer(index_buffer);
      cmdbuf.drawIndexed((uint32_t)num_indices, 1, 0, 0, 1);

      cmdbuf.end(swapChain.image(i));
		}
	}

	void draw()
	{
    vku::semaphore sema(device);

		// Get next image in the swap chain (back/front buffer)
    currentBuffer = swapChain.acquireNextImage(sema);

    queue.submit(sema, drawCmdBuffers[currentBuffer]);

		// Present the current buffer to the swap chain
		// This will display the image
    swapChain.present(queue, currentBuffer);

    postPresentCmdBuffer.beginCommandBuffer();
    postPresentCmdBuffer.pipelineBarrier(swapChain.image(currentBuffer));
    postPresentCmdBuffer.endCommandBuffer();

    queue.submit(nullptr, postPresentCmdBuffer);

    queue.waitIdle();
	}

	void updateUniformBuffers()
	{
		// Update matrices
		uniform_data.projectionMatrix = glm::perspective(deg_to_rad(60.0f), (float)width / (float)height, 0.1f, 256.0f);

		uniform_data.viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, zoom));

		uniform_data.modelMatrix = glm::mat4();
		uniform_data.modelMatrix = glm::rotate(uniform_data.modelMatrix, deg_to_rad(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		uniform_data.modelMatrix = glm::rotate(uniform_data.modelMatrix, deg_to_rad(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		uniform_data.modelMatrix = glm::rotate(uniform_data.modelMatrix, deg_to_rad(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		void *dest = uniform_buffer.map();
 		memcpy(dest, &uniform_data, sizeof(uniform_data));
    uniform_buffer.unmap();
	}

	void render() override
	{
		device.waitIdle();

		draw();

		device.waitIdle();
	}

	void viewChanged() override
	{
		// This function is called by the base example class 
		// each time the view is changed by user input
		updateUniformBuffers();
	}
};



int main(const int argc, const char *argv[]) {
  triangle_example my_example;
  triangle_example my_example2;

	while (vku::window::poll()) {
  }

	return 0;
}
