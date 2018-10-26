#ifndef MYGEOMETRY
#define MYGEOMETRY

#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <string.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <omp.h>
#include <vector_types.h>
#include <vector_functions.h>
// #include <cutil_inline.h>
// #include <cutil_math.h>

using namespace std;

namespace
{

bool operator!=(const float2 &f1, const float2 &f2) {
	return ((f1.x != f2.x) || (f1.y != f2.y));
}


class Line2D
{
public:
	float2 P0, P1;

	Line2D()
	{
	}

	Line2D(float2& _P0, float2& _P1)
	{
		P0 = _P0;
		P1 = _P1;
	}
};

class Line3D
{
public:
	float3 P0, P1;

	Line3D()
	{
	}

	Line3D(float3& _P0, float3& _P1)
	{
		P0 = _P0;
		P1 = _P1;
	}
};

// inSegment(): determine if a point is inside a segment
//    Input:  a point P, and a collinear segment S
//    Return: 1 = P is inside S
//            0 = P is not inside S
int inSegment(const float2& P, const Line2D& S)
{
    if (S.P0.x != S.P1.x) {    // S is not vertical
        if (S.P0.x <= P.x && P.x <= S.P1.x)
            return 1;
        if (S.P0.x >= P.x && P.x >= S.P1.x)
            return 1;
    }
    else {    // S is vertical, so test y coordinate
        if (S.P0.y <= P.y && P.y <= S.P1.y)
            return 1;
        if (S.P0.y >= P.y && P.y >= S.P1.y)
            return 1;
    }
    return 0;
}
//===================================================================

// intersect2D_2Segments(): the intersection of 2 finite 2D segments
//    Input:  two finite segments S1 and S2
//    Output: *I0 = intersect point (when it exists)
//            *I1 = endpoint of intersect segment [I0,I1] (when it exists)
//    Return: 0=disjoint (no intersect)
//            1=intersect in unique point I0
//            2=overlap in segment from I0 to I1
int intersect2D_Segments( Line2D S1, Line2D S2, float2* I0, float2* I1 )
{
	#define SMALL_NUM  0.00000001 // anything that avoids division overflow
	// dot product (3D) which allows vector operations in arguments
	#define perp(u,v)  ((u).x * (v).y - (u).y * (v).x)  // perp product (2D)

    float2    u = S1.P1 - S1.P0;
    float2    v = S2.P1 - S2.P0;
    float2    w = S1.P0 - S2.P0;
    float     D = perp(u,v);

    // test if they are parallel (includes either being a point)
    if (fabs(D) < SMALL_NUM) {          // S1 and S2 are parallel
        if (perp(u,w) != 0 || perp(v,w) != 0) {
            return 0;                   // they are NOT collinear
        }
        // they are collinear or degenerate
        // check if they are degenerate points
        float du = dot(u,u);
        float dv = dot(v,v);
        if (du==0 && dv==0) {           // both segments are points
            if (S1.P0 != S2.P0)         // they are distinct points
                return 0;
            *I0 = S1.P0;                // they are the same point
            return 1;
        }
        if (du==0) {                    // S1 is a single point
            if (inSegment(S1.P0, S2) == 0)  // but is not in S2
                return 0;
            *I0 = S1.P0;
            return 1;
        }
        if (dv==0) {                    // S2 a single point
            if (inSegment(S2.P0, S1) == 0)  // but is not in S1
                return 0;
            *I0 = S2.P0;
            return 1;
        }
        // they are collinear segments - get overlap (or not)
        float t0, t1;                   // endpoints of S1 in eqn for S2
        float2 w2 = S1.P1 - S2.P0;
        if (v.x != 0) {
                t0 = w.x / v.x;
                t1 = w2.x / v.x;
        }
        else {
                t0 = w.y / v.y;
                t1 = w2.y / v.y;
        }
        if (t0 > t1) {                  // must have t0 smaller than t1
                float t=t0; t0=t1; t1=t;    // swap if not
        }
        if (t0 > 1 || t1 < 0) {
            return 0;     // NO overlap
        }
        t0 = t0<0? 0 : t0;              // clip to min 0
        t1 = t1>1? 1 : t1;              // clip to max 1
        if (t0 == t1) {                 // intersect is a point
            *I0 = S2.P0 + t0 * v;
            return 1;
        }

        // they overlap in a valid subsegment
        *I0 = S2.P0 + t0 * v;
        *I1 = S2.P0 + t1 * v;
        return 2;
    }

    // the segments are skew and may intersect in a point
    // get the intersect parameter for S1
    float     sI = perp(v,w) / D;
    if (sI < 0 || sI > 1)               // no intersect with S1
        return 0;

    // get the intersect parameter for S2
    float     tI = perp(u,w) / D;
    if (tI < 0 || tI > 1)               // no intersect with S2
        return 0;

    *I0 = S1.P0 + sI * u;               // compute S1 intersect point
    return 1;
}
//===================================================================

// intersect2D_2Segments(): the intersection of 2 finite 2D segments
//    Input:  two finite segments S1 and S2
//    Output: *I0 = intersect point (when it exists)
//            *I1 = endpoint of intersect segment [I0,I1] (when it exists)
//    Return: 0=disjoint (no intersect)
//            1=intersect in unique point I0
//            2=overlap in segment from I0 to I1
int intersect2D_Segments(const Line2D& S1, const Line2D& S2)
{
	//    Return: 0=disjoint (no intersect)
	//            1=intersect in unique point I0
	//            2=overlap in segment from I0 to I1

	#define SMALL_NUM  0.00000001 // anything that avoids division overflow
	// dot product (3D) which allows vector operations in arguments
	#define perp(u,v)  ((u).x * (v).y - (u).y * (v).x)  // perp product (2D)

	float2    u = S1.P1 - S1.P0;
    float2    v = S2.P1 - S2.P0;
    float2    w = S1.P0 - S2.P0;
    float     D = perp(u,v);

    // test if they are parallel (includes either being a point)
    if (fabs(D) < SMALL_NUM) {          // S1 and S2 are parallel
        if (perp(u,w) != 0 || perp(v,w) != 0) {
            return 0;                   // they are NOT collinear
        }
        // they are collinear or degenerate
        // check if they are degenerate points
        float du = dot(u,u);
        float dv = dot(v,v);
        if (du==0 && dv==0) {           // both segments are points
			if ((S1.P0.x != S2.P0.x) || (S1.P0.y != S2.P0.y))         // they are distinct points
                return 0;
            return 1;
        }
        if (du==0) {                    // S1 is a single point
            if (inSegment(S1.P0, S2) == 0)  // but is not in S2
                return 0;
            return 1;
        }
        if (dv==0) {                    // S2 a single point
            if (inSegment(S2.P0, S1) == 0)  // but is not in S1
                return 0;
            return 1;
        }
        // they are collinear segments - get overlap (or not)
        float t0, t1;                   // endpoints of S1 in eqn for S2
        float2 w2 = S1.P1 - S2.P0;
        if (v.x != 0) {
                t0 = w.x / v.x;
                t1 = w2.x / v.x;
        }
        else {
                t0 = w.y / v.y;
                t1 = w2.y / v.y;
        }
        if (t0 > t1) {                  // must have t0 smaller than t1
                float t=t0; t0=t1; t1=t;    // swap if not
        }
        if (t0 > 1 || t1 < 0) {
            return 0;     // NO overlap
        }
        t0 = t0<0? 0 : t0;              // clip to min 0
        t1 = t1>1? 1 : t1;              // clip to max 1
        if (t0 == t1) {                 // intersect is a point
            return 1;
        }

        // they overlap in a valid subsegment
        return 2;
    }

    // the segments are skew and may intersect in a point
    // get the intersect parameter for S1
    float     sI = perp(v,w) / D;
    if (sI < 0 || sI > 1)               // no intersect with S1
        return 0;

    // get the intersect parameter for S2
    float     tI = perp(u,w) / D;
    if (tI < 0 || tI > 1)               // no intersect with S2
        return 0;

    return 1;
}
//===================================================================

// dist3D_Line_to_Line():
//    Input:  two 3D lines L1 and L2
//    Return: the shortest distance between L1 and L2
float dist3D_Line_to_Line( Line3D L1, Line3D L2)
{
    float3   u = L1.P1 - L1.P0;
    float3   v = L2.P1 - L2.P0;
    float3   w = L1.P0 - L2.P0;
    float    a = dot(u,u);        // always >= 0
    float    b = dot(u,v);
    float    c = dot(v,v);        // always >= 0
    float    d = dot(u,w);
    float    e = dot(v,w);
    float    D = a*c - b*b;       // always >= 0
    float    sc, tc;

    // compute the line parameters of the two closest points
    if (D < SMALL_NUM) {         // the lines are almost parallel
        sc = 0.0;
        tc = (b>c ? d/b : e/c);   // use the largest denominator
    }
    else {
        sc = (b*e - c*d) / D;
        tc = (a*e - b*d) / D;
    }

    // get the difference of the two closest points
    float3   dP = w + (sc * u) - (tc * v);  // = L1(sc) - L2(tc)

    return length(dP);   // return the closest distance
}
//===================================================================

// dist3D_Segment_to_Segment():
//    Input:  two 3D line segments S1 and S2
//    Return: the shortest distance between S1 and S2
float dist3D_Segment_to_Segment( Line3D S1, Line3D S2)
{
    float3   u = S1.P1 - S1.P0;
    float3   v = S2.P1 - S2.P0;
    float3   w = S1.P0 - S2.P0;
    float    a = dot(u,u);        // always >= 0
    float    b = dot(u,v);
    float    c = dot(v,v);        // always >= 0
    float    d = dot(u,w);
    float    e = dot(v,w);
    float    D = a*c - b*b;       // always >= 0
    float    sc, sN, sD = D;      // sc = sN / sD, default sD = D >= 0
    float    tc, tN, tD = D;      // tc = tN / tD, default tD = D >= 0

    // compute the line parameters of the two closest points
    if (D < SMALL_NUM) { // the lines are almost parallel
        sN = 0.0;        // force using point P0 on segment S1
        sD = 1.0;        // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                // get the closest points on the infinite lines
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < 0.0) {       // sc < 0 => the s=0 edge is visible
            sN = 0.0;
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1 => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0) {           // tc < 0 => the t=0 edge is visible
        tN = 0.0;
        // recompute sc for this edge
        if (-d < 0.0)
            sN = 0.0;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD) {      // tc > 1 => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (abs(sN) < SMALL_NUM ? 0.0 : sN / sD);
    tc = (abs(tN) < SMALL_NUM ? 0.0 : tN / tD);

    // get the difference of the two closest points
    float3   dP = w + (sc * u) - (tc * v);  // = S1(sc) - S2(tc)

    return length(dP);   // return the closest distance
}
//===================================================================

// closest2D_Point_to_Line(): finds the closest 2D Point to a Line2D
//    Input:  an array P[] of n points, and a Line2D L
//    Return: the index i of the Point P[i] closest to L
int closest2D_Point_to_Line(float2* P, int n, Line2D L)
{
    // Get coefficients of the implicit line equation.
    // Do NOT normalize since scaling by a constant
    // is irrelevant for just comparing distances.
    float a = L.P0.y - L.P1.y;
    float b = L.P1.x - L.P0.x;
    float c = L.P0.x * L.P1.y - L.P1.x * L.P0.y;

    // initialize min index and distance to P[0]
    int mi = 0;
    float min = a * P[0].x + b * P[0].y + c;
    if (min < 0) min = -min;    // absolute value

    // loop through Point array testing for min distance to L
    for (int i=1; i<n; i++) {
        // just use dist squared (sqrt not needed for comparison)
        float dist = a * P[i].x + b * P[i].y + c;
        if (dist < 0) dist = -dist;   // absolute value
        if (dist < min) {    // this point is closer
            mi = i;          // so have a new minimum
            min = dist;
        }
    }
    return mi;    // the index of the closest Point P[mi]
}
//===================================================================

// dist_Point_to_Line(): get the distance of a point to a line.
//    Input:  a Point P and a Line2D L (in any dimension)
//    Return: the shortest distance from P to L
float dist_Point_to_Line(float2 P, Line2D L)
{
    float2 v = L.P1 - L.P0;
    float2 w = P - L.P0;

    double c1 = dot(w,v);
    double c2 = dot(v,v);
    double b = c1 / c2;

    float2 Pb = L.P0 + b * v;
    return length(P - Pb);
}
//===================================================================

// dist_Point_to_Line(): get the distance of a point to a line.
//    Input:  a Point P and a Line2D L (in any dimension)
//    Return: the shortest distance from P to L
float2 closest_Point_on_Line(float2 P, Line2D L)
{
    float2 v = L.P1 - L.P0;
    float2 w = P - L.P0;

    double c1 = dot(w,v);
    double c2 = dot(v,v);
    double b = c1 / c2;

    float2 Pb = L.P0 + b * v;
    return Pb;
}
//===================================================================

// dist_Point_to_Segment(): get the distance of a point to a segment.
//    Input:  a Point P and a Segment S (in any dimension)
//    Return: the shortest distance from P to S
float dist_Point_to_Segment(float2 P, Line2D S)
{
    float2 v = S.P1 - S.P0;
    float2 w = P - S.P0;

    double c1 = dot(w,v);
    if ( c1 <= 0 )
        return length(P - S.P0);

    double c2 = dot(v,v);
    if ( c2 <= c1 )
        return length(P - S.P1);

    double b = c1 / c2;
    float2 Pb = S.P0 + b * v;
    return length(P - Pb);
}
//===================================================================

// dist_Point_to_Segment(): get the distance of a point to a segment.
//    Input:  a Point P and a Segment S (in any dimension)
//    Return: the shortest distance from P to S
float2 closest_Point_on_Segment(float2 P, Line2D S)
{
    float2 v = S.P1 - S.P0;
    float2 w = P - S.P0;

    double c1 = dot(w,v);
    if ( c1 <= 0 )
        return S.P0;

    double c2 = dot(v,v);
    if ( c2 <= c1 )
        return S.P1;

    double b = c1 / c2;
    float2 Pb = S.P0 + b * v;
    return Pb;
}
//===================================================================

float2 rotate2D_vector(float2 vec, double degrees)
{
	double theta = degrees * MYPI / 180.0;

	double cs = cos(theta);
	double sn = sin(theta);

	float2 ret;
	ret.x = vec.x * cs - vec.y * sn;
	ret.y = vec.x * sn + vec.y * cs;

	return ret;
}
}
#endif
