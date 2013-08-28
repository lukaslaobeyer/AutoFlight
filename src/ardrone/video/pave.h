#ifndef PAVE_H
#define PAVE_H

#include <cstdint>

typedef struct // Parrot video encapsulation, see official documentation
{
	uint8_t signature[4];   // Should containt "PaVE"
	uint8_t version;        // Version code
	uint8_t video_codec;    // ID of currently used codec
	uint16_t header_size;   // Size of the PaVE header
	uint32_t payload_size;  // Amount of data following the header
	uint16_t encoded_stream_width;
	uint16_t encoded_stream_height;
	uint16_t display_width;
	uint16_t display_height;
	uint32_t frame_number;  // Frame number
	uint32_t timestamp;     // Timestamp in milliseconds
	uint8_t total_chunks;   // Unused
	uint8_t chunk_index;    // Unused
	uint8_t frame_type;     // One of the types defined in ardrone::video::frame_type
	uint8_t control;        // Special commands like end-of-stream or advertised frames (whatever Parrot means with that)
	uint32_t stream_byte_position_lw; // Byte position of the current payload in the encoded stream - lower 32-bit word
	uint32_t stream_byte_position_uw; // Byte position of the current payload in the encoded stream - upper 32-bit word
	uint16_t stream_id;     // This ID indentifies packets that should be recorded together
	uint8_t total_slices;   // Number of slizes composing the current frame
	uint8_t header1_size;   // H.264 only: size of SPS inside payload - no SPS present if value is zero
	uint8_t header2_size;   // H.264 only: size of PPS inside payload - no PPS present if value is zero
	uint8_t reserved2[2];	// Unused
	uint32_t advertised_size; // Size of frames announced as advertised frames
	uint8_t reserved3[12];  // Unused
} __attribute__ ((packed)) PaVE;

#endif
