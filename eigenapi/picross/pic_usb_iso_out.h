#pragma once
#include <stdint.h>

namespace pic { namespace detail {

struct iso_out_span {
    uint64_t first_frame;
    bool submitted;
};

// Locate an already-submitted USB frame by its bus-frame number. Completion
// status is deliberately irrelevant: resubmission may leave every URB pending.
// Return the flattened frame index, or the ring's size when unavailable.
template <class ReadSpan>
unsigned scheduled_iso_out_frame(bool running, uint64_t target_frame,
                                 unsigned urb_count, unsigned frames_per_urb, ReadSpan read_span)
{
    const unsigned unavailable = urb_count * frames_per_urb;
    if (!running || frames_per_urb == 0) return unavailable;
    for (unsigned urb = 0; urb < urb_count; ++urb)
    {
        const iso_out_span span = read_span(urb);
        if (span.submitted && target_frame >= span.first_frame
            && target_frame - span.first_frame < frames_per_urb)
            return urb * frames_per_urb + static_cast<unsigned>(target_frame - span.first_frame);
    }
    return unavailable;
}

inline uint64_t next_iso_out_frame(uint64_t bus_frame, uint64_t next_unwritten_frame, unsigned lead_frames)
{
    const uint64_t earliest = bus_frame + lead_frames;
    return earliest > next_unwritten_frame ? earliest : next_unwritten_frame;
}

} } // namespace pic::detail
