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

// std::filesystem::path make_temp_filename()
// {
//     static unsigned counter = std::random_device{}();
//     return std::filesystem::temp_directory_path() / ("grid_" + std::to_string(counter++) + ".dat");
// }
//
// std::pair<std::filesystem::path, std::fstream> make_temp_file()
// {
//     for (int i = 0; i < 10; ++i)
//     {
//         auto temp_filename = make_temp_filename();
//         if (!std::filesystem::exists(temp_filename))
//         {
//             std::fstream file(temp_filename,
//                               std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
//             if (file)
//                 return {temp_filename, std::move(file)};
//         }
//     }
//
//     GRIDLIB_THROW("Unable to make temporary file");
// }

// struct GridData
// {
//     std::filesystem::path filename;
//     std::streampos temp_file_offset = 0;
//     GridLib::Size size;
//     Xyz::Vector2D tie_point;
//     GridLib::SpatialInfo spatial_info;
// };

// std::vector<GridData> prepare_grids(
//     const std::vector<std::filesystem::path>& filenames,
//     std::ostream& temp_file)
// {
//     std::vector<GridData> grids;
//     for (const auto& filename : filenames)
//     {
//         auto grid = GridLib::read_grid(filename, GridLib::GridFileType::AUTO_DETECT);
//         if (!grid.values().empty())
//         {
//             const auto temp_file_pos = temp_file.tellp();
//             auto [rows, cols] = grid.size();
//             temp_file.write(reinterpret_cast<const char*>(grid.values().data()),
//                             std::streamsize(rows * cols * sizeof(float)));
//             grids.push_back({
//                 filename,
//                 temp_file_pos,
//                 grid.size(),
//                 grid.tie_point(),
//                 grid.spatial_info()
//             });
//         }
//         else
//         {
//             std::cerr << "Grid in " << filename << " is empty or invalid.\n";
//         }
//     }
//     return grids;
// }

// bool check_grids(const std::vector<GridData>& grids)
// {
//     if (grids.empty())
//         return false;
//
//     const auto& first = grids.front();
//
//     bool result = true;
//
//     for (const auto& grid : grids)
//     {
//         const auto& si = grid.spatial_info;
//         if (si.row_axis() != first.spatial_info.row_axis()
//             || si.column_axis() != first.spatial_info.column_axis()
//             || si.vertical_axis() != first.spatial_info.vertical_axis()
//             || si.horizontal_unit != first.spatial_info.horizontal_unit
//             || si.vertical_unit != first.spatial_info.vertical_unit
//             || si.crs != first.spatial_info.crs)
//         {
//             std::cerr << "Spatial information in "
//                 << grid.filename << " does not match those in "
//                 << first.filename << ".\n";
//             result = false;
//         }
//     }
//
//     return result;
// }

// void sort_grids(std::vector<GridLib::Grid>& grids)
// {
//     std::ranges::sort(grids, [](const auto& a, const auto& b)
//     {
//         const auto& apc = a.spatial_info().location();
//         const auto& bpc = b.spatial_info().location();
//         if (apc[1] != bpc[1])
//             return apc[1] > bpc[1];
//         return apc[0] < bpc[0];
//     });
// }

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

// std::vector<GridLib::Grid>
// read_grids(const std::vector<std::filesystem::path>& filenames)
// {
//     std::vector<GridLib::Grid> grids;
//     for (auto& filename : filenames)
//     {
//         if (auto grid = GridLib::read_grid(filename); !grid.values().empty())
//         {
//             if (grid.spatial_info().location() != Xyz::Vector3D())
//             {
//                 grids.push_back(grid);
//             }
//             else
//             {
//                 std::cerr << filename
//                     << " was ignored because it doesn't have planar coordinates.\n";
//             }
//         }
//     }
//     return grids;
// }

// Xyz::Vector2D get_dimensions(const GridLib::GridView& grid)
// {
//     auto [rows, cols] = grid.size();
//     return {double(rows), double(cols)};
// }

// using Size = GridLib::Size;

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

// template <std::floating_point T>
// T get_fraction(T value)
// {
//     T int_part;
//     return std::modf(value, &int_part);
// }
//
// using SignedIndex = Xyz::Vector<int64_t, 2>;

// std::optional<SignedIndex> get_insertion_point(const GridData& grid,
//                                          const GridLib::PositionTransformer& dst_trans)
// {
//     const GridLib::PositionTransformer src_trans(grid.spatial_info.matrix,
//                                                  grid.tie_point);
//     const auto wp = src_trans.grid_to_world({0, 0});
//     const auto gp = dst_trans.world_to_grid(wp);
//
//     if (std::abs(get_fraction(gp.x())) > 1e-6
//         || std::abs(get_fraction(gp.y())) > 1e-6)
//     {
//         std::cerr << "Grid " << grid.filename
//             << " has a non-integer grid point at ("
//             << gp.x() << ", " << gp.y() << ").\n";
//         return std::nullopt;
//     }
//
//     return SignedIndex(static_cast<int64_t>(std::round(gp.x())),
//                  static_cast<int64_t>(std::round(gp.y())));
// }

// [[nodiscard]]
// std::vector<std::pair<const GridData*, SignedIndex>>
// get_insertion_points(const std::vector<GridData>& grids)
// {
//     if (grids.empty())
//         return {};
//
//     std::vector<std::pair<const GridData*, SignedIndex>> result;
//
//     const GridLib::PositionTransformer dst_trans(grids[0].spatial_info.matrix,
//                                                  grids[0].tie_point);
//     for (const auto& grid : grids)
//     {
//         if (auto index = get_insertion_point(grid, dst_trans))
//             result.emplace_back(&grid, *index);
//     }
//
//     return result;
// }

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

// GridLib::Size rearrange_grids(
//     std::vector<std::pair<const GridData*, SignedIndex>>& insertion_points)
// {
//     if (insertion_points.empty())
//         return {0, 0};
//
//     int64_t min_row = INT64_MAX, min_col = INT64_MAX;
//     int64_t max_row = INT64_MIN, max_col = INT64_MIN;
//
//     for (const auto& [grid, pos] : insertion_points)
//     {
//         min_row = std::min(min_row, pos[0]);
//         min_col = std::min(min_col, pos[1]);
//         max_row = std::max(max_row, pos[0] + int64_t(grid->size[0]));
//         max_col = std::max(max_col, pos[1] + int64_t(grid->size[1]));
//     }
//
//     Xyz::Vector<int64_t, 2> pos_offset(-min_row, -min_col);
//
//     // Adjust the insertion points to be relative to the minimum row and column.
//     for (auto& [grid, pos] : insertion_points)
//     {
//         pos += pos_offset;
//     }
//
//     std::sort(insertion_points.begin(), insertion_points.end(), [](const auto& a, const auto& b)
//     {
//         if (std::get<1>(a)[0] != std::get<1>(b)[0])
//             return std::get<1>(a)[0] < std::get<1>(b)[0];
//         return std::get<1>(a)[1] < std::get<1>(b)[1];
//     });
//
//     return {size_t(max_row - min_row), size_t(max_col - min_col)};
// }

int main(int argc, char* argv[])
{
    try
    {
        auto args = parse_arguments(argc, argv);
        std::cout << std::fixed << std::setprecision(2);

        // auto [temp_filename, temp_file] = make_temp_file();
        const auto filenames = get_filenames(args.values("FILE").as_strings());

        GridLib::MultiGridReader reader;
        for (const auto& filename : filenames)
        {
            reader.read_grid(filename);
        }

        const auto result = reader.get_grid();
        if (result.empty())
            return 1;
        // auto grids = prepare_grids(filenames, temp_file);
        //
        // if (!check_grids(grids))
        // {
        //     std::cerr << "Grids do not match.\n";
        //     return 1;
        // }
        //
        // auto insertion_points = get_insertion_points(grids);
        // auto combined_size = rearrange_grids(insertion_points);
        // for (auto& [grid, pos] : insertion_points)
        // {
        //     std::cout << "Grid: " << grid->filename
        //         << ", Insertion point: (" << pos[0] << ", " << pos[1] << ")\n";
        // }
        //
        // std::cout << "Combined grid size: "
        //     << combined_size[0] << " rows, "
        //     << combined_size[1] << " columns.\n";
        //
        // GridLib::Grid result(combined_size);
        // result.spatial_info() = insertion_points[0].first->spatial_info;
        // result.spatial_info().tie_point += Xyz::vector_cast<double>(insertion_points[0].second);
        //
        // std::vector<float> data;
        // for (const auto& [grid, pos] : insertion_points)
        // {
        //     data.resize(grid->size[0] * grid->size[1]);
        //     temp_file.seekg(grid->temp_file_offset);
        //     auto size = std::streamsize(grid->size[0] * grid->size[1] * sizeof(float));
        //     temp_file.read(reinterpret_cast<char*>(data.data()), size);
        //     for (int64_t row = 0; row < grid->size[0]; ++row)
        //     {
        //         for (int64_t col = 0; col < grid->size[1]; ++col)
        //         {
        //             auto index = GridLib::Index(row + pos[0], col + pos[1]);
        //             result[index] = data[row * grid->size[1] + col];
        //         }
        //     }
        // }
        //
        // // GridLib::Grid result(GridLib::Size{rows, cols});
        // // result.spatial_info() = grids[0].spatial_info();
        // // auto result_values = result.values();
        // // for (const auto& [grid, pos] : insertion_points)
        // // {
        // //     auto [n_rows, n_cols] = grid->size();
        // //     auto subview = result_values.subarray(pos[0], pos[1], n_rows, n_cols);
        // //     Chorasmia::copy(grid->values(), subview, Chorasmia::Index2DMode::ROWS);
        // // }
        //

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
