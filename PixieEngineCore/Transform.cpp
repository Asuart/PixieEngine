#include "pch.h"
#include "Transform.h"

Transform::Transform() {};

Transform::Transform(const glm::mat4& m)
    : m(m), mInv(glm::inverse(m)) {}

Transform::Transform(const glm::mat4& m, const glm::mat4& mInv)
    : m(m), mInv(mInv) {}

const glm::mat4& Transform::GetMatrix() const {
    return m;
}

const glm::mat4& Transform::GetInverseMatrix() const {
    return mInv;
}

glm::vec3 Transform::ApplyPoint(glm::vec3 p) const {
    float xp = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
    float yp = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
    float zp = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
    float wp = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];
    if (wp == 1)
        return glm::vec3(xp, yp, zp);
    else
        return glm::vec3(xp, yp, zp) / wp;
}

glm::vec3 Transform::ApplyVector(glm::vec3 v) const {
    return glm::vec3(
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

glm::vec3 Transform::ApplyNormal(glm::vec3 n) const {
    float x = n.x, y = n.y, z = n.z;
    return glm::vec3(
        mInv[0][0] * x + mInv[1][0] * y + mInv[2][0] * z,
        mInv[0][1] * x + mInv[1][1] * y + mInv[2][1] * z,
        mInv[0][2] * x + mInv[1][2] * y + mInv[2][2] * z);
}

Ray Transform::ApplyRay(const Ray& r, float* tMax) const {
    glm::vec3 o = (*this).ApplyPoint(glm::vec3(r.o));
    glm::vec3 d = (*this).ApplyVector(r.d);

    float lengthSquared = glm::length2(d);
    if (lengthSquared > 0) {
        o += d * MachineEpsilon;
    }

    return Ray(o, d);
}

RTInteraction Transform::ApplyInteraction(const RTInteraction& in) const {
    RTInteraction ret(in);
    ret.p = (*this).ApplyPoint(in.p);
    ret.n = (*this).ApplyNormal(in.n);
    if (glm::length2(ret.n) > 0)
        ret.n = glm::normalize(ret.n);
    ret.wo = (*this).ApplyVector(in.wo);
    if (glm::length2(ret.wo) > 0)
        ret.wo = glm::normalize(ret.wo);
    return ret;
}

Bounds3f Transform::ApplyBounds(const Bounds3f& b) const {
    Bounds3f bt;
    for (int i = 0; i < 8; ++i)
        bt = Union(bt, (*this).ApplyBounds(b));
    return bt;
}

glm::vec3 Transform::ApplyInversePoint(glm::vec3 p) const {
    float x = p.x, y = p.y, z = p.z;
    float xp = (mInv[0][0] * x + mInv[0][1] * y) + (mInv[0][2] * z + mInv[0][3]);
    float yp = (mInv[1][0] * x + mInv[1][1] * y) + (mInv[1][2] * z + mInv[1][3]);
    float zp = (mInv[2][0] * x + mInv[2][1] * y) + (mInv[2][2] * z + mInv[2][3]);
    float wp = (mInv[3][0] * x + mInv[3][1] * y) + (mInv[3][2] * z + mInv[3][3]);
    if (wp == 1)
        return glm::vec3(xp, yp, zp);
    else
        return glm::vec3(xp, yp, zp) / wp;
}

glm::vec3 Transform::ApplyInverseVector(glm::vec3 v) const {
    float x = v.x, y = v.y, z = v.z;
    return glm::vec3(
        mInv[0][0] * x + mInv[0][1] * y + mInv[0][2] * z,
        mInv[1][0] * x + mInv[1][1] * y + mInv[1][2] * z,
        mInv[2][0] * x + mInv[2][1] * y + mInv[2][2] * z);
}

glm::vec3 Transform::ApplyInverseNormal(glm::vec3 n) const {
    float x = n.x, y = n.y, z = n.z;
    return glm::vec3(
        m[0][0] * x + m[1][0] * y + m[2][0] * z,
        m[0][1] * x + m[1][1] * y + m[2][1] * z,
        m[0][2] * x + m[1][2] * y + m[2][2] * z);
}

Ray Transform::ApplyInverseRay(const Ray& r, float* tMax) const {
    glm::vec3 o = ApplyInversePoint(r.o);
    glm::vec3 d = ApplyInverseVector(r.d);

    float lengthSquared = glm::length2(d);
    if (lengthSquared > 0) {
        o -= d * MachineEpsilon;
    }

    return Ray(o, d);
}

RTInteraction Transform::ApplyInverseInteraction(const RTInteraction& in) const {
    RTInteraction ret(in);
    Transform t = Inverse(*this);
    ret.p = t.ApplyPoint(in.p);
    ret.n = t.ApplyNormal(in.n);
    if (glm::length2(ret.n) > 0)
        ret.n = glm::normalize(ret.n);
    ret.wo = t.ApplyVector(in.wo);
    if (glm::length2(ret.wo) > 0)
        ret.wo = glm::normalize(ret.wo);
    return ret;
}

void Transform::Decompose(glm::vec3* T, glm::mat4* R, glm::mat4* S) const {
    T->x = m[0][3];
    T->y = m[1][3];
    T->z = m[2][3];

    glm::mat4 M = m;
    for (int i = 0; i < 3; ++i)
        M[i][3] = M[3][i] = 0.f;
    M[3][3] = 1.f;

    float norm;
    int count = 0;
    *R = M;
    do {
        glm::mat4 Rit = glm::inverse(glm::transpose(*R));
        glm::mat4 Rnext = (*R + Rit) / 2.0f;

        norm = 0;
        for (int i = 0; i < 3; ++i) {
            float n = std::abs((*R)[i][0] - Rnext[i][0]) +
                std::abs((*R)[i][1] - Rnext[i][1]) +
                std::abs((*R)[i][2] - Rnext[i][2]);
            norm = std::max(norm, n);
        }

        *R = Rnext;
    } while (++count < 100 && norm > .0001);

    *S = glm::inverse(*R) * M;
}

bool Transform::IsIdentity() const {
    return m == glm::mat4();
}

bool Transform::HasScale(float tolerance) const {
    float la2 = glm::length2((*this).ApplyVector(glm::vec3(1, 0, 0)));
    float lb2 = glm::length2((*this).ApplyVector(glm::vec3(0, 1, 0)));
    float lc2 = glm::length2((*this).ApplyVector(glm::vec3(0, 0, 1)));
    return (std::abs(la2 - 1) > tolerance || std::abs(lb2 - 1) > tolerance || std::abs(lc2 - 1) > tolerance);
}

bool Transform::SwapsHandedness() const {
    glm::mat3 s(
        m[0][0], m[0][1], m[0][2],
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2]);
    return glm::determinant(s) < 0;
}

bool Transform::operator==(const Transform& t) const {
    return t.m == m;
}

bool Transform::operator!=(const Transform& t) const {
    return t.m != m;
}

Transform Transform::operator*(const Transform& t2) const {
    return Transform(m * t2.m, t2.mInv * mInv);
}

Transform Inverse(const Transform& t) {
    return Transform(t.GetInverseMatrix(), t.GetMatrix());
}

Transform Transpose(const Transform& t) {
    return Transform(glm::transpose(t.GetMatrix()), glm::transpose(t.GetInverseMatrix()));
}

Transform Rotate(float sinTheta, float cosTheta, glm::vec3 axis) {
    glm::vec3 a = glm::normalize(axis);
    glm::mat4 m;
    m[0][0] = a.x * a.x + (1 - a.x * a.x) * cosTheta;
    m[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
    m[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
    m[0][3] = 0;

    m[1][0] = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
    m[1][1] = a.y * a.y + (1 - a.y * a.y) * cosTheta;
    m[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
    m[1][3] = 0;

    m[2][0] = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
    m[2][1] = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
    m[2][2] = a.z * a.z + (1 - a.z * a.z) * cosTheta;
    m[2][3] = 0;

    return Transform(m, glm::transpose(m));
}

Transform Rotate(float theta, glm::vec3 axis) {
    float sinTheta = std::sin(glm::radians(theta));
    float cosTheta = std::cos(glm::radians(theta));
    return Rotate(sinTheta, cosTheta, axis);
}

Transform RotateFromTo(glm::vec3 from, glm::vec3 to) {
    glm::vec3 refl;
    if (std::abs(from.x) < 0.72f && std::abs(to.x) < 0.72f)
        refl = glm::vec3(1, 0, 0);
    else if (std::abs(from.y) < 0.72f && std::abs(to.y) < 0.72f)
        refl = glm::vec3(0, 1, 0);
    else
        refl = glm::vec3(0, 0, 1);

    glm::vec3 u = refl - from, v = refl - to;
    glm::mat4 r;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            r[i][j] = ((i == j) ? 1 : 0) - 2 / glm::dot(u, u) * u[i] * u[j] -
            2 / glm::dot(v, v) * v[i] * v[j] +
            4 * glm::dot(u, v) / (glm::dot(u, u) * glm::dot(v, v)) * v[i] * u[j];

    return Transform(r, glm::transpose(r));
}

Transform Translate(glm::vec3 delta) {
    glm::mat4 m(
        1, 0, 0, delta.x,
        0, 1, 0, delta.y,
        0, 0, 1, delta.z,
        0, 0, 0, 1);
    glm::mat4 minv(
        1, 0, 0, -delta.x,
        0, 1, 0, -delta.y,
        0, 0, 1, -delta.z,
        0, 0, 0, 1);
    return Transform(m, minv);
}

Transform Scale(float x, float y, float z) {
    glm::mat4 m(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1);
    glm::mat4 minv(
        1 / x, 0, 0, 0,
        0, 1 / y, 0, 0,
        0, 0, 1 / z, 0,
        0, 0, 0, 1);
    return Transform(m, minv);
}

Transform RotateX(float theta) {
    float sinTheta = std::sin(glm::radians(theta));
    float cosTheta = std::cos(glm::radians(theta));
    glm::mat4 m(
        1, 0, 0, 0,
        0, cosTheta, -sinTheta, 0,
        0, sinTheta, cosTheta, 0,
        0, 0, 0, 1);
    return Transform(m, glm::transpose(m));
}

Transform RotateY(float theta) {
    float sinTheta = std::sin(glm::radians(theta));
    float cosTheta = std::cos(glm::radians(theta));
    glm::mat4 m(
        cosTheta, 0, sinTheta, 0,
        0, 1, 0, 0,
        -sinTheta, 0, cosTheta, 0,
        0, 0, 0, 1);
    return Transform(m, glm::transpose(m));
}
Transform RotateZ(float theta) {
    float sinTheta = std::sin(glm::radians(theta));
    float cosTheta = std::cos(glm::radians(theta));
    glm::mat4 m(
        cosTheta, -sinTheta, 0, 0,
        sinTheta, cosTheta, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
    return Transform(m, glm::transpose(m));
}

Transform LookAt(glm::vec3 pos, glm::vec3 look, glm::vec3 up) {
    glm::mat4 worldFromCamera;

    worldFromCamera[0][3] = pos.x;
    worldFromCamera[1][3] = pos.y;
    worldFromCamera[2][3] = pos.z;
    worldFromCamera[3][3] = 1;

    glm::vec3 dir = glm::normalize(look - pos);
    glm::vec3 right = glm::normalize(glm::cross(glm::normalize(up), dir));
    glm::vec3 newUp = glm::cross(dir, right);

    worldFromCamera[0][0] = right.x;
    worldFromCamera[1][0] = right.y;
    worldFromCamera[2][0] = right.z;
    worldFromCamera[3][0] = 0.;
    worldFromCamera[0][1] = newUp.x;
    worldFromCamera[1][1] = newUp.y;
    worldFromCamera[2][1] = newUp.z;
    worldFromCamera[3][1] = 0.;
    worldFromCamera[0][2] = dir.x;
    worldFromCamera[1][2] = dir.y;
    worldFromCamera[2][2] = dir.z;
    worldFromCamera[3][2] = 0.;

    glm::mat4 cameraFromWorld = glm::inverse(worldFromCamera);
    return Transform(cameraFromWorld, worldFromCamera);
}

Transform Orthographic(float zNear, float zFar) {
    return Scale(1, 1, 1 / (zFar - zNear)) * Translate(glm::vec3(0, 0, -zNear));
}

Transform Perspective(float fov, float n, float f) {
    glm::mat4 persp(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, f / (f - n), -f * n / (f - n),
        0, 0, 1, 0);
    float invTanAng = 1 / std::tan(glm::radians(fov) / 2);
    return Scale(invTanAng, invTanAng, 1) * Transform(persp);
}