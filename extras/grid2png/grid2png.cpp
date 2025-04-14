//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-07-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <fstream>
#include <filesystem>
#include <Argos/Argos.hpp>
#include <fmt/format.h>
#include <Chorasmia/Index2DMap.hpp>
#include <GridLib/Rasterize.hpp>
#include <GridLib/ReadGrid.hpp>
#include <Yimage/Png/WritePng.hpp>

void make_png(const std::string& filename,
              const Chorasmia::ArrayView2D<float>& grid,
              Chorasmia::Index2DMode index_mode)
{
    std::cout << filename << "\n";
    auto bmp = GridLib::rasterize_rgba(
        grid,
        GridLib::ElevationGradient(GridLib::make_default_gradient_2500()),
        index_mode);
    Yimage::write_png(filename, bmp.view());
}

void makeTiles(const GridLib::GridView& grid,
               unsigned rows, unsigned cols,
               const std::string& fileName)
{
    std::filesystem::path path(fileName);
    auto extension = path.extension().string();
    auto prefix = path.replace_extension().string();
    auto index_mode = GridLib::get_index_mode_for_top_left_origin(grid);
    for (size_t i = 0; i < grid.row_count(); i += rows)
    {
        for (size_t j = 0; j < grid.col_count(); j += cols)
        {
            make_png(fmt::format("{}_{:04}_{:04}{}",
                                 prefix, i, j, extension),
                     grid.elevations().subarray(i, j, rows, cols),
                     index_mode);
        }
    }
}

int main(int argc, char* argv[])
{
    using namespace argos;
    auto args = ArgumentParser(argv[0], true)
        .allow_abbreviated_options(true)
        .add(Argument("FILE").help("A DEM or GeoTIFF file."))
        .add(Argument("OUTPUT")
            .help("The name of the output PNG file(s)."))
        .add(Option{"-p", "--position"}.argument("ROW,COL")
            .help("Set the bottom left row and column in the input file"
                " that will be processed. Defaults to 0, 0."))
        .add(Option{"-s", "--size"}.argument("ROWS,COLS")
            .help("The tile size. Defaults to the entire grid."))
        .parse(argc, argv);

    auto size = args.value("--size")
        .split_n(',', 2)
        .as_uints({UINT_MAX, UINT_MAX});

    auto inFileName = args.value("FILE").as_string();
    auto outFileName = args.value("OUTPUT").as_string();

    try
    {
        auto grid = GridLib::read_grid(inFileName);
        size[0] = std::min(size[0], unsigned(grid.row_count()));
        size[1] = std::min(size[1], unsigned(grid.col_count()));
        std::cout << "\n";
        if (args.has("-p"))
        {
            auto pos = args.value("--position").split(',', 2, 2).as_uints();
            auto index_mode = GridLib::get_index_mode_for_top_left_origin(grid.view());
            make_png(outFileName,
                     grid.elevations().subarray(pos[0], pos[1], size[0], size[1]).view(),
                     index_mode);
        }
        else
        {
            makeTiles(grid.view(), size[0], size[1], outFileName);
        }
    }
    catch (std::exception& ex)
    {
        std::cout << "Exception: " << ex.what() << "\n";
    }
    return 0;
}
