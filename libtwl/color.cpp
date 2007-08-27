#include "color.h"


namespace twl
{


// Default desktop color
color_t clDesktop(BxRGB(58, 110, 165));
// Window title-bar colors
color_t clActiveWindowL(BxRGB(10, 36, 106));
color_t clActiveWindowR(BxRGB(166, 202, 240));
color_t clActiveWindow(clActiveWindowL);
color_t clInActiveWindowL(BxRGB(128, 128, 128));
color_t clInActiveWindowR(BxRGB(192, 192, 192));
color_t clInActiveWindow(clInActiveWindowL);
// Panel colors
color_t clPanelLight(BxRGB(255, 255, 255));
color_t clPanelShade1(BxRGB(64, 64, 64));
color_t clPanelShade2(BxRGB(128, 128, 128));
color_t clPanelFill(BxRGB(212, 208, 200));


};
