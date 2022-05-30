#include <cmath>
#include "EyeLink.h"

EyeLink::EyeLink() {

}

EyeLink::~EyeLink() {

}

bool EyeLink::inverseKinema(double px, double py, double &diffX, double &diffY) {
  // check eye position
  const double a = m_l * m_l - py * py;
  if (a < 0.0) {
    return false;
  }
  const double b = m_l * m_l - px * px;
  if (b < 0.0) {
    return false;
  }
  // calc link position
  const double x = px - m_lx + sqrt(m_l * m_l - py * py);
  const double y = py + m_ly + sqrt(m_l * m_l - px * px);
  // from link original position
  diffX = x - m_x0;
  diffY = y - m_y0;
  return true;
}

bool EyeLink::inverseKinema30(double ex, double ey, double &diffX, double &diffY) {
  const double PI = 3.14159;
  const double eyeOriginX = 3.56; // change this with link length
  const double eyeOriginY = 7.15;
  const double cos30 = cos(PI / 6.0);
  const double sin30 = sin(PI / 6.0);
  const double px = cos30 * ex - sin30 * ey + eyeOriginX;
  const double py = sin30 * ex + cos30 * ey + eyeOriginY;
  return this->inverseKinema(px, py, diffX, diffY);
}
