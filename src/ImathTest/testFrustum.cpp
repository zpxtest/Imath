//
// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the OpenEXR Project.
//

#ifdef NDEBUG
#    undef NDEBUG
#endif

#include "testFrustum.h"
#include <ImathEuler.h>
#include <ImathFrustum.h>
#include <ImathFun.h>
#include <ImathVec.h>
#include <assert.h>
#include <iostream>

// Include ImathForward *after* other headers to validate forward declarations
#include <ImathForward.h>

using namespace std;

namespace
{

void
testFrustumPlanes (IMATH_INTERNAL_NAMESPACE::Frustumf& frustum)
{
    bool                          ortho = frustum.orthographic ();
    IMATH_INTERNAL_NAMESPACE::V3f o (0.0f, 0.0f, 0.0f);
    float                         eps = 5.0e-4f;

    for (auto xRo: {0.0f, 100.0f, 200.0f})
    {
        for (auto yRo: {0.0f, 105.0f, 210.0f, 315.0f})
        {
            for (auto zRo: {0.0f, 110.0f, 220.0f, 330.0f})
            {
                for (auto xTr:
                     {-10.0f,
                      -8.0f,
                      -6.0f,
                      -4.0f,
                      -2.0f,
                      0.0f,
                      2.0f,
                      4.0f,
                      6.0f,
                      8.0f})
                {
                    for (auto yTr:
                         {-10.0f, -7.0f, -4.0f, -1.0f, 2.0f, 5.0f, 8.0f})
                    {
                        for (auto zTr: {-10.0f, -6.0f, -2.0f, 2.0f, 6.0f})
                        {
                            float xRoRad = xRo * (2.0f * float (M_PI) / 360.0f);
                            float yRoRad = yRo * (2.0f * float (M_PI) / 360.0f);
                            float zRoRad = zRo * (2.0f * float (M_PI) / 360.0f);
                            IMATH_INTERNAL_NAMESPACE::Eulerf e (
                                xRoRad, yRoRad, zRoRad);
                            IMATH_INTERNAL_NAMESPACE::M44f mView =
                                e.toMatrix44 ();
                            mView.translate (
                                IMATH_INTERNAL_NAMESPACE::V3f (xTr, yTr, zTr));

                            IMATH_INTERNAL_NAMESPACE::Plane3f planes0[6];
                            frustum.planes (planes0);

                            IMATH_INTERNAL_NAMESPACE::Plane3f planes[6];
                            frustum.planes (planes, mView);

                            IMATH_INTERNAL_NAMESPACE::V3f up =
                                IMATH_INTERNAL_NAMESPACE::V3f (0, 1, 0);
                            assert ((up ^ planes0[0].normal) > 0.0);
                            mView.multDirMatrix (up, up);
                            assert ((up ^ planes[0].normal) > 0.0);

                            IMATH_INTERNAL_NAMESPACE::V3f pt =
                                (!ortho) ? o
                                         : IMATH_INTERNAL_NAMESPACE::V3f (
                                               0.0f, frustum.top (), 0.0f);
                            float d = planes0[0].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));
                            pt = pt * mView;
                            d  = planes[0].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));

                            IMATH_INTERNAL_NAMESPACE::V3f right =
                                IMATH_INTERNAL_NAMESPACE::V3f (1, 0, 0);
                            assert ((right ^ planes0[1].normal) > 0.0);
                            mView.multDirMatrix (right, right);
                            assert ((right ^ planes[1].normal) > 0.0);

                            pt = (!ortho) ? o
                                          : IMATH_INTERNAL_NAMESPACE::V3f (
                                                frustum.right (), 0.0f, 0.0f);
                            d  = planes0[1].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));
                            pt = pt * mView;
                            d  = planes[1].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));

                            IMATH_INTERNAL_NAMESPACE::V3f down =
                                IMATH_INTERNAL_NAMESPACE::V3f (0, -1, 0);
                            assert ((down ^ planes0[2].normal) > 0.0);
                            mView.multDirMatrix (down, down);
                            assert ((down ^ planes[2].normal) > 0.0);

                            pt = (!ortho) ? o
                                          : IMATH_INTERNAL_NAMESPACE::V3f (
                                                0.0f, frustum.bottom (), 0.0f);
                            d  = planes0[2].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));
                            pt = pt * mView;
                            d  = planes[2].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));

                            IMATH_INTERNAL_NAMESPACE::V3f left =
                                IMATH_INTERNAL_NAMESPACE::V3f (-1, 0, 0);
                            assert ((left ^ planes0[3].normal) > 0.0);
                            mView.multDirMatrix (left, left);
                            assert ((left ^ planes[3].normal) > 0.0);

                            pt = (!ortho) ? o
                                          : IMATH_INTERNAL_NAMESPACE::V3f (
                                                frustum.left (), 0.0f, 0.0f);
                            d  = planes0[3].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));
                            pt = pt * mView;
                            d  = planes[3].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));

                            IMATH_INTERNAL_NAMESPACE::V3f front =
                                IMATH_INTERNAL_NAMESPACE::V3f (0, 0, 1);
                            assert ((front ^ planes0[4].normal) > 0.0);
                            mView.multDirMatrix (front, front);
                            assert ((front ^ planes[4].normal) > 0.0);

                            pt = IMATH_INTERNAL_NAMESPACE::V3f (
                                0.0f, 0.0f, -frustum.nearPlane ());
                            d = planes0[4].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));
                            pt = pt * mView;
                            d  = planes[4].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));

                            IMATH_INTERNAL_NAMESPACE::V3f back =
                                IMATH_INTERNAL_NAMESPACE::V3f (0, 0, -1);
                            assert ((back ^ planes0[5].normal) > 0.0);
                            mView.multDirMatrix (back, back);
                            assert ((back ^ planes[5].normal) > 0.0);

                            pt = IMATH_INTERNAL_NAMESPACE::V3f (
                                0.0f, 0.0f, -frustum.farPlane ());
                            d = planes0[5].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));
                            pt = pt * mView;
                            d  = planes[5].distanceTo (pt);
                            assert (IMATH_INTERNAL_NAMESPACE::iszero (d, eps));
                        }
                    }
                }
            }
        }
    }
}

} // namespace

void
testFrustum ()
{
    cout << "Testing functions in ImathFrustum.h";

    cout << "\nperspective ";

    float n = 1.7f;
    float f = 567.0f;
    float l = -3.5f;
    float r = 2.0f;
    float b = -1.3f;
    float t = 0.9f;

    IMATH_INTERNAL_NAMESPACE::Frustum<float> frustum (n, f, l, r, t, b, false);

    assert (
        IMATH_INTERNAL_NAMESPACE::abs<float> (
            frustum.fovx () - (atan2 (r, n) - atan2 (l, n))) < 1e-6);
    assert (
        IMATH_INTERNAL_NAMESPACE::abs<float> (
            frustum.fovy () - (atan2 (t, n) - atan2 (b, n))) < 1e-6);
    cout << "1";
    assert (
        IMATH_INTERNAL_NAMESPACE::abs<float> (
            frustum.aspectExc () - ((r - l) / (t - b))) < 1e-6);
    assert (
        IMATH_INTERNAL_NAMESPACE::abs<float> (
            frustum.aspect () - ((r - l) / (t - b))) < 1e-6);
    cout << "2";

    IMATH_INTERNAL_NAMESPACE::M44f m = frustum.projectionMatrixExc ();
    assert (
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][0] - ((2 * n) / (r - l))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][1]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][2]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][3]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][0]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][1] - ((2 * n) / (t - b))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][2]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][3]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][0] - ((r + l) / (r - l))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][1] - ((t + b) / (t - b))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][2] - (-(f + n) / (f - n))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][3] - -1.0f) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[3][0]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[3][1]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (
            m[3][2] - ((-2 * f * n) / (f - n))) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[3][3]) < 1e-6f);
    m = frustum.projectionMatrix ();
    assert (
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][0] - ((2 * n) / (r - l))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][1]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][2]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][3]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][0]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][1] - ((2 * n) / (t - b))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][2]) < 1e-6 &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][3]) < 1e-6 &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][0] - ((r + l) / (r - l))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][1] - ((t + b) / (t - b))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][2] - (-(f + n) / (f - n))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][3] - -1.0f) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[3][0]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[3][1]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (
            m[3][2] - ((-2 * f * n) / (f - n))) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[3][3]) < 1e-6f);
    cout << "3";

    cout << "\nplanes ";
    testFrustumPlanes (frustum);

    cout << "\nexceptions ";
    IMATH_INTERNAL_NAMESPACE::Frustum<float> badFrustum;

    bool caught;

    badFrustum.set (n, n, l, r, t, b, false);
    caught = false;
    try
    {
        (void) badFrustum.projectionMatrixExc ();
        assert (!"near == far didn't throw an exception");
    }
    catch (std::domain_error&)
    {
        caught = true;
    }
    assert (caught);
    cout << "1";

    badFrustum.set (n, f, l, l, t, b, false);
    caught = false;
    try
    {
        (void) badFrustum.projectionMatrixExc ();
        assert (!"left == right didn't throw an exception");
    }
    catch (std::domain_error&)
    {
        caught = true;
    }
    assert (caught);
    cout << "2";

    badFrustum.set (n, f, l, r, t, t, false);
    caught = false;
    try
    {
        (void) badFrustum.projectionMatrixExc ();
        assert (!"top == bottom didn't throw an exception");
    }
    catch (std::domain_error&)
    {
        caught = true;
    }
    assert (caught);
    cout << "3";

    cout << "\northographic ";

    frustum.setOrthographic (true);

    m = frustum.projectionMatrix ();
    assert (
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][0] - (2 / (r - l))) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][1]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][2]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[0][3]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][0]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][1] - (2 / (t - b))) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][2]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[1][3]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][0]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][1]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][2] - (-2 / (f - n))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[2][3]) < 1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[3][0] - (-(r + l) / (r - l))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[3][1] - (-(t + b) / (t - b))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[3][2] - (-(f + n) / (f - n))) <
            1e-6f &&
        IMATH_INTERNAL_NAMESPACE::abs<float> (m[3][3] - 1.0f) < 1e-6f);
    cout << "1";

    cout << "\nplanes ";
    testFrustumPlanes (frustum);

    // TODO - There are many little functions in IMATH_INTERNAL_NAMESPACE::Frustum which
    // aren't tested here.  Those test should be added.  But this is
    // a start.

    IMATH_INTERNAL_NAMESPACE::Frustum<float> f1 (n, f, l, r, t, b, false);
    IMATH_INTERNAL_NAMESPACE::Frustum<float> f2 (n, f, l, r, t, b, true);
    assert (f1 != f2);
    f2.set (n + 0.1f, f, l, r, t, b, false);
    assert (f1 != f2);
    f2.set (n, f + 0.1f, l, r, t, b, false);
    assert (f1 != f2);
    f2.set (n, f, l + 0.1f, r, t, b, false);
    assert (f1 != f2);
    f2.set (n, f, l, r + 0.1f, t, b, false);
    assert (f1 != f2);
    f2.set (n, f, l, r, t + 0.1f, b, false);
    assert (f1 != f2);
    f2.set (n, f, l, r, t, b + 0.1f, false);
    assert (f1 != f2);
    cout << "\npassed inequality test";

    f1 = f2;
    assert (f1 == f2);
    cout << "\npassed equality test";

    long                                  zMax = 100;
    long                                  zMin = 1;
    float                                 zero = 0;
    float                                 one  = 1;
    IMATH_INTERNAL_NAMESPACE::Vec3<float> v3 (zero, zero, one);

    f1.set (n, f, one, zero, one);
    f2.setExc (n, f, one, zero, one);

    assert (f1 == f2);

    assert (
        f1.ZToDepth (zMin, zMin, zMax) == f1.ZToDepthExc (zMin, zMin, zMax));
    assert (f1.normalizedZToDepth (float(zMin)) == f1.normalizedZToDepthExc (float(zMin)));
    assert (f1.DepthToZ (n, zMin, zMax) == f1.DepthToZExc (n, zMin, zMax));
    assert (f1.worldRadius (v3, one) == f1.worldRadiusExc (v3, one));
    assert (f1.screenRadius (v3, one) == f1.screenRadiusExc (v3, one));
    assert (f1.projectPointToScreen (v3) == f1.projectPointToScreenExc (v3));

    cout << "\npassed noexcept equality verification";

    cout << "\nok\n\n";
}
