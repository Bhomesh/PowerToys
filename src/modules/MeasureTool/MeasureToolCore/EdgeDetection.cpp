#include "pch.h"

#include "EdgeDetection.h"

const uint8_t DEFAULT_TOLERANCE = 1;
const long CURSOR_OFFSET_AMOUNT_X = 4;
const long CURSOR_OFFSET_AMOUNT_Y = 4;

template<bool IsX, bool Increment>
inline long FindEdge(const BGRATextureView& texture, const POINT centerPoint, const uint8_t tolerance)
{
    long xOffset;
    long yOffset;
    if constexpr (IsX)
    {
        xOffset = Increment ? CURSOR_OFFSET_AMOUNT_X : -CURSOR_OFFSET_AMOUNT_X;
        yOffset = 1;
    }
    else
    {
        xOffset = 1;
        yOffset = Increment ? CURSOR_OFFSET_AMOUNT_Y : -CURSOR_OFFSET_AMOUNT_Y;
    }

    const size_t maxDim = IsX ? texture.width : texture.height;

    long x = std::clamp<long>(centerPoint.x + xOffset, 1, static_cast<long>(texture.width - 2));
    long y = std::clamp<long>(centerPoint.y + yOffset, 1, static_cast<long>(texture.height - 2));

    const uint32_t startPixel = texture.GetPixel(x, y);
    while (true)
    {
        long oldX = x;
        long oldY = y;
        if constexpr (IsX)
        {
            if constexpr (Increment)
            {
                if (++x == maxDim)
                    break;
            }
            else
            {
                if (--x == 0)
                    break;
            }
        }
        else
        {
            if constexpr (Increment)
            {
                if (++y == maxDim)
                    break;
            }
            else
            {
                if (--y == 0)
                    break;
            }
        }

        const uint32_t nextPixel = texture.GetPixel(x, y);
        if (!texture.PixelsClose(startPixel, nextPixel, tolerance))
        {
            return IsX ? oldX : oldY; // Return previous value, since we've hit a different pixel color.
        }
    }

    return Increment ? static_cast<long>(IsX ? texture.width : texture.height) : 0;
}

RECT DetectEdges(const BGRATextureView& texture, const POINT centerPoint, const uint8_t tolerance)
{
    return RECT{ .left = FindEdge<true, false>(texture, centerPoint, tolerance),
                 .top = FindEdge<false, false>(texture, centerPoint, tolerance),
                 .right = FindEdge<true, true>(texture, centerPoint, tolerance),
                 .bottom = FindEdge<false, true>(texture, centerPoint, tolerance) };
}