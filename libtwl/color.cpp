#include "color.h"


namespace twl
{


// Default desktop color
color_t clDesktop        (BxColorFormat_FromRGB(cfA8R8G8B8,  58, 110, 165));
// Window title-bar colors
color_t clActiveWindowL  (BxColorFormat_FromRGB(cfA8R8G8B8,  10,  36, 106));
color_t clActiveWindowR  (BxColorFormat_FromRGB(cfA8R8G8B8, 166, 202, 240));
color_t clActiveWindow   (clActiveWindowL);
color_t clInActiveWindowL(BxColorFormat_FromRGB(cfA8R8G8B8, 128, 128, 128));
color_t clInActiveWindowR(BxColorFormat_FromRGB(cfA8R8G8B8, 192, 192, 192));
color_t clInActiveWindow (clInActiveWindowL);
// Panel colors
color_t clPanelLight     (BxColorFormat_FromRGB(cfA8R8G8B8, 255, 255, 255));
color_t clPanelShade1    (BxColorFormat_FromRGB(cfA8R8G8B8,  64,  64,  64));
color_t clPanelShade2    (BxColorFormat_FromRGB(cfA8R8G8B8, 128, 128, 128));
color_t clPanelFill      (BxColorFormat_FromRGB(cfA8R8G8B8, 212, 208, 200));


};
