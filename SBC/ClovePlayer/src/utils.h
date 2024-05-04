#pragma once

struct RGB
{
  unsigned char R;
  unsigned char G;
  unsigned char B;
};

struct HSV
{
  double H;
  double S;
  double V;
};

struct RGB HSVToRGB(struct HSV hsv);

inline float mapToFloat(float x, float a, float b, float c, float d)
{
  float f = (x - a) / (b - a) * (d - c) + c;
  return f;
}


inline double mapToDouble(double x, double a, double b, double c, double d)
{
  double f = (x - a) / (b - a) * (d - c) + c;
  return f;
}
