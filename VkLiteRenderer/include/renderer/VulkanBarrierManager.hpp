
#pragma once

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cstdint>
#include <cassert>

class VulkanBarrierManager
{
public:
    enum class ResourceType : uint8_t
    {
        BUFFER,
        IMAGE
    };

    struct SyncRequest
    {
        VkAccessFlags        access_mask = 0;
        VkPipelineStageFlags stage_mask = 0;
        VkImageLayout        layout = VK_IMAGE_LAYOUT_UNDEFINED; // For images only
        VkImageAspectFlags   aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT; // For images only
        uint32_t            mip_levels = VK_REMAINING_MIP_LEVELS;
        uint32_t            array_layers = VK_REMAINING_ARRAY_LAYERS;
        uint32_t            base_mip_level = 0;
        uint32_t            base_array_layer = 0;
        uint32_t            src_queue_family = VK_QUEUE_FAMILY_IGNORED;
        uint32_t            dst_queue_family = VK_QUEUE_FAMILY_IGNORED;
    };

    struct SyncState
    {
        struct BarrierMasks
        {
            VkAccessFlags        access_mask = 0;
            VkPipelineStageFlags stage_mask = 0;

            void reset()
            {
                access_mask = 0;
                stage_mask = 0;
            }
        };

        BarrierMasks last_writer_masks;
        BarrierMasks active_readers_masks;
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        uint32_t current_queue_family = VK_QUEUE_FAMILY_IGNORED;

        void reset()
        {
            last_writer_masks.reset();
            active_readers_masks.reset();
            layout = VK_IMAGE_LAYOUT_UNDEFINED;
            current_queue_family = VK_QUEUE_FAMILY_IGNORED;
        }
    };

    struct BarrierData
    {
        VkAccessFlags        src_access_mask = 0;
        VkAccessFlags        dst_access_mask = 0;
        VkPipelineStageFlags src_stage_mask = 0;
        VkPipelineStageFlags dst_stage_mask = 0;
        VkImageLayout        src_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout        dst_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        ResourceType         resource_type = ResourceType::BUFFER;
        uintptr_t           resource_handle = 0;
        VkImageAspectFlags   aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
        uint32_t            mip_levels = VK_REMAINING_MIP_LEVELS;
        uint32_t            array_layers = VK_REMAINING_ARRAY_LAYERS;
        uint32_t            base_mip_level = 0;
        uint32_t            base_array_layer = 0;
        uint32_t            src_queue_family = VK_QUEUE_FAMILY_IGNORED;
        uint32_t            dst_queue_family = VK_QUEUE_FAMILY_IGNORED;
    };

    struct Resource
    {
        ResourceType type;
        uintptr_t    handle;
        uint64_t     hash;

        // Constructor for easier creation
        Resource(ResourceType t, uintptr_t h)
            : type(t), handle(h), hash(calculate_hash(h))
        {
        }

        Resource(ResourceType t, uintptr_t h, uint64_t custom_hash)
            : type(t), handle(h), hash(custom_hash)
        {
        }

        // Equality operators for containers
        bool operator==(const Resource& other) const
        {
            return type == other.type && handle == other.handle;
        }

        bool operator!=(const Resource& other) const
        {
            return !(*this == other);
        }

    private:
        static uint64_t calculate_hash(uintptr_t handle)
        {
            // Better hash function than just using std::hash directly
            uint64_t h = static_cast<uint64_t>(handle);
            h ^= h >> 33;
            h *= 0xff51afd7ed558ccdULL;
            h ^= h >> 33;
            h *= 0xc4ceb9fe1a85ec53ULL;
            h ^= h >> 33;
            return h;
        }
    };

private:
    std::unordered_map<uint64_t, SyncState> resource_states_;
    std::vector<BarrierData> pending_barriers_;

    // Comprehensive mask of all write access types (including missing ones)
    static constexpr VkAccessFlags ALL_WRITE_ACCESSES_MASK =
        VK_ACCESS_SHADER_WRITE_BIT |
        VK_ACCESS_TRANSFER_WRITE_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_HOST_WRITE_BIT |
        VK_ACCESS_MEMORY_WRITE_BIT |
        VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT |
        VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT |
        VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR |
        VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV |
        VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV; // This can be write for some ops

    // Comprehensive mask of all read access types
    static constexpr VkAccessFlags ALL_READ_ACCESSES_MASK =
        VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
        VK_ACCESS_INDEX_READ_BIT |
        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
        VK_ACCESS_UNIFORM_READ_BIT |
        VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
        VK_ACCESS_SHADER_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
        VK_ACCESS_TRANSFER_READ_BIT |
        VK_ACCESS_HOST_READ_BIT |
        VK_ACCESS_MEMORY_READ_BIT |
        VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT |
        VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT |
        VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR |
        VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV |
        VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;

    // Enhanced validation with proper stage/access combinations
    bool is_valid_stage_access_combination(VkPipelineStageFlags stages, VkAccessFlags access) const
    {
        // Allow empty access with specific stages
        if (access == 0)
        {
            return stages == VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT ||
                   stages == VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT ||
                   stages == VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        }

        // Validate specific stage/access combinations
        if ((access & VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT) &&
            !(stages & VK_PIPELINE_STAGE_VERTEX_INPUT_BIT))
            return false;

        if ((access & VK_ACCESS_UNIFORM_READ_BIT) &&
            !(stages & (VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                       VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
                       VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
                       VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                       VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT )))
            return false;

        return true;
    }

    // Helper to get appropriate aspect mask for depth/stencil images
    VkImageAspectFlags get_depth_stencil_aspect_mask(VkAccessFlags access) const
    {
        VkImageAspectFlags aspect = 0;
        if (access & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                     VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT))
        {
            aspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
            // Note: Should ideally check format to determine if stencil is present
            aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        return aspect != 0 ? aspect : VK_IMAGE_ASPECT_COLOR_BIT;
    }

    // Helper to check if layout transition actually changes layout
    bool is_layout_transition_needed(VkImageLayout old_layout, VkImageLayout new_layout) const
    {
        if (new_layout == VK_IMAGE_LAYOUT_UNDEFINED)
            return false;

        if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED)
            return new_layout != VK_IMAGE_LAYOUT_UNDEFINED;

        return old_layout != new_layout;
    }

