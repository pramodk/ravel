//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014, Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory.
//
// This file is part of Ravel.
// Written by Kate Isaacs, kisaacs@acm.org, All rights reserved.
// LLNL-CODE-663885
//
// For details, see https://github.com/scalability-llnl/ravel
// Please also see the LICENSE file for our notice and the LGPL.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License (as published by
// the Free Software Foundation) version 2.1 dated February 1999.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the terms and
// conditions of the GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//////////////////////////////////////////////////////////////////////////////
#include "colormap.h"
#include <iostream>

// Initial color/value pair in constructor
ColorMap::ColorMap(QColor color, float value, bool _categorical)
    : minValue(0),
      maxValue(1),
      maxClamp(1),
      categorical(_categorical),
      colors(new QVector<ColorValue *>())
{
    colors->push_back(new ColorValue(color, value));
}

ColorMap::~ColorMap()
{
    for (QVector<ColorValue *>::Iterator itr = colors->begin();
         itr != colors->end(); ++itr)
    {
        delete *itr;
        *itr = NULL;
    }
    delete colors;
}

ColorMap::ColorMap(const ColorMap & copy)
{
    minValue = copy.minValue;
    maxValue = copy.maxValue;
    maxClamp = copy.maxClamp;
    categorical = copy.categorical;
    colors = new QVector<ColorValue *>();
    for (QVector<ColorValue *>::Iterator itr = copy.colors->begin();
         itr != copy.colors->end(); ++itr)
    {
        colors->push_back(new ColorValue((*itr)->color, (*itr)->value));
    }
}

void ColorMap::setRange(double low, double high)
{
    minValue = low;
    maxValue = high;
    maxClamp = high;
}

void ColorMap::setClamp(double clamp)
{
    maxClamp = clamp;
}

void ColorMap::addColor(QColor color, float stop)
{
    bool added = false;
    colors->begin();
    // Keep colors in order by value
    for (QVector<ColorValue *>::Iterator itr = colors->begin();
         itr != colors->end(); ++itr)
    {
        if (stop < (*itr)->value)
        {
            colors->insert(itr, new ColorValue(color, stop));
            added = true;
        }
    }
    if (!added) {
        colors->push_back(new ColorValue(color, stop));
    }
}

QColor ColorMap::color(double value, double opacity)
{
    // Do something else if we're categorical
    if (categorical)
        return categorical_color(value);

    // Find the colors at either end of the given value and blend them them
    ColorValue base1 = ColorValue(QColor(0,0,0,opacity*255), 0);
    ColorValue base2 = ColorValue(QColor(0,0,0,opacity*255), 1);
    ColorValue* low = &base1;
    ColorValue* high = &base2;
    double norm_value = (value - minValue) / (maxClamp - minValue);
    for (QVector<ColorValue* >::Iterator itr = colors->begin();
         itr != colors->end(); itr++)
    {
        if ((*itr)->value > norm_value)
        {
            high = (*itr);
            return average(low, high, norm_value, opacity);
        } else {
            low = (*itr);
        }
    }
    return QColor(low->color.red(), low->color.green(), low->color.blue(),
                  opacity*255);
}

// In categorical, we only take the minValue into account and the number of
// input colors. This is somewhat magical and should probably be turned into
// something that is more elegant and makes sense.
QColor ColorMap::categorical_color(double value)
{
    int cat_value = int(value - minValue) % colors->size();
    return colors->at(cat_value)->color;
}

// Weighted average of two color values based on where norm falls
QColor ColorMap::average(ColorValue * low, ColorValue * high, double norm,
                         double opacity)
{
    int r, g, b, a;
    float alpha = (norm - low->value) / (high->value - low->value);
    r = (1 - alpha) * low->color.red() + alpha * high->color.red();
    g = (1 - alpha) * low->color.green() + alpha * high->color.green();
    b = (1 - alpha) * low->color.blue() + alpha * high->color.blue();
    a = opacity * 255;
    return QColor(r, g, b, a);
}
