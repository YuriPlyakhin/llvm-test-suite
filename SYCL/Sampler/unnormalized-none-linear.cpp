// RUN: %clangxx -fsycl -fsycl-targets=%sycl_triple %s -o %t.out
// RUN: %HOST_RUN_PLACEHOLDER %t.out %HOST_CHECK_PLACEHOLDER
// RUN: %GPU_RUN_PLACEHOLDER %t.out %GPU_CHECK_PLACEHOLDER
// RUN: %CPU_RUN_PLACEHOLDER %t.out %CPU_CHECK_PLACEHOLDER
// XFAIL: cpu
// XFAIL: cuda

// CPU failing at moment. Waiting on fix.
// CUDA failing all linear interpolation at moment. Waiting on fix.

/*
    This file sets up an image, initializes it with data,
    and verifies that the data is sampled correctly with a
    sampler configured UNNORMALIZED coordinate_normalization_mode
    NONE address_mode and LINEAR filter_mode
*/

#include <CL/sycl.hpp>

using namespace cl::sycl;

// pixel data-type for RGBA operations (which is the minimum image type)
using pixelT = sycl::uint4;

// will output a pixel as {r,g,b,a}.  provide override if a different pixelT is
// defined.
void outputPixel(sycl::uint4 somePixel) {
  std::cout << "{" << somePixel[0] << "," << somePixel[1] << "," << somePixel[2]
            << "," << somePixel[3] << "} ";
}

// some constants.

// 4 pixels on a side. 1D at the moment
constexpr long width = 4;

constexpr auto unnormalized = coordinate_normalization_mode::unnormalized;
constexpr auto none = addressing_mode::none;
constexpr auto linear = filtering_mode::linear;

void test_unnormalized_none_linear_sampler(image_channel_order ChanOrder,
                                           image_channel_type ChanType) {
  int numTests = 11; // drives the size of the testResults buffer, and the
                     // number of report iterations. Kludge.

  // we'll use these four pixels for our image. Makes it easy to measure
  // interpolation and spot "off-by-one" probs.
  pixelT leftEdge{1, 2, 3, 4};
  pixelT body{49, 48, 47, 46};
  pixelT bony{59, 58, 57, 56};
  pixelT rightEdge{11, 12, 13, 14};

  queue Q;
  const sycl::range<1> ImgRange_1D(width);
  { // closure
    // - create an image
    image<1> image_1D(ChanOrder, ChanType, ImgRange_1D);
    event E_Setup = Q.submit([&](handler &cgh) {
      auto image_acc = image_1D.get_access<pixelT, access::mode::write>(cgh);
      cgh.single_task<class setupUnormLinear>([=]() {
        image_acc.write(0, leftEdge);
        image_acc.write(1, body);
        image_acc.write(2, bony);
        image_acc.write(3, rightEdge);
      });
    });
    E_Setup.wait();

    // use a buffer to report back test results.
    buffer<pixelT, 1> testResults((range<1>(numTests)));

    // sampler
    auto UnNorm_None_Linear_sampler = sampler(unnormalized, none, linear);

    event E_Test = Q.submit([&](handler &cgh) {
      auto image_acc = image_1D.get_access<pixelT, access::mode::read>(cgh);
      auto test_acc = testResults.get_access<access::mode::write>(cgh);

      cgh.single_task<class im1D_Unorm_Linear>([=]() {
        int i = 0; // the index for writing into the testResult buffer.

        // UnNormalized Pixel Locations when using Linear Interpolation
        // (0 -------- ](1 ---------- ](2 ----------- ](3---------- ](4)

        // 0-2 read three pixels at inner boundary locations, float coordinates,
        // sample:   UnNormalized +  None  + Linear
        test_acc[i++] =
            image_acc.read(1.0f, UnNorm_None_Linear_sampler); // {25,25,25,25}
        test_acc[i++] =
            image_acc.read(2.0f, UnNorm_None_Linear_sampler); // {54,53,52,51}
        test_acc[i++] =
            image_acc.read(3.0f, UnNorm_None_Linear_sampler); // {35,35,35,35}

        // 3-6 read four pixels at exact center locations, float,  sample:
        // Unnormalized +  None  + Linear
        test_acc[i++] =
            image_acc.read(0.5f, UnNorm_None_Linear_sampler); // {1,2,3,4}
        test_acc[i++] =
            image_acc.read(1.5f, UnNorm_None_Linear_sampler); // {49,48,47,46}
        test_acc[i++] =
            image_acc.read(2.5f, UnNorm_None_Linear_sampler); // {59,58,57,56}
        test_acc[i++] =
            image_acc.read(3.5f, UnNorm_None_Linear_sampler); // {11,12,13,14}

        // 7-10 read four pixels at inexact upper boundary, float coord, sample:
        // Unnormalized +  None  + Linear
        test_acc[i++] = image_acc.read(
            0.9999f, UnNorm_None_Linear_sampler); // {25,25,25,25}
        test_acc[i++] = image_acc.read(
            1.9999f, UnNorm_None_Linear_sampler); // {54,53,52,51}
        test_acc[i++] = image_acc.read(
            2.9999f, UnNorm_None_Linear_sampler); // {35,35,35,35}
        test_acc[i++] = image_acc.read(
            3.9999f,
            UnNorm_None_Linear_sampler); // {6,6,7,7}  // <<--- should it be
                                         // interpolating with the bg color?
                                         // That doesn't seem right. But it is
                                         // what it is. No longer verifying
                                         // this value in FILECHECK, pending
                                         // further investigation.
      });
    });
    E_Test.wait();

    // REPORT RESULTS
    auto test_acc = testResults.get_access<access::mode::read>();
    for (int i = 0, idx = 0; i < numTests; i++, idx++) {
      if (i == 0) {
        idx = 1;
        std::cout << "read three pixels at inner boundary locations, float "
                     "coordinates,  sample:   UnNormalized +  None  + Linear"
                  << std::endl;
      }
      if (i == 3) {
        idx = 0;
        std::cout << "read four pixels at exact center locations, float,  "
                     "sample:   Unnormalized +  None  + Linear"
                  << std::endl;
      }
      if (i == 7) {
        idx = 0;
        std::cout << "read four pixels at inexact upper boundary, float coord, "
                     " sample:   Unnormalized +  None  + Linear"
                  << std::endl;
      }

      pixelT testPixel = test_acc[i];
      std::cout << i << " -- " << idx << ": ";
      outputPixel(testPixel);
      std::cout << std::endl;
    }
  } // ~image / ~buffer
}

