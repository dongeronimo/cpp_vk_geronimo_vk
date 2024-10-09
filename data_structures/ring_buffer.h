#pragma once
#include <array>

/// <summary>
/// ring buffers are necessary because we have many frames being rendered at the same time
/// and a lot of data is frame-specific, like uniform buffers. The size of the ring buffer
/// will always be MAX_FRAMES_IN_FLIGHT
/// </summary>
/// <typeparam name="N"></typeparam>
template <typename N>
using ring_buffer_t = std::array<N, MAX_FRAMES_IN_FLIGHT>;
