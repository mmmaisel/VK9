// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright(c) 2018 Christopher Joseph Dean Schaefer

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#define MAX_DESCRIPTOR 2048

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include "Perf_StateManager.h"

#include "C9.h"
#include "CDevice9.h"
#include "CStateBlock9.h"
#include "CTexture9.h"
#include "CCubeTexture9.h"
#include "CVolumeTexture9.h"
#include "CVertexBuffer9.h"
#include "CIndexBuffer9.h"
#include "CQuery9.h"

#include "Utilities.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/format.hpp>

typedef std::unordered_map<UINT, StreamSource> map_type;

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* layerPrefix, const char* message, void* userData)
{
	switch (flags)
	{
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		BOOST_LOG_TRIVIAL(info) << "------DebugReport(Info)------";
		BOOST_LOG_TRIVIAL(info) << "ObjectType: " << objectType;
		BOOST_LOG_TRIVIAL(info) << "Object: " << object;
		BOOST_LOG_TRIVIAL(info) << "Location: " << location;
		BOOST_LOG_TRIVIAL(info) << "MessageCode: " << messageCode;
		BOOST_LOG_TRIVIAL(info) << "LayerPrefix: " << layerPrefix;
		BOOST_LOG_TRIVIAL(info) << "Message: " << message;
		BOOST_LOG_TRIVIAL(info) << "-----------------------------";
		break;
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		BOOST_LOG_TRIVIAL(warning) << "------DebugReport(Warn)------";
		BOOST_LOG_TRIVIAL(warning) << "ObjectType: " << objectType;
		BOOST_LOG_TRIVIAL(warning) << "Object: " << object;
		BOOST_LOG_TRIVIAL(warning) << "Location: " << location;
		BOOST_LOG_TRIVIAL(warning) << "MessageCode: " << messageCode;
		BOOST_LOG_TRIVIAL(warning) << "LayerPrefix: " << layerPrefix;
		BOOST_LOG_TRIVIAL(warning) << "Message: " << message;
		BOOST_LOG_TRIVIAL(warning) << "-----------------------------";
		break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		BOOST_LOG_TRIVIAL(warning) << "------DebugReport(Perf)------";
		BOOST_LOG_TRIVIAL(warning) << "ObjectType: " << objectType;
		BOOST_LOG_TRIVIAL(warning) << "Object: " << object;
		BOOST_LOG_TRIVIAL(warning) << "Location: " << location;
		BOOST_LOG_TRIVIAL(warning) << "MessageCode: " << messageCode;
		BOOST_LOG_TRIVIAL(warning) << "LayerPrefix: " << layerPrefix;
		BOOST_LOG_TRIVIAL(warning) << "Message: " << message;
		BOOST_LOG_TRIVIAL(warning) << "-----------------------------";
		break;
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		BOOST_LOG_TRIVIAL(error) << "------DebugReport(Error)------";
		BOOST_LOG_TRIVIAL(error) << "ObjectType: " << objectType;
		BOOST_LOG_TRIVIAL(error) << "Object: " << object;
		BOOST_LOG_TRIVIAL(error) << "Location: " << location;
		BOOST_LOG_TRIVIAL(error) << "MessageCode: " << messageCode;
		BOOST_LOG_TRIVIAL(error) << "LayerPrefix: " << layerPrefix;
		BOOST_LOG_TRIVIAL(error) << "Message: " << message;
		BOOST_LOG_TRIVIAL(error) << "------------------------------";
		break;
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		BOOST_LOG_TRIVIAL(warning) << "------DebugReport(Debug)------";
		BOOST_LOG_TRIVIAL(warning) << "ObjectType: " << objectType;
		BOOST_LOG_TRIVIAL(warning) << "Object: " << object;
		BOOST_LOG_TRIVIAL(warning) << "Location: " << location;
		BOOST_LOG_TRIVIAL(warning) << "MessageCode: " << messageCode;
		BOOST_LOG_TRIVIAL(warning) << "LayerPrefix: " << layerPrefix;
		BOOST_LOG_TRIVIAL(warning) << "Message: " << message;
		BOOST_LOG_TRIVIAL(warning) << "------------------------------";
		break;
	default:
		BOOST_LOG_TRIVIAL(error) << "------DebugReport(?)------";
		BOOST_LOG_TRIVIAL(error) << "ObjectType: " << objectType;
		BOOST_LOG_TRIVIAL(error) << "Object: " << object;
		BOOST_LOG_TRIVIAL(error) << "Location: " << location;
		BOOST_LOG_TRIVIAL(error) << "MessageCode: " << messageCode;
		BOOST_LOG_TRIVIAL(error) << "LayerPrefix: " << layerPrefix;
		BOOST_LOG_TRIVIAL(error) << "Message: " << message;
		BOOST_LOG_TRIVIAL(error) << "--------------------------";
		break;
	}

	return VK_FALSE;
}

VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetKHR(
	VkCommandBuffer                             commandBuffer,
	VkPipelineBindPoint                         pipelineBindPoint,
	VkPipelineLayout                            layout,
	uint32_t                                    set,
	uint32_t                                    descriptorWriteCount,
	const VkWriteDescriptorSet*                 pDescriptorWrites)
{
	if (pfn_vkCmdPushDescriptorSetKHR == nullptr)
	{
		return;
	}

	pfn_vkCmdPushDescriptorSetKHR(
		commandBuffer,
		pipelineBindPoint,
		layout,
		set,
		descriptorWriteCount,
		pDescriptorWrites
	);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
	VkInstance                                  instance,
	const VkDebugReportCallbackCreateInfoEXT*   pCreateInfo,
	const VkAllocationCallbacks*                pAllocator,
	VkDebugReportCallbackEXT*                   pCallback)
{
	if (pfn_vkCreateDebugReportCallbackEXT == nullptr)
	{
		return VK_NOT_READY;
	}

	return pfn_vkCreateDebugReportCallbackEXT(
		instance,
		pCreateInfo,
		pAllocator,
		pCallback
	);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
	VkInstance                                  instance,
	VkDebugReportCallbackEXT                    callback,
	const VkAllocationCallbacks*                pAllocator)
{
	if (pfn_vkDestroyDebugReportCallbackEXT == nullptr)
	{
		return;
	}

	pfn_vkDestroyDebugReportCallbackEXT(
		instance,
		callback,
		pAllocator
	);
}

VKAPI_ATTR void VKAPI_CALL vkDebugReportMessageEXT(
	VkInstance                                  instance,
	VkDebugReportFlagsEXT                       flags,
	VkDebugReportObjectTypeEXT                  objectType,
	uint64_t                                    object,
	size_t                                      location,
	int32_t                                     messageCode,
	const char*                                 pLayerPrefix,
	const char*                                 pMessage)
{
	if (pfn_vkDebugReportMessageEXT == nullptr)
	{
		return;
	}

	pfn_vkDebugReportMessageEXT(
		instance,
		flags,
		objectType,
		object,
		location,
		messageCode,
		pLayerPrefix,
		pMessage
	);
}

StateManager::StateManager(boost::program_options::variables_map& options)
	: mOptions(options)
{

}

StateManager::~StateManager()
{

}

void StateManager::DestroyDevice(size_t id)
{
	//TODO: figure out a way to stop everything from blowing up when doing reset.
	//if (mDevices.size() == 1)
	//{
	//	mSurfaces.clear();
	//	mSwapChains.clear();
	//}	
	//mDevices[id].reset();
}

void StateManager::CreateDevice(size_t id, void* argument1)
{
	CDevice9* device9 = (CDevice9*)argument1;
	auto instance = mInstances[id];
	auto physicalDevice = instance->mPhysicalDevices[device9->mAdapter];
	auto device = std::make_shared<RealDevice>(instance->mInstance, physicalDevice, device9->mPresentationParameters.BackBufferWidth, device9->mPresentationParameters.BackBufferHeight);

	if (pfn_vkCmdPushDescriptorSetKHR == nullptr)
	{
		pfn_vkCmdPushDescriptorSetKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(device->mDevice.getProcAddr("vkCmdPushDescriptorSetKHR"));
	}

	//The user wants d3d9 to auto-detect so I'll go ahead and grab the swap and pull it's format.
	auto& presentationParameters = device9->mPresentationParameters;
	if (presentationParameters.BackBufferFormat == D3DFMT_UNKNOWN)
	{
		HWND handle = device9->mFocusWindow;
		auto swapChain = GetSwapChain(device, handle, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight);
		presentationParameters.BackBufferFormat = ConvertFormat(swapChain->mSurfaceFormat);				   
	}

	if (presentationParameters.AutoDepthStencilFormat == D3DFMT_UNKNOWN)
	{
		presentationParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	}

	mDevices.push_back(device);
}

void StateManager::DestroyInstance(size_t id)
{
	mInstances[id].reset();
}

void StateManager::CreateInstance()
{
#ifdef _DEBUG
	bool enableDebugLayers = true;
#else
	bool enableDebugLayers = false;
#endif

	if (mOptions.count("EnableDebugLayers"))
	{
		enableDebugLayers = mOptions["EnableDebugLayers"].as<uint32_t>();
	}

	auto ptr = std::make_shared<RealInstance>(enableDebugLayers);

	//Get an instance handle.
	auto& vulkanInstance = ptr->mInstance;
	
	if (enableDebugLayers)
	{
		pfn_vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vulkanInstance.getProcAddr("vkCreateDebugReportCallbackEXT"));
		pfn_vkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vulkanInstance.getProcAddr("vkDebugReportMessageEXT"));
		pfn_vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vulkanInstance.getProcAddr("vkDestroyDebugReportCallbackEXT"));

		vk::DebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
		callbackCreateInfo.flags = vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::ePerformanceWarning;
		callbackCreateInfo.pfnCallback = &DebugReportCallback;
		callbackCreateInfo.pUserData = this;

		vk::Result result = vulkanInstance.createDebugReportCallbackEXT(&callbackCreateInfo, nullptr, &ptr->mCallback);
		if (result == vk::Result::eSuccess)
		{
			BOOST_LOG_TRIVIAL(info) << "StateManager::CreateInstance vkCreateDebugReportCallbackEXT succeeded.";
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateInstance vkCreateDebugReportCallbackEXT failed with return code of " << GetResultString((VkResult)result);
			return;
		}
	}

	mInstances.push_back(ptr);


}

void StateManager::DestroyVertexBuffer(size_t id)
{
	mVertexBuffers[id].reset();
}

void StateManager::CreateVertexBuffer(size_t id, void* argument1)
{
	vk::Result result;
	auto device = mDevices[id];
	CVertexBuffer9* vertexBuffer9 = bit_cast<CVertexBuffer9*>(argument1);
	auto ptr = std::make_shared<RealVertexBuffer>(device.get());

	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.size = vertexBuffer9->mLength;
	bufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
	//bufferCreateInfo.flags = 0;									 

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

	result = (vk::Result)vmaCreateBuffer(ptr->mRealDevice->mAllocator, (VkBufferCreateInfo*)&bufferCreateInfo, &allocInfo, (VkBuffer*)&ptr->mBuffer, &ptr->mAllocation, &ptr->mAllocationInfo);

	//d3d9 apps assume memory is cleared
	result = (vk::Result)vmaMapMemory(ptr->mRealDevice->mAllocator, ptr->mAllocation, &ptr->mData);
	memset(ptr->mData, 0, vertexBuffer9->mLength);
	vmaUnmapMemory(ptr->mRealDevice->mAllocator, ptr->mAllocation);
	ptr->mData = nullptr;

	uint32_t attributeStride = 0;

	if (vertexBuffer9->mFVF)
	{
		if ((vertexBuffer9->mFVF & D3DFVF_XYZ) == D3DFVF_XYZ)
		{
			attributeStride += (sizeof(float) * 3);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
		{
			attributeStride += sizeof(uint32_t);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX1) == D3DFVF_TEX1)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX2) == D3DFVF_TEX2)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX3) == D3DFVF_TEX3)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX4) == D3DFVF_TEX4)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX5) == D3DFVF_TEX5)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX6) == D3DFVF_TEX6)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX7) == D3DFVF_TEX7)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX8) == D3DFVF_TEX8)
		{
			attributeStride += (sizeof(float) * 2);
		}

		//mSize = mLength / (sizeof(float)*3 + sizeof(DWORD));
		ptr->mSize = vertexBuffer9->mLength / attributeStride;
		vertexBuffer9->mSize = ptr->mSize;
	}
	else
	{
		ptr->mSize = vertexBuffer9->mSize;
	}

	mVertexBuffers.push_back(ptr);
}

void StateManager::DestroyIndexBuffer(size_t id)
{
	mIndexBuffers[id].reset();
}

void StateManager::CreateIndexBuffer(size_t id, void* argument1)
{
	vk::Result result;
	auto device = mDevices[id];
	CIndexBuffer9* indexBuffer9 = bit_cast<CIndexBuffer9*>(argument1);
	auto ptr = std::make_shared<RealIndexBuffer>(device.get());

	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.size = indexBuffer9->mLength;
	bufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
	//bufferCreateInfo.flags = 0;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

	result = (vk::Result)vmaCreateBuffer(ptr->mRealDevice->mAllocator, (VkBufferCreateInfo*)&bufferCreateInfo, &allocInfo, (VkBuffer*)&ptr->mBuffer, &ptr->mAllocation, &ptr->mAllocationInfo);

	switch (indexBuffer9->mFormat)
	{
	case D3DFMT_INDEX16:
		ptr->mIndexType = vk::IndexType::eUint16;
		indexBuffer9->mSize = indexBuffer9->mLength / sizeof(uint16_t); //WORD
		break;
	case D3DFMT_INDEX32:
		ptr->mIndexType = vk::IndexType::eUint32;
		indexBuffer9->mSize = indexBuffer9->mLength / sizeof(uint32_t);
		break;
	default:
		BOOST_LOG_TRIVIAL(fatal) << "CIndexBuffer9::CIndexBuffer9 invalid D3DFORMAT of " << indexBuffer9->mFormat;
		break;
	}

	ptr->mSize = indexBuffer9->mSize;

	mIndexBuffers.push_back(ptr);
}

void StateManager::DestroyTexture(size_t id)
{
	mTextures[id].reset();
}

void StateManager::CreateTexture(size_t id, void* argument1)
{
	vk::Result result;
	auto device = mDevices[id];
	CTexture9* texture9 = bit_cast<CTexture9*>(argument1);
	std::shared_ptr<RealTexture> ptr = std::make_shared<RealTexture>(device.get());

	ptr->mRealFormat = ConvertFormat(texture9->mFormat);

	if (ptr->mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateTexture unknown format: " << texture9->mFormat;
	}

	ptr->mExtent = vk::Extent3D(texture9->mWidth, texture9->mHeight, 1);
	ptr->mLevels = texture9->mLevels;
	ptr->mLayers = 1;

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.format = ptr->mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
	imageCreateInfo.extent = ptr->mExtent;
	imageCreateInfo.mipLevels = texture9->mLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eColorAttachment;
	//imageCreateInfo.flags = 0;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined; //VK_IMAGE_LAYOUT_PREINITIALIZED;

	auto& vulkanDevice = device->mDevice;

	VmaAllocationCreateInfo imageAllocInfo = {};
	imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	result = (vk::Result)vmaCreateImage(ptr->mRealDevice->mAllocator, (VkImageCreateInfo*)&imageCreateInfo, &imageAllocInfo, (VkImage*)&ptr->mImage, &ptr->mImageAllocation, &ptr->mImageAllocationInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface vmaCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = ptr->mImage;
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.format = ptr->mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	imageViewCreateInfo.subresourceRange.levelCount = texture9->mLevels;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	switch (texture9->mFormat)
	{
	case D3DFMT_A8:
		//TODO: Revisit A8 mapping.
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eR);
		break;
	case D3DFMT_L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_L16:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	result = vulkanDevice.createImageView(&imageViewCreateInfo, nullptr, &ptr->mImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateTexture vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	device->SetImageLayout(ptr->mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

	mTextures.push_back(ptr);
}

void StateManager::DestroyCubeTexture(size_t id)
{
	mTextures[id].reset();
}

void StateManager::CreateCubeTexture(size_t id, void* argument1)
{
	vk::Result result;
	auto device = mDevices[id];
	CCubeTexture9* texture9 = bit_cast<CCubeTexture9*>(argument1);
	std::shared_ptr<RealTexture> ptr = std::make_shared<RealTexture>(device.get());

	ptr->mRealFormat = ConvertFormat(texture9->mFormat);

	if (ptr->mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture unknown format: " << texture9->mFormat;
	}

	ptr->mExtent = vk::Extent3D(texture9->mEdgeLength, texture9->mEdgeLength, 1);
	ptr->mLevels = texture9->mLevels;
	ptr->mLayers = 6;

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.format = ptr->mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
	imageCreateInfo.extent = vk::Extent3D(texture9->mEdgeLength, texture9->mEdgeLength, 1);
	imageCreateInfo.mipLevels = texture9->mLevels;
	imageCreateInfo.arrayLayers = 6;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;
	imageCreateInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined; //VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	auto& vulkanDevice = device->mDevice;

	VmaAllocationCreateInfo imageAllocInfo = {};
	imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	result = (vk::Result)vmaCreateImage(ptr->mRealDevice->mAllocator, (VkImageCreateInfo*)&imageCreateInfo, &imageAllocInfo, (VkImage*)&ptr->mImage, &ptr->mImageAllocation, &ptr->mImageAllocationInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface vmaCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = ptr->mImage;
	imageViewCreateInfo.viewType = vk::ImageViewType::eCube; //e2D
	imageViewCreateInfo.format = ptr->mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	//imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.levelCount = texture9->mLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 6;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	switch (texture9->mFormat)
	{
	case D3DFMT_A8:
		//TODO: Revisit A8 mapping.
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eR);
		break;
	case D3DFMT_L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_L16:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	result = vulkanDevice.createImageView(&imageViewCreateInfo, nullptr, &ptr->mImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	device->SetImageLayout(ptr->mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

	mTextures.push_back(ptr);
}

void StateManager::DestroyVolumeTexture(size_t id)
{
	mTextures[id].reset();
}

void StateManager::CreateVolumeTexture(size_t id, void* argument1)
{
	vk::Result result;
	std::shared_ptr<RealDevice> device = mDevices[id];
	CVolumeTexture9* texture9 = bit_cast<CVolumeTexture9*>(argument1);
	std::shared_ptr<RealTexture> ptr = std::make_shared<RealTexture>(device.get());

	ptr->mRealFormat = ConvertFormat(texture9->mFormat);

	if (ptr->mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture unknown format: " << texture9->mFormat;
	}

	ptr->mExtent = vk::Extent3D(texture9->mWidth, texture9->mHeight, 1);
	ptr->mLevels = texture9->mLevels;
	ptr->mLayers = 1;

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e3D;
	imageCreateInfo.format = ptr->mRealFormat;
	imageCreateInfo.extent = vk::Extent3D(texture9->mWidth, texture9->mHeight, texture9->mDepth);
	imageCreateInfo.mipLevels = texture9->mLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;
	//imageCreateInfo.flags = vk::ImageCreateFlagBits::eVolumeCompatible;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined; //VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	auto& vulkanDevice = device->mDevice;

	VmaAllocationCreateInfo imageAllocInfo = {};
	imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	result = (vk::Result)vmaCreateImage(ptr->mRealDevice->mAllocator, (VkImageCreateInfo*)&imageCreateInfo, &imageAllocInfo, (VkImage*)&ptr->mImage, &ptr->mImageAllocation, &ptr->mImageAllocationInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface vmaCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = ptr->mImage;
	imageViewCreateInfo.viewType = vk::ImageViewType::e3D;
	imageViewCreateInfo.format = ptr->mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	//imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.levelCount = texture9->mLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	switch (texture9->mFormat)
	{
	case D3DFMT_A8:
		//TODO: Revisit A8 mapping.
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eR);
		break;
	case D3DFMT_L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_L16:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	result = vulkanDevice.createImageView(&imageViewCreateInfo, nullptr, &ptr->mImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	device->SetImageLayout(ptr->mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

	mTextures.push_back(ptr);
}

void StateManager::DestroySurface(size_t id)
{
	if (mSurfaces.size())
	{
		mSurfaces[id].reset();
	}
}

void StateManager::CreateSurface(size_t id, void* argument1)
{
	auto device = mDevices[id];
	CSurface9* surface9 = bit_cast<CSurface9*>(argument1);
	vk::Image* parentImage = nullptr;

	if (surface9->mTexture != nullptr)
	{
		parentImage = &mTextures[surface9->mTexture->mId]->mImage;
	}

	std::shared_ptr<RealSurface> ptr = std::make_shared<RealSurface>(device.get(), surface9, parentImage);

	auto& realFormat = ptr->mRealFormat;
	if (realFormat == vk::Format::eD16UnormS8Uint || realFormat == vk::Format::eD24UnormS8Uint || realFormat == vk::Format::eD32SfloatS8Uint)
	{
		device->SetImageLayout(ptr->mStagingImage, vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
	}
	else if (realFormat == vk::Format::eS8Uint)
	{
		device->SetImageLayout(ptr->mStagingImage, vk::ImageAspectFlagBits::eStencil, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
	}
	else if (realFormat == vk::Format::eD16Unorm)
	{
		device->SetImageLayout(ptr->mStagingImage, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
	}
	else
	{
		device->SetImageLayout(ptr->mStagingImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
	}

	mSurfaces.push_back(ptr);
}

void StateManager::DestroyVolume(size_t id)
{
	mSurfaces[id].reset();
}

void StateManager::CreateVolume(size_t id, void* argument1)
{
	auto device = mDevices[id];
	CVolume9* volume9 = bit_cast<CVolume9*>(argument1);
	std::shared_ptr<RealSurface> ptr = std::make_shared<RealSurface>(device.get(), volume9);

	device->SetImageLayout(ptr->mStagingImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

	mSurfaces.push_back(ptr);
}

void StateManager::DestroyShader(size_t id)
{
	mShaderConverters[id].reset();
}

void StateManager::CreateShader(size_t id, void* argument1, void* argument2, void* argument3)
{
	//vk::Result result;
	auto device = mDevices[id];
	DWORD* pFunction = (DWORD*)(argument1);
	bool isVertex = (bool)(argument2);
	size_t* size = (size_t*)(argument3);

	if (isVertex)
	{
		std::shared_ptr<ShaderConverter> ptr = std::make_shared<ShaderConverter>(device->mDevice, device->mDeviceState.mVertexShaderConstantSlots);
		ptr->Convert((uint32_t*)pFunction);
		(*size) = ptr->mConvertedShader.Size;
		mShaderConverters.push_back(ptr);
	}
	else
	{
		std::shared_ptr<ShaderConverter> ptr = std::make_shared<ShaderConverter>(device->mDevice, device->mDeviceState.mPixelShaderConstantSlots);
		ptr->Convert((uint32_t*)pFunction);
		(*size) = ptr->mConvertedShader.Size;
		mShaderConverters.push_back(ptr);
	}

}

void StateManager::DestroyQuery(size_t id)
{
	mQueries[id].reset();
}

void StateManager::CreateQuery(size_t id, void* argument1)
{
	auto device = mDevices[id];
	CQuery9* query9 = bit_cast<CQuery9*>(argument1);
	std::shared_ptr<RealQuery> ptr = std::make_shared<RealQuery>(device.get());

	vk::QueryPoolCreateInfo info;
	info.queryCount = 1;

	/*
	case D3DQUERYTYPE_VCACHE:
	break;
	case D3DQUERYTYPE_RESOURCEMANAGER:
	break;
	case D3DQUERYTYPE_VERTEXSTATS:
	break;
	case D3DQUERYTYPE_PIPELINETIMINGS:
	break;
	case D3DQUERYTYPE_INTERFACETIMINGS:
	break;
	case D3DQUERYTYPE_VERTEXTIMINGS:
	break;
	case D3DQUERYTYPE_PIXELTIMINGS:
	break;
	case D3DQUERYTYPE_BANDWIDTHTIMINGS:
	break;
	case D3DQUERYTYPE_CACHEUTILIZATION:
	break;
	case D3DQUERYTYPE_EVENT:
	break;
	*/

	/*
	eInputAssemblyVertices = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT,
	eInputAssemblyPrimitives = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT,
	eVertexShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT,
	eGeometryShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT,
	eGeometryShaderPrimitives = VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT,
	eClippingInvocations = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT,
	eClippingPrimitives = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT,
	eFragmentShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT,
	eTessellationControlShaderPatches = VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT,
	eTessellationEvaluationShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT,
	eComputeShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT
	*/

	switch (query9->mType)
	{
	case D3DQUERYTYPE_OCCLUSION:
		info.queryType = vk::QueryType::eOcclusion;
		break;
	case D3DQUERYTYPE_TIMESTAMP:
		info.queryType = vk::QueryType::eTimestamp;
		break;
	case D3DQUERYTYPE_TIMESTAMPDISJOINT:
		info.queryType = vk::QueryType::eTimestamp;
		break;
	case D3DQUERYTYPE_TIMESTAMPFREQ:
		info.queryType = vk::QueryType::eTimestamp;
		break;
	default:
		info.queryType = vk::QueryType::ePipelineStatistics;
		info.pipelineStatistics = vk::QueryPipelineStatisticFlagBits::eVertexShaderInvocations;
		BOOST_LOG_TRIVIAL(info) << "StateManager::CreateQuery Unsupported query type " << query9->mType;
		break;
	}

	vk::ResultValue<vk::QueryPool> poolResult = device->mDevice.createQueryPool(info, nullptr); //&(ptr->mQueryPool)
	if (poolResult.result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateQuery vkCreateQueryPool failed with return code of " << GetResultString((VkResult)poolResult.result);
		return;
	}
	ptr->mQueryPool = poolResult.value;

	mQueries.push_back(ptr);
}

std::shared_ptr<RealSwapChain> StateManager::GetSwapChain(std::shared_ptr<RealDevice> realDevice, HWND handle, uint32_t width, uint32_t height)
{
	auto it = mSwapChains.find(handle);
	if (it != mSwapChains.end())
	{
		return (*it).second;
	}
	else
	{
		vk::Instance instance = realDevice->mInstance;
		vk::PhysicalDevice physicalDevice = realDevice->mPhysicalDevice;
		vk::Device device = realDevice->mDevice;
		HWND windowHandle = handle;

		if (realDevice->mDeviceState.mRenderTarget != nullptr && realDevice->mDeviceState.mRenderTarget->mColorSurface != nullptr)
		{
			width = realDevice->mDeviceState.mRenderTarget->mColorSurface->mExtent.width;
			height = realDevice->mDeviceState.mRenderTarget->mColorSurface->mExtent.height;
		}

		auto output = std::make_shared<RealSwapChain>(instance, physicalDevice, device, windowHandle, width, height);
		mSwapChains[handle] = output;

		return output;
	}
}