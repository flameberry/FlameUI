#pragma once

template<typename T>
struct fuiVec2
{
    T X, Y;

    fuiVec2()
        : X((T)0), Y((T)0)
    {
    }

    fuiVec2(T x, T y)
        : X(x), Y(y)
    {
    }

    fuiVec2(T val)
        : X(val), Y(val)
    {
    }
};

template<typename T>
struct fuiVec3
{
    T X, Y, Z;

    fuiVec3()
        : X((T)0), Y((T)0), Z((T)0)
    {
    }


    fuiVec3(T x, T y, T z)
        : X(x), Y(y), Z(z)
    {
    }

    fuiVec3(T val)
        : X(val), Y(val), Z(val)
    {
    }
};

template<typename T>
struct fuiVec4
{
    T X, Y, Z, W;

    fuiVec4()
        : X((T)0), Y(T(0)), Z((T)0), W((T)0)
    {
    }


    fuiVec4(T x, T y, T z, T w)
        : X(x), Y(y), Z(z), W(w)
    {
    }

    fuiVec4(T val)
        : X(val), Y(val), Z(val), W(val)
    {
    }
};


namespace FlameUI {
    struct Vertex
    {
        fuiVec3<float> position;
        fuiVec4<float> color;
        fuiVec2<float> texture_uv;
        float texture_index;

        Vertex()
            : position(0.0f), color(1.0f), texture_uv(0.0f), texture_index(-1.0f)
        {
        }

        Vertex(const fuiVec3<float>& position, const fuiVec4<float>& color, const fuiVec2<float>& texture_uv = fuiVec2(0.0f), float texture_index = -1.0f)
            : position(position), color(color), texture_uv(texture_uv), texture_index(texture_index)
        {
        }
    };
}
