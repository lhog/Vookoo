////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2016
//
// Vookoo: descriptor set wraps VkDescriptorSet
// 

#ifndef VKU_DESCRIPTOR_SET_INCLUDED
#define VKU_DESCRIPTOR_SET_INCLUDED

#include <vku/resource.hpp>
#include <vku/sampler.hpp>

namespace vku {

class descriptorSetLayout {
public:
  descriptorSetLayout() {
  }

  descriptorSetLayout &buffer(const vku::buffer &buffer) {
    VkDescriptorBufferInfo d = {};
    d.buffer = buffer.buf();
    d.offset = 0;
    d.range = buffer.size();
    buffers_.emplace_back(d);
  }

  descriptorSetLayout &sampler(const vku::sampler &sampler, const vku::image &image) {
    VkDescriptorImageInfo d = {};
    d.sampler = sampler.get();
    d.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    d.imageView = image.view();
    samplers_.emplace_back(d);
  }

  VkDescriptorSet allocateDescriptorSets(const vku::device &device, const vku::descriptorPool &pool) {
    info_.descriptorPool = pool.get();
    info_.descriptorSetCount = 1;
    info_.pSetLayouts = 
    VkDescriptorSet result = VK_NULL_HANDLE;
    VkResult err = vkAllocateDescriptorSets(device, &info_, &result);
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

  descriptorSet(const vku::device &device, const vku::descriptorPool &pool, vku::descriptorSetLayout &layout) : resource(device), pool_(pool) {
    set(layout.allocateDescriptorSets(device, pool), true);
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
    vkDestroyDescriptorPool(device(), pool(), nullptr);
  }

private:
  void copy(const descriptorSet &rhs) {
    (resource&)*this = (const resource&)rhs;
  }

  void move(descriptorSet &&rhs) {
    (resource&)*this = (resource&&)rhs;
  }

  //VkWriteDescriptorSet writeDescriptorSet = {};
  vku::descriptorPool pool_;
};


} // vku

#endif
