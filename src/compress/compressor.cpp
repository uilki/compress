#include "compressor.h"

#include <algorithm>
#include <cassert>
#include <vector>

using data_type = unsigned char;
using Iterator  = std::vector<data_type>::const_iterator;

namespace {
bool isSame(data_type d, compres::Colors color) {
  return d == static_cast<data_type>(color);
}

bool allSameColor(Iterator b, Iterator e, compres::Colors color) {
  return std::all_of(b, e, [color](data_type c) { return isSame(c, color); });
}
} // namespace

namespace compres {
struct Compressor::Impl {
  void processData(std::shared_ptr<Data> data) {
    data_        = data;
    currByteIdx_ = 7;
    currByte_    = 0;
    int row      = 0;
    for (auto rowBegin { data_->pixels.cbegin() };
         rowBegin != data_->pixels.cend();
         std::advance(rowBegin, data_->imageWidth), ++row) {
      assert(std::distance(rowBegin, data_->pixels.cend()) >= 0);
      auto rowEnd { rowBegin + data_->imageWidth };
      assert(std::distance(rowEnd, data_->pixels.cend()) >= 0);
      if (allSameColor(rowBegin, rowEnd, Colors::white)) {
        data_->emptyRows.push_back(row);
        continue;
      }
      compressRow(rowBegin, rowEnd);
    }
  }

  void compressRow(Iterator b, Iterator e) {
    while (b != e) {
      int repeatCount = std::min(4l, std::distance(b, e));
      if (isSame(*b, Colors::white)
          && allSameColor(b, b + repeatCount, Colors::white)) {
        addMarker(Mark::white);
      }
      else if (isSame(*b, Colors::black)
               && allSameColor(b, b + repeatCount, Colors::black)) {
        addMarker(Mark::black);
      }
      else {
        addMarker(Mark::other);
        compressFourPixels(b, b + repeatCount);
      }
      std::advance(b, repeatCount);
    }
  }

  void compressFourPixels(Iterator b, Iterator e) {
    std::for_each(b, e, [this](data_type p) {
      for (int shift { 7 }; shift >= 0; --shift) {
        unsigned short mask = (p >> shift) & 0x01;

        mask <<= currByteIdx_;
        currByte_ |= mask;
        nextIdx();
      }
    });
  }

  void nextIdx() {
    if (currByteIdx_ != 0) {
      --currByteIdx_;
    }
    else {
      data_->compressedData.push_back(currByte_);
      currByteIdx_ = 7;
      currByte_    = 0;
    }
  }

  void addMarker(Mark mark) {
    if (mark == 0) {
      nextIdx();
      return;
    }

    for (int shift { 1 }; shift >= 0; --shift) {
      unsigned short mask = (mark >> shift) & 0x01;
      mask <<= currByteIdx_;
      currByte_ |= mask;
      nextIdx();
    }
  }

  std::shared_ptr<Data> data_;
  data_type             currByte_;
  int                   currByteIdx_;
}; // struct Compressor::Impl

Compressor::Compressor()
    : impl_ { std::make_unique<Impl>() } {}

Compressor::~Compressor() = default;

void Compressor::processData(std::shared_ptr<Data> data) {
  impl_->processData(data);
}
} // namespace compres
