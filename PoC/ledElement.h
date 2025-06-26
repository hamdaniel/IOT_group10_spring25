#ifndef LEDELEMENT_H
#define LEDELEMENT_H

#include <Adafruit_NeoPixel.h>


#define COL_LEN 16


class LedElement
{
	private:
		int convertIdx(int idx);

	protected:
		Adafruit_NeoPixel* pixels;
		int offset;
		int num_pixels;



	
	public:
		LedElement(Adafruit_NeoPixel* p, int o, int n);
		~LedElement() = default;
		virtual void lightPixel(int idx, uint32_t color);
		void lightSolid(uint32_t color);
		void clearPixels();
		void show();
		
		uint32_t generateColor(int r, int g, int b);
	
};

#endif //LEDELEMENT_H