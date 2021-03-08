#include "Image.h"
#include <stb_image.h>

#include "Core/BindingData.h"
#include "Core/Rendering/Helper/Buffer.h"
#include "Core/Rendering/Helper/ImageUtils.h"

namespace Tristeon::Data
{
	Image::Image(const std::string& filePath)
	{
		_pixels = stbi_load(filePath.c_str(), &_width, &_height, &_channels, STBI_rgb_alpha);
		if (_pixels)
			createImage();
	}

	Image::~Image()
	{
		Core::binding_data.device.freeMemory(_memory);
		Core::binding_data.device.destroySampler(_sampler);
		Core::binding_data.device.destroyImageView(_view);
		Core::binding_data.device.destroyImage(_image);
	}

	unsigned char* Image::pixels() const
	{
		return _pixels;
	}

	int Image::width() const
	{
		return _width;
	}

	int Image::height() const
	{
		return _height;
	}

	int Image::channels() const
	{
		return _channels;
	}

	int Image::size() const
	{
		return _width * _height * _channels;
	}

	vk::Image Image::image() const
	{
		return _image;
	}

	vk::ImageView Image::view() const
	{
		return _view;
	}

	vk::DeviceMemory Image::memory() const
	{
		return _memory;
	}

	vk::Sampler Image::sampler() const
	{
		return _sampler;
	}

	void Image::createImage()
	{
		//Get image size and data
		vk::DeviceSize const size = _width * _height * 4;

		//Create staging buffer to allow vulkan to optimize the texture buffer's memory
		const Core::Rendering::Buffer staging = Core::Rendering::Buffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		staging.copyFromData(_pixels, (uint32_t)size, 0);

		//Create vulkan image
		Core::Rendering::ImageUtils::createImage(
			_width, _height,
			vk::Format::eR8G8B8A8Unorm,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			_image, _memory);

		//Transition to transfer destination, transfer data, transition to shader read only
		Core::Rendering::ImageUtils::transitionImageLayout(_image, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 },
			vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
			vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
			{}, vk::AccessFlagBits::eTransferWrite,
			~0U, ~0U
		);

		Core::Rendering::ImageUtils::copyBufferToImage(staging.buffer, _image, _width, _height);

		Core::Rendering::ImageUtils::transitionImageLayout(_image, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 },
			vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
			vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
			vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,
			~0U, ~0U
		);

		//Create image view
		const vk::ImageViewCreateInfo viewCi{ {}, _image, vk::ImageViewType::e2D, vk::Format::eR8G8B8A8Unorm, {}, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };
		_view = Core::binding_data.device.createImageView(viewCi);

		const auto ci = vk::SamplerCreateInfo({},
			vk::Filter::eLinear, vk::Filter::eLinear,
			vk::SamplerMipmapMode::eLinear,
			vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
			0, VK_TRUE, 16, VK_FALSE,
			vk::CompareOp::eAlways,
			0, 0,
			vk::BorderColor::eIntOpaqueBlack,
			VK_FALSE);
		
		_sampler = Core::binding_data.device.createSampler(ci);
	}
}