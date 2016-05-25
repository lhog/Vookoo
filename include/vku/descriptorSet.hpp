////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2016
//
// Vookoo: descriptor set wraps VkDescriptorSet
// 

#ifndef VKU_DESCRIPTOR_SET_INCLUDED
#define VKU_DESCRIPTOR_SET_INCLUDED

#include <vku/resource.hpp>

namespace vku {

class descriptorSetLayout {
public:
  descriptorSetLayout() {
  }

  descriptorSetLayout &buffer(const vku::buffer &buffer) {
    buffers_.emplace_back(buffer.desc());
  }

  descriptorSetLayout &sampler(const vku::sampler &sampler) {
    samplers_.emplace_back(sampler.desc());
  }

  VkDescriptorSet allocateDescriptorSet(const vku::device &device, const vku::descriptorSetPool &pool) {
    info_.descriptorPool = pool.get();
    info_.descriptorSetCount = (uint32_t)(buffers_.size() + samplers_.size());
    VkDescriptorSet result = VK_NULL_HANDLE;
    VkResult err = vkAllocateDescriptorSets(device, &info_, nullptr, &result);
    if (err) throw error(err, __FILE__, __LINE__);

    return result;
  }
private:
  std::vector<VkDescriptorBufferInfo> buffers_;
  std::vector<VkDescriptorImageInfo> samplers_;
  VkDescriptorSetAllocateInfo info_;
};

class descriptorSet : public resource<VkDescriptorSet, descriptorSet> {
public:
  descriptorSet() {
  }

  descriptorSet(const vku::device &device, const vku::descriptorSetPool &pool, vku::descriptorSetLayout &layout) : resource(device) {
    set(layout.allocateDescriptorSet(device, pool), true);
  }

  // allocate a descriptor set for a buffer
  VkWriteDescriptorSet *allocateDescriptorSet(const buffer &buffer, const VkDescriptorSetLayout *layout, VkDescriptorSet *descriptorSets) {
    // Update descriptor sets determining the shader binding points
    // For every binding point used in a shader there needs to be one
    // descriptor set matching that binding point

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorSet = get();
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
  descriptorSet(descriptorSet &&rhs) {
    move(std::move(rhs));
  }

  /// move operator
  descriptorSet &operator=(descriptorSet &&rhs) {
    move(std::move(rhs));
    return *this;
  }

  /// copy constructor
  descriptorSet(const descriptorSet &rhs) {
    copy(rhs);
  }

  /// copy operator
  descriptorSet &operator=(const descriptorSet &rhs) {
    copy(rhs);
    return *this;
  }

  void destroy() {
    vkDestroyDescriptorPool(device(), get(), nullptr);
  }

private:
  void copy(const descriptorSet &rhs) {
    (resource&)*this = (const resource&)rhs;
  }

  void move(descriptorSet &&rhs) {
    (resource&)*this = (resource&&)rhs;
  }

  VkWriteDescriptorSet writeDescriptorSet = {};
};


} // vku

#endif
