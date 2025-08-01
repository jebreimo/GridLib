//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-07-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <fstream>
#include <iostream>
#include <Argos/Argos.hpp>
#include <Chorasmia/ArrayView2DAlgorithms.hpp>
#include <Chorasmia/Index2DMap.hpp>
#include <GridLib/Rasterize.hpp>
#include <GridLib/ReadGrid.hpp>
#include <GridLib/WriteJsonGrid.hpp>
#include <Xyz/Pgram3.hpp>
#include <Yglob/Yglob.hpp>
#include <Yimage/Png/WritePng.hpp>

#include "GridLib/PositionTransformer.hpp"

struct GridFileName
{
    std::string fileName;
    unsigned row;
    unsigned column;
};

argos::ParsedArguments parse_arguments(int argc, char* argv[])
{
    using namespace argos;
    return ArgumentParser(argv[0])
        .add(Argument("FILE").count(1, UINT16_MAX))
        .add(Option("-o", "--output").argument("FILE")
            .help("File name for output. stdout is used by default."))
        .parse(argc, argv);
}

bool check_grids(const std::vector<GridLib::Grid>& grids)
{
    if (grids.empty())
        return false;

    const std::pair size(grids[0].row_count(), grids[0].col_count());
    const auto bounds = GridLib::get_bounds(grids[0]);

    for (const auto& grid : grids)
    {
        std::pair grid_size(grid.row_count(), grid.col_count());
        if (grid_size != size)
        {
            std::cerr << "The grids must all have the same number of rows and columns.\n";
            return false;
        }

        auto grid_bounds = GridLib::get_bounds(grid.view());
        if (grid_bounds.edge0 != bounds.edge0
            || grid_bounds.edge1 != bounds.edge1)
        {
            std::cerr << "The grids must all same sizes in model space.\n";
            return false;
        }
    }

    return true;
}

void sort_grids(std::vector<GridLib::Grid>& grids)
{
    std::ranges::sort(grids, [](const auto& a, const auto& b)
    {
        const auto& apc = a.spatial_data().location();
        const auto& bpc = b.spatial_data().location();
        if (apc[1] != bpc[1])
            return apc[1] > bpc[1];
        return apc[0] < bpc[0];
    });
}

std::u8string to_u8(const std::string& str)
{
    return {reinterpret_cast<const char8_t*>(str.data()), str.size()};
}

std::vector<std::filesystem::path> get_filenames(const argos::ParsedArguments& args)
{
    std::vector<std::filesystem::path> filenames;
    const auto flags = Yglob::PathIteratorFlags::CASE_INSENSITIVE_PATHS
                       | Yglob::PathIteratorFlags::NO_DIRECTORIES;
    for (const auto& arg : args.values("FILE").as_strings())
    {
        for (auto path : Yglob::PathIterator(to_u8(arg), flags))
            filenames.push_back(std::move(path));
    }
    return filenames;
}

std::vector<GridLib::Grid>
read_grids(const std::vector<std::filesystem::path>& filenames)
{
    std::vector<GridLib::Grid> grids;
    for (auto& filename : filenames)
    {
        if (auto grid = GridLib::read_grid(filename); !grid.values().empty())
        {
            if (grid.model().location() != Xyz::Vector3D())
            {
                grids.push_back(grid);
            }
            else
            {
                std::cerr << filename
                    << " was ignored because it doesn't have planar coordinates.\n";
            }
        }
    }
    return grids;
}

// Xyz::Pgram3<double> get_grid_bounds(const std::vector<GridLib::Grid>& grids)
// {
//     if (grids.empty())
//         return {};
//
//     Xyz::Vector2D min(DBL_MAX, DBL_MAX);
//     Xyz::Vector2D max(DBL_TRUE_MIN, DBL_TRUE_MIN);
//
//     for (const auto& grid : grids)
//     {
//         auto rect = GridLib::get_bounding_rect(grid.view());
//         auto rect_min = Xyz::get_min(rect);
//         min = {std::min(min[0], rect_min[0]), std::min(min[1], rect_min[1])};
//         auto rect_max = Xyz::get_max(rect);
//         max = {std::max(max[0], rect_max[0]), std::max(max[1], rect_max[1])};
//     }
//
//     return Xyz::Pgram3<double>{min, max - min};
// }

Xyz::Vector2D get_dimensions(const GridLib::GridView& grid)
{
    return {double(grid.row_count()), double(grid.col_count())};
}

using Size = Xyz::Vector<size_t, 2>;

[[nodiscard]]
std::vector<std::pair<const GridLib::Grid*, Size>>
get_insertion_points(const std::vector<GridLib::Grid>& grids)
{
    if (grids.empty())
        return {};

    std::vector<std::pair<const GridLib::Grid*, Size>> result;

    const GridLib::PositionTransformer dst_trans(grids[0]);
    for (const auto& grid : grids)
    {
        const GridLib::PositionTransformer src_trans(grid);
        auto mp0 = src_trans.grid_to_model({0, 0});
        auto gp0 = Xyz::vector_cast<size_t>(dst_trans.model_to_grid(mp0));
        result.emplace_back(&grid, gp0);
    }

    return result;
}

std::pair<size_t, size_t>
get_combined_grid_size(const std::vector<std::pair<const GridLib::Grid*, Size>>& insertion_points)
{
    if (insertion_points.empty())
        return {0, 0};

    size_t max_row = 0;
    size_t max_col = 0;

    for (const auto& [grid, pos] : insertion_points)
    {
        max_row = std::max(max_row, pos[0] + grid->row_count());
        max_col = std::max(max_col, pos[1] + grid->col_count());
    }

    return {max_row, max_col};
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
               const GridLib::GridView& values,
               Chorasmia::Index2DMode index_mode)
{
    const auto bmp = GridLib::rasterize_rgba(
        values,
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

        const auto filenames = get_filenames(args);
        auto grids = read_grids(filenames);
        if (grids.empty())
        {
            std::cerr << "No valid grids were found.\n";
            return 1;
        }

        if (!check_grids(grids))
            return 1;

        sort_grids(grids);
        auto insertion_points = get_insertion_points(grids);
        auto [rows, cols] = get_combined_grid_size(insertion_points);
        std::cout << rows << "x" << cols << '\n';

        GridLib::Grid result(rows, cols);
        result.set_tie_point(grids[0].tie_point());
        result.model() = grids[0].model();
        auto result_values = result.values();
        for (const auto& [grid, pos] : insertion_points)
        {
            auto subview = result_values.subarray(pos[0], pos[1], grid->row_count(),
                                                  grid->col_count());
            Chorasmia::copy(grid->values(), subview, Chorasmia::Index2DMode::ROWS);
        }

        std::filesystem::path filename(args.value("--output").as_string());
        if (filename.extension() == ".png")
        {
            const auto index_mode = GridLib::get_index_mode_for_top_left_origin(result.view());
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
