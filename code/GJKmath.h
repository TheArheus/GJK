
union v3
{
    struct
    {
        float X;
        float Y;
        float Z;
    };
    float E[3];
};

inline v3 V3(float X, float Y, float Z)
{
    v3 Result;

    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;

    return Result;
}

inline v3 operator+(v3 A, v3 B)
{
    v3 Result;

    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;

    return Result;
}

inline v3 operator-(v3 A, v3 B)
{
    v3 Result;

    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;

    return Result;
}

inline v3 operator-(v3 A, float B)
{
    v3 Result;

    Result.X = A.X - B;
    Result.Y = A.Y - B;
    Result.Z = A.Z - B;

    return Result;
}

inline v3 operator-(float A, v3 B)
{
    v3 Result;

    Result.X = B.X - A;
    Result.Y = B.Y - A;
    Result.Z = B.Z - A;

    return Result;
}

inline v3 operator*(v3 A, v3 B)
{
    v3 Result;

    Result.X = A.X * B.X;
    Result.Y = A.Y * B.Y;
    Result.Z = A.Z * B.Z;

    return Result;
}

inline v3 operator*(v3 A, float B)
{
    v3 Result;

    Result.X = A.X * B;
    Result.Y = A.Y * B;
    Result.Z = A.Z * B;

    return Result;
}

inline v3 operator*(float A, v3 B)
{
    v3 Result;

    Result.X = A * B.X;
    Result.Y = A * B.Y;
    Result.Z = A * B.Z;

    return Result;
}

inline v3 operator/(v3 A, v3 B)
{
    v3 Result;

    Result.X = A.X / B.X;
    Result.Y = A.Y / B.Y;
    Result.Z = A.Z / B.Z;

    return Result;
}

inline v3 operator/(v3 A, float B)
{
    v3 Result;

    Result.X = A.X / B;
    Result.Y = A.Y / B;
    Result.Z = A.Z / B;

    return Result;
}

inline bool operator==(v3 A, v3 B)
{
    bool Result = false;
    if((A.X == B.X) && (A.Y == B.Y) && (A.Z == B.Z)) Result = true;
    return Result;
}

inline float
DotProduct(v3 A, v3 B)
{
    float Result = A.X*B.X + A.Y*B.Y + A.Z*B.Z;
    return Result;
}

inline v3 CrossProduct(v3 A, v3 B)
{
    v3 Result;

    Result.X = (A.Y*B.Z - A.Z*B.Y);
    Result.Y = (A.Z*B.X - A.X*B.Z);
    Result.Z = (A.X*B.Y - A.Y*B.X);

    return Result;
}
