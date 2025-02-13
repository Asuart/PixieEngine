#pragma once
#include "pch.h"
#include "Math.h"

template <typename T>
struct Complex {
    T re, im;

    Complex(T re) : re(re), im(0) {}
    Complex(T re, T im) : re(re), im(im) {}

    Complex operator-() const {
        return { -re, -im };
    }

    Complex operator+(Complex z) const {
        return { re + z.re, im + z.im };
    }

    Complex operator-(Complex z) const {
        return { re - z.re, im - z.im };
    }

    Complex operator*(Complex z) const {
        return { re * z.re - im * z.im, re * z.im + im * z.re };
    }

    Complex operator/(Complex z) const {
        T scale = 1 / (z.re * z.re + z.im * z.im);
        return { scale * (re * z.re + im * z.im), scale * (im * z.re - re * z.im) };
    }

    friend Complex operator+(T value, Complex z) {
        return Complex(value) + z;
    }

    friend Complex operator-(T value, Complex z) {
        return Complex(value) - z;
    }

    friend Complex operator*(T value, Complex z) {
        return Complex(value) * z;
    }

    friend Complex operator/(T value, Complex z) {
        return Complex(value) / z;
    }
};

template <typename T>
T real(const Complex<T>& z) {
    return z.re;
}

template <typename T>
T imag(const Complex<T>& z) {
    return z.im;
}

template <typename T>
T norm(const Complex<T>& z) {
    return z.re * z.re + z.im * z.im;
}

template <typename T>
T abs(const Complex<T>& z) {
    return glm::sqrt(norm(z));
}

template <typename T>
Complex<T> sqrt(const Complex<T>& z) {
    T n = abs(z), t1 = glm::sqrt(T(.5) * (n + glm::abs(z.re))), t2 = T(.5) * z.im / t1;
    if (n == 0) {
        return 0;
    }
    if (z.re >= 0) {
        return { t1, t2 };
    }
    else {
        return { glm::abs(t2), std::copysign(t1, z.im) };
    }
}
