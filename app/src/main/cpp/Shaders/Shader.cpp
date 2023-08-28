#include "Shader.hpp"
#include "fstream"


namespace st::renderer
{
	std::vector<char> Shader::readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}


	vk::ShaderModule Shader::createShaderModule(const vk::Device& device, const std::vector<char>& code)
	{
		vk::ShaderModuleCreateInfo createInfo { vk::ShaderModuleCreateFlags {}, code.size(), reinterpret_cast<const uint32_t*>(code.data()) };

		vk::ShaderModule shaderModule = device.createShaderModule(createInfo);

		return shaderModule;
	}

}
