//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-10-25.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <ostream>

#define WRITE_STRING(name) \
        do { \
            if (!rec.name.empty()) \
                os << #name ": " << rec.name << '\n'; \
        } while (false)

#define WRITE_OPTIONAL(name) \
        do { \
            if (rec.name) \
                os << #name ": " << *rec.name << '\n'; \
        } while (false)

#define CAST_AND_WRITE_OPTIONAL(name, type) \
        do { \
            if (rec.name) \
                os << #name ": " << type(*rec.name) << '\n'; \
        } while (false)

#define WRITE_ARRAY(name) \
        do { \
            for (int i = 0; i < std::size(rec.name); ++i) \
            { \
                if (rec.name[i]) \
                    os << #name "[" << i << "]: " << *rec.name[i] << '\n'; \
            } \
        } while (false)
