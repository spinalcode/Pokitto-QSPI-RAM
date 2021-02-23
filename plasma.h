unsigned short pal[256*2]; // assign a 256 entry array to hold the palette

int PntClr(int x, int y){
//	return Pokitto::Display::getPixel(x,y);
    uint8_t temp[1];
    readFromAddressQuad(x+screenWidth*y,temp,1);
//    readFromAddress(x+220*y,temp,1);
    return (int)temp[0];
}
void Dot (int x, int y, uint8_t c){
	//game.display.drawPixel(x,y,c);
	writeToAddressQuad(x+screenWidth*y, &c, 1);
	//writeToAddress(x+220*y, temp, 1);
}
int RandMinMax(int min, int max){
    return rand() % max + min;
}
int Adjust (int xa, int ya, int x, int y, int xb, int yb){
	if(PntClr(x, y) != 0) return 0;
	int q = abs(xa - xb) + abs(ya - yb);
	int v = (PntClr(xa, ya) + PntClr(xb, yb)) / 2 + (RandMinMax(0,q*10)) / 10;
	if (v < 1) v = 1;
	if (v > 255) v = 255;
	Dot(x, y, v);
	return 1;
}
void SubDivide (int x1, int y1, int x2, int y2){
	if ((x2 - x1 < 2) && (y2 - y1 < 2)) return;
	int x = (x1 + x2) / 2;
	int y = (y1 + y2) / 2;
	Adjust(x1, y1, x, y1, x2, y1);
	Adjust(x1, y2, x, y2, x2, y2);
	Adjust(x2, y1, x2, y, x2, y2);
	Adjust(x1, y1, x1, y, x1, y2);
	if(PntClr(x, y) == 0)	{
		int v = PntClr(x1, y1) + PntClr(x2, y1) + PntClr(x2, y2);
		v = v + PntClr(x1, y2) + PntClr(x1, y) + PntClr(x, y1);
		v = v + PntClr(x2, y) + PntClr(x, y2);
		v = v / 8;
		Dot(x, y, v);
	}
	SubDivide(x1, y1, x, y);
	SubDivide(x, y, x2, y2);
	SubDivide(x, y1, x2, y);
	SubDivide(x1, y, x, y2);
}
void make_plasma(int x1=0,int y1=0,int x2=399,int y2=299){
	//game.display.clear();
	if(x1<0)x1=0;
	if(y1<0)y1=0;
	if(x2>399)x2=399;
	if(y2>299)y2=299;

	Dot(x1, y1, RandMinMax(0,255));
	Dot(x2, y1, RandMinMax(0,255));
	Dot(x2, y2, RandMinMax(0,255));
	Dot(x1, y2, RandMinMax(0,255));
	SubDivide(x1, y1, x2, y2);
}
void make_pal(void){
	int a,s,r,g,b;
	for(a=0; a<=63; a++){
		s = 0; 	r = a; 		g = 63-a;	b = 0;		pal[a+s] = Pokitto::Display::RGBto565(r*4,g*4,b*4);
		s = 64; r = 63-a;	g = 0;		b = a; 		pal[a+s] = Pokitto::Display::RGBto565(r*4,g*4,b*4);
		s = 128; r = 0;	 	g = 0;		b = 63-a;	pal[a+s] = Pokitto::Display::RGBto565(r*4,g*4,b*4);
		s = 192; r = 0;		g = a;		b = 0;	 	pal[a+s] = Pokitto::Display::RGBto565(r*4,g*4,b*4);
	}
    Pokitto::Display::load565Palette(&pal[0]); // load a palette the same way as any other palette in any other screen mode
    //for(int t=0; t<255; t++){
    //    pal[256+t] = pal[t];
    //}
}
