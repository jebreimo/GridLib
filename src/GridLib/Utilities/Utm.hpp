//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-02-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "GridLib/GridMemberTypes.hpp"

namespace GridLib
{
    SphericalCoords utm_to_geo(double easting, double northing, int utm_zone);
}
