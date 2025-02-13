//
// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the OpenEXR Project.
//

#ifdef NDEBUG
#    undef NDEBUG
#endif

#include "testExtractSHRT.h"
#include <ImathEuler.h>
#include <ImathFun.h>
#include <ImathMatrixAlgo.h>
#include <ImathRandom.h>
#include <assert.h>
#include <exception>
#include <iostream>
#include <stdio.h>

#if 0
#    define debug(x) (printf x, fflush (stdout))
#else
#    define debug(x)
#endif

using namespace std;
using namespace IMATH_INTERNAL_NAMESPACE;

namespace
{

float
rad (float deg)
{
    return deg * float(M_PI / 180);
}

void
testMatrix (const M33f M)
{
    //
    // Extract the rotation angle from M, and convert the
    // angle back to a matrix, N.
    //

    V2f   s (0.f), t (0.f);
    float h, r = 0.f;

    if (!extractSHRT (M, s, h, r, t, true))
    {
        cout << "Unable to extractSHRT" << std::endl;
        assert (false);
    }

    M33f N;

    N *= M33f ().setScale (s);
    N *= M33f ().setShear (h);
    N *= M33f ().setRotation (r);
    N *= M33f ().setTranslation (t);

    debug (("Re-scale: %f %f\n", s[0], s[1]));
    debug (("Re-shear: %f\n", h));
    debug (("Re-rot  : %f\n", r));
    debug (("Re-trans: %f %f\n", t[0], t[1]));

    //
    // Verify that the entries in M and N do not
    // differ too much.
    //

    M33f D (M - N);

    for (int j = 0; j < 3; ++j)
    {
        for (int k = 0; k < 3; ++k)
        {
            if (abs (D[j][k]) > 0.00001)
            {
                cout << "unexpectedly large matrix to "
                        "euler angles conversion error: "
                     << D[j][k] << endl;

                cout << j << " " << k << endl;

                cout << "M\n" << M << endl;
                cout << "N\n" << N << endl;
                cout << "D\n" << D << endl;

                assert (false);
            }
        }
    }
}

void
testRandomAngles33 ()
{
    Rand48 random (0);

    for (int i = 0; i < 100000; ++i)
    {
        debug (("iteration: %d\n", i));

        M33f M;

        //
        // Scale M.
        //

        V2f s (float(random.nextf (0.000001, 2.0)), 
               float(random.nextf (0.000001, 2.0)));

        for (int j = 0; j < 2; j++)
            if (random.nextf (0.0, 1.0) >= 0.5) s[j] *= -1;

        M *= M33f ().setScale (s);

        //
        // Shear M.
        //

        float h = float(random.nextf (0.000001, 2.));
        if (random.nextf (0.0, 1.0) >= 0.5) h *= -1;

        M *= M33f ().setShear (h);

        //
        // Rotate M.
        //

        float r = rad (float(random.nextf (-180, 180)));

        M *= M33f ().setRotation (r);

        //
        // Translate M.
        //

        V2f t (float(random.nextf (-10, 10)),
               float(random.nextf (-10, 10)));

        M *= M33f ().setTranslation (t);

        //
        // Add a small random error to the elements of M
        //

        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 2; ++k)
                M[j][k] += float(random.nextf (-1e-7, 1e-7));

        debug (("Scale   : %f %f\n", s[0], s[1]));
        debug (("Shear   : %f\n", h));
        debug (("Rot     : %f\n", r));
        debug (("Trans   : %f %f\n", t[0], t[1]));

        //
        // Extract Euler angles from M, convert the Euler angles
        // back to a matrix, N, and verify that the entries in M
        // and N do not differ too much.
        //

        testMatrix (M);

        debug (("\n"));
    }
}

void
testAngles33 (float angle)
{
    M33f M;
    M.setRotation (rad (angle));

    //
    // With rounding errors from e.toMatrix.
    //

    testMatrix (M);

    //
    // Without rounding errors (assuming that
    // all angles are multiples of 90 degrees).
    //

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            if (M[i][j] < -0.5)
                M[i][j] = -1;
            else if (M[i][j] > 0.5)
                M[i][j] = 1;
            else
                M[i][j] = 0;

    testMatrix (M);
}

void
testMatrix (const M44f M)
{
    //
    // Extract Euler angles from M, and convert the
    // Euler angles back to a matrix, N.
    //

    V3f s, h, r, t;

    extractSHRT (M, s, h, r, t, true);

    M44f N;

    N.translate (t); // ... matrix compositions
    N.rotate (r);
    N.shear (h);
    N.scale (s);

    debug (("Re-scale: %f %f %f\n", s[0], s[1], s[2]));
    debug (("Re-shear: %f %f %f\n", h[0], h[1], h[2]));
    debug (("Re-rot  : %f %f %f\n", r[0], r[1], r[2]));
    debug (("Re-trans: %f %f %f\n", t[0], t[1], t[2]));

    //
    // Verify that the entries in M and N do not
    // differ too much.
    //

    M44f D (M - N);

    for (int j = 0; j < 4; ++j)
    {
        for (int k = 0; k < 4; ++k)
        {
            if (abs (D[j][k]) > 0.00001)
            {
                cout << "unexpectedly large matrix to "
                        "euler angles conversion error: "
                     << D[j][k] << endl;

                cout << j << " " << k << endl;

                cout << "M\n" << M << endl;
                cout << "N\n" << N << endl;
                cout << "D\n" << D << endl;

                assert (false);
            }
        }
    }
}

void
testRandomAngles44 ()
{
    Rand48 random (0);

    for (int i = 0; i < 100000; ++i)
    {
        debug (("iteration: %d\n", i));

        M44f M;

        //
        // Translate M.
        //

        V3f t (
            float(random.nextf (-10, 10)),
            float(random.nextf (-10, 10)),
            float(random.nextf (-10, 10)));

        M.translate (t);

        //
        // Rotate M.
        //

        V3f r (
            rad (float(random.nextf (-180, 180))),
            rad (float(random.nextf (-180, 180))),
            rad (float(random.nextf (-180, 180))));

        M.rotate (r);

        //
        // Shear M.
        //

        V3f h (
            float(random.nextf (0.000001, 2.0)),
            float(random.nextf (0.000001, 2.0)),
            float(random.nextf (0.000001, 2.0)));

        for (int j = 0; j < 3; j++)
            if (random.nextf (0.0, 1.0) >= 0.5) h[j] *= -1;

        M.shear (h);

        //
        // Scale M.
        //

        V3f s (
            float(random.nextf (0.000001, 2.0)),
            float(random.nextf (0.000001, 2.0)),
            float(random.nextf (0.000001, 2.0)));

        for (int j = 0; j < 3; j++)
            if (random.nextf (0.0, 1.0) >= 0.5) s[j] *= -1;

        M.scale (s);

        //
        // Add a small random error to the elements of M
        //

        for (int j = 0; j < 4; ++j)
            for (int k = 0; k < 3; ++k)
                M[j][k] += float(random.nextf (-1e-7, 1e-7));

        debug (("Scale   : %f %f %f\n", s[0], s[1], s[2]));
        debug (("Shear   : %f %f %f\n", h[0], h[1], h[2]));
        debug (("Rot     : %f %f %f\n", r[0], r[1], r[2]));
        debug (("Trans   : %f %f %f\n", t[0], t[1], t[2]));

        //
        // Extract Euler angles from M, convert the Euler angles
        // back to a matrix, N, and verify that the entries in M
        // and N do not differ too much.
        //

        testMatrix (M);

        debug (("\n"));
    }
}

void
testAngles44 (V3f angles)
{
    Eulerf e (rad (angles.x), rad (angles.y), rad (angles.z));

    M44f M (e.toMatrix44 ());

    //
    // With rounding errors from e.toMatrix.
    //

    testMatrix (M);

    //
    // Without rounding errors (assuming that
    // all angles are multiples of 90 degrees).
    //

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (M[i][j] < -0.5)
                M[i][j] = -1;
            else if (M[i][j] > 0.5)
                M[i][j] = 1;
            else
                M[i][j] = 0;

    testMatrix (M);
}

void
test ()
{
    cout << "  random angles" << endl;

    cout << "    3x3" << endl;
    testRandomAngles33 ();

    cout << "    4x4" << endl;
    testRandomAngles44 ();

    cout << "  special angles" << endl;

    cout << "    3x3" << endl;
    for (int i = 0; i < 360; i += 90)
        testAngles33 (float (i));

    cout << "    4x4" << endl;
    for (int i = 0; i < 360; i += 90)
        for (int j = 0; j < 360; j += 90)
            for (int k = 0; k < 360; k += 90)
                testAngles44 (V3f (float(i), float(j), float(k)));
}

} // namespace

void
testExtractSHRT ()
{
    try
    {
        cout << "Testing extraction of scale, shear, rotation, translation "
             << "from matrices" << endl;

        cout << "Imath::extractSHRT()" << endl;
        test ();

        cout << "ok\n" << endl;
    }
    catch (std::exception& e)
    {
        cerr << "  Caught exception: " << e.what () << endl;
    }
}