public:
    VulkanBarrierManager() = default;

    // Disable copy operations to prevent accidental copying
    VulkanBarrierManager(const VulkanBarrierManager&) = delete;
    VulkanBarrierManager& operator=(const VulkanBarrierManager&) = delete;

    // Enable move operations
    VulkanBarrierManager(VulkanBarrierManager&&) = default;
    VulkanBarrierManager& operator=(VulkanBarrierManager&&) = default;

    void reset()
    {
        resource_states_.clear();
        pending_barriers_.clear();
    }

    // Reset specific resource (useful when resource is destroyed)
    void reset_resource(const Resource& resource)
    {
        auto it = resource_states_.find(resource.hash);
        if (it != resource_states_.end())
        {
            it->second.reset();
        }
    }

    // Request access to a resource and determine if a barrier is needed
    bool request_resource_access(const Resource& resource, const SyncRequest& sync_req)
    {
        // Enhanced input validation
        if (sync_req.stage_mask == 0)
        {
            assert(false && "Stage mask cannot be zero");
            return false;
        }

        if (!is_valid_stage_access_combination(sync_req.stage_mask, sync_req.access_mask))
        {
            assert(false && "Invalid stage/access combination");
            return false;
        }

        // For images, validate that layout is specified for layout-sensitive operations
        if (resource.type == ResourceType::IMAGE &&
            sync_req.access_mask != 0 &&
            sync_req.layout == VK_IMAGE_LAYOUT_UNDEFINED)
        {
            // Allow undefined layout only for specific cases
            bool allow_undefined = (sync_req.access_mask & (VK_ACCESS_TRANSFER_READ_BIT |
                                                            VK_ACCESS_TRANSFER_WRITE_BIT)) != 0;
            if (!allow_undefined)
            {
                assert(false && "Image layout must be specified for most image operations");
                return false;
            }
        }

        auto& sync_state = resource_states_[resource.hash];

        // Check if a barrier is needed based on current sync state
        BarrierData barrier;
        bool needs_barrier = calculate_barrier(sync_state, sync_req, barrier);

        if (needs_barrier)
        {
            barrier.resource_type = resource.type;
            barrier.resource_handle = resource.handle;
            barrier.aspect_mask = sync_req.aspect_mask;
            barrier.mip_levels = sync_req.mip_levels;
            barrier.array_layers = sync_req.array_layers;
            barrier.base_mip_level = sync_req.base_mip_level;
            barrier.base_array_layer = sync_req.base_array_layer;
            barrier.src_queue_family = sync_req.src_queue_family;
            barrier.dst_queue_family = sync_req.dst_queue_family;

            // Auto-detect depth/stencil aspect if not specified correctly
            if (resource.type == ResourceType::IMAGE &&
                barrier.aspect_mask == VK_IMAGE_ASPECT_COLOR_BIT)
            {
                VkImageAspectFlags ds_aspect = get_depth_stencil_aspect_mask(sync_req.access_mask);
                if (ds_aspect != VK_IMAGE_ASPECT_COLOR_BIT)
                {
                    barrier.aspect_mask = ds_aspect;
                }
            }

            pending_barriers_.push_back(barrier);
        }

        return needs_barrier;
    }

    // Overloaded convenience methods
    bool request_buffer_access(VkBuffer buffer, VkAccessFlags access, VkPipelineStageFlags stages)
    {
        if (buffer == VK_NULL_HANDLE)
        {
            assert(false && "Buffer handle cannot be null");
            return false;
        }

        Resource resource(ResourceType::BUFFER, reinterpret_cast<uintptr_t>(buffer));
        SyncRequest sync_req = { access, stages };
        return request_resource_access(resource, sync_req);
    }

    bool request_image_access(VkImage image, VkAccessFlags access, VkPipelineStageFlags stages,
                             VkImageLayout layout, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT)
    {
        if (image == VK_NULL_HANDLE)
        {
            assert(false && "Image handle cannot be null");
            return false;
        }

        Resource resource(ResourceType::IMAGE, reinterpret_cast<uintptr_t>(image));
        SyncRequest sync_req = { access, stages, layout, aspect };
        return request_resource_access(resource, sync_req);
    }

    // Enhanced barrier flushing with better return semantics
    std::vector<BarrierData> flush_pending_barriers()
    {
        if (pending_barriers_.empty())
            return {};

        auto barriers = std::move(pending_barriers_);
        pending_barriers_.clear();
        pending_barriers_.shrink_to_fit(); // Free memory if many barriers were accumulated
        return barriers;
    }

    // Get pending barriers without flushing (for inspection)
    const std::vector<BarrierData>& get_pending_barriers() const noexcept
    {
        return pending_barriers_;
    }

    // Check if there are pending barriers
    bool has_pending_barriers() const noexcept
    {
        return !pending_barriers_.empty();
    }

    // Get number of pending barriers
    size_t get_pending_barriers_count() const noexcept
    {
        return pending_barriers_.size();
    }

    // Clear specific resource state (useful for resource cleanup)
    void clear_resource_state(const Resource& resource)
    {
        resource_states_.erase(resource.hash);
    }

    // Clear all resource states
    void clear_all_resource_states()
    {
        resource_states_.clear();
    }

    // Get current resource state (for debugging)
    const SyncState* get_resource_state(const Resource& resource) const
    {
        auto it = resource_states_.find(resource.hash);
        return it != resource_states_.end() ? &it->second : nullptr;
    }

    // Enhanced barrier emission with better error handling
    void emit_barriers(VkCommandBuffer cmd_buffer, const std::vector<BarrierData>& barriers)
    {
        if (barriers.empty() || cmd_buffer == VK_NULL_HANDLE)
            return;

        std::vector<VkImageMemoryBarrier> image_barriers;
        std::vector<VkBufferMemoryBarrier> buffer_barriers;
        VkPipelineStageFlags src_stage_mask = 0;
        VkPipelineStageFlags dst_stage_mask = 0;

        // Reserve with better estimation
        size_t image_count = 0, buffer_count = 0;
        for (const auto& barrier : barriers)
        {
            if (barrier.resource_type == ResourceType::IMAGE) ++image_count;
            else ++buffer_count;
        }

        image_barriers.reserve(image_count);
        buffer_barriers.reserve(buffer_count);

        for (const auto& barrier : barriers)
        {
            // Validate barrier data
            if (barrier.resource_handle == 0)
            {
                assert(false && "Invalid resource handle in barrier");
                continue;
            }

            src_stage_mask |= barrier.src_stage_mask;
            dst_stage_mask |= barrier.dst_stage_mask;

            if (barrier.resource_type == ResourceType::IMAGE)
            {
                VkImageMemoryBarrier img_barrier = {};
                img_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                img_barrier.pNext = nullptr;
                img_barrier.srcAccessMask = barrier.src_access_mask;
                img_barrier.dstAccessMask = barrier.dst_access_mask;
                img_barrier.oldLayout = barrier.src_layout;
                img_barrier.newLayout = barrier.dst_layout;
                img_barrier.srcQueueFamilyIndex = barrier.src_queue_family;
                img_barrier.dstQueueFamilyIndex = barrier.dst_queue_family;
                img_barrier.image = reinterpret_cast<VkImage>(barrier.resource_handle);
                img_barrier.subresourceRange.aspectMask = barrier.aspect_mask;
                img_barrier.subresourceRange.baseMipLevel = barrier.base_mip_level;
                img_barrier.subresourceRange.levelCount = barrier.mip_levels;
                img_barrier.subresourceRange.baseArrayLayer = barrier.base_array_layer;
                img_barrier.subresourceRange.layerCount = barrier.array_layers;

                image_barriers.push_back(img_barrier);
            }
            else if (barrier.resource_type == ResourceType::BUFFER)
            {
                VkBufferMemoryBarrier buf_barrier = {};
                buf_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                buf_barrier.pNext = nullptr;
                buf_barrier.srcAccessMask = barrier.src_access_mask;
                buf_barrier.dstAccessMask = barrier.dst_access_mask;
                buf_barrier.srcQueueFamilyIndex = barrier.src_queue_family;
                buf_barrier.dstQueueFamilyIndex = barrier.dst_queue_family;
                buf_barrier.buffer = reinterpret_cast<VkBuffer>(barrier.resource_handle);
                buf_barrier.offset = 0;
                buf_barrier.size = VK_WHOLE_SIZE;

                buffer_barriers.push_back(buf_barrier);
            }
        }

        // Only emit if we have valid barriers and stages
        if ((src_stage_mask != 0 && dst_stage_mask != 0) &&
            (!image_barriers.empty() || !buffer_barriers.empty()))
        {
            vkCmdPipelineBarrier(
                cmd_buffer,
                src_stage_mask,
                dst_stage_mask,
                0, // dependencyFlags
                0, nullptr, // memoryBarriers
                static_cast<uint32_t>(buffer_barriers.size()),
                buffer_barriers.empty() ? nullptr : buffer_barriers.data(),
                static_cast<uint32_t>(image_barriers.size()),
                image_barriers.empty() ? nullptr : image_barriers.data()
            );
        }
    }

private:
    bool calculate_barrier(SyncState& sync_state, const SyncRequest& sync_req, BarrierData& barrier)
    {
        const VkPipelineStageFlags dst_stage_mask = sync_req.stage_mask;
        const VkAccessFlags dst_access_mask = sync_req.access_mask;
        const VkImageLayout dst_layout = sync_req.layout;

        // Reset barrier data
        barrier = {};

        // Check if layout transition is needed
        const bool need_layout_transition = is_layout_transition_needed(sync_state.layout, dst_layout);

        const bool is_write_operation = (dst_access_mask & ALL_WRITE_ACCESSES_MASK) != 0;
        const bool is_read_operation = (dst_access_mask & ALL_READ_ACCESSES_MASK) != 0;

        // Handle queue family transitions
        const bool need_queue_transition = (sync_req.src_queue_family != VK_QUEUE_FAMILY_IGNORED &&
                                           sync_req.dst_queue_family != VK_QUEUE_FAMILY_IGNORED &&
                                           sync_req.src_queue_family != sync_req.dst_queue_family);

        // Determine if we need execution dependency
        bool need_execution_dependency = false;

        if (is_write_operation || need_layout_transition || need_queue_transition)
        {
            // Write operation: need to wait for all previous accesses
            if (sync_state.active_readers_masks.stage_mask != 0)
            {
                barrier.src_stage_mask |= sync_state.active_readers_masks.stage_mask;
                barrier.src_access_mask |= sync_state.active_readers_masks.access_mask;
                need_execution_dependency = true;
            }

            if (sync_state.last_writer_masks.stage_mask != 0)
            {
                barrier.src_stage_mask |= sync_state.last_writer_masks.stage_mask;
                barrier.src_access_mask |= sync_state.last_writer_masks.access_mask;
                need_execution_dependency = true;
            }

            // Clear readers and update writer
            sync_state.active_readers_masks.reset();
            sync_state.last_writer_masks.stage_mask = dst_stage_mask;
            sync_state.last_writer_masks.access_mask = dst_access_mask;
        }
        else if (is_read_operation)
        {
            // Read operation: only need to wait for previous writes
            if (sync_state.last_writer_masks.stage_mask != 0)
            {
                barrier.src_stage_mask |= sync_state.last_writer_masks.stage_mask;
                barrier.src_access_mask |= sync_state.last_writer_masks.access_mask & ALL_WRITE_ACCESSES_MASK;
                need_execution_dependency = true;
            }

            // Add to active readers
            sync_state.active_readers_masks.stage_mask |= dst_stage_mask;
            sync_state.active_readers_masks.access_mask |= dst_access_mask;
        }

        // Determine if barrier is needed
        const bool need_barrier = need_execution_dependency || need_layout_transition || need_queue_transition;

        if (need_barrier)
        {
            barrier.dst_access_mask = dst_access_mask;
            barrier.dst_stage_mask = dst_stage_mask;

            // Use more appropriate fallback stage
            if (barrier.src_stage_mask == 0)
            {
                barrier.src_stage_mask = is_write_operation ?
                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            }

            barrier.src_layout = sync_state.layout;
            barrier.dst_layout = dst_layout;
        }

        // Update layout in sync state
        if (dst_layout != VK_IMAGE_LAYOUT_UNDEFINED)
        {
            sync_state.layout = dst_layout;
        }

        // Update queue family
        if (sync_req.dst_queue_family != VK_QUEUE_FAMILY_IGNORED)
        {
            sync_state.current_queue_family = sync_req.dst_queue_family;
        }

        return need_barrier;
    }
};