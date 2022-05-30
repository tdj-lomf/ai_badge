#ifndef EYELINK_H_INCLUDE
#define EYELINK_H_INCLUDE

class EyeLink {
public:
  EyeLink();
  virtual ~EyeLink();
  bool inverseKinema(double px, double py, double &diffX, double &diffY);
  bool inverseKinema30(double ex, double ey, double &diffX, double &diffY);
  
private:
  const double m_l = 14.0;
  const double m_x0 = 11.7;
  const double m_y0 = 27.6;
  const double m_lx = 4.0;
  const double m_ly = 7.0;
};

#endif