#ifndef RENDERER_SHADERS_SHADERS_HPP
#define RENDERER_SHADERS_SHADERS_HPP

#include <vector>
#include <vulkan/vulkan.hpp>


namespace st::renderer
{
	class Shader
	{

	public:
		static std::vector<char> readFile(const std::string& filename);

		static vk::ShaderModule createShaderModule(const vk::Device& device, const std::vector<char>& code);
	};


};

#endif // RENDERER_SHADERS_SHADERS_HPP
