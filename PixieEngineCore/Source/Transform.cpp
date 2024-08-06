
#include "Transform.h"

Transform::Transform() {};

Transform::Transform(const Mat4& m)
    : m(m), mInv(glm::inverse(m)) {}

Transform::Transform(const Mat4& m, const Mat4& mInv)
    : m(m), mInv(mInv) {}

const Mat4& Transform::GetMatrix() const {
    return m;
}

const Mat4& Transform::GetInverseMatrix() const {
    return mInv;
}

Vec3 Transform::ApplyPoint(Vec3 p) const {
    Float xp = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
    Float yp = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
    Float zp = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
    Float wp = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];
    if (wp == 1.0f) {
        return Vec3(xp, yp, zp);
    }
    else {
        return Vec3(xp, yp, zp) / wp;
    }
}

Vec3 Transform::ApplyVector(Vec3 v) const {
    return Vec3(
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

Vec3 Transform::ApplyNormal(Vec3 n) const {
    Float x = n.x, y = n.y, z = n.z;
    return Vec3(
        mInv[0][0] * x + mInv[1][0] * y + mInv[2][0] * z,
        mInv[0][1] * x + mInv[1][1] * y + mInv[2][1] * z,
        mInv[0][2] * x + mInv[1][2] * y + mInv[2][2] * z);
}

Ray Transform::ApplyRay(const Ray& r, Float* tMax) const {
    Vec3 o = (*this).ApplyPoint(Vec3(r.origin));
    Vec3 d = (*this).ApplyVector(r.direction);

    Float lengthSquared = length2(d);
    if (lengthSquared > 0) {
        o += d * MachineEpsilon;
    }

    return Ray(r.x, r.y, o, d);
}

SurfaceInteraction Transform::ApplyInteraction(const SurfaceInteraction& in) const {
    SurfaceInteraction ret(in);
    ret.position = (*this).ApplyPoint(in.position);
    ret.normal = (*this).ApplyNormal(in.normal);
    if (length2(ret.normal) > 0.0f) {
        ret.normal = glm::normalize(ret.normal);
    }
    ret.wo = (*this).ApplyVector(in.wo);
    if (length2(ret.wo) > 0) {
        ret.wo = glm::normalize(ret.wo);
    }
    return ret;
}

Bounds3f Transform::ApplyBounds(const Bounds3f& b) const {
    Bounds3f bt;
    for (int32_t i = 0; i < 8; ++i) {
        bt = Union(bt, (*this).ApplyBounds(b));
    }
    return bt;
}

Vec3 Transform::ApplyInversePoint(Vec3 p) const {
    Float x = p.x, y = p.y, z = p.z;
    Float xp = (mInv[0][0] * x + mInv[0][1] * y) + (mInv[0][2] * z + mInv[0][3]);
    Float yp = (mInv[1][0] * x + mInv[1][1] * y) + (mInv[1][2] * z + mInv[1][3]);
    Float zp = (mInv[2][0] * x + mInv[2][1] * y) + (mInv[2][2] * z + mInv[2][3]);
    Float wp = (mInv[3][0] * x + mInv[3][1] * y) + (mInv[3][2] * z + mInv[3][3]);
    if (wp == 1.0f) {
        return Vec3(xp, yp, zp);
    }
    else {
        return Vec3(xp, yp, zp) / wp;
    }
}

Vec3 Transform::ApplyInverseVector(Vec3 v) const {
    Float x = v.x, y = v.y, z = v.z;
    return Vec3(
        mInv[0][0] * x + mInv[0][1] * y + mInv[0][2] * z,
        mInv[1][0] * x + mInv[1][1] * y + mInv[1][2] * z,
        mInv[2][0] * x + mInv[2][1] * y + mInv[2][2] * z);
}

Vec3 Transform::ApplyInverseNormal(Vec3 n) const {
    Float x = n.x, y = n.y, z = n.z;
    return Vec3(
        m[0][0] * x + m[1][0] * y + m[2][0] * z,
        m[0][1] * x + m[1][1] * y + m[2][1] * z,
        m[0][2] * x + m[1][2] * y + m[2][2] * z);
}

Ray Transform::ApplyInverseRay(const Ray& r, Float* tMax) const {
    Vec3 o = ApplyInversePoint(r.origin);
    Vec3 d = ApplyInverseVector(r.direction);

    Float lengthSquared = length2(d);
    if (lengthSquared > 0.0f) {
        o -= d * MachineEpsilon;
    }

    return Ray(r.x, r.y,o, d);
}

SurfaceInteraction Transform::ApplyInverseInteraction(const SurfaceInteraction& in) const {
    SurfaceInteraction ret(in);
    Transform t = Inverse(*this);
    ret.position = t.ApplyPoint(in.position);
    ret.normal = t.ApplyNormal(in.normal);
    if (length2(ret.normal) > 0.0f) {
        ret.normal = glm::normalize(ret.normal);
    }
    ret.wo = t.ApplyVector(in.wo);
    if (length2(ret.wo) > 0.0f) {
        ret.wo = glm::normalize(ret.wo);
    }
    return ret;
}

void Transform::Decompose(Vec3* T, Mat4* R, Mat4* S) const {
    T->x = m[0][3];
    T->y = m[1][3];
    T->z = m[2][3];

    Mat4 M = m;
    for (int32_t i = 0; i < 3; ++i) {
        M[i][3] = M[3][i] = 0.0f;
    }
    M[3][3] = 1.0f;

    Float norm;
    int32_t count = 0;
    *R = M;
    do {
        Mat4 Rit = glm::inverse(glm::transpose(*R));
        Mat4 Rnext = (*R + Rit) / (Float)2.0f;

        norm = 0;
        for (int32_t i = 0; i < 3; ++i) {
            Float n = std::abs((*R)[i][0] - Rnext[i][0]) + std::abs((*R)[i][1] - Rnext[i][1]) + std::abs((*R)[i][2] - Rnext[i][2]);
            norm = std::max(norm, n);
        }

        *R = Rnext;
    } while (++count < 100 && norm > 0.0001f);

    *S = glm::inverse(*R) * M;
}

bool Transform::IsIdentity() const {
    return m == Mat4();
}

bool Transform::HasScale(Float tolerance) const {
    Float la2 = length2((*this).ApplyVector(Vec3(1, 0, 0)));
    Float lb2 = length2((*this).ApplyVector(Vec3(0, 1, 0)));
    Float lc2 = length2((*this).ApplyVector(Vec3(0, 0, 1)));
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

Transform Rotate(Float sinTheta, Float cosTheta, Vec3 axis) {
    Vec3 a = glm::normalize(axis);
    Mat4 m;
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

Transform Rotate(Float theta, Vec3 axis) {
    Float sinTheta = std::sin(glm::radians(theta));
    Float cosTheta = std::cos(glm::radians(theta));
    return Rotate(sinTheta, cosTheta, axis);
}

Transform RotateFromTo(Vec3 from, Vec3 to) {
    Vec3 refl;
    if (std::abs(from.x) < 0.72f && std::abs(to.x) < 0.72f) {
        refl = Vec3(1, 0, 0);
    }
    else if (std::abs(from.y) < 0.72f && std::abs(to.y) < 0.72f) {
        refl = Vec3(0, 1, 0);
    }
    else {
        refl = Vec3(0, 0, 1);
    }

    Vec3 u = refl - from, v = refl - to;
    Mat4 r;
    for (int32_t i = 0; i < 3; ++i) {
        for (int32_t j = 0; j < 3; ++j) {
            r[i][j] = ((i == j) ? 1 : 0) - 2 / glm::dot(u, u) * u[i] * u[j] -
                2 / glm::dot(v, v) * v[i] * v[j] +
                4 * glm::dot(u, v) / (glm::dot(u, u) * glm::dot(v, v)) * v[i] * u[j];
        }
    }

    return Transform(r, glm::transpose(r));
}

Transform Translate(Vec3 delta) {
    Mat4 m(
        1, 0, 0, delta.x,
        0, 1, 0, delta.y,
        0, 0, 1, delta.z,
        0, 0, 0, 1);
    Mat4 minv(
        1, 0, 0, -delta.x,
        0, 1, 0, -delta.y,
        0, 0, 1, -delta.z,
        0, 0, 0, 1);
    return Transform(m, minv);
}

Transform Scale(Float x, Float y, Float z) {
    Mat4 m(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1);
    Mat4 minv(
        1 / x, 0, 0, 0,
        0, 1 / y, 0, 0,
        0, 0, 1 / z, 0,
        0, 0, 0, 1);
    return Transform(m, minv);
}

Transform RotateX(Float theta) {
    Float sinTheta = std::sin(glm::radians(theta));
    Float cosTheta = std::cos(glm::radians(theta));
    Mat4 m(
        1, 0, 0, 0,
        0, cosTheta, -sinTheta, 0,
        0, sinTheta, cosTheta, 0,
        0, 0, 0, 1);
    return Transform(m, glm::transpose(m));
}

Transform RotateY(Float theta) {
    Float sinTheta = std::sin(glm::radians(theta));
    Float cosTheta = std::cos(glm::radians(theta));
    Mat4 m(
        cosTheta, 0, sinTheta, 0,
        0, 1, 0, 0,
        -sinTheta, 0, cosTheta, 0,
        0, 0, 0, 1);
    return Transform(m, glm::transpose(m));
}
Transform RotateZ(Float theta) {
    Float sinTheta = std::sin(glm::radians(theta));
    Float cosTheta = std::cos(glm::radians(theta));
    Mat4 m(
        cosTheta, -sinTheta, 0, 0,
        sinTheta, cosTheta, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
    return Transform(m, glm::transpose(m));
}

Transform LookAt(Vec3 pos, Vec3 look, Vec3 up) {
    Mat4 worldFromCamera;

    worldFromCamera[0][3] = pos.x;
    worldFromCamera[1][3] = pos.y;
    worldFromCamera[2][3] = pos.z;
    worldFromCamera[3][3] = 1;

    Vec3 dir = glm::normalize(look - pos);
    Vec3 right = glm::normalize(glm::cross(glm::normalize(up), dir));
    Vec3 newUp = glm::cross(dir, right);

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

    Mat4 cameraFromWorld = glm::inverse(worldFromCamera);
    return Transform(cameraFromWorld, worldFromCamera);
}

Transform Orthographic(Float zNear, Float zFar) {
    return Scale(1, 1, 1 / (zFar - zNear)) * Translate(Vec3(0, 0, -zNear));
}

Transform Perspective(Float fov, Float n, Float f) {
    Mat4 persp(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, f / (f - n), -f * n / (f - n),
        0, 0, 1, 0);
    Float invTanAng = 1 / std::tan(glm::radians(fov) / 2);
    return Scale(invTanAng, invTanAng, 1) * Transform(persp);
}