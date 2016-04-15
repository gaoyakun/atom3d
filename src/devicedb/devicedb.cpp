#include "devicedb.h"
#include "displayinfo.h"

static ATOM_DeviceDB::Device defaultDeviceLib[] = {
	// NVidia
	{ 0x10DE, 0x0020, "VNDIA RIVA TNT", 0 }, // NV04
	{ 0x10DE, 0x0028, "VNDIA RIVA TNT2/TNT2 PRO", 0 }, // NV05
	{ 0x10DE, 0x0029, "VNDIA RIVA TNT2 Ultra", 0 }, // NV05
	{ 0x10DE, 0x002A, "TNT2", 0 }, // NV05
	{ 0x10DE, 0x002B, "TNT2", 0 }, // NV05
	{ 0x10DE, 0x002C, "Nvidia Vanta/Vanta LT", 0 }, // NV05
	{ 0x10DE, 0x002D, "NVIDIA RIVA TNT2 Model 64/Model 64 Pro", 0 }, // NV05
	{ 0x10DE, 0x002E, "VANTA", 0 }, // NV6
	{ 0x10DE, 0x002F, "VANTA", 0 }, // NV6
	{ 0x10DE, 0x0040, "GeForce 6800 Ultra", 2 },
	{ 0x10DE, 0x0041, "GeForce 6800", 2 },
	{ 0x10DE, 0x0042, "GeForce 6800 LE", 2 },
	{ 0x10DE, 0x0043, "GeForce 6800 XE", 2 },
	{ 0x10DE, 0x0044, "GeForce 6800 XT", 2 },
	{ 0x10DE, 0x0045, "GeForce 6800 GT", 2 },
	{ 0x10DE, 0x0046, "GeForce 6800 GT", 2 },
	{ 0x10DE, 0x0047, "GeForce 6800 GS", 2 },
	{ 0x10DE, 0x0048, "GeForce 6800 XT", 2 },
	{ 0x10DE, 0x004E, "Quadro FX 4000", 2 },
	{ 0x10DE, 0x00C0, "GeForce 6800 GS", 2 },
	{ 0x10DE, 0x00C1, "GeForce 6800", 2 },
	{ 0x10DE, 0x00C2, "GeForce 6800 LE", 2 },
	{ 0x10DE, 0x00C3, "GeForce 6800 XT", 2 },
	{ 0x10DE, 0x00CD, "Quadro FX 3450/4000 SDI", 2 },
	{ 0x10DE, 0x00CE, "Quadro FX 1400", 1 },
	{ 0x10DE, 0x00F1, "GeForce 6600 GT", 1 },
	{ 0x10DE, 0x00F2, "GeForce 6600", 1 },
	{ 0x10DE, 0x00F3, "GeForce 6200", 0 },
	{ 0x10DE, 0x00F4, "GeForce 6600 LE", 1 },
	{ 0x10DE, 0x00F5, "GeForce 7800 GS", 1 },
	{ 0x10DE, 0x00F6, "GeForce 6800 GS/XT", 1 },
	{ 0x10DE, 0x00F8, "Quadro FX 3400/4400", 1 },
	{ 0x10DE, 0x00F9, "GeForce 6800 Series GPU", 1 },
	{ 0x10DE, 0x00FA, "GeForce PCX 5750", 1 },
	{ 0x10DE, 0x00FB, "GeForce PCX 5900", 1 },
	{ 0x10DE, 0x00FC, "GeForce PCX 5300", 1 },
	{ 0x10DE, 0x00FD, "Quadro PCI-E Series", 1 },
	{ 0x10DE, 0x00FE, "Quadro FX 1300", 1 },
	{ 0x10DE, 0x00FF, "GeForce PCX 4300", 1 },
	{ 0x10DE, 0x0090, "GeForce 7800 GTX", 2 },
	{ 0x10DE, 0x0091, "GeForce 7800 GTX ", 2 },
	{ 0x10DE, 0x0092, "GeForce 7800 GT", 2 },
	{ 0x10DE, 0x0093, "GeForce 7800 GS", 2 },
	{ 0x10DE, 0x0095, "GeForce 7800 SLI", 2 },
	{ 0x10DE, 0x009D, "Quadro FX 4500", 1 },
	{ 0x10DE, 0x0100, "GeForce 256", 0 }, // NV10
	{ 0x10DE, 0x0101, "GeForce DDR", 0 }, // NV10
	{ 0x10DE, 0x0102, "GeForce 256 Ultra", 0 }, // NV10
	{ 0x10DE, 0x0103, "Quadro", 0 }, // NV10
	{ 0x10DE, 0x0110, "GeForce2 MX/MX 400", 0 }, // NV11
	{ 0x10DE, 0x0111, "GeForce2 MX 100/200", 0 }, // NV11
	{ 0x10DE, 0x0112, "GeForce2 Go", 0 }, // NV11
	{ 0x10DE, 0x0113, "Quadro 2 MXR/EX", 0 }, // NV11
	{ 0x10DE, 0x0140, "GeForce 6600 GT", 1 },
	{ 0x10DE, 0x0141, "GeForce 6600", 1 },
	{ 0x10DE, 0x0142, "GeForce 6600 LE", 1 },
	{ 0x10DE, 0x0143, "GeForce 6600 VE", 1 },
	{ 0x10DE, 0x0144, "NVIDIA GeForce Go 6600", 1 },
	{ 0x10DE, 0x0145, "GeForce 6610 XL", 1 },
	{ 0x10DE, 0x0146, "NVIDIA GeForce Go 6200 TE/6600 TE", 0 },
	{ 0x10DE, 0x0147, "GeForce 6700 XL", 1 },
	{ 0x10DE, 0x0148, "NVIDIA GeForce Go 6600", 1 },
	{ 0x10DE, 0x0149, "NVIDIA GeForce Go 6600 GT", 1 },
	{ 0x10DE, 0x014A, "Quadro NVS 440", 0 },
	{ 0x10DE, 0x014C, "Quadro FX 540M", 1 },
	{ 0x10DE, 0x014D, "Quadro FX 550", 1 },
	{ 0x10DE, 0x014E, "Quadro FX 540", 1 },
	{ 0x10DE, 0x014F, "GeForce 6200 ", 0 },
	{ 0x10DE, 0x0150, "NVIDIA GeForce2 GTS/GeForce2 Pro", 0 },
	{ 0x10DE, 0x0151, "NVIDIA GeForce2 Ti", 0 },
	{ 0x10DE, 0x0152, "NVIDIA GeForce2 Ultra", 0 },
	{ 0x10DE, 0x0153, "NVIDIA Quadro2 Pro", 0 },
	{ 0x10DE, 0x0160, "GeForce 6500", 0 },
	{ 0x10DE, 0x0161, "GeForce 6200 TurboCache(TM)", 0 },
	{ 0x10DE, 0x0162, "GeForce 6200SE TurboCache(TM)", 0 },
	{ 0x10DE, 0x0163, "GeForce 6200 LE", 0 },
	{ 0x10DE, 0x0164, "NVIDIA NV44", 0 },
	{ 0x10DE, 0x0165, "Quadro NVS 285", 0 },
	{ 0x10DE, 0x0166, "NVIDIA GeForce Go 6250", 0 },
	{ 0x10DE, 0x0167, "NVIDIA GeForce Go 6200", 0 },
	{ 0x10DE, 0x0168, "NVIDIA GeForce Go 6400", 0 },
	{ 0x10DE, 0x0169, "GeForce 6250", 0 },
	{ 0x10DE, 0x016A, "GeForce 7100 GS", 0 },
	{ 0x10DE, 0x016B, "NVIDIA NV44GLM", 0 },
	{ 0x10DE, 0x016C, "NVIDIA NV44GLM", 0 },
	{ 0x10DE, 0x016D, "NVIDIA NV44GLM", 0 },
	{ 0x10DE, 0x016E, "NVIDIA NV44GL", 0 },
	{ 0x10DE, 0x0170, "NVIDIA GeForce4 MX 460", 0 },
	{ 0x10DE, 0x0171, "NVIDIA GeForce4 MX 440", 0 },
	{ 0x10DE, 0x0172, "NVIDIA GeForce4 MX 420", 0 },
	{ 0x10DE, 0x0173, "NVIDIA GeForce4 MX 440 SE", 0 },
	{ 0x10DE, 0x0174, "NVIDIA GeForce4 MX 440 Go", 0 },
	{ 0x10DE, 0x0175, "NVIDIA GeForce4 MX 420 Go", 0 },
	{ 0x10DE, 0x0176, "NVIDIA GeForce4 MX 420 Go 32M", 0 },
	{ 0x10DE, 0x0177, "NVIDIA GeForce4 MX 460 Go", 0 },
	{ 0x10DE, 0x0178, "NVIDIA Quadro4 550 XGL", 0 },
	{ 0x10DE, 0x0179, "NVIDIA GeForce4 MX 440 Go 64M", 0 },
	{ 0x10DE, 0x017A, "NVIDIA Quadro NVS", 0 },
	{ 0x10DE, 0x017B, "Quadro4 550 XGL", 0 },
	{ 0x10DE, 0x017C, "NVIDIA Quadro4 500 Go GL", 0 },
	{ 0x10DE, 0x017D, "NVIDIA GeForce4 410 Go 16M", 0 },
	{ 0x10DE, 0x0181, "NVIDIA GeForce4 MX 440 with AGP8X", 0 },
	{ 0x10DE, 0x0182, "VIDIA GeForce4 MX 440SE with AGP8X", 0 },
	{ 0x10DE, 0x0183, "NVIDIA GeForce4 MX 420 with AGP8X", 0 },
	{ 0x10DE, 0x0185, "NVIDIA GeForce4 MX 4000", 0 },
	{ 0x10DE, 0x0186, "NVIDIA GeForce4 448 Go", 0 },
	{ 0x10DE, 0x0187, "NVIDIA GeForce4 488 Go", 0 },
	{ 0x10DE, 0x0188, "NVIDIA Quadro4 580 XGL", 0 },
	{ 0x10DE, 0x018A, "NVIDIA Quadro NVS with AGP8X", 0 },
	{ 0x10DE, 0x018B, "NVIDIA Quadro4 380 XGL", 0 },
	{ 0x10DE, 0x018C, "NVIDIA Quadro NVS 50 PCI", 0 },
	{ 0x10DE, 0x018D, "NVIDIA GeForce4 448 Go", 0 },
	{ 0x10DE, 0x0191, "GeForce 8800 GTX", 2 },
	{ 0x10DE, 0x0193, "GeForce 8800 GTS", 2 },
	{ 0x10DE, 0x0194, "GeForce 8800 Ultra", 2 },
	{ 0x10DE, 0x0197, "Tesla C870", 2 },
	{ 0x10DE, 0x019D, "Quadro FX 5600", 2 },
	{ 0x10DE, 0x019E, "Quadro FX 4600", 2 },
	{ 0x10DE, 0x01D0, "GeForce 7350 LE", 1 },
	{ 0x10DE, 0x01D1, "GeForce 7300 LE", 1 },
	{ 0x10DE, 0x01D2, "GeForce 7550 SE", 1 },
	{ 0x10DE, 0x01D3, "GeForce 7300 SE/7200 GS", 1 },
	{ 0x10DE, 0x01D5, "GeForce 7300 LE", 1 },
	{ 0x10DE, 0x01D7, "GeForce Go 7300", 1 },
	{ 0x10DE, 0x01D8, "GeForce Go 7400", 1 },
	{ 0x10DE, 0x01DB, "Quadro NVS 120M", 2 },
	{ 0x10DE, 0x01DC, "NVIDIA Quadro FX 350M", 2 },
	{ 0x10DE, 0x01DD, "GeForce 7500 LE", 2 },
	{ 0x10DE, 0x01DE, "Quadro FX 350", 1 },
	{ 0x10DE, 0x01DF, "GeForce 7300 GS", 0 },
	{ 0x10DE, 0x0200, "GeForce3", 0 },
	{ 0x10DE, 0x0201, "GeForce3 Ti 200", 0 },
	{ 0x10DE, 0x0202, "GeForce3 Ti 500", 0 },
	{ 0x10DE, 0x0203, "Quadro DCC", 0 },
	{ 0x10DE, 0x0210, "NV48", 2 },
	{ 0x10DE, 0x0211, "GeForce 6800", 2 },
	{ 0x10DE, 0x0212, "GeForce 6800 LE", 2 },
	{ 0x10DE, 0x0215, "GeForce 6800 GT", 2 },
	{ 0x10DE, 0x0218, "GeForce 6800 XT", 2 },
	{ 0x10DE, 0x0220, "NV44", 0 },
	{ 0x10DE, 0x0221, "GeForce 6200", 0 },
	{ 0x10DE, 0x0222, "GeForce 6200 A-LE", 0 },
	{ 0x10DE, 0x0228, "NV44M", 0 },
	{ 0x10DE, 0x0240, "GeForce 6150", 0 },
	{ 0x10DE, 0x0241, "GeForce 6150 LE", 0 },
	{ 0x10DE, 0x0242, "GeForce 6100", 0 },
	{ 0x10DE, 0x0244, "GeForce Go 6150", 0 },
	{ 0x10DE, 0x0245, "Quadro NVS 210S / NVIDIA GeForce 6150LE", 0 },
	{ 0x10DE, 0x0247, "Geforce 6100 Go", 0 },
	{ 0x10DE, 0x0250, "Geforce 4 Ti 4600", 0 },
	{ 0x10DE, 0x0251, "Geforce 4 Ti 4400", 0 },
	{ 0x10DE, 0x0252, "Geforce 4 Ti", 0 },
	{ 0x10DE, 0x0253, "Geforce 4 Ti 4200", 0 },
	{ 0x10DE, 0x0258, "Quadro4 900 XGL", 0 },
	{ 0x10DE, 0x0259, "Quadro4 750 XGL", 0 },
	{ 0x10DE, 0x025B, "Quadro4 700 XGL", 0 },
	{ 0x10DE, 0x0280, "GeForce4 Ti 4800", 0 },
	{ 0x10DE, 0x0281, "GeForce4 Ti 4200 With AGP 8X", 0 },
	{ 0x10DE, 0x0282, "GeForce4 Ti 4800 SE", 0 },
	{ 0x10DE, 0x0286, "GeForce4 4200 Go", 0 },
	{ 0x10DE, 0x0288, "Quadro4 980 XGL", 0 },
	{ 0x10DE, 0x0289, "Quadro4 780 XGL", 0 },
	{ 0x10DE, 0x028C, "Quadro4 700 Go GL", 0 },
	{ 0x10DE, 0x0290, "GeForce 7900 GTX", 2 },
	{ 0x10DE, 0x0291, "GeForce 7900 GT/GTO", 2 },
	{ 0x10DE, 0x0292, "GeForce 7900 GS ", 2 },
	{ 0x10DE, 0x0293, "GeForce 7950 GX2", 2 },
	{ 0x10DE, 0x0294, "GeForce 7950 GX2", 2 },
	{ 0x10DE, 0x0295, "GeForce 7950 GT", 2 },
	{ 0x10DE, 0x0297, "GeForce Go 7950 GTX", 2 },
	{ 0x10DE, 0x0298, "GeForce Go 7900 GS", 2 },
	{ 0x10DE, 0x0299, "GeForce Go 7900 GTX", 2 },
	{ 0x10DE, 0x029C, "Quadro FX 5500", 2 },
	{ 0x10DE, 0x029D, "Quadro FX 3500", 2 },
	{ 0x10DE, 0x029E, "Quadro FX 1500", 2 },
	{ 0x10DE, 0x029F, "Quadro FX 4500 X2", 2 },
	{ 0x10DE, 0x02E0, "GeForce 7600 GT", 2 },
	{ 0x10DE, 0x02E1, "GeForce 7600 GS", 1 },
	{ 0x10DE, 0x02E2, "GeForce 7300 GT", 2 },
	{ 0x10DE, 0x02E3, "GeForce 7900 GS", 2 },
	{ 0x10DE, 0x02E4, "GeForce 7950 GT", 2 },
	{ 0x10DE, 0x0300, "NV30", 1 },
	{ 0x10DE, 0x0301, "GeForce FX 5800 Ultra", 1 },
	{ 0x10DE, 0x0302, "GeForce FX 5800", 1 },
	{ 0x10DE, 0x0308, "Quadro FX 2000", 1 },
	{ 0x10DE, 0x0309, "Quadro FX 1000", 1 },
	{ 0x10DE, 0x030A, "ICE FX 2000", 1 },
	{ 0x10DE, 0x0311, "GeForce FX 5600 Ultra", 1 },
	{ 0x10DE, 0x0312, "GeForce FX 5600", 1 },
	{ 0x10DE, 0x0313, "NV31", 1 },
	{ 0x10DE, 0x0314, "GeForce FX 5600XT", 1 },
	{ 0x10DE, 0x0316, "NV31M", 1 },
	{ 0x10DE, 0x0317, "NV31M Pro", 1 },
	{ 0x10DE, 0x0318, "NV31 GL", 1 },
	{ 0x10DE, 0x0319, "NV31 GL", 1 },
	{ 0x10DE, 0x031A, "GeForce FX Go 5600", 1 },
	{ 0x10DE, 0x031B, "GeForce FX Go 5650", 1 },
	{ 0x10DE, 0x031C, "Quadro FX Go 700", 1 },
	{ 0x10DE, 0x031D, "NV31GLM", 1 },
	{ 0x10DE, 0x031E, "NV31GLM Pro", 1 },
	{ 0x10DE, 0x031F, "NV31GLM Pro", 1 },
	{ 0x10DE, 0x0320, "GeForce FX 5200", 0 },
	{ 0x10DE, 0x0321, "GeForce FX 5200 Ultra", 0 },
	{ 0x10DE, 0x0322, "GeForce FX 5200", 0 },
	{ 0x10DE, 0x0323, "GeForce FX 5200 LE", 0 },
	{ 0x10DE, 0x0324, "GeForce FX Go 5200", 0 },
	{ 0x10DE, 0x0325, "GeForce FX Go 5250/5500", 0 },
	{ 0x10DE, 0x0326, "GeForce FX 5500", 0 },
	{ 0x10DE, 0x0327, "GeForce FX 5100", 0 },
	{ 0x10DE, 0x0328, "GeForce FX Go 5200 32/64M", 0 },
	{ 0x10DE, 0x0329, "NV34MAP", 0 },
	{ 0x10DE, 0x032A, "Quadro NVS 55/280 PCI", 0 },
	{ 0x10DE, 0x032B, "Quadro FX 500/FX 600", 0 },
	{ 0x10DE, 0x032C, "GeForce FX Go 53x0", 0 },
	{ 0x10DE, 0x032D, "NVIDIA GeForce FX Go 5100", 0 },
	{ 0x10DE, 0x032F, "NV34GL", 0 },
	{ 0x10DE, 0x0330, "GeForce FX 5900 Ultra", 1 },
	{ 0x10DE, 0x0331, "GeForce FX 5900", 1 },
	{ 0x10DE, 0x0332, "GeForce FX 5900 XT", 1 },
	{ 0x10DE, 0x0333, "GeForce FX 5950 Ultra", 1 },
	{ 0x10DE, 0x0334, "GeForce FX 5900 ZT", 1 },
	{ 0x10DE, 0x0338, "Quadro FX 3000", 1 },
	{ 0x10DE, 0x033F, "Quadro FX 700", 1 },
	{ 0x10DE, 0x0341, "GeForce FX 5700 Ultra", 1 },
	{ 0x10DE, 0x0342, "GeForce FX 5700", 1 },
	{ 0x10DE, 0x0343, "GeForce FX 5700 LE", 1 },
	{ 0x10DE, 0x0344, "GeForce FX 5700 VE", 1 },
	{ 0x10DE, 0x0345, "NV36", 1 },
	{ 0x10DE, 0x0347, "GeForce FX Go 5700", 1 },
	{ 0x10DE, 0x0348, "GeForce FX Go 5700", 1 },
	{ 0x10DE, 0x0349, "NV36M Pro", 1 },
	{ 0x10DE, 0x034B, "NV36MAP", 1 },
	{ 0x10DE, 0x034C, "Quadro FX Go 1000", 1 },
	{ 0x10DE, 0x034E, "Quadro FX 1100", 1 },
	{ 0x10DE, 0x034F, "NV36GL", 1 },
	{ 0x10DE, 0x038B, "GeForce 7650 GS", 1 },
	{ 0x10DE, 0x0390, "GeForce 7650 GS", 1 },
	{ 0x10DE, 0x0391, "GeForce 7600 GT ", 2 },
	{ 0x10DE, 0x0392, "GeForce 7600 GS ", 1 },
	{ 0x10DE, 0x0393, "GeForce 7300 GT ", 2 },
	{ 0x10DE, 0x0394, "GeForce 7600 LE", 2 },
	{ 0x10DE, 0x0395, "GeForce 7300 GT", 2 },
	{ 0x10DE, 0x0398, "GeForce Go 7600", 2 },
	{ 0x10DE, 0x039E, "Quadro FX 560", 1 },
	{ 0x10DE, 0x03D0, "GeForce 6150SE nForce 430", 0 },
	{ 0x10DE, 0x03D1, "GeForce 6100 nForce 405", 0 },
	{ 0x10DE, 0x03D2, "GeForce 6100 nForce 400", 0 },
	{ 0x10DE, 0x03D5, "GeForce 6100 nForce 420", 0 },
	{ 0x10DE, 0x0400, "GeForce 8600 GTS", 2 },
	{ 0x10DE, 0x0401, "GeForce 8600 GT", 2 },
	{ 0x10DE, 0x0402, "GeForce 8600 GT ", 2 },
	{ 0x10DE, 0x0403, "GeForce 8600GS", 2 },
	{ 0x10DE, 0x0404, "GeForce 8400 GS", 2 },
	{ 0x10DE, 0x0405, "GeForce 9500m GS", 2 },
	{ 0x10DE, 0x0406, "GeForce 8300 GS", 2 },
	{ 0x10DE, 0x0407, "GeForce 8600M GT", 2 },
	{ 0x10DE, 0x040A, "Quadro FX 370", 0 },
	{ 0x10DE, 0x040C, "Quadro NVS 570M", 0 },
	{ 0x10DE, 0x040E, "Quadro FX 570", 0 },
	{ 0x10DE, 0x040F, "Quadro FX 1700", 1 },
	{ 0x10DE, 0x0420, "GeForce 8400 SE", 2 },
	{ 0x10DE, 0x0421, "GeForce 8500 GT", 2 },
	{ 0x10DE, 0x0422, "GeForce 8400 GS", 2 },
	{ 0x10DE, 0x0423, "GeForce 8300 GS", 2 },
	{ 0x10DE, 0x0424, "GeForce 8400 GS", 2 },
	{ 0x10DE, 0x0426, "Geforce 8400M GT GPU", 2 },
	{ 0x10DE, 0x0427, "Geforce 8400M GS", 2 },
	{ 0x10DE, 0x0428, "GeForce 8400M G", 2 },
	{ 0x10DE, 0x0429, "Quadro NVS 135/140M", 2 },
	{ 0x10DE, 0x042C, "Quadro NVS 135M", 2 },
	{ 0x10DE, 0x042D, "Quadro FX 360 M", 2 },
	{ 0x10DE, 0x042F, "Quadro NVS 290", 0 },
	{ 0x10DE, 0x053A, "GeForce 7050 PV / NVIDIA nForce 630a", 1 },
	{ 0x10DE, 0x053B, "GeForce 7050 PV / NVIDIA nForce 630a", 1 },
	{ 0x10DE, 0x053E, "GeForce 7025 / NVIDIA nForce 630a", 1 },
	{ 0x10DE, 0x05E0, "GeForce GTX 295", 2 },
	{ 0x10DE, 0x05E1, "GeForce GTX 280", 2 },
	{ 0x10DE, 0x05E2, "GeForce GTX 260", 2 },
	{ 0x10DE, 0x05E3, "GeForce GTX 285", 2 },
	{ 0x10DE, 0x05E7, "Tesla C1060", 2 },
	{ 0x10DE, 0x05F8, "Quadroplex 2200 S4", 2 },
	{ 0x10DE, 0x05F9, "Quadro CX", 2 },
	{ 0x10DE, 0x05FD, "Quadro FX 5800", 2 },
	{ 0x10DE, 0x05FE, "Quadro FX 4800", 2 },
	{ 0x10DE, 0x0600, "GeForce 8800 GTS 512", 2 },
	{ 0x10DE, 0x0601, "GeForce 9800 GT", 2 },
	{ 0x10DE, 0x0602, "GeForce 8800 GT", 2 },
	{ 0x10DE, 0x0604, "GeForce 9800 GX2", 2 },
	{ 0x10DE, 0x0605, "GeForce 9800 GT", 2 },
	{ 0x10DE, 0x0606, "GeForce 8800 GS", 2 },
	{ 0x10DE, 0x060B, "GeForce 9800M GT", 2 },
	{ 0x10DE, 0x060D, "GeForce 8800 GS", 2 },
	{ 0x10DE, 0x0610, "GeForce 9600 GSO", 2 },
	{ 0x10DE, 0x0611, "GeForce 8800 GT", 2 },
	{ 0x10DE, 0x0612, "GeForce 9800 GTX/9800 GTX+", 2 },
	{ 0x10DE, 0x0613, "GeForce 9800 GTX+", 2 },
	{ 0x10DE, 0x0614, "GeForce 9800 GT", 2 },
	{ 0x10DE, 0x0615, "GeForce GTS 250", 2 },
	{ 0x10DE, 0x0619, "Quadro FX 4700 X2", 2 },
	{ 0x10DE, 0x061A, "Quadro FX 3700", 2 },
	{ 0x10DE, 0x061B, "Quadro VX 200", 2 },
	{ 0x10DE, 0x0622, "GeForce 9600 GT", 2 },
	{ 0x10DE, 0x0623, "GeForce 9600 GS", 2 },
	{ 0x10DE, 0x0625, "GeForce 9600 GSO 512", 2 },
	{ 0x10DE, 0x062D, "GeForce 9600 GT", 2 },
	{ 0x10DE, 0x062E, "GeForce 9600 GT", 2 },
	{ 0x10DE, 0x0637, "GeForce 9600 GT", 2 },
	{ 0x10DE, 0x0638, "Quadro FX 1800", 2 },
	{ 0x10DE, 0x0640, "GeForce 9500 GT", 2 },
	{ 0x10DE, 0x0641, "GeForce 9400 GT", 2 },
	{ 0x10DE, 0x0642, "GeForce 8400 GS", 2 },
	{ 0x10DE, 0x0643, "GeForce 9500 GT", 2 },
	{ 0x10DE, 0x0644, "GeForce 9500 GS", 2 },
	{ 0x10DE, 0x0645, "GeForce 9500 GS", 2 },
	{ 0x10DE, 0x0646, "GeForce 9500 GS", 2 },
	{ 0x10DE, 0x0648, "GeForce 9600M GS", 2 },
	{ 0x10DE, 0x0649, "GeForce 9600M GT", 2 },
	{ 0x10DE, 0x064A, "GeForce 9700M GT", 2 },
	{ 0x10DE, 0x0652, "GeForce GT 130M", 2 },
	{ 0x10DE, 0x065C, "Quadro FX 770M", 2 },
	{ 0x10DE, 0x06E0, "GeForce 9300 GE", 2 },
	{ 0x10DE, 0x06E1, "GeForce 9300 GS", 2 },
	{ 0x10DE, 0x06E2, "GeForce 8400", 2 },
	{ 0x10DE, 0x06E3, "GeForce 8400 SE", 2 },
	{ 0x10DE, 0x06E4, "GeForce 8400 GS", 2 },
	{ 0x10DE, 0x06E6, "G100", 2 },
	{ 0x10DE, 0x06E7, "GeForce 9300 SE", 2 },
	{ 0x10DE, 0x06E9, "GeForce 9300M GS", 2 },
	{ 0x10DE, 0x06EA, "Quadro NVS 150M", 2 },
	{ 0x10DE, 0x06EB, "Quadro NVS 160M", 2 },
	{ 0x10DE, 0x06F8, "Quadro NVS 420", 0 },
	{ 0x10DE, 0x06F9, "Quadro FX 370 LP", 0 },
	{ 0x10DE, 0x06FA, "Quadro NVS 450", 0 },
	{ 0x10DE, 0x06FD, "NVS 295", 0 },
	{ 0x10DE, 0x07E0, "GeForce 7150 / NVIDIA nForce 630i", 0 },
	{ 0x10DE, 0x07E1, "GeForce 7100 / NVIDIA nForce 630i", 0 },
	{ 0x10DE, 0x07E2, "GeForce 7050 / NVIDIA nForce 630i", 0 },
	{ 0x10DE, 0x07E3, "GeForce 7050 / NVIDIA nForce 610i", 0 },
	{ 0x10DE, 0x07E5, "GeForce 7050 / NVIDIA nForce 620i", 0 },
	{ 0x10DE, 0x0848, "GeForce 8300", 2 },
	{ 0x10DE, 0x0849, "GeForce 8200", 2 },
	{ 0x10DE, 0x084A, "nForce 730a", 2 },
	{ 0x10DE, 0x084B, "GeForce 8200 ", 2 },
	{ 0x10DE, 0x084C, "nForce 780a SLI", 2 },
	{ 0x10DE, 0x084D, "nForce 750a SLI", 2 },
	{ 0x10DE, 0x084F, "GeForce 8100 / nForce 720a", 2 },
	{ 0x10DE, 0x0A65, "GeForce 210 / GeForce 210", 1 },

	// ATi
	{ 0x1002, 0x958D, "ATI FireGL V3600"  ,2 }, //RV630
	{ 0x1002, 0x958C, "ATI FireGL V5600"  ,2 }, //RV630
	{ 0x1002, 0x940F, "ATI FireGL V7600"  ,2 }, //R600
	{ 0x1002, 0x9511, "ATI FireGL V7700"  ,2 }, //RV670 GL
	{ 0x1002, 0x940B, "ATI FireGL V8600"  ,2 }, //R600
	{ 0x1002, 0x940A, "ATI FireGL V8650"  ,2 }, //R600
	{ 0x1002, 0x95CE, "ATI FirePro 2260"  ,2 }, //RV620
	{ 0x1002, 0x95CF, "ATI FirePro 2260"  ,2 }, //RV620
	{ 0x1002, 0x95CD, "ATI FirePro 2450"  ,2 }, //RV620
	{ 0x1002, 0x94A3, "ATI FirePro M7740" ,2 }, //M97 GL
	{ 0x1002, 0x9557, "ATI FirePro RG220" ,2 }, //M93 GL
	{ 0x1002, 0x95CC, "ATI FirePRO V3700" ,2 }, //RV620 GL
	{ 0x1002, 0x949F, "ATI FirePro V3750 (FireGL)" ,2 }, //RV730
	{ 0x1002, 0x949E, "ATI FirePro V5700 (FireGL)" ,2 }, //RV730
	{ 0x1002, 0x949C, "ATI FirePro V7750 (FireGL)" ,2 }, //RV730
	{ 0x1002, 0x9446, "ATI FirePro V7770 (FireGL)" ,2 }, //RV770  GLXT
	{ 0x1002, 0x9456, "ATI FirePro V8700 (FireGL)" ,2 }, //RV770  GLXT
	{ 0x1002, 0x9447, "ATI FirePro V8700 Duo (FireGL)" ,2 }, //R700 GLXT
	{ 0x1002, 0x9444, "ATI FirePro V8750 (FireGL)" ,2 }, //RV770  GLXT
	{ 0x1002, 0x958F, "ATI Mobility FireGL Graphics Processor" ,2 }, //M76 GL
	{ 0x1002, 0x9595, "ATI Mobility FireGL V5700", 2 }, //M86 GL
	{ 0x1002, 0x9713, "ATI Mobility Radeon 4100"  ,2 }, //RS880MC
	{ 0x1002, 0x94C9, "ATI Mobility Radeon HD 2400" ,1 }, //M72-M
	{ 0x1002, 0x94C8, "ATI Mobility Radeon HD 2400 XT" ,1 }, //M74-M
	{ 0x1002, 0x9581, "ATI Mobility Radeon HD 2600" ,1 }, //M76M
	{ 0x1002, 0x9583, "ATI Mobility Radeon HD 2600 XT" ,1 }, //M76XT
	{ 0x1002, 0x95C4, "ATI Mobility Radeon HD 3400 Series" ,2 }, //M82
	{ 0x1002, 0x95C2, "ATI Mobility Radeon HD 3430" ,2 }, //M82-SE
	{ 0x1002, 0x95C4, "ATI Mobility Radeon HD 3450" ,2 }, //M82-S
	{ 0x1002, 0x9591, "ATI Mobility Radeon HD 3650" ,2 }, //M86-M
	{ 0x1002, 0x9593, "ATI Mobility Radeon HD 3670" ,2 }, //M86-XT
	{ 0x1002, 0x9504, "ATI Mobility Radeon HD 3850" ,2 }, //M88
	{ 0x1002, 0x9506, "ATI Mobility Radeon HD 3850 X2"  ,2 }, //M88
	{ 0x1002, 0x9508, "ATI Mobility Radeon HD 3870" ,2 }, //M88 XT
	{ 0x1002, 0x9509, "ATI Mobility Radeon HD 3870 X2"  ,2 }, //M88
	{ 0x1002, 0x9712, "ATI Mobility Radeon HD 4200" ,2 }, //RS880M
	{ 0x1002, 0x9552, "ATI Mobility Radeon HD 4300 Series" ,2 }, //M92
	{ 0x1002, 0x9553, "ATI Mobility Radeon HD 4500 Series" ,2 }, //M92
	{ 0x1002, 0x9555, "ATI Mobility Radeon HD 4500 Series" ,2 }, //M93
	{ 0x1002, 0x9480, "ATI Mobility Radeon HD 4650" ,2 }, //M96
	{ 0x1002, 0x9488, "ATI Mobility Radeon HD 4670" ,2 }, //M96 XT
	{ 0x1002, 0x94A0, "ATI Mobility Radeon HD 4830" ,2 }, //M97
	{ 0x1002, 0x944A, "ATI Mobility Radeon HD 4850" ,2 }, //M98
	{ 0x1002, 0x944B, "ATI Mobility Radeon HD 4850 X2"  ,2 }, //M98
	{ 0x1002, 0x94A1, "ATI Mobility Radeon HD 4860" ,2 }, //M97 XT
	{ 0x1002, 0x945A, "ATI Mobility Radeon HD 4870" ,2 }, //M98 XT
	{ 0x1002, 0x68B0, "ATI Mobility Radeon HD 5800 Series" ,2 }, //EG BROADWAY XT
	{ 0x1002, 0x68A0, "ATI Mobility Radeon HD 5800 Series" ,2 }, //EG BROADWAY XT
	{ 0x1002, 0x68A1, "ATI Mobility Radeon HD 5800 Series" ,2 }, //EG BROADWAY PRO/LP
	{ 0x1002, 0x68B0, "ATI Mobility Radeon HD 5800 Series" ,2 }, //EG BROADWAY XT
	{ 0x1002, 0x958B, "ATI Mobility Radeon HD 2600 XT Gemini" ,1 }, //M76
	{ 0x1002, 0x9611, "ATI Radeon 3100 Graphics"  ,1 }, //RS780C
	{ 0x1002, 0x9613, "ATI Radeon 3100 Graphics"  ,1 }, //RS780MC
	{ 0x1002, 0x9711, "ATI Radeon 4100"   ,2 }, //RS880C
	{ 0x1002, 0x94CB, "ATI Radeon E2400"  ,1 }, //M72-CSP128
	{ 0x1002, 0x9491, "ATI Radeon E4690"  ,2 }, //M96 CSP
	{ 0x1002, 0x9487, "ATI Radeon Graphics Processor"  ,2 }, //RV730 XT (AGP)
	{ 0x1002, 0x948F, "ATI Radeon Graphics Processor"  ,2 }, //RV730 PRO (AGP)
	{ 0x1002, 0x9541, "ATI Radeon Graphics Processor"  ,2 }, //RV710
	{ 0x1002, 0x954E, "ATI Radeon Graphics Processor", 2 }, //RV710
	{ 0x1002, 0x94B1, "ATI Radeon Graphics Processor", 2 }, //RV740 XT
	{ 0x1002, 0x94C7, "ATI Radeon HD 2350", 1 }, //RV610-DT (CE)
	{ 0x1002, 0x94CC, "ATI Radeon HD 2400", 1 }, //RV610
	{ 0x1002, 0x94C5, "ATI Radeon HD 2400 LE" ,1 }, //RV610 LE
	{ 0x1002, 0x94C3, "ATI Radeon HD 2400 PRO" ,1 }, //RV610-DT (LE)
	{ 0x1002, 0x94C4, "ATI Radeon HD 2400 PRO AGP" ,1 }, //RV610LE
	{ 0x1002, 0x94C1, "ATI Radeon HD 2400 XT" ,1 }, //RV610-DT (Pro)
	{ 0x1002, 0x958E, "ATI Radeon HD 2600 LE", 1 }, //RV630 LE
	{ 0x1002, 0x9589, "ATI Radeon HD 2600 Pro" ,2 }, //RV630 PRO
	{ 0x1002, 0x9587, "ATI Radeon HD 2600 Pro AGP" ,2 }, //RV630 PRO
	{ 0x1002, 0x9588, "ATI Radeon HD 2600 XT" ,2 }, //RV630 XT
	{ 0x1002, 0x9586, "ATI Radeon HD 2600 XT AGP" ,2 }, //RV630 XT
	{ 0x1002, 0x9405, "ATI Radeon HD 2900 GT" ,2 }, //R600
	{ 0x1002, 0x9403, "ATI Radeon HD 2900 PRO", 2 }, //R600
	{ 0x1002, 0x9400, "ATI Radeon HD 2900 XT" ,2 }, //R600
	{ 0x1002, 0x9401, "ATI Radeon HD 2900 XT" ,2 }, //R600
	{ 0x1002, 0x9402, "ATI Radeon HD 2900 XT" ,2 }, //R600
	{ 0x1002, 0x9610, "ATI Radeon HD 3200 Graphics" ,2 }, //RS780
	{ 0x1002, 0x9612, "ATI Radeon HD 3200 Graphics" ,2 }, //RS780M
	{ 0x1002, 0x9614, "ATI Radeon HD 3300 Graphics" ,2 }, //RS780D
	{ 0x1002, 0x95C7, "ATI Radeon HD 3430", 2 }, //RV620 CE
	{ 0x1002, 0x95C5, "ATI Radeon HD 3450", 2 }, //RV620 LE
	{ 0x1002, 0x95C6, "ATI Radeon HD 3450", 2 }, //RV620 LE AGP
	{ 0x1002, 0x95C9, "ATI Radeon HD 3450", 2 }, //RV620
	{ 0x1002, 0x95C0, "ATI Radeon HD 3470", 2 }, //RV620 PRO
	{ 0x1002, 0x9590, "ATI Radeon HD 3600 Series" ,2 }, //RV635
	{ 0x1002, 0x9597, "ATI Radeon HD 3600 Series" ,2 }, //RV635 PRO AGP
	{ 0x1002, 0x9598, "ATI Radeon HD 3600 Series" ,2 }, //RV635 XT
	{ 0x1002, 0x9599, "ATI Radeon HD 3600 Series" ,2 }, //RV635 PRO
	{ 0x1002, 0x9596, "ATI Radeon HD 3650 AGP"    ,2 }, //RV635 XT AGP
	{ 0x1002, 0x9507, "ATI Radeon HD 3830", 2 }, //RV670
	{ 0x1002, 0x9505, "ATI Radeon HD 3850", 2 }, //RV670 PRO
	{ 0x1002, 0x9515, "ATI Radeon HD 3850 AGP" ,2 }, //RV670 AGP
	{ 0x1002, 0x9513, "ATI Radeon HD 3850 X2" ,2 }, //R680 PRO
	{ 0x1002, 0x9519, "ATI FireStream 9170" ,2 }, //RV670
	{ 0x1002, 0x9501, "ATI Radeon HD 3870", 2 }, //RV670 XT
	{ 0x1002, 0x950F, "ATI Radeon HD 3870 X2" ,2 }, //R680
	{ 0x1002, 0x9710, "ATI Radeon HD 4200", 2 }, //RS880
	{ 0x1002, 0x954F, "ATI Radeon HD 4350", 2 }, //RV710
	{ 0x1002, 0x9540, "ATI Radeon HD 4550", 2 }, //RV710
	{ 0x1002, 0x9498, "ATI Radeon HD 4600 Series" ,2 }, //RV730 PRO
	{ 0x1002, 0x9490, "ATI Radeon HD 4600 Series" ,2 }, //RV730 XT
	{ 0x1002, 0x9495, "ATI Radeon HD 4600 Series" ,2 }, //RV730 PRO AGP
	{ 0x1002, 0x944E, "ATI Radeon HD 4700 Series" ,2 }, //RV770  CE
	{ 0x1002, 0x94B4, "ATI Radeon HD 4700 Series" ,2 }, //RV740 LE
	{ 0x1002, 0x94B3, "ATI Radeon HD 4770" ,2 }, //RV740 PRO
	{ 0x1002, 0x94B5, "ATI Radeon HD 4770" ,2 }, //RV740 PRO
	{ 0x1002, 0x9440, "ATI Radeon HD 4800 Series" ,2 }, //RV770 XT
	{ 0x1002, 0x9442, "ATI Radeon HD 4800 Series" ,2 }, //RV770  PRO
	{ 0x1002, 0x944C, "ATI Radeon HD 4800 Series" ,2 }, //RV770 LE
	{ 0x1002, 0x9460, "ATI Radeon HD 4800 Series" ,2 }, //RV790  XT
	{ 0x1002, 0x9462, "ATI Radeon HD 4800 Series" ,2 }, //RV790 GT
	{ 0x1002, 0x9443, "ATI Radeon HD 4850 X2"     ,2 }, //R700 PRO
	{ 0x1002, 0x9441, "ATI Radeon HD 4870 X2"     ,2 }, //R700 XT
	{ 0x1002, 0x68B8, "ATI Radeon HD 5700 Series" ,2 }, //EG JUNIPER XT
	{ 0x1002, 0x68BE, "ATI Radeon HD 5700 Series" ,2 }, //EG JUNIPER LE
	{ 0x1002, 0x6898, "ATI Radeon HD 5800 Series" ,2 }, //EG CYPRESS XT
	{ 0x1002, 0x6899, "ATI Radeon HD 5800 Series" ,2 }, //EG CYPRESS PRO
	{ 0x1002, 0x4154, "ATI FireGL T2", 1}, //RV350GL
	{ 0x1002, 0x4174, "ATI FireGL T2 Secondary", 1}, //RV350GL
	{ 0x1002, 0x5B64, "ATI FireGL V3100", 1}, //RV370GL
	{ 0x1002, 0x5B74, "ATI FireGL V3100 Secondary", 1}, //RV370GL
	{ 0x1002, 0x3E54, "ATI FireGL V3200", 1}, //RV380GL
	{ 0x1002, 0x3E74, "ATI FireGL V3200 Secondary", 1}, //RV380GL
	{ 0x1002, 0x7152, "ATI FireGL V3300", 1}, //RV515GL
	{ 0x1002, 0x7172, "ATI FireGL V3300 Secondary", 1}, //RV515GL
	{ 0x1002, 0x7153, "ATI FireGL V3350", 1}, //RV515GL
	{ 0x1002, 0x7173, "ATI FireGL V3350 Secondary", 1}, //RV515GL
	{ 0x1002, 0x71D2, "ATI FireGL V3400", 1}, //RV530GL
	{ 0x1002, 0x71F2, "ATI FireGL V3400 Secondary", 1}, //RV530GL
	{ 0x1002, 0x958D, "ATI FireGL V3600", 1}, //RV630GL
	{ 0x1002, 0x5E48, "ATI FireGL V5000", 1}, //RV410GL
	{ 0x1002, 0x5E68, "ATI FireGL V5000 Secondary", 1}, //RV410GL
	{ 0x1002, 0x5551, "ATI FireGL V5100", 1}, //RV423GL
	{ 0x1002, 0x5571, "ATI FireGL V5100 Secondary", 1}, //RV423GL
	{ 0x1002, 0x71DA, "ATI FireGL V5200", 1}, //RV530GL
	{ 0x1002, 0x71FA, "ATI FireGL V5200 Secondary", 1}, //RV530GL
	{ 0x1002, 0x7105, "ATI FireGL V5300", 1}, //R520GL
	{ 0x1002, 0x7125, "ATI FireGL V5300 Secondary", 1}, //R520GL
	{ 0x1002, 0x958C, "ATI FireGL V5600", 1}, //RV630GL
	{ 0x1002, 0x5550, "ATI FireGL V7100", 1}, //RV423GL
	{ 0x1002, 0x5570, "ATI FireGL V7100 Secondary", 1}, //RV423GL
	{ 0x1002, 0x5D50, "ATI FireGL V7200", 1}, //R480GL
	{ 0x1002, 0x7104, "ATI FireGL V7200", 1}, //R520GL
	{ 0x1002, 0x5D70, "ATI FireGL V7200 Secondary", 1}, //R480GL	
	{ 0x1002, 0x7124, "ATI FireGL V7200 Secondary", 1}, //R520GL
	{ 0x1002, 0x710E, "ATI FireGL V7300", 1}, //R520GL
	{ 0x1002, 0x712E, "ATI FireGL V7300 Secondary", 1}, //R520GL
	{ 0x1002, 0x710F, "ATI FireGL V7350", 1}, //R520GL
	{ 0x1002, 0x712F, "ATI FireGL V7350 Secondary",	1}, //R520GL
	{ 0x1002, 0x940F, "ATI FireGL V7600", 1}, //R600GL
	{ 0x1002, 0x9511, "ATI FireGL V7700", 1}, //RV630GL
	{ 0x1002, 0x940B, "ATI FireGL V8600", 1}, //R600GL
	{ 0x1002, 0x940A, "ATI FireGL V8650", 1}, //R600GL
	{ 0x1002, 0x4E47, "ATI FireGL X1", 1}, //R300GL
	{ 0x1002, 0x4E67, "ATI FireGL X1 Secondary", 1}, //R300GL
	{ 0x1002, 0x4E4B, "ATI FireGL X2-256/X2-256t", 1}, //R350GL
	{ 0x1002, 0x4E6B, "ATI FireGL X2-256/X2-256t Secondary", 1}, //R350GL
	{ 0x1002, 0x4A4D, "ATI FireGL X3-256", 1}, //R420GL
	{ 0x1002, 0x4A6D, "ATI FireGL X3-256 Secondary", 1}, //R420GL
	{ 0x1002, 0x4147, "ATI FireGL Z1", 1}, //R300GL
	{ 0x1002, 0x4167, "ATI FireGL Z1 Secondary", 1}, //R300GL
	{ 0x1002, 0x5B65, "ATI FireMV 2200", 1}, //RV370
	{ 0x1002, 0x5B75, "ATI FireMV 2200 Secondary", 1}, //RV370
	{ 0x1002, 0x719B, "ATI FireMV 2250", 1}, //RV515
	{ 0x1002, 0x71BB, "ATI FireMV 2250 Secondary", 1}, //RV515
	{ 0x1002, 0x95CE, "ATI FireMV 2260", 1}, //RV610
	{ 0x1002, 0x95CF, "ATI FireMV 2260", 1}, //RV610
	{ 0x1002, 0x3151, "ATI FireMV 2400", 1}, //RV380
	{ 0x1002, 0x3171, "ATI FireMV 2400 Secondary", 1}, //RV380
	{ 0x1002, 0x95CD, "ATI FireMV 2450", 1}, //RV610
	{ 0x1002, 0x724E, "ATI FireStream 2U", 1}, //R580
	{ 0x1002, 0x726E, "ATI FireStream 2U Secondary", 1}, //R580
	{ 0x1002, 0x4E54, "ATI MOBILITY FIRE GL T2/T2e", 0}, //M10GL
	{ 0x1002, 0x5464, "ATI MOBILITY FireGL V3100", 0}, //M22GL
	{ 0x1002, 0x3154, "ATI MOBILITY FireGL V3200", 0}, //M24GL
	{ 0x1002, 0x564A, "ATI MOBILITY FireGL V5000", 1}, //M26GL
	{ 0x1002, 0x564B, "ATI MOBILITY FireGL V5000", 1}, //M26GL
	{ 0x1002, 0x5D49, "ATI MOBILITY FireGL V5100", 1}, //M28GL
	{ 0x1002, 0x71C4, "ATI MOBILITY FireGL V5200", 1}, //M56GL
	{ 0x1002, 0x71D4, "ATI MOBILITY FireGL V5250", 1}, //M56GL
	{ 0x1002, 0x7106, "ATI MOBILITY FireGL V7100", 1}, //M58GL
	{ 0x1002, 0x7103, "ATI MOBILITY FireGL V7200", 1}, //M58GL
	{ 0x1002, 0x4E52, "ATI MOBILITY /ATI RADEON 9500", 1}, //M10
	{ 0x1002, 0x4E56, "ATI MOBILITY /ATI RADEON 9550", 1}, //M12
	{ 0x1002, 0x4E50, "ATI MOBILITY /ATI RADEON 9600/9700 Series", 1}, //M10
	{ 0x1002, 0x4A4E, "ATI MOBILITY /ATI RADEON 9800", 1}, //M18
	{ 0x1002, 0x7210, "ATI MOBILITY /ATI RADEON HD 2300", 1}, //M71
	{ 0x1002, 0x7211, "ATI MOBILITY /ATI RADEON HD 2300", 1}, //M71
	{ 0x1002, 0x94C9, "ATI MOBILITY /ATI RADEON HD 2400", 1}, //M72
	{ 0x1002, 0x9581, "ATI MOBILITY /ATI RADEON HD 2600", 1}, //M76
	{ 0x1002, 0x9508, "ATI MOBILITY /ATI RADEON HD 3870", 2}, 	//M76
	{ 0x1002, 0x95C2, "ATI MOBILITY /ATI RADEON HD 3430", 2}, //M72
	{ 0x1002, 0x9591, "ATI MOBILITY /ATI RADEON HD 3650", 2}, //M76
	{ 0x1002, 0x9504, "ATI MOBILITY /ATI RADEON HD 3850", 2}, //M76
	{ 0x1002, 0x714A, "ATI MOBILITY /ATI RADEON X1300",	1}, //M52
	{ 0x1002, 0x7149, "ATI MOBILITY /ATI RADEON X1300",	1}, //M52
	{ 0x1002, 0x714C, "ATI MOBILITY /ATI RADEON X1300",	1}, //M52
	{ 0x1002, 0x714B, "ATI MOBILITY /ATI RADEON X1300",	1}, //M52
	{ 0x1002, 0x718B, "ATI MOBILITY /ATI RADEON X1350",	1}, //M52
	{ 0x1002, 0x718C, "ATI MOBILITY /ATI RADEON X1350",	1}, //M52
	{ 0x1002, 0x7196, "ATI MOBILITY /ATI RADEON X1350",	1}, //M52
	{ 0x1002, 0x7145, "ATI MOBILITY /ATI RADEON X1400",	1}, //M54
	{ 0x1002, 0x7186, "ATI MOBILITY /ATI RADEON X1450",	1}, //M54
	{ 0x1002, 0x718D, "ATI MOBILITY /ATI RADEON X1450",	1}, //M54
	{ 0x1002, 0x71C5, "ATI MOBILITY /ATI RADEON X1600",	1}, //M56
	{ 0x1002, 0x71D5, "ATI MOBILITY /ATI RADEON X1700",	1}, //M56
	{ 0x1002, 0x71DE, "ATI MOBILITY /ATI RADEON X1700",	1}, //M56
	{ 0x1002, 0x7102, "ATI MOBILITY /ATI RADEON X1800",	1}, //M58
	{ 0x1002, 0x7284, "ATI MOBILITY /ATI RADEON X1900",	1}, //M58
	{ 0x1002, 0x718A, "ATI MOBILITY /ATI RADEON X2300",	1}, //M54
	{ 0x1002, 0x7188, "ATI MOBILITY /ATI RADEON X2300",	1}, //M54
	{ 0x1002, 0x5461, "ATI MOBILITY /ATI RADEON X300", 0}, //M22
	{ 0x1002, 0x5460, "ATI MOBILITY /ATI RADEON X300", 0}, //M22
	{ 0x1002, 0x3152, "ATI MOBILITY /ATI RADEON X300", 0}, //M24
	{ 0x1002, 0x3150, "ATI MOBILITY /ATI RADEON X600", 0}, //M24
	{ 0x1002, 0x5462, "ATI MOBILITY /ATI RADEON X600 SE", 0}, //M24C
	{ 0x1002, 0x5652, "ATI MOBILITY /ATI RADEON X700", 1}, //M26
	{ 0x1002, 0x5653, "ATI MOBILITY /ATI RADEON X700", 1}, //M26
	{ 0x1002, 0x564F, "ATI MOBILITY /ATI RADEON X700 XL", 1}, //M26
	{ 0x1002, 0x5D4A, "ATI MOBILITY /ATI RADEON X800", 1}, //M28
	{ 0x1002, 0x5D48, "ATI MOBILITY/ATI RADEON X800 XT", 1}, //M28
	{ 0x1002, 0x94C8, "ATI MOBILITY /ATI RADEON HD 2400 XT", 1}, //M72
	{ 0x1002, 0x9583, "ATI MOBILITY /ATI RADEON HD 2600 XT", 1}, 	//M76
	{ 0x1002, 0x9506, "ATI MOBILITY /ATI RADEON HD 3850 X2", 2}, //M76
	{ 0x1002, 0x9509, "ATI MOBILITY /ATI RADEON HD 3870 X2", 2}, //M76
	{ 0x1002, 0x71D6, "ATI MOBILITY /ATI RADEON X1700 XT", 1}, //M56
	{ 0x1002, 0x7101, "ATI MOBILITY /ATI RADEON X1800 XT", 1}, //M58
	{ 0x1002, 0x95C4, "ATI MOBILITY /ATI RADEON HD 3400 Series", 2}, //M72
	{ 0x1002, 0x5673, "ATI MOBILITY /ATI RADEON X700 Secondary", 1}, //M26
	{ 0x1002, 0x5653, "ATI MOBILITY/ATI RADEON X700", 1}, //RV410
	{ 0x1002, 0x796E, "ATI RADEON 2100", 1}, //RS690
	{ 0x1002, 0x9611, "ATI RADEON 3100 Graphics", 1}, //RS780
	{ 0x1002, 0x9613, "ATI RADEON 3100 Graphics", 1}, //RS780M
	{ 0x1002, 0x4153, "ATI RADEON 9550/X1050 Series", 1}, //RV350
	{ 0x1002, 0x4173, "ATI RADEON 9550/X1050 Series Secondary",	1}, //RV350
	{ 0x1002, 0x4150, "ATI RADEON 9600 Series", 1}, //RV350
	{ 0x1002, 0x4E51, "ATI RADEON 9600 Series", 1}, //RV350
	{ 0x1002, 0x4151, "ATI RADEON 9600 Series", 1}, //RV350
	{ 0x1002, 0x4155, "ATI RADEON 9600 Series", 1}, //RV350
	{ 0x1002, 0x4152, "ATI RADEON 9600 Series", 1}, //RV360
	{ 0x1002, 0x94C5, "ATI RADEON HD 2400 LE", 1}, //RV610
	{ 0x1002, 0x94C3, "ATI RADEON HD 2400 PRO", 1}, //RV610
	{ 0x1002, 0x4E71, "ATI RADEON 9600 Series Secondary", 1}, //RV350
	{ 0x1002, 0x4171, "ATI RADEON 9600 Series Secondary", 1}, //RV350
	{ 0x1002, 0x4170, "ATI RADEON 9600 Series Secondary", 1}, //RV350
	{ 0x1002, 0x4175, "ATI RADEON 9600 Series Secondary", 1}, //RV350
	{ 0x1002, 0x4172, "ATI RADEON 9600 Series Secondary", 1}, //RV360
	{ 0x1002, 0x94CB, "ATI RADEON E2400", 1}, 	//M72
	{ 0x1002, 0x94C7, "ATI RADEON HD 2350", 1}, //RV610
	{ 0x1002, 0x94CC, "ATI RADEON HD 2400", 1}, //RV610
	{ 0x1002, 0x94C4, "ATI RADEON HD 2400 PRO AGP", 1}, //RV610
	{ 0x1002, 0x94C1, "ATI RADEON HD 2400 XT", 1}, //RV610
	{ 0x1002, 0x958E, "ATI RADEON HD 2600 LE", 1}, //RV630
	{ 0x1002, 0x9589, "ATI RADEON HD 2600 Pro", 1}, //RV630
	{ 0x1002, 0x9587, "ATI RADEON HD 2600 Pro AGP", 1}, //RV630
	{ 0x1002, 0x9588, "ATI RADEON HD 2600 XT", 1}, //RV630
	{ 0x1002, 0x9586, "ATI RADEON HD 2600 XT AGP", 1}, //RV630
	{ 0x1002, 0x9405, "ATI RADEON HD 2900 GT", 1}, //R600
	{ 0x1002, 0x9403, "ATI RADEON HD 2900 PRO", 1}, //R600
	{ 0x1002, 0x9400, "ATI RADEON HD 2900 XT", 1}, //R600
	{ 0x1002, 0x9401, "ATI RADEON HD 2900 XT", 1}, //R600
	{ 0x1002, 0x9402, "ATI RADEON HD 2900 XT", 1}, //R600
	{ 0x1002, 0x9610, "ATI RADEON HD 3200 Graphics", 2}, //RS780
	{ 0x1002, 0x9612, "ATI RADEON HD 3200 Graphics", 2}, //RS780M
	{ 0x1002, 0x9614, "ATI RADEON HD 3300 Graphics", 2}, //RS780
	{ 0x1002, 0x95C7, "ATI RADEON HD 3430", 2}, //RV610
	{ 0x1002, 0x95C5, "ATI RADEON HD 3450", 2}, //RV610
	{ 0x1002, 0x95C0, "ATI RADEON HD 3470",	2}, //RV610
	{ 0x1002, 0x9590, "ATI RADEON HD 3600 Series", 2}, //RV630
	{ 0x1002, 0x9596, "ATI RADEON HD 3600 Series", 2}, //RV630
	{ 0x1002, 0x9597, "ATI RADEON HD 3600 Series", 2}, //RV630
	{ 0x1002, 0x9598, "ATI RADEON HD 3600 Series", 2}, //RV630
	{ 0x1002, 0x9599, "ATI RADEON HD 3600 Series", 2}, //RV630
	{ 0x1002, 0x9505, "ATI RADEON HD 3850", 2}, //RV630
	{ 0x1002, 0x9501, "ATI RADEON HD 3870", 2}, //RV630
	{ 0x1002, 0x950F, "ATI RADEON HD 3870 X2", 2}, //RV630
	{ 0x1002, 0x791E, "ATI RADEON X1200 Series", 1}, //RS690
	{ 0x1002, 0x791F, "ATI RADEON X1200 Series", 1}, //RS690M
	{ 0x1002, 0x7288, "ATI RADEON X1950 GT", 1}, //R580
	{ 0x1002, 0x7248, "ATI RADEON X1950 XTX Uber - Limited Edition", 1}, //R580
	{ 0x1002, 0x7268, "ATI RADEON X1950 XTX Uber - Limited Edition Secondary", 1}, //R580
	{ 0x1002, 0x72A8, "ATI RADEON X1950 GT Secondary", 1}, //R580
	{ 0x1002, 0x554E, "ATI RADEON X800 GT",	1}, 	//R430
	{ 0x1002, 0x556E, "ATI RADEON X800 GT Secondary", 1}, //R430
	{ 0x1002, 0x554D, "ATI RADEON X800 XL",	1}, //R430
	{ 0x1002, 0x556D, "ATI RADEON X800 XL Secondary", 1}, //R430
	{ 0x1002, 0x4B4B, "ATI RADEON X850 PRO", 1}, //R481
	{ 0x1002, 0x4B4A, "ATI RADEON X850 SE", 1}, //R481
	{ 0x1002, 0x4B49, "ATI RADEON X850 XT", 1}, //R481
	{ 0x1002, 0x4B6B, "ATI RADEON X850 PRO Secondary", 1}, //R481
	{ 0x1002, 0x4B6A, "ATI RADEON X850 SE Secondary", 1}, //R481
	{ 0x1002, 0x4B4C, "ATI RADEON X850 XT Platinum Edition", 1}, //R481
	{ 0x1002, 0x4B6C, "ATI RADEON X850 XT Platinum Edition Secondary", 1}, //R481
	{ 0x1002, 0x4B69, "ATI RADEON X850 XT Secondary", 1}, //R481
	{ 0x1002, 0x793F, "ATI RADEON Xpress 1200 Series", 1}, //RS600
	{ 0x1002, 0x7941, "ATI RADEON Xpress 1200 Series", 1}, //RS600
	{ 0x1002, 0x7942, "ATI RADEON Xpress 1200 Series", 1}, //RS600M
	{ 0x1002, 0x5A61, "ATI RADEON Xpress Series", 0}, //RC410
	{ 0x1002, 0x5A63, "ATI RADEON Xpress Series", 0}, //RC410
	{ 0x1002, 0x5A62, "ATI RADEON Xpress Series", 0}, //RC410M
	{ 0x1002, 0x5A41, "ATI RADEON Xpress Series", 0}, //RS400
	{ 0x1002, 0x5A43, "ATI RADEON Xpress Series", 0}, //RS400
	{ 0x1002, 0x5A42, "ATI RADEON Xpress Series", 0}, //RS400M
	{ 0x1002, 0x5954, "ATI RADEON Xpress Series", 0}, //RS480
	{ 0x1002, 0x5854, "ATI RADEON Xpress Series", 0}, //RS480
	{ 0x1002, 0x5974, "ATI RADEON Xpress Series", 0}, //RS482
	{ 0x1002, 0x5874, "ATI RADEON Xpress Series", 0}, //RS482
	{ 0x1002, 0x5975, "ATI RADEON Xpress Series", 0}, //RS482M
	{ 0x1002, 0x5955, "ATI RADEON Xpress Series", 0}, //RS480M
	{ 0x1002, 0x958B, "ATI MOBILITY RADEON HD 2600 XT Gemini", 1}, //M76
	{ 0x1002, 0x4144, "ATI RADEON 9500", 1}, //R300
	{ 0x1002, 0x4145, "ATI RADEON 9200 (M+X)", 0}, //R300
	{ 0x1002, 0x4966, "ATI RADEON 9000/9000 PRO", 0}, //RV250
	{ 0x1002, 0x496E, "ATI RADEON 9000/9000 PRO Secondary", 0}, //RV250
	{ 0x1002, 0x4967, "ATI RADEON 9000", 0}, //RV250
	{ 0x1002, 0x496F, "ATI RADEON 9000 Secondary", 0}, //RV250
	{ 0x1002, 0x4C66, "ATI MOBILITY RADEON 9000 (M9)", 0}, //RV250
	{ 0x1002, 0x4149, "ATI RADEON 9500", 1}, //R350
	{ 0x1002, 0x4E45, "ATI RADEON 9500 PRO / 9700",	1}, //R300
	{ 0x1002, 0x4E65, "ATI RADEON 9500 PRO / 9700 Secondary", 1}, //R300
	{ 0x1002, 0x4164, "ATI RADEON 9500 Secondary", 1}, //R300
	{ 0x1002, 0x4169, "ATI RADEON 9500 Secondary", 1}, //R350
	{ 0x1002, 0x4E46, "ATI RADEON 9600 TX",	1}, //R300
	{ 0x1002, 0x4E66, "ATI RADEON 9600 TX Secondary", 1}, //R300
	{ 0x1002, 0x4146, "ATI RADEON 9600TX", 1}, //R300
	{ 0x1002, 0x4166, "ATI RADEON 9600TX Secondary", 1}, //R300
	{ 0x1002, 0x4E44, "ATI RADEON 9700 PRO", 1}, //R300
	{ 0x1002, 0x4E64, "ATI RADEON 9700 PRO Secondary", 1}, //R300
	{ 0x1002, 0x4E49, "ATI RADEON 9800", 1}, //R350
	{ 0x1002, 0x4E48, "ATI RADEON 9800 PRO", 1}, //R350
	{ 0x1002, 0x4E68, "ATI RADEON 9800 PRO Secondary", 1}, //R350
	{ 0x1002, 0x4148, "ATI RADEON 9800 SE",	1}, //R350
	{ 0x1002, 0x4168, "ATI RADEON 9800 SE Secondary", 1}, //R350
	{ 0x1002, 0x4E69, "ATI RADEON 9800 Secondary", 1}, //R350
	{ 0x1002, 0x4E4A, "ATI RADEON 9800 XT",	1}, //R360
	{ 0x1002, 0x4E6A, "ATI RADEON 9800 XT Secondary", 1}, //R360
	{ 0x1002, 0x7146, "ATI RADEON X1300 / X1550 Series", 1}, //RV515
	{ 0x1002, 0x7166, "ATI RADEON X1300 / X1550 Series Secondary", 1}, //RV515
	{ 0x1002, 0x714D, "ATI RADEON X1300 Series", 1}, //RV515
	{ 0x1002, 0x71C3, "ATI RADEON X1300 Series", 1}, //RV535
	{ 0x1002, 0x718F, "ATI RADEON X1300 Series", 1}, //RV515PCI
	{ 0x1002, 0x714E, "ATI RADEON X1300 Series", 1}, //RV515PCI
	{ 0x1002, 0x715E, "ATI RADEON X1300 Series", 1}, //RV515
	{ 0x1002, 0x716D, "ATI RADEON X1300 Series Secondary", 1}, //RV515
	{ 0x1002, 0x71E3, "ATI RADEON X1300 Series Secondary", 1}, //RV535
	{ 0x1002, 0x71AF, "ATI RADEON X1300 Series Secondary", 1}, //RV515PCI
	{ 0x1002, 0x716E, "ATI RADEON X1300 Series Secondary", 1}, //RV515PCI
	{ 0x1002, 0x717E, "ATI RADEON X1300 Series Secondary", 1}, //RV515
	{ 0x1002, 0x7142, "ATI RADEON X1300/X1550 Series", 1}, //RV515
	{ 0x1002, 0x7180, "ATI RADEON X1300/X1550 Series", 1}, //RV515
	{ 0x1002, 0x7183, "ATI RADEON X1300/X1550 Series", 1}, //RV515
	{ 0x1002, 0x7187, "ATI RADEON X1300/X1550 Series", 1}, //RV515
	{ 0x1002, 0x71A7, "ATI RADEON X1300/X1550 Series Secondary", 1}, //RV515
	{ 0x1002, 0x7162, "ATI RADEON X1300/X1550 Series Secondary", 1}, //RV515
	{ 0x1002, 0x71A0, "ATI RADEON X1300/X1550 Series Secondary", 1}, //RV515
	{ 0x1002, 0x71A3, "ATI RADEON X1300/X1550 Series Secondary", 1}, //RV515
	{ 0x1002, 0x719F, "ATI RADEON X1550 64-bit", 1}, //RV515
	{ 0x1002, 0x7147, "ATI RADEON X1550 64-bit", 1}, //RV515
	{ 0x1002, 0x715F, "ATI RADEON X1550 64-bit", 1}, //RV515
	{ 0x1002, 0x7167, "ATI RADEON X1550 64-bit Secondary", 1}, //RV515
	{ 0x1002, 0x717F, "ATI RADEON X1550 64-bit Secondary", 1}, //RV515
	{ 0x1002, 0x7193, "ATI RADEON X1550 Series", 1}, //RV515
	{ 0x1002, 0x7143, "ATI RADEON X1550 Series", 1}, //RV515
	{ 0x1002, 0x71B3, "ATI RADEON X1550 Series Secondary", 1}, //RV515
	{ 0x1002, 0x7163, "ATI RADEON X1550 Series Secondary", 1}, //RV515
	{ 0x1002, 0x71CE, "ATI RADEON X1600 Pro / ATI RADEON X1300 XT",	1}, //RV530
	{ 0x1002, 0x71EE, "ATI RADEON X1600 Pro / ATI RADEON X1300 XT Secondary", 1}, //RV530
	{ 0x1002, 0x71C0, "ATI RADEON X1600 Series", 1}, //RV530
	{ 0x1002, 0x71C2, "ATI RADEON X1600 Series", 1}, //RV530
	{ 0x1002, 0x7181, "ATI RADEON X1600 Series", 1}, //RV515
	{ 0x1002, 0x71CD, "ATI RADEON X1600 Series", 1}, //RV530
	{ 0x1002, 0x7140, "ATI RADEON X1600 Series", 1}, //RV515
	{ 0x1002, 0x71E2, "ATI RADEON X1600 Series Secondary", 1}, //RV530
	{ 0x1002, 0x71E6, "ATI RADEON X1600 Series Secondary", 1}, //RV530
	{ 0x1002, 0x71A1, "ATI RADEON X1600 Series Secondary", 1}, //RV515
	{ 0x1002, 0x71ED, "ATI RADEON X1600 Series Secondary", 1}, //RV530
	{ 0x1002, 0x71E0, "ATI RADEON X1600 Series Secondary", 1}, //RV530
	{ 0x1002, 0x7160, "ATI RADEON X1600 Series Secondary", 1}, //RV515
	{ 0x1002, 0x71C6, "ATI RADEON X1650 Series", 1}, //RV530
	{ 0x1002, 0x71C1, "ATI RADEON X1650 Series", 1}, //RV535
	{ 0x1002, 0x7293, "ATI RADEON X1650 Series", 1}, //R580
	{ 0x1002, 0x7291, "ATI RADEON X1650 Series", 1}, //R580
	{ 0x1002, 0x71C7, "ATI RADEON X1650 Series", 1}, //RV535
	{ 0x1002, 0x71E1, "ATI RADEON X1650 Series Secondary", 1}, //RV535
	{ 0x1002, 0x72B3, "ATI RADEON X1650 Series Secondary", 1}, //R580
	{ 0x1002, 0x72B1, "ATI RADEON X1650 Series Secondary", 1}, //R580
	{ 0x1002, 0x71E7, "ATI RADEON X1650 Series Secondary", 1}, //RV535
	{ 0x1002, 0x7100, "ATI RADEON X1800 Series", 1}, //R520
	{ 0x1002, 0x7108, "ATI RADEON X1800 Series", 1}, //R520
	{ 0x1002, 0x7109, "ATI RADEON X1800 Series", 1}, //R520
	{ 0x1002, 0x710A, "ATI RADEON X1800 Series", 1}, //R520
	{ 0x1002, 0x710B, "ATI RADEON X1800 Series", 1}, //R520
	{ 0x1002, 0x710C, "ATI RADEON X1800 Series", 1}, //R520
	{ 0x1002, 0x7120, "ATI RADEON X1800 Series Secondary", 1}, //R520
	{ 0x1002, 0x7128, "ATI RADEON X1800 Series Secondary", 1}, //R520
	{ 0x1002, 0x7129, "ATI RADEON X1800 Series Secondary", 1}, //R520
	{ 0x1002, 0x712A, "ATI RADEON X1800 Series Secondary", 1}, //R520
	{ 0x1002, 0x712C, "ATI RADEON X1800 Series Secondary", 1}, //R520
	{ 0x1002, 0x712B, "ATI RADEON X1800 Series Secondary", 1}, //R520
	{ 0x1002, 0x7243, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x7245, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x7246, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x7247, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x7248, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x7249, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x724A, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x724B, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x724C, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x724D, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x724F, "ATI RADEON X1900 Series", 1}, //R580
	{ 0x1002, 0x7263, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x7265, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x7266, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x7267, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x7268, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x7269, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x726A, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x726B, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x726C, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x726D, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x726F, "ATI RADEON X1900 Series Secondary", 1}, //R580
	{ 0x1002, 0x7280, "ATI RADEON X1950 Series", 1}, //R580
	{ 0x1002, 0x7240, "ATI RADEON X1950 Series", 1}, //R580
	{ 0x1002, 0x7244, "ATI RADEON X1950 Series", 1}, //R580
	{ 0x1002, 0x72A0, "ATI RADEON X1950 Series Secondary", 1}, //R580
	{ 0x1002, 0x7260, "ATI RADEON X1950 Series Secondary", 1}, //R580
	{ 0x1002, 0x7264, "ATI RADEON X1950 Series Secondary", 1}, //R580
	{ 0x1002, 0x5B60, "ATI RADEON X300/X550/X1050 Series", 1}, //RV370
	{ 0x1002, 0x5B63, "ATI RADEON X300/X550/X1050 Series", 1}, //RV370
	{ 0x1002, 0x5B73, "ATI RADEON X300/X550/X1050 Series Secondary", 1}, //RV370
	{ 0x1002, 0x5B70, "ATI RADEON X300/X550/X1050 Series Secondary", 1}, //RV370
	{ 0x1002, 0x5657, "ATI RADEON X550/X700 Series", 1}, //RV410
	{ 0x1002, 0x5677, "ATI RADEON X550/X700 Series Secondary", 1}, //RV410
	{ 0x1002, 0x5B62, "ATI RADEON X600 Series",	1}, //RV380x
	{ 0x1002, 0x5B72, "ATI RADEON X600 Series Secondary", 1}, 	//RV380x
	{ 0x1002, 0x3E50, "ATI RADEON X600/X550 Series", 1}, //RV380
	{ 0x1002, 0x3E70, "ATI RADEON X600/X550 Series Secondary", 1}, //RV380
	{ 0x1002, 0x5E4D, "ATI RADEON X700", 1}, //RV410
	{ 0x1002, 0x5E4B, "ATI RADEON X700 PRO", 1}, //RV410
	{ 0x1002, 0x5E6B, "ATI RADEON X700 PRO Secondary", 1}, //RV410
	{ 0x1002, 0x5E4C, "ATI RADEON X700 SE", 1}, //RV410
	{ 0x1002, 0x5E6C, "ATI RADEON X700 SE Secondary", 1}, //RV410
	{ 0x1002, 0x5E6D, "ATI RADEON X700 Secondary", 1}, //RV410
	{ 0x1002, 0x5E4A, "ATI RADEON X700 XT",	1}, //RV410
	{ 0x1002, 0x5E6A, "ATI RADEON X700 XT Secondary", 1}, //RV410
	{ 0x1002, 0x5E4F, "ATI RADEON X700/X550 Series", 1}, //RV410
	{ 0x1002, 0x5E6F, "ATI RADEON X700/X550 Series Secondary", 1}, //RV410
	{ 0x1002, 0x554B, "ATI RADEON X800 GT",	1}, //R423
	{ 0x1002, 0x556B, "ATI RADEON X800 GT Secondary", 1}, //R423
	{ 0x1002, 0x5549, "ATI RADEON X800 GTO", 1}, //R423
	{ 0x1002, 0x554F, "ATI RADEON X800 GTO", 1}, //R430
	{ 0x1002, 0x5D4F, "ATI RADEON X800 GTO", 1}, //R480
	{ 0x1002, 0x554D, "ATI RADEON X800 CrossFire Edition", 1}, //R430
	{ 0x1002, 0x556D, "ATI RADEON X800 CrossFire Edition Secondary", 1}, //R430
	{ 0x1002, 0x5D52, "ATI RADEON X850 CrossFire Edition", 1}, //R480
	{ 0x1002, 0x5D72, "ATI RADEON X850 CrossFire Edition Secondary", 1}, //R480
	{ 0x1002, 0x5569, "ATI RADEON X800 GTO Secondary", 1}, //R423
	{ 0x1002, 0x556F, "ATI RADEON X800 GTO Secondary", 1}, //R430
	{ 0x1002, 0x5D6F, "ATI RADEON X800 GTO Secondary", 1}, //R480
	{ 0x1002, 0x4A49, "ATI RADEON X800 PRO", 1}, //R420
	{ 0x1002, 0x4A69, "ATI RADEON X800 PRO Secondary", 1}, //R420
	{ 0x1002, 0x4A4F, "ATI RADEON X800 SE", 1}, //R420
	{ 0x1002, 0x4A6F, "ATI RADEON X800 SE Secondary", 1}, //R420
	{ 0x1002, 0x4A48, "ATI RADEON X800 Series", 1}, //R420
	{ 0x1002, 0x4A4A, "ATI RADEON X800 Series", 1}, //R420
	{ 0x1002, 0x5548, "ATI RADEON X800 Series", 1}, //R423
	{ 0x1002, 0x4A4C, "ATI RADEON X800 Series", 1}, //R420
	{ 0x1002, 0x4A68, "ATI RADEON X800 Series Secondary", 1}, //R420
	{ 0x1002, 0x4A6A, "ATI RADEON X800 Series Secondary", 1}, //R420
	{ 0x1002, 0x4A6C, "ATI RADEON X800 Series Secondary", 1}, //R420
	{ 0x1002, 0x5568, "ATI RADEON X800 Series Secondary", 1}, //R423
	{ 0x1002, 0x4A54, "ATI RADEON X800 VE",	1}, //R420
	{ 0x1002, 0x4A74, "ATI RADEON X800 VE Secondary", 1}, //R420
	{ 0x1002, 0x4A4B, "ATI RADEON X800 XT", 1}, //R420
	{ 0x1002, 0x5D57, "ATI RADEON X800 XT", 1}, //R423
	{ 0x1002, 0x4A50, "ATI RADEON X800 XT Platinum Edition", 1}, //R420
	{ 0x1002, 0x554A, "ATI RADEON X800 XT Platinum Edition", 1}, //R423
	{ 0x1002, 0x4A70, "ATI RADEON X800 XT Platinum Edition Secondary", 1}, //R420
	{ 0x1002, 0x556A, "ATI RADEON X800 XT Platinum Edition Secondary", 1}, //R423
	{ 0x1002, 0x4A6B, "ATI RADEON X800 XT Secondary", 1}, //R420
	{ 0x1002, 0x5D77, "ATI RADEON X800 XT Secondary", 1}, //R423
	{ 0x1002, 0x5D52, "ATI RADEON X850 XT",	1}, //R480
	{ 0x1002, 0x5D4D, "ATI RADEON X850 XT Platinum Edition", 1}, //R480
	{ 0x1002, 0x5D6D, "ATI RADEON X850 XT Platinum Edition Secondary", 1}, //R480
	{ 0x1002, 0x5D72, "ATI RADEON X850 XT Secondary", 1}, //R480
	{ 0x1002, 0x0B12, "ATI READEON X1900", 1}, // R580
	{ 0x1002, 0x5157, "ATI READEON 7500", 0}, // RV200
	{ 0x1002, 0x5158, "ATI READEON 7500 QX", 0}, // RV200
	{ 0x1002, 0x514C, "ATI READEON 8500 / 8500 LE", 0}, // RV200
	{ 0x1002, 0x514D, "ATI READEON 9100 Series", 0}, // RV200
	{ 0x1002, 0x516D, "ATI READEON 9100 Series Secondary", 0}, // RV200
	{ 0x1002, 0x7833, "ATI READEON 9100 PRO", 0}, // RV200
	{ 0x1002, 0x5C61, "ATI MOBILITY READEON 9200", 0}, // RV280
	{ 0x1002, 0x5C63, "ATI MOBILITY READEON 9200", 0}, // RV280 (M9+)
	{ 0x1002, 0x5D44, "ATI READEON 9200 SE Series Secondary", 0}, // RV280
	{ 0x1002, 0x5D45, "ATI FireMV 2200 PCI Secondary", 0}, // RV280

	// Sis
	{ 0x1039, 0x0018, "SiS950", 0 },
	{ 0x1039, 0x0200, "SiS5597/98", 0 },
	{ 0x1039, 0x0300, "SiS330", 0 },
	{ 0x1039, 0x0305, "SiS305", 0 },
	{ 0x1039, 0x0315, "SiS315", 0 },
	{ 0x1039, 0x0330, "SiS330", 0 },
	{ 0x1039, 0x1039, "SiS5597 SVGAa", 0 },
	{ 0x1039, 0x5300, "SiS540", 0 },
	{ 0x1039, 0x5596, "SiS5596", 0 },
	{ 0x1039, 0x6205, "SiS6206", 0 },
	{ 0x1039, 0x6225, "SiS 950 m2284dxs", 0 },
	{ 0x1039, 0x6236, "SiS6236", 0 },
	{ 0x1039, 0x6300, "SiS630/730", 0 },
	{ 0x1039, 0x6306, "SiS530/620", 0 },
	{ 0x1039, 0x6325, "SiS650", 0 },
	{ 0x1039, 0x6326, "SiS6326 AGP", 0 },
	{ 0x1039, 0x6330, "SiS661FX/M661FX/760/741/M760/M741", 0 },
	{ 0x1039, 0x9876, "SiS6215", 0 },

};

static const int numDefaultDevices = sizeof (defaultDeviceLib)/sizeof(defaultDeviceLib[0]);

bool ATOM_DeviceDB::findDevice (unsigned short vendorId, unsigned short deviceId, Device *device)
{
	if (device)
	{
		for (unsigned i = 0; i < numDefaultDevices; ++i)
		{
			if (defaultDeviceLib[i].vendorId == vendorId && defaultDeviceLib[i].deviceId == deviceId)
			{
				*device = defaultDeviceLib[i];
				return true;
			}
		}
	}
	return false;
}

static ATOM_DDB_DisplayInfo & getDispInfo ()
{
	static ATOM_DDB_DisplayInfo info;
	return info;
}

unsigned ATOM_DeviceDB::getNumDevices (void)
{
	return getDispInfo().GetNumDevices ();
}

unsigned short ATOM_DeviceDB::getVendorId (unsigned device)
{
	return getDispInfo().GetDeviceVendorId (device);
}

unsigned short ATOM_DeviceDB::getDeviceId (unsigned device)
{
	return getDispInfo().GetDeviceId (device);
}

