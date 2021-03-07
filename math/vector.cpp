#include "vector.h"


namespace sprint {

const Vec3 vector<3>::Zero(0.0f, 0.0f, 0.0f);
const Vec3 vector<3>::One(1.0f, 1.0f, 1.0f);
const Vec3 vector<3>::Forward(0.0f, 0.0f, 1.0f);
const Vec3 vector<3>::Backward(0.0f, 0.0f, -1.0f);
const Vec3 vector<3>::Right(1.0f, 0.0f, 0.0f);
const Vec3 vector<3>::Left(-1.0f, 0.0f, 0.0f);
const Vec3 vector<3>::Up(0.0f, 1.0f, 0.0f);
const Vec3 vector<3>::Down(0.0f, -1.0f, 0.0f);

const Vec2 vector<2>::Zero(0.0f, 0.0f);
const Vec2 vector<2>::One(1.0f, 1.0f);

}