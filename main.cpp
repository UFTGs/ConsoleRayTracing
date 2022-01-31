#include <iostream>
#include <Windows.h>
#include "Vectors.h"
#include "RayTracing.h"
#include <list>

// Width and height of console screen in char units
#define SW 200
#define SH 50

// Aspect ratio of screen in chars "pixels"
double screenAspect = SW * 1.0 / SH;
// Aspect ratio of single char in console out
double pixelAspect = 8.0 / 16.0;
// Full aspect ratio for correct projection transforms
double aspect = screenAspect * pixelAspect;

// Screen buffer
CHAR_INFO* buffer = new CHAR_INFO[SW * SH];

// Characters sorted by brightness intensity
//char gradient[] = " .:!/r(l1Z4H9W8$@";
char gradient[] = "@$8W9H4Z1l(r/!:. ";

// Global frame counter, some kind of timer
long t = 0;

// Stuff needed for fast console buffer swapping
HANDLE consoleHandle;
COORD consBufSize;
COORD consBufCoord;
SMALL_RECT srctWriteRect;

/// <summary>
/// Sphere object class
/// </summary>
class SphereObj {
public:
    SphereObj(float _r, Vector3 _p, float _ph) : r(_r), p(_p), ph(_ph), color(FOREGROUND_INTENSITY) {}
    SphereObj(float _r, Vector3 _p, float _ph, WORD _col) : r(_r), p(_p), ph(_ph), color(_col) {}
    float r;
    Vector3 p;
    float ph;
    WORD color;
};

// List of all scene objects (scene can contains only spheres ;))
std::list<SphereObj> objects = std::list<SphereObj>();

// The global light source position
Vector3 light = Vector3(sin(t / 10.0) * 2, 0, -cos(t / 10.0) * 2);

/// <summary>
/// The clamp function, why not :D
/// </summary>
/// <param name="value">The value to clamp</param>
/// <param name="min">The minimal value bound</param>
/// <param name="max">The maximal value bound</param>
int clamp(int value, int min, int max)
{
    return min(max, max(min, value));
}

/// <summary>
/// Function directly sets the screen buffer to console out buffer
/// </summary>
void FlushBuffer()
{
    WriteConsoleOutputA(consoleHandle, buffer, consBufSize, consBufCoord, &srctWriteRect);
}

/// <summary>
/// Fills the screen buffer by specified character
/// </summary>
/// <param name="c">Character to fill</param>
void FillScreen(char c)
{
    for (int i = 0; i < SW * SH; i++)
    {
        CHAR_INFO charInfo;
        charInfo.Char.AsciiChar = c;
        charInfo.Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
        buffer[i] = charInfo;
    }
}

/// <summary>
/// Recursively checks intersections between ray and all scene objects
/// </summary>
/// <param name="ro">Ray casting start point</param>
/// <param name="rd">Ray direction</param>
/// <param name="lvl">Current recursivity level</param>
CHAR_INFO TraceRay(Vector3 ro, Vector3 rd, int lvl)
{
    // Intersection result
    SphereObj intersObj = SphereObj(0, 0, 0);
    float inters = -1;

    // Iterate all scene objects
    std::list<SphereObj>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it)
    {
        // Try intersect object
        float intersection = RISphere(ro, rd, it->p, it->r);

        if (intersection > 0)
        {
            // Select only nearest object intersction
            if ((inters > 0 && intersection < inters) || (inters < 0))
            {
                inters = intersection;
                intersObj = *it;
            }
        }
    }

    CHAR_INFO charInfo;

    // Checks intersection with the light source bulb
    float li = RISphere(ro, rd, light, 0.2);
    if ((li > 0 && li < inters) || (li > 0 && inters < 0))
    {
        charInfo.Char.AsciiChar = gradient[16];
        charInfo.Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
        return charInfo;
    }

    if (inters > 0) // Ray has intersection
    {
        // Intersection point
        Vector3 itPoint = ro + rd * inters;
        // Normal to surface at intersection point
        Vector3 n = (itPoint - intersObj.p).Norm();

        // Diffuse component of object surface
        float diff = fmaxf(0, n * (light - itPoint).Norm());

        // Color by Phong model
        char color = 16 * (0.0 + diff);

        // Select character from intensity gradient
        charInfo.Char.AsciiChar = gradient[clamp(color, 0, 16)];

        if (lvl < 4) // If ray has less 4 reflects call recursy
        {
            CHAR_INFO ci = TraceRay(itPoint, (rd - n * 2 * (n * rd)).Norm(), lvl + 1);
            if (ci.Char.AsciiChar != '\0')
            {
                // If recursive call return intersect, changes the color of the surface by a reflected object color
                charInfo.Attributes = ci.Attributes | (intersObj.color);
                return charInfo;
            }
        }

        // No reflectionm, set own color
        charInfo.Attributes = intersObj.color;
        if (clamp(color, 0, 16) > 6)
            charInfo.Attributes |= BACKGROUND_INTENSITY;
        return charInfo;
    }

    // No intersections
    charInfo.Char.AsciiChar = '\0';
    return charInfo;
}

/// <summary>
/// Updates scene objects positons
/// </summary>
void UpdateObjects()
{
    std::list<SphereObj>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it)
    {
        it->p = Vector3(cos(t / 30.0 + it->ph) * 2.2, sin(t / 25.0 + it->ph) * 2, -1 + sin(t / 50.0 + it->ph) * 2);
    }
    light = Vector3(sin(t / 10.0) * 1, sin(t/33.0+6)*1, -cos(t / 15.0) * 1);
}

/// <summary>
/// Initiates ray tracing for each "pixel" of the screen
/// </summary>
void ScreenRayTracing()
{
    for (int i = 0; i < SW; ++i)
    {
        for (int j = 0; j < SH; ++j)
        {
            // UV coordinates of pixel on the screen
            // screen bounded range (-1;1) by X and Y axis
            // point (0,0) places on center of the screen
            //             y 1
            //      -----------------
            //      |       |       |
            // x -1 |------0,0------| x 1
            //      |       |       |
            //      -----------------
            //             y -1
            Vector2 uv = Vector2(i * 1.0 / SW, 1.0 - j * 1.0 / SH) * 2.0 - 1.0;
            uv.x *= aspect;

            // Ray casting point (camera position)
            Vector3 ro = Vector3(0, 0, 4);
            // Ray direction. Z-coord of vector controls the distance between ray casting point and projection plane, some kind of FOV
            Vector3 rd = Vector3(uv, -1.5).Norm();

            // Do trace ray
            CHAR_INFO ci = TraceRay(ro, rd, 0);
            if (ci.Char.AsciiChar != '\0')
            {
                // If the ray has intersected an object sets "pixel" by intersection result
                buffer[j * SW + i] = ci;
            }
        }
    }
}

int main()
{
    //setlocale(LC_ALL, "Russian");
    // Getting console handler for fast buffer swapping
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    consBufSize.X = SW;
    consBufSize.Y = SH;
    consBufCoord.X = 0;
    consBufCoord.Y = 0;
    srctWriteRect.Top = 0;
    srctWriteRect.Left = 0;
    srctWriteRect.Bottom = SH - 1;
    srctWriteRect.Right = SW - 1;

    // Adding some objects to scene
    objects.push_back(SphereObj(0.8, Vector3(0), 0, BACKGROUND_RED));
    objects.push_back(SphereObj(0.5, Vector3(0), 10, BACKGROUND_GREEN));
    objects.push_back(SphereObj(1.5, Vector3(0), 20, BACKGROUND_BLUE));

    while (true)
    {
        FillScreen(' ');

        ScreenRayTracing();

        FlushBuffer();

        UpdateObjects();

        Sleep(20);

        ++t;
    }
}