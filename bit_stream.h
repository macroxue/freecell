#include <assert.h>

template <int MAX_BITS>
class BitStream {
 public:
  class Reader {
   public:
    Reader(const BitStream& b) : stream_(b.stream_) {}
    int Read(int total) { return ReadBits(BitsNeededFor(total)); }

   private:
    unsigned ReadBits(int bits) {
      unsigned value;
      int avail_chunk_bits = kChunkBits - read_chunk_bits_;
      if (avail_chunk_bits >= bits) {
        value = (stream_[read_chunk_] >> read_chunk_bits_);
        read_chunk_bits_ += bits;
      } else {
        assert(read_chunk_ < kNumChunks);
        value = (avail_chunk_bits > 0)
                    ? (stream_[read_chunk_] >> read_chunk_bits_)
                    : 0;
        value |= stream_[++read_chunk_] << avail_chunk_bits;
        read_chunk_bits_ = bits - avail_chunk_bits;
      }
      return value & ((1U << bits) - 1);
    }

    const unsigned* stream_;
    short read_chunk_ = 0;
    short read_chunk_bits_ = 0;
  };

  BitStream() : write_chunk_(0), written_chunk_bits_(0) { stream_[0] = 0; }

  void Write(int part, int total) { WriteBits(part, BitsNeededFor(total)); }

  int copy_size() const {
    return reinterpret_cast<const char*>(&stream_[write_chunk_ + 1]) -
           reinterpret_cast<const char*>(this);
  }

  static int BitsNeededFor(int total) {
    return (total == 1) ? 1 : 32 - __builtin_clz(total - 1);
  }

 private:
  void WriteBits(unsigned value, int bits) {
    int free_chunk_bits = kChunkBits - written_chunk_bits_;
    if (free_chunk_bits >= bits) {
      stream_[write_chunk_] |= (value << written_chunk_bits_);
      written_chunk_bits_ += bits;
    } else {
      assert(write_chunk_ < kNumChunks - 1);
      if (free_chunk_bits > 0)
        stream_[write_chunk_] |= (value << written_chunk_bits_);
      stream_[++write_chunk_] = (value >> free_chunk_bits);
      written_chunk_bits_ = bits - free_chunk_bits;
    }
  }

  static constexpr int kChunkBits = 32;
  static constexpr int kNumChunks = (MAX_BITS + 31) >> 5;
  short write_chunk_;
  short written_chunk_bits_;
  unsigned stream_[kNumChunks];
};
