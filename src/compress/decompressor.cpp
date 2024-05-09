#include "decompressor.h"

#include "types.h"

#include <algorithm>
#include <assert.h>

using data_type = unsigned char;
using Iterator  = std::vector<data_type>::iterator;

namespace compres {
struct Decompressor::Impl {
  Impl()
      : image_ { nullptr } {}

  void processData(std::shared_ptr<Data> data) {
    image_ = data;
    image_->pixels.resize(image_->imageWidth * image_->imageHeight);

    currByteIdx_    = 7;
    currCompressed_ = image_->compressedData.begin();

    int emptyRowIdx = 0;
    int row         = 0;
    for (auto currDecompressed { image_->pixels.begin() };
         currDecompressed < image_->pixels.end();
         std::advance(currDecompressed, image_->imageWidth), ++row) {
      assert(currDecompressed != image_->pixels.end());
      if (emptyRowIdx < image_->emptyRows.size()
          && row == image_->emptyRows[emptyRowIdx]) {
        assert(currDecompressed + image_->imageWidth - 1
               != image_->pixels.end());
        std::fill(currDecompressed, currDecompressed + image_->imageWidth,
                  static_cast<data_type>(Colors::white));
        ++emptyRowIdx;
        continue;
      }
      decompressRow(currDecompressed, currDecompressed + image_->imageWidth);
    }
  }

  void decompressRow(Iterator b, Iterator e) {
    while (b != e) {
      auto fillCount = std::min(4l, std::distance(b, e));
      switch (getMark()) {
      case Mark::white:
        std::fill(b, b + fillCount, static_cast<data_type>(Colors::white));
        break;
      case Mark::black:
        std::fill(b, b + fillCount, static_cast<data_type>(Colors::black));
        break;
      default:
        fillData(b, fillCount);
      }
      std::advance(b, fillCount);
    }
  }

  void fillData(Iterator b, int count) {
    std::generate_n(b, count, [this]() { return nextByte(); });
  }

  data_type nextByte() {
    data_type byte = 0;
    for (int i { 0 }; i <= 7; ++i) {
      auto bit = nextBit();
      byte     = (byte << 1) | bit;
    }
    return byte;
  }

  data_type nextBit() {
    data_type bit = *currCompressed_ << (7 - currByteIdx_);
    bit >>= 7;
    nextIdx();
    return bit;
  }

  void nextIdx() {
    if (currByteIdx_ != 0) {
      --currByteIdx_;
    }
    else {
      currByteIdx_ = 7;
      ++currCompressed_;
    }
  }

  data_type getMark() {
    data_type mark = nextBit();
    if (mark == 0x00)
      return mark;
    mark = (mark << 1) | (nextBit());

    return mark;
  }

  std::shared_ptr<Data> image_;
  data_type             currByteIdx_;
  Iterator              currCompressed_;
};

Decompressor::Decompressor()
    : impl_ { std::make_unique<Impl>() } {}

Decompressor::~Decompressor() = default;

void Decompressor::processData(std::shared_ptr<Data> data) {
  impl_->processData(data);
}

} // namespace compres
