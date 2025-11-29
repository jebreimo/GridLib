//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-07-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <fstream>
#include <iostream>
#include <random>
#include <Argos/Argos.hpp>
#include <GridLib/MultiGridReader.hpp>
#include <GridLib/Rasterize.hpp>
#include <GridLib/ReadGrid.hpp>
#include <GridLib/WriteJsonGrid.hpp>
#include <Yglob/Yglob.hpp>
#include <Yimage/Png/WritePng.hpp>

namespace fs = std::filesystem;

argos::ParsedArguments parse_arguments(int argc, char* argv[])
{
    using namespace argos;
    return ArgumentParser(argv[0])
        .add(Argument("FILE").count(1, UINT16_MAX))
        .add(Option("-o", "--output").argument("FILE")
            .help("File name for output. stdout is used by default."))
        .add(Option("-t", "--tile").argument("<ROWS>x<COLS>")
            .help("Divide the input grids into tiles of the given size "
                "after stitching them together. Tiles without data are not output."))
        .parse(argc, argv);
}

std::u8string to_u8(const std::string& str)
{
    return {reinterpret_cast<const char8_t*>(str.data()), str.size()};
}

std::vector<fs::path>
get_filenames(const std::vector<std::string>& raw_paths)
{
    std::vector<fs::path> filenames;
    const auto flags = Yglob::PathIteratorFlags::CASE_INSENSITIVE_PATHS
                       | Yglob::PathIteratorFlags::NO_DIRECTORIES;
    for (const auto& raw_path : raw_paths)
    {
        for (auto path : Yglob::PathIterator(to_u8(raw_path), flags))
            filenames.push_back(std::move(path));
    }
    return filenames;
}

void write_json(const fs::path& path, const GridLib::Grid& grid)
{
    std::ostream* stream = &std::cout;
    std::ofstream ofs;
    if (!path.empty())
    {
        ofs.open(path);
        if (!ofs)
        {
            std::cerr << "Could not open output file: " << path << '\n';
            return;
        }
        stream = &ofs;
    }
    GridLib::write_json(*stream, grid);
}

void write_png(const fs::path& path,
               const GridLib::IGrid& grid,
               Chorasmia::Index2DMode index_mode)
{
    const auto bmp = GridLib::rasterize_rgba(
        grid,
        GridLib::ElevationGradient(GridLib::make_default_gradient_2500()),
        index_mode);
    Yimage::write_png(path, bmp.view());
}

int main(int argc, char* argv[])
{
    try
    {
        auto args = parse_arguments(argc, argv);
        std::cout << std::fixed << std::setprecision(2);

        const auto filenames = get_filenames(args.values("FILE").as_strings());

        GridLib::MultiGridReader reader;
        for (const auto& filename : filenames)
        {
            reader.read_grid(filename);
        }

        auto tile_arg = args.value("--tile")
            .split_n('x', 2)
            .as_ulongs({ULONG_MAX, ULONG_MAX});
        GridLib::Size tile_size{tile_arg[0], tile_arg[1]};
        fs::path filename(args.value("--output").as_string());
        const auto extension = filename.extension();

        auto total_size = reader.size();
        for (size_t row = 0; row < total_size.rows; row += tile_size.rows)
        {
            for (size_t col = 0; col < total_size.columns; col += tile_size.columns)
            {
                const auto extent = GridLib::Extent({row, col}, tile_size);
                if (!reader.has_data(extent))
                    continue;
                const auto grid = reader.get_grid(extent);
                auto grid_name = filename;
                if (!grid_name.empty())
                {
                    grid_name.replace_extension("")
                        .concat("_" + std::to_string(row) + "_" + std::to_string(col))
                        .replace_extension(extension);
                }
                if (extension == ".png")
                {
                    const auto index_mode = GridLib::get_index_mode_for_top_left_origin(
                        grid.spatial_info());
                    write_png(grid_name, grid, index_mode);
                }
                else
                {
                    write_json(filename, grid);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    catch (...)
    {
        std::cerr << "An unknown error occurred.\n";
        return 1;
    }

    return 0;
}