int main() {

  queue Q;
  device D = Q.get_device();

  if (D.has(aspect::image)) {
    // the _int8 channels are one byte per channel, or four bytes per pixel (for
    // RGBA) the _int16/fp16 channels are two bytes per channel, or eight bytes
    // per pixel (for RGBA) the _int32/fp32  channels are four bytes per
    // channel, or sixteen bytes per pixel (for RGBA).
    // CUDA has limited support for image_channel_type, so the tests use
    // unsigned_int32
    test_unnormalized_none_linear_sampler(image_channel_order::rgba,
                                          image_channel_type::unsigned_int32);
  } else {
    std::cout << "device does not support image operations" << std::endl;
  }

  return 0;
}
// clang-format off
// CHECK: read three pixels at inner boundary locations, float coordinates,  sample:   UnNormalized +  None  + Linear
// CHECK-NEXT: 0 -- 1: {25,25,25,25}
// CHECK-NEXT: 1 -- 2: {54,53,52,51}
// CHECK-NEXT: 2 -- 3: {35,35,35,35}
// CHECK-NEXT: read four pixels at exact center locations, float,  sample:   Unnormalized +  None  + Linear
// CHECK-NEXT: 3 -- 0: {1,2,3,4}
// CHECK-NEXT: 4 -- 1: {49,48,47,46}
// CHECK-NEXT: 5 -- 2: {59,58,57,56}
// CHECK-NEXT: 6 -- 3: {11,12,13,14}
// CHECK-NEXT: read four pixels at inexact upper boundary, float coord,  sample:   Unnormalized +  None  + Linear
// CHECK-NEXT: 7 -- 0: {25,25,25,25}
// CHECK-NEXT: 8 -- 1: {54,53,52,51}
// CHECK-NEXT: 9 -- 2: {35,35,35,35}
// clang-format on