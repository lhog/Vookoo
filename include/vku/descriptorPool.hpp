////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2016
//
// Vookoo: descriptor pool wraps VkDescriptorPool
// 

#ifndef VKU_DESCRIPTOR_POOL_INCLUDED
#define VKU_DESCRIPTOR_POOL_INCLUDED

#include <vku/resource.hpp>

namespace vku {

class descriptorPoolLayout {
public:
  descriptorPoolLayout() {
  }

  descriptorPoolLayout &uniformBuffers(uint32_t count) {
    return descriptors(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, count);
  }

  descriptorPoolLayout &samplers(uint32_t count) {
    return descriptors(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, count);
  }

  descriptorPoolLayout &descriptors(VkDescriptorType type, uint32_t count) {
    VkDescriptorPoolSize ps = {};
    ps.type = type;
    ps.descriptorCount = count;
    poolSizes_.emplace_back(ps);
    return *this;
  }

  VkDescriptorPool createDescriptorPool(const vku::device &device) {
    info_.poolSizeCount = (uint32_t)poolSizes_.size();
    info_.pPoolSizes = poolSizes_.data();
    VkDescriptorPool pool = VK_NULL_HANDLE;
    VkResult err = vkCreateDescriptorPool(device, &info_, nullptr, &pool);
    if (err) throw error(err, __FILE__, __LINE__);
    return pool;
  }
private:
  std::vector<VkDescriptorPoolSize> poolSizes_;
  VkDescriptorPoolCreateInfo info_;
};

class descriptorPool : public resource<VkDescriptorPool, descriptorPool> {
public:
  descriptorPool() {
  }

  descriptorPool(const vku::device &device, vku::descriptorPoolLayout &layout) : resource(device) {
    set(layout.createDescriptorPool(device), true);
  }

  // allocate a descriptor set for a buffer
  VkWriteDescriptorSet *allocateDescriptorSet(const buffer &buffer, const VkDescriptorSetLayout *layout, VkDescriptorSet *descriptorSets) {
    // Update descriptor sets determining the shader binding points
    // For every binding point used in a shader there needs to be one
    // descriptor set matching that binding point

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layout;

    VkResult err = vkAllocateDescriptorSets(device(), &allocInfo, descriptorSets);
    if (err) throw error(err, __FILE__, __LINE__);

    // Binding 0 : Uniform buffer
    VkDescriptorBufferInfo desc = buffer.desc();
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSets[0];
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.pBufferInfo = &desc;
    // Binds this uniform buffer to binding point 0
    writeDescriptorSet.dstBinding = 0;

    return &writeDescriptorSet;
  }

  /// move constructor
  descriptorPool(descriptorPool &&rhs) {
    move(std::move(rhs));
  }

  /// move operator
  descriptorPool &operator=(descriptorPool &&rhs) {
    move(std::move(rhs));
    return *this;
  }

  /// copy constructor
  descriptorPool(const descriptorPool &rhs) {
    copy(rhs);
  }

  /// copy operator
  descriptorPool &operator=(const descriptorPool &rhs) {
    copy(rhs);
    return *this;
  }

  void destroy() {
    vkDestroyDescriptorPool(device(), get(), nullptr);
  }

private:
  void copy(const descriptorPool &rhs) {
    (resource&)*this = (const resource&)rhs;
  }

  void move(descriptorPool &&rhs) {
    (resource&)*this = (resource&&)rhs;
  }

  VkWriteDescriptorSet writeDescriptorSet = {};
};


} // vku

#endif
