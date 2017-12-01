#pragma once
#include <vulkan/vulkan.hpp>

namespace vk { class Instance; }

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				/**
				 * \brief Simple class that retrieves details about the queue families and queues supported by a device, and selects the best possible queue families based on that
				 */
				struct QueueFamilyIndices
				{
					/**
					 * \brief The graphics family
					 */
					int graphicsFamily = -1;
					/**
					 * \brief The present family
					 */
					int presentFamily = -1;

					/**
					 * \return Returns true if both the graphics and the present family have been found 
					 */
					bool isComplete() const;

					/**
					 * \brief Tries to find a suitable graphics and present queue
					 * \param device The physicaldevice, used to get the available queue families
					 * \param surface The surface, used to get the available queue families
					 * \return 
					 */
					static QueueFamilyIndices get(vk::PhysicalDevice device, vk::SurfaceKHR surface);
				};

				/**
				 * \brief Device is an interface that wraps around the creation/selection of the vk::Device, vk::PhysicalDevice and the graphics/present queue.
				 */
				class Device
				{
				public:
					/**
					 * \brief Creates a new instance of Device. Creates a vk::Device, selects a vk::PhysicalDevice and loads the graphics and present queue.
					 * \param instance The vk::Instance used to create the vk devices
					 * \param surface 
					 */
					Device(vk::Instance instance, vk::SurfaceKHR surface);
					/**
					 * \brief Destroys the created vk::Device. PhysicalDevice and the present queues will clean themselves up on destruction of device/instance.
					 */
					~Device();

					/**
					 * \brief Returns the physical device
					 * \return The physical device
					 */
					vk::PhysicalDevice getPhysical() const { return physicalDevice; }
					/**
					 * \brief Returns the logical device
					 * \return The vk::Device
					 */
					vk::Device getLogical() const { return device; }
					
					/**
					 * \brief Returns the graphics queue attached to the device
					 * \return A vk::Queue object representing the graphics queue
					 */
					vk::Queue getGraphicsQueue() const { return graphicsQueue; }
					/**
					 * \brief Returns the present queue attached to the device
					 * \return A vk::Queue object representing the present queue
					 */
					vk::Queue getPresentQueue() const { return presentQueue; }
				private:
					/**
					 * \brief The selected physical device (GPU)
					 */
					vk::PhysicalDevice physicalDevice;
					/**
					 * \brief The created logical device
					 */
					vk::Device device;

					/**
					 * \brief The graphics queue attached to the device
					 */
					vk::Queue graphicsQueue;
					/**
					 * \brief The present queue attached to the device
					 */
					vk::Queue presentQueue;

					/**
					 * \brief Selects the physical device based on the application's requirements
					 * \param surface The surface used to check for the capabilities of the physical device
					 */
					void getPhysical(vk::Instance, vk::SurfaceKHR surface);
					/**
					 * \brief Creates a logical device
					 * \param surface The surface, used to check for the physical device's capabilities
					 */
					void createLogical(vk::SurfaceKHR surface);

					/**
					 * \brief Rates a given physical device based on its features
					 * \param device The physical device to be rated
					 * \return The physical device's score
					 */
					static int rateDevice(vk::PhysicalDevice device);
					/**
					 * \brief Checks if a given physical device meets the requirements.
					 * \param device The physical device to be checked
					 * \param surface The surface, used to check for the physical device's capabilities
					 * \return True if the physical device meets the requirements
					 */
					static bool isSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface);
				};
			}
		}
	}
}