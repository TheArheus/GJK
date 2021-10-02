#include "GJKmath.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct shape
{
    int VerticesCount;
    v3* Vertices;
};

struct simplex
{
    int PointsCount;
    v3* Points;
};

static v3 FindCenter(shape* Shape)
{
    v3 Result = {};

    for(int Idx = 0; Idx < Shape->VerticesCount; ++Idx)
    {
        Result = Result + Shape->Vertices[Idx];
    }

    Result = Result / (float)Shape->VerticesCount;

    return Result;
}

static simplex* CreateNewSimplex(v3 StartPoint)
{
    simplex* Simplex = (simplex*)malloc(sizeof(simplex));
    Simplex->PointsCount = 1;
    Simplex->Points = (v3*)malloc(sizeof(v3));
    Simplex->Points[0] = StartPoint;

    return Simplex;
}

static void AddNewPointToSimplex(simplex* Simplex, v3 NewPoint)
{
    Simplex->PointsCount += 1;
    Simplex->Points = (v3*)realloc(Simplex->Points, sizeof(v3)*Simplex->PointsCount);
    Simplex->Points[Simplex->PointsCount - 1] = NewPoint;
}

static void RemovePointToSimplex(simplex* Simplex, v3 Point)
{
    for(int Idx = 0; Idx < Simplex->PointsCount; ++Idx)
    {
        /*if(Idx == Simplex->PointsCount)
        {
            break;
        }
        else*/ if(Simplex->Points[Idx] == Point)
        {
            Simplex->Points[Idx] = Simplex->Points[Simplex->PointsCount - 1];
            break;
        }
    }
    Simplex->PointsCount -= 1;
    Simplex->Points = (v3*)realloc(Simplex->Points, sizeof(v3)*Simplex->PointsCount);
}

static v3 FindFurthestPoint(shape* Shape, v3 Direction)
{
    v3 Result;

    float DotMax = 0;
    int DotMaxIdx = 0;
    for(int Idx = 0; Idx < Shape->VerticesCount; ++Idx)
    {
        float Dot = DotProduct(Shape->Vertices[Idx], Direction);

        if(Dot > DotMax)
        {
            DotMaxIdx = Idx;
            DotMax = Dot;
        }
    }

    Result = Shape->Vertices[DotMaxIdx];

    return Result;
}

static v3 FindSupport(shape* A, shape* B, v3 Direction)
{
    v3 Result = FindFurthestPoint(A, Direction) - FindFurthestPoint(B, -1.0f*Direction);

    return Result;
}

static bool HandleLine(simplex* Simplex, v3* Direction)
{
    bool Result = false;

    v3 AO = V3(0, 0, 0) - Simplex->Points[0];
    v3 AB = Simplex->Points[1] - Simplex->Points[0];
    if(DotProduct(AB, AO) > 0)
    {
        v3 ABPerp = CrossProduct(CrossProduct(AB, AO), AB);
        *Direction = ABPerp;
    }
    else
    {
        RemovePointToSimplex(Simplex, Simplex->Points[0]);
        *Direction = AO;
    }

    return Result;
}

v3 TripleProduct(v3 A, v3 B, v3 C)
{
    v3 Result = B*DotProduct(C, A) - A*DotProduct(C, B);

    return Result;
}

static bool HandleTriangle(simplex* Simplex, v3* Direction)
{
    bool Result = true;

    v3 A = Simplex->Points[2];
    v3 B = Simplex->Points[1];
    v3 C = Simplex->Points[0];

    v3 AB = B - A;
    v3 AC = C - A;
    v3 AO = V3(0, 0, 0) - A;
    v3 ABC = CrossProduct(AB, AC);

    v3 ABPerp = CrossProduct(AB, ABC);
    v3 ACPerp = CrossProduct(ABC, AC);

    if(DotProduct(ACPerp, AO) > 0)
    {
        if(DotProduct(AC, AO) > 0)
        {
            RemovePointToSimplex(Simplex, B);
            *Direction = CrossProduct(CrossProduct(AC, AO), AC);
            Result = false;
        }
        else
        {
            if(DotProduct(AB, AO) > 0)
            {
                RemovePointToSimplex(Simplex, C);
                *Direction = CrossProduct(CrossProduct(AC, AO), AB);
                Result = false;
            }
            else
            {
                RemovePointToSimplex(Simplex, B);
                RemovePointToSimplex(Simplex, C);
                *Direction = AO;
                Result = false;
            }
        }
    }
    else
    {
        if(DotProduct(AB, ABC) > 0)
        {
            if (DotProduct(AB, AO) > 0)
            {
                RemovePointToSimplex(Simplex, C);
                *Direction = CrossProduct(CrossProduct(AC, AO), AB);
                Result = false;
            }
            else
            {
                RemovePointToSimplex(Simplex, B);
                RemovePointToSimplex(Simplex, C);
                *Direction = AO;
                Result = false;
            }
        }
        else
        {
            if(ABC.Z > 0)
            {
                *Direction = ABC;
            }
            else
            {
                Simplex->Points[1] = C;
                Simplex->Points[0] = B;
                *Direction = (-1.0f * ABC);
            }
        }
    }

    /* First Version
    if(DotProduct(ABPerp, AO) > 0)
    {
        RemovePointToSimplex(Simplex, C);
        *Direction = ABPerp;
        Result = false;
    }
    else if(DotProduct(ACPerp, AO) > 0)
    {
        RemovePointToSimplex(Simplex, B);
        *Direction = ACPerp;
        Result = false;
    }
    */

    return Result;
}

static bool HandleSimplex(simplex* Simplex, v3* Direction)
{
    bool Result = false;

    if (Simplex->PointsCount == 2)
    {
        Result = HandleLine(Simplex, Direction);
    }
    else
    {
        Result = HandleTriangle(Simplex, Direction);
    }

    return Result;
}

static bool Collision(shape* A, shape* B)
{
    bool Result = false;

    v3 Dir = FindCenter(B) - FindCenter(A);
    if(Dir == V3(0, 0, 0)){Dir = V3(1.0f, 0.0f, 0.0f);}
    v3 Support = FindSupport(A, B, Dir);
    simplex* Simplex = CreateNewSimplex(Support);
    Dir = -1.0f*Simplex->Points[0];

    for(;;)
    {
        v3 NewPoint = FindSupport(A, B, Dir);
        if(DotProduct(NewPoint, Dir) < 0) {Result = false; break;}
        AddNewPointToSimplex(Simplex, NewPoint);
        if(HandleSimplex(Simplex, &Dir)) {Result = true; break;}
    }

    return Result;
}

static bool IsPointInTriangle(v3 Point, v3 A, v3 B, v3 C)
{
    bool Result = true;

    v3 AB = B - A;
    v3 BC = C - B;
    v3 CA = A - C;

    v3 AP = Point - A;
    v3 BP = Point - B;
    v3 CP = Point - C;

    float Cross1 = CrossProduct(AB, AP).Z;
    float Cross2 = CrossProduct(BC, BP).Z;
    float Cross3 = CrossProduct(CA, CP).Z;

    if((Cross1 > 0.0f) || (Cross2 > 0.0f) || (Cross3 > 0.0f)) Result = false;

    return Result;
}

int GetListElement(int* IndexList, int ListSize, int PolyIdx)
{
    int Result;

    if(PolyIdx < 0)
    {
        Result = IndexList[PolyIdx % ListSize + ListSize];
    }
    else if(PolyIdx >= ListSize)
    {
        Result = IndexList[PolyIdx % ListSize];
    }
    else
    {
        Result = IndexList[PolyIdx];
    }

    return Result;
}

void RemoveElementFromList(int* IndexList, int* ListSize, int PolyIdx)
{
    for(int i = 0; i < *ListSize; ++i)
    {
        if(IndexList[i] >= PolyIdx)
        {
            IndexList[i] = IndexList[i + 1];
        }
    }
    *ListSize -= 1;
}

#define RemovedElement INT32_MAX
static void PolygonTriangulate(shape* Shape, int** Triangles)
{
    bool Result = false;

    int ListSize = Shape->VerticesCount;
    int IndexListCount = sizeof(int)*ListSize;
    int* IndexList = (int*)malloc(IndexListCount);
    for(int i = 0; i < ListSize; ++i)
    {
        IndexList[i] = i;
    }

    int TotalTriangleCount = Shape->VerticesCount - 2;
    int TotalTriangleCountIdx = TotalTriangleCount*3;

    int* TrianglesResult = (int*)malloc(sizeof(int)*TotalTriangleCountIdx);
    int TriangleIdx = 0;

    while(ListSize > 3)
    {
        for(int PolyIdx = 0; PolyIdx < ListSize; ++PolyIdx)
        {

            int cur  = IndexList[PolyIdx];
            int prev = GetListElement(IndexList, ListSize, PolyIdx - 1);
            int next = GetListElement(IndexList, ListSize, PolyIdx + 1);

            v3 A = Shape->Vertices[cur];
            v3 B = Shape->Vertices[prev];
            v3 C = Shape->Vertices[next];

            v3 AB = B - A;
            v3 AC = C - A;
            
            if(CrossProduct(AB, AC).Z < 0)
            {
                continue;
            }

            bool IsEar = true;

            for(int i = 0; i < Shape->VerticesCount; ++i)
            {
                if((cur == i) || (prev == i) || (next == i))
                {
                    continue;
                }

                v3 Point = Shape->Vertices[i];
                
                if(IsPointInTriangle(Point, B, A, C))
                {
                    IsEar = false;
                    break;
                }
            }

            if(IsEar)
            {
                TrianglesResult[TriangleIdx++] = prev;
                TrianglesResult[TriangleIdx++] = cur;
                TrianglesResult[TriangleIdx++] = next;

                RemoveElementFromList(IndexList, &ListSize, PolyIdx);
            }
        }
    }

    TrianglesResult[TriangleIdx++] = IndexList[0];
    TrianglesResult[TriangleIdx++] = IndexList[1];
    TrianglesResult[TriangleIdx++] = IndexList[2];

    *Triangles = TrianglesResult;
}

int main(int argc, char** argv)
{
    shape* Shape1 = (shape*)malloc(sizeof(shape));
    shape* Shape2 = (shape*)malloc(sizeof(shape));
    shape* Shape3 = (shape*)malloc(sizeof(shape));

    Shape1->VerticesCount = 3;
    Shape1->Vertices = (v3*)malloc(sizeof(v3)*Shape1->VerticesCount);

    Shape1->Vertices[0] = V3(0.00f, -0.64f, 0.0f);
    Shape1->Vertices[1] = V3(3.48f,  2.88f, 0.0f);
    Shape1->Vertices[2] = V3(4.72f, -0.70f, 0.0f);

    Shape2->VerticesCount = 4;
    Shape2->Vertices = (v3*)malloc(sizeof(v3)*Shape2->VerticesCount);

    Shape2->Vertices[0] = V3(-1.48f,  0.60f, 0.0f);
    Shape2->Vertices[1] = V3( 0.24f,  1.22f, 0.0f);
    Shape2->Vertices[2] = V3( 0.86f, -0.94f, 0.0f);
    Shape2->Vertices[3] = V3(-1.50f, -0.72f, 0.0f);

    Shape3->VerticesCount = 9;
    Shape3->Vertices = (v3*)malloc(sizeof(v3)*Shape3->VerticesCount);

    Shape3->Vertices[0] = V3(-4,  6, 0.0f);
    Shape3->Vertices[1] = V3( 0,  2, 0.0f);
    Shape3->Vertices[2] = V3( 2,  5, 0.0f);
    Shape3->Vertices[3] = V3( 7,  0, 0.0f);
    Shape3->Vertices[4] = V3( 5, -6, 0.0f);
    Shape3->Vertices[5] = V3( 3,  3, 0.0f);
    Shape3->Vertices[6] = V3( 0, -5, 0.0f);
    Shape3->Vertices[7] = V3(-6,  0, 0.0f);
    Shape3->Vertices[8] = V3(-2,  1, 0.0f);

    int* Shape3Triangles = 0;
    PolygonTriangulate(Shape3, &Shape3Triangles);

    bool Collided = Collision(Shape1, Shape2);    

    return 0;
}

