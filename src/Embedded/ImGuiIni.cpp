#include "ImGuiIni.hpp"

namespace ShoutBlast
{
    static std::string gData = R"([Window][WindowOverViewport_11111111]
Pos=0,21
Size=800,579
Collapsed=0

[Window][Test]
Size=802,682
Collapsed=0
DockId=0x08BD597D,0

[Window][Debug##Default]
ViewportPos=737,389
ViewportId=0x16723995
Size=817,481
Collapsed=0

[Window][Stations]
Pos=187,118
Size=613,482
Collapsed=0
DockId=0x00000004,0

[Window][Player]
Pos=0,21
Size=800,95
Collapsed=0
DockId=0x00000002,0

[Window][Genres]
Pos=0,118
Size=185,353
Collapsed=0
DockId=0x00000001,0

[Window][Visualizer]
Pos=0,473
Size=185,127
Collapsed=0
DockId=0x00000005,0

[Window][FFT]
ViewportPos=955,207
ViewportId=0x76D70F4E
Size=566,463
Collapsed=0

[Window][Code]
Pos=187,118
Size=613,482
Collapsed=0
DockId=0x00000004,1

[Window][IDE]
Pos=291,108
Size=1309,552
Collapsed=0
DockId=0x08BD597D,1

[Window][Log]
Pos=187,118
Size=613,482
Collapsed=0
DockId=0x00000004,2

[Window][Testing]
ViewportPos=207,91
ViewportId=0x5E649FDA
Pos=525,116
Size=519,366
Collapsed=0
DockId=0x00000004,0

[Window][MainBody]
ViewportPos=0,19
ViewportId=0x579F37DB
Size=800,581
Collapsed=0

[Table][0x76409A2E,4]
RefScale=15
Column 0  Weight=1.1925
Column 1  Width=27
Column 2  Width=75
Column 3  Width=82

[Docking][Data]
DockSpace       ID=0x08BD597D Window=0x1BBC0F80 Pos=50,103 Size=800,579 Split=Y Selected=0x0F1C79E0
  DockNode      ID=0x00000002 Parent=0x08BD597D SizeRef=476,95 CentralNode=1 HiddenTabBar=1 Selected=0x788C46BD
  DockNode      ID=0x00000009 Parent=0x08BD597D SizeRef=476,482 Split=X
    DockNode    ID=0x00000003 Parent=0x00000009 SizeRef=276,370 Split=Y Selected=0xB454DD9E
      DockNode  ID=0x00000001 Parent=0x00000003 SizeRef=185,353 Selected=0xB454DD9E
      DockNode  ID=0x00000005 Parent=0x00000003 SizeRef=185,127 Selected=0x5C1B5396
    DockNode    ID=0x00000004 Parent=0x00000009 SizeRef=913,370 Selected=0x0F1C79E0

)";

    std::string GetImGuiIniData()
    {
        return gData;
    }
}