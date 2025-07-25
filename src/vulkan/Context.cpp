//
// Created by Amila Abeygunasekara on Sun 20/07/2025.
//

#include "Context.h"

#include <format>
#include <stdexcept>

namespace vkpbr::vk {

Context::Context()
{
    vkb::InstanceBuilder builder;
    auto instanceRet = builder.set_app_name("PBR Engine")
                              .request_validation_layers()
                              .use_default_debug_messenger()
                              .build();
    if (!instanceRet)
    {
        throw std::runtime_error(std::format("Failed to create Vulkan instance: {}\n", instanceRet.error().message()));
    }
    vkbInstance_ = instanceRet.value();
    instance = vkbInstance_.instance;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not create an OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable window resizing for now
    pWindow = glfwCreateWindow(1280, 720, "PBR Engine", nullptr, nullptr);

    VkResult glfwResult = glfwCreateWindowSurface(vkbInstance_, pWindow, nullptr, &surface);
    if (glfwResult != VK_SUCCESS)
    {
        throw std::runtime_error(std::format("Failed to create Vulkan surface: {}\n", std::to_string(glfwResult)));
    }

    vkb::PhysicalDeviceSelector selector(vkbInstance_);
    auto physicalDeviceRet = selector.set_surface(surface).select();
    if (!physicalDeviceRet)
    {
        throw std::runtime_error(std::format("Failed to select physical device: {}\n", physicalDeviceRet.error().message()));
    }

    vkb::DeviceBuilder deviceBuilder(physicalDeviceRet.value());
    auto deviceRet = deviceBuilder.build();
    if (!deviceRet)
    {
        throw std::runtime_error(std::format("Failed to create physical device: {}\n", deviceRet.error().message()));
    }
    vkbDevice_ = deviceRet.value();
    device = vkbDevice_.device;

    auto graphicsQueueRet = vkbDevice_.get_queue(vkb::QueueType::graphics);
    if (!graphicsQueueRet)
    {
        throw std::runtime_error(std::format("Failed to get graphics queue: {}\n", graphicsQueueRet.error().message()));
    }
    graphicsQueue = graphicsQueueRet.value();

    vkb::SwapchainBuilder swapchainBuilder(vkbDevice_);
    auto swapchainRet = swapchainBuilder.build();
    if (!swapchainRet)
    {
        throw std::runtime_error(std::format("Failed to create swapchain: {}\n", swapchainRet.error().message()));
    }
    vkbSwapchain_ = swapchainRet.value();
    swapchain = vkbSwapchain_.swapchain;
}

Context::~Context()
{
    vkb::destroy_swapchain(vkbSwapchain_);
    vkb::destroy_device(vkbDevice_);
    vkb::destroy_surface(vkbInstance_, surface);
    vkb::destroy_instance(vkbInstance_);
    glfwDestroyWindow(pWindow);
    glfwTerminate();
}

} // vkpbr::vk