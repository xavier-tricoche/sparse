/*
----------------------------------------------------------------------

This source file is part of Expé
(EXperimental Point Engine)

Copyright (c) 2004-2007 by
 - Computer Graphics Laboratory, ETH Zurich
 - IRIT, University of Toulouse
 - Gael Guennebaud.

----------------------------------------------------------------------

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.

http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
----------------------------------------------------------------------
*/



#ifndef _ExpeColor_h_
#define _ExpeColor_h_

#include "ExpeCore.h"
#include "ExpeVector4.h"
#include "ExpeMath.h"
#include "ExpeRgba.h"

#ifdef EXPE_QT_SUPPORT
#include <QColor>
#endif

namespace Expe
{

class Color
{
public:
    
    union
    {
        #ifdef EXPE_USE_SSE
        v4sf vec;
        #endif
        struct {
            Real r, g, b, a;
        };
    };
    
    Color(Real _r=1., Real _g=1., Real _b=1., Real _a=1.)
        : r(_r), g(_g), b(_b), a(_a)
    {
    }
    Color(const Color& c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
    }
    Color(const Real* rgb, Real alpha)
    {
        r = rgb[0];
        g = rgb[1];
        b = rgb[2];
        a = alpha;
    }
    Color(const Real* rgba)
    {
        r = rgba[0];
        g = rgba[1];
        b = rgba[2];
        a = rgba[3];
    }

    #ifdef EXPE_QT_SUPPORT
    Color(const QColor& c, Real alpha=1.)
    {
        r = (Real)c.red()/255.;
        g = (Real)c.green()/255.;
        b = (Real)c.blue()/255.;
        a = alpha;
    }
    QColor toQColor(void) const
    {
        QColor c((int)(r*255.), (int)(g*255.), (int)(b*255.));
        return c;
    }
    #endif
    
//     inline operator Vector4& ()
//     {
//         return *reinterpret_cast<Vector4*>(this);
//     }
    
//     inline operator const Vector4& () const
//     {
//         return *reinterpret_cast<const Vector4*>(this);
//     }
    
    inline operator const float* () const {return &r; }
    inline operator float* () {return &r; }
    
    inline Color& operator = (const Color& c)
    {
        #ifdef EXPE_USE_SSE
        vec = c.vec;
        #else
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        #endif
        return *this;
    }
    
    inline Color operator * (Color d) const
    {
        Color c;
        c.r = r * d.r;
        c.g = g * d.g;
        c.b = b * d.b;
        c.a = a * d.a;
        return c;
    }
    inline Color operator + (Color d) const
    {
        Color c;
        c.r = r + d.r;
        c.g = g + d.g;
        c.b = b + d.b;
        c.a = a + d.a;
        return c;
    }
    
    inline Color operator * (float f) const
    {
        Color c;
        c.r = r * f;
        c.g = g * f;
        c.b = b * f;
        c.a = a * f;
        return c;
    }
    
    inline Color operator / (Color d) const
    {
        Color c;
        c.r = r / d.r;
        c.g = g / d.g;
        c.b = b / d.b;
        c.a = a / d.a;
        return c;
    }

    inline Color operator / (float f) const
    {
        Color c;
        c.r = r / f;
        c.g = g / f;
        c.b = b / f;
        c.a = a / f;
        return c;
    }
    
    inline Color& operator *= (float f)
    {
        r *= f;
        g *= f;
        b *= f;
        a *= f;
        return *this;
    }
    inline Color& operator += (const Color& c)
    {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }
    inline const Color& fromLerp(Real t, const Color& c0, const Color& c1)
    {
        Real t1 = (1.-t);
        r = t1*c0.r + t*c1.r;
        g = t1*c0.g + t*c1.g;
        b = t1*c0.b + t*c1.b;
        a = t1*c0.a + t*c1.a;
        return *this;
    }
    
    inline void saturate(void)
    {
        r = Math::Clamp(r, 0.f, 1.f);
        g = Math::Clamp(g, 0.f, 1.f);
        b = Math::Clamp(b, 0.f, 1.f);
        a = Math::Clamp(a, 0.f, 1.f);
    }
    
    inline static Color Colorize(Real level)
    {
        level = Math::Clamp(level,0.f,1.f);
        if (level<=0.5)
            return Color(0., 2.*level, 1.-2.*level);
        else
            return Color(2.*level, 1.-2.*level, 0.);
    }
    
    inline const Color& fromBGRA(unsigned long rgba)
    {
        r = ((Real)(rgba>>16 & 0xff))/255.;
        g = ((Real)(rgba>>8 & 0xff))/255.;
        b = ((Real)(rgba>>0 & 0xff))/255.;
        a = ((Real)(rgba>>24 & 0xff))/255.;
        return *this;
    }
    inline const Color& fromRGBA(unsigned long rgba)
    {
        r = ((Real)(rgba & 0xff))/255.;
        g = ((Real)(rgba>>8 & 0xff))/255.;
        b = ((Real)(rgba>>16 & 0xff))/255.;
        a = ((Real)(rgba>>24 & 0xff))/255.;
        return *this;
    }
    inline const Color& fromRGB(unsigned long rgba)
    {
        r = ((Real)(rgba & 0xff))/255.;
        g = ((Real)(rgba>>8 & 0xff))/255.;
        b = ((Real)(rgba>>16 & 0xff))/255.;
        a = 1.;
        return *this;
    }
    inline const Color& fromBGR(unsigned long rgba)
    {
        b = ((Real)(rgba & 0xff))/255.;
        g = ((Real)(rgba>>8 & 0xff))/255.;
        r = ((Real)(rgba>>16 & 0xff))/255.;
        a = 1.;
        return *this;
    }
    inline unsigned long toRGBA(void) const
    {
        unsigned char val8;
        unsigned long val32 = 0;

        // Convert to 32bit pattern
        // (ABRG = 8888)

        // Alpha
        val8 = (unsigned char)(a * 255);
        val32 = val8 << 24;

        // Blue
        val8 = (unsigned char)(b * 255);
        val32 += val8 << 16;

        // Green
        val8 = (unsigned char)(g * 255);
        val32 += val8 << 8;

        // Red
        val8 = (unsigned char)(r * 255);
        val32 += val8;


        return val32;
    }
    inline unsigned long toABGR(void) const
    {
        unsigned char val8;
        unsigned long val32 = 0;

        // Convert to 32bit pattern
        // (RGBA = 8888)

        // Red
        val8 = (unsigned char)(r * 255);
        val32 = val8 << 24;

        // Green
        val8 = (unsigned char)(g * 255);
        val32 += val8 << 16;

        // Blue
        val8 = (unsigned char)(b * 255);
        val32 += val8 << 8;

        // Alpha
        val8 = (unsigned char)(a * 255);
        val32 += val8;

        return val32;
    }
    inline unsigned long toBGRA(void) const
    {
        unsigned char val8;
        unsigned long val32 = 0;

        // Convert to 32bit pattern
        // (RGBA = 8888)

        // Alpha
        val8 = (unsigned char)(a * 255);
        val32 += val8 << 24;
        
        // Red
        val8 = (unsigned char)(r * 255);
        val32 = val8 << 16;

        // Green
        val8 = (unsigned char)(g * 255);
        val32 += val8 << 8;

        // Blue
        val8 = (unsigned char)(b * 255);
        val32 += val8;

        return val32;
    }

    static const Color WHITE;
    static const Color BLACK;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color GREY30;
    static const Color GREY50;
    static const Color GREY80;
};

template <>
struct TypeInfo<Color>
{
    // FIXME Id
    enum {NofComponents=4};
    typedef Real        ComponentType;
    typedef Vector4i    IntegerType;
    typedef Color       FloatingPointType;
};

}

#include <qmetatype.h>
Q_DECLARE_METATYPE(Expe::Color);

#endif
 
