#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include "ledElement.h"

#define COL_LEN 16

class LedMatrix : public LedElement
{

	int convertIdx(int idx);

	
	public:
	LedMatrix(Adafruit_NeoPixel* p, int o, int n);
	~LedMatrix() = default;
	virtual void lightPixel(int idx, uint32_t color) override;
	
	
	
	
};

#endif // LEDMATRIX_H