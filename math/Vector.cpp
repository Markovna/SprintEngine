#include "Vector.h"

const Vec3 Vector<3>::Zero(0.0f, 0.0f, 0.0f);
const Vec3 Vector<3>::One(1.0f, 1.0f, 1.0f);
const Vec3 Vector<3>::Up(0.0f, 0.0f, 1.0f);
const Vec3 Vector<3>::Down(0.0f, 0.0f, -1.0f);
const Vec3 Vector<3>::Forward(1.0f, 0.0f, 0.0f);
const Vec3 Vector<3>::Backward(-1.0f, 0.0f, 0.0f);
const Vec3 Vector<3>::Right(0.0f, 1.0f, 0.0f);
const Vec3 Vector<3>::Left(0.0f, -1.0f, 0.0f);

const Vec2 Vector<2>::Zero(0.0f, 0.0f);
const Vec2 Vector<2>::One(1.0f, 1.0f);