#pragma once

struct Complex
{
    double r;
    double i;

    Complex(double a, double b) : r{a}, i{b} {}

    double magnitude2() const
    {
        return r*r + i*i;
    }

    Complex operator* (const Complex& a) const
    {
        return Complex( r*a.r - i*a.i, i*a.r + r*a.i );
    }

    Complex operator+ (const Complex& a) const
    {
        return Complex( r+a.r, i+a.i );
    }

};

inline bool in_bounds(double x)
{
    return x >=0 && x < p.dim;
}

inline bool escaped(const Complex& c)
{
    return c.magnitude2() > ( p.threshold * p.threshold );
}

inline int get_i(const double& x)
{
    return (x + p.offset_x) * (p.dim / p.x_range);
}

inline int get_j(const double& y)
{
    return (y + p.offset_y) * (p.dim / p.y_range);
}

inline double get_x(const int& i)
{
    return (double) i / (p.dim / p.x_range) - p.offset_x;
}

inline double get_y(const int& j)
{
    return (double) j / (p.dim / p.y_range) - p.offset_y;
}

inline void recurse(Complex& c, const Complex& o)
{
    c = c*c + c + o;
    //c = c*c + o;
    //c = c*c*c + Complex{2.0, 0}*c*c + o;
}
