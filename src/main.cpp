#include "example_filter.h"
#include "vulkan_helpers.hpp"
#include "iostream"


Flux init_flux(Params param) {
    auto flux = Flux();

    flux.right = -param.k * param.h;
    flux.left = -param.k * param.h;
    flux.top = -param.k * param.h;
    flux.bottom = -param.k * param.h;
    flux.left_edge = -param.kL * param.h * 2;
    flux.right_edge = -param.kR * param.h * 2;
    flux.top_edge = -param.kU * param.h * 2;
    flux.bottom_edge = -param.kD * param.h * 2;
    flux.C = -4 * param.k * param.h;
    flux.CLgran = flux.right + flux.left_edge + flux.top + flux.bottom;
    flux.CRgran = flux.right_edge + flux.left + flux.top + flux.bottom;
    flux.CUgran = flux.right + flux.left + flux.top_edge + flux.bottom;
    flux.DUgran = flux.right + flux.left + flux.top + flux.bottom_edge;
    flux.CLUgran = flux.right + flux.left_edge + flux.top_edge + flux.bottom;
    flux.CRUgran = flux.right_edge + flux.left + flux.top_edge + flux.bottom;
    flux.CLDgran = flux.right + flux.left_edge + flux.top + flux.bottom_edge;
    flux.DRUgran = flux.right_edge + flux.left + flux.top + flux.bottom_edge;

    return flux;
}

auto main(int argc, char *argv[]) -> int {
    const auto width = 10;
    const auto height = 10;
    const auto a = 2.0f; // saxpy scaling factor

    Params param = Params();
    param.h = 0.1;
    param.k = 50;

    param.dt = 1;
    param.dz = 1;
    param.Tb = 240;
    param.T0 = 0;

    param.kL = 1;
    param.kR = 0;
    param.kU = 10;
    param.kD = 10;

    auto y = std::vector<float>(width * height, 0.0f);
    auto x = std::vector<float>(width * height, 0.0f);

    ExampleFilter f("shaders/saxpy.spv");
    auto d_y = vuh::Array<float>::fromHost(y, f.device, f.physDevice);
    auto d_x = vuh::Array<float>::fromHost(x, f.device, f.physDevice);

    auto p_d_x = &d_x;
    auto p_d_y = &d_y;

    for (int i = 0; i < 500; i++) {
        f(*p_d_y, *p_d_x, {width, height, param, init_flux(param)});

        std::swap(p_d_x, p_d_y);
    }

    auto out_tst = std::vector<float>{};
    d_x.to_host(out_tst);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            std::cout << out_tst[i * width + j] << ' ';
        }
        std::cout << std::endl;
    }
    return 0;
}
