//
// Displaylist generated by OBJN64 
// Input: heart_p4.h
//

unsigned short Text_heart_p4_gui_diff[] = {
0x4afd, 0x4afd, 0x4afd, 0xffc3, 0xffc3, 0xffc3, 0x0001, 0x0001, 0x4afd, 0xbe3f, 0x4afd, 0xffc3, 0xfff5, 
0xffc3, 0x0001, 0x0001, 0x4afd, 0x4afd, 0x4afd, 0xffc3, 0xffc3, 0xffc3, 0x0001, 0x0001, 0xeb81, 0xeb81, 
0xeb81, 0xf82d, 0xf82d, 0xf82d, 0x0001, 0x0001, 0xeb81, 0xfe67, 0xeb81, 0xf82d, 0xfcf7, 0xf82d, 0x0001, 
0x0001, 0xeb81, 0xeb81, 0xeb81, 0xf82d, 0xf82d, 0xf82d, 0x0001, 0x0001, 0xfb19, 0xfb19, 0xfb19, 0xfb19, 
0xfb19, 0xfb19, 0xfb19, 0xfb19, 0xd001, 0xd001, 0xf001, 0xf001, 0xf98d, 0xf98d, 0xfb19, 0xfb19, }; 

Vtx_tn Vtx_heart_p4_heart_p4_0[16] = {
{     2,      8,      6, 0,   1166,    400, 204, 204, 204, 255},
{     0,      7,      0, 0,   1236,    668, 204, 204, 204, 255},
{     0,     11,      7, 0,   1177,    665, 204, 204, 204, 255},
{     2,      6,     10, 0,   1108,    402, 204, 204, 204, 255},
{     0,     -1,     10, 0,    876,    103, 204, 204, 204, 255},
{     2,      0,      9, 0,   1099,    342, 204, 204, 204, 255},
{     2,      6,      0, 0,    268,   1868, 204, 204, 204, 255},
{     0,      7,     11, 0,    882,    652, 204, 204, 204, 255},
{     4,    -10,      0, 0,   1209,    339, 204, 204, 204, 255},
{     0,    -14,      0, 0,   1220,     97, 204, 204, 204, 255},
{     2,      8,     -6, 0,   1166,    400, 204, 204, 204, 255},
{     0,     -1,    -10, 0,    876,    103, 204, 204, 204, 255},
{     2,      6,    -10, 0,   1108,    402, 204, 204, 204, 255},
{     2,      0,     -9, 0,   1099,    342, 204, 204, 204, 255},
{     0,      7,    -11, 0,    882,    652, 204, 204, 204, 255},
{     0,     11,     -7, 0,   1177,    665, 204, 204, 204, 255},
};

Gfx Vtx_heart_p4_heart_p4_dl[] = {
	gsSPVertex(&Vtx_heart_p4_heart_p4_0[0], 16, 0),
	gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
	gsSP2Triangles(0, 5, 6, 0, 0, 7, 3, 0),
	gsSP2Triangles(8, 4, 9, 0, 6, 5, 8, 0),
	gsSP2Triangles(10, 1, 6, 0, 11, 12, 13, 0),
	gsSP2Triangles(10, 13, 12, 0, 14, 10, 12, 0),
	gsSP2Triangles(8, 11, 13, 0, 6, 8, 13, 0),
	gsSP2Triangles(0, 6, 1, 0, 3, 7, 4, 0),
	gsSP2Triangles(0, 3, 5, 0, 0, 2, 7, 0),
	gsSP2Triangles(8, 5, 4, 0, 10, 15, 1, 0),
	gsSP2Triangles(11, 14, 12, 0, 10, 6, 13, 0),
	gsSP2Triangles(14, 15, 10, 0, 8, 9, 11, 0),
	gsSPEndDisplayList(),
};

Gfx Wtx_heart_p4[] = {
	gsDPLoadTextureBlock(Text_heart_p4_gui_diff, G_IM_FMT_RGBA, G_IM_SIZ_16b, 
		8,8, 0, G_TX_WRAP|G_TX_NOMIRROR, G_TX_WRAP|G_TX_NOMIRROR,
		3,3, G_TX_NOLOD, G_TX_NOLOD),
	gsSPDisplayList(Vtx_heart_p4_heart_p4_dl),
	gsSPEndDisplayList(),
};

