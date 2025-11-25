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

argos::ParsedArguments parse_arguments(int argc, char* argv[])
{
    using namespace argos;
    return ArgumentParser(argv[0])
        .add(Argument("FILE").count(1, UINT16_MAX))
        .add(Option("-o", "--output").argument("FILE")
            .help("File name for output. stdout is used by default."))
        .parse(argc, argv);
}

std::u8string to_u8(const std::string& str)
{
    return {reinterpret_cast<const char8_t*>(str.data()), str.size()};
}

std::vector<std::filesystem::path>
get_filenames(const std::vector<std::string>& raw_paths)
{
    std::vector<std::filesystem::path> filenames;
    const auto flags = Yglob::PathIteratorFlags::CASE_INSENSITIVE_PATHS
                       | Yglob::PathIteratorFlags::NO_DIRECTORIES;
    for (const auto& raw_path : raw_paths)
    {
        for (auto path : Yglob::PathIterator(to_u8(raw_path), flags))
            filenames.push_back(std::move(path));
    }
    return filenames;
}

void write_json(const std::filesystem::path& path, const GridLib::Grid& grid)
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

void write_png(const std::filesystem::path& path,
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

        const auto result = reader.get_grid(GridLib::Extent{{0, 0}});
        if (result.empty())
            return 1;

        std::filesystem::path filename(args.value("--output").as_string());
        if (filename.extension() == ".png")
        {
            const auto index_mode = GridLib::get_index_mode_for_top_left_origin(
                result.spatial_info());
            write_png(filename, result.view(), index_mode);
        }
        else
        {
            write_json(filename, result);
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
