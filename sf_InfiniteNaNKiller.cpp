static const char* const CLASS = "sf_InfiniteNaNKiller";
static const char* const HELP =
  "InfiniteNaNKiller.\n\n"
  "Removes any INFs and NaNs present in any channel.\n"
  "Samples pixels around INF/NaN to determine an appropriate value using the sampling matrix.\n"
  "INFs/NaNs defaults to \"default value\" if sampling is unsuccessful.\n\n"
  "Developed by: Bryan \"fox\" Dunkley";

#include "DDImage/PixelIop.h"
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"
#include "DDImage/Interest.h"
#include "DDImage/DDMath.h"
#include "DDImage/Convolve.h"

using namespace DD::Image;
using namespace std;

class sf_INFKillerOp : public PixelIop
{

private:
	float default_value;
	float sampleMatrixValues[9];
	ConvolveArray sampleMatrix;

public:
#if DD_IMAGE_VERSION_MAJOR >= 5
  sf_INFKillerOp(Node* node) : PixelIop(node)
#else
  sf_INFKillerOp() : PixelIop()
#endif
  {
	  default_value = 1.0f;
      sampleMatrixValues[0] = 1.0f;
	  sampleMatrixValues[1] = 1.0f;
	  sampleMatrixValues[2] = 1.0f;
	  sampleMatrixValues[3] = 1.0f;
	  sampleMatrixValues[4] = 0.0f;
	  sampleMatrixValues[5] = 1.0f;
	  sampleMatrixValues[6] = 1.0f;
	  sampleMatrixValues[7] = 1.0f;
	  sampleMatrixValues[8] = 1.0f;
	  sampleMatrix.set(3,3,sampleMatrixValues);
  }
  
  void in_channels(int input_number, ChannelSet& channels) const
  {

  }

  virtual void knobs(Knob_Callback);
  int knob_changed(Knob* k);
  const char* Class() const { return CLASS; }
  const char* node_help() const { return HELP; }
  const char* node_shape() const { return "[]";}
  static const Iop::Description d;
  void _validate(bool);
  void pixel_engine(const Row &in, int y, int x, int r, ChannelMask, Row &out);
};

void sf_INFKillerOp::knobs(Knob_Callback f)
{
	Float_knob(f, &default_value, "default_value", "default value");
	Tooltip(f, "Default inf/NaN value if pixel sampling fails.");
	Tab_knob(f, "Sampling");
	Array_knob(f, &sampleMatrix, 3, 3, "sampling_grid", "sampling grid", true);
    Tooltip(f, "Define the pixel sampling offset in each direction, with the center of the matrix being the inf/NaN found.");
}

int sf_INFKillerOp::knob_changed(Knob* k)
{
	return 1;
}

#if DD_IMAGE_VERSION_MAJOR >= 5
static Iop* build(Node* node) { return new sf_INFKillerOp(node); }
#else
static Iop* build() { return new sf_INFKillerOp(); }
#endif
const Iop::Description sf_INFKillerOp::d(CLASS, 0, build);

void sf_INFKillerOp::_validate(bool for_real)
{
    set_out_channels(Mask_All);
    PixelIop::_validate(for_real);
}

void sf_INFKillerOp::pixel_engine(const Row &in, int y, int x, int r, ChannelMask channels, Row &out)
{
	foreach (z, channels)
	{
		Interest pixels(input0(), z, false);

		const float* channelIn = in[z] + x;
	    float* channelOut = out.writable(z) + x;
	    const float* END = channelIn + (r - x);

	    int rowPos = x;

	    while (channelIn < END)
	    {
	    	float value = *channelIn++;
	    	float avg_temp = 0.0f;
			float temp;
			if (!isfinite(value) || isnan(value))
			{
    			float avgdenom = 0.0f;

				if (int(sampleMatrixValues[3]) != 0)
				{
					avgdenom++;
    				temp = pixels.at(rowPos - int(sampleMatrixValues[3]), y, z);
    				if (!isfinite(temp) || isnan(temp))
					{
    					temp = 0.0f;
    					avgdenom--;
					}
    				avg_temp += temp;
				}

				if (int(sampleMatrixValues[0]) != 0)
				{
					avgdenom++;
    				temp = pixels.at(rowPos - int(sampleMatrixValues[0]), y - int(sampleMatrixValues[0]), z);
    				if (!isfinite(temp) || isnan(temp))
    				{
    					temp = 0.0f;
    					avgdenom--;
    				}
    				avg_temp += temp;
				}

				if (int(sampleMatrixValues[6]) != 0)
				{
					avgdenom++;
    				temp = pixels.at(rowPos - int(sampleMatrixValues[6]) , y + int(sampleMatrixValues[6]), z);
    				if (!isfinite(temp) || isnan(temp))
    				{
						temp = 0.0f;
    					avgdenom--;
    				}
    				avg_temp += temp;
				}

				if (int(sampleMatrixValues[1]) != 0)
				{
					avgdenom++;
    				temp = pixels.at(rowPos, y - int(sampleMatrixValues[1]), z);
    				if (!isfinite(temp) || isnan(temp))
    				{
						temp = 0.0f;
    					avgdenom--;
    				}
    				avg_temp += temp;
				}

				if (int(sampleMatrixValues[7]) != 0)
				{
					avgdenom++;
    				temp = pixels.at(rowPos, y + int(sampleMatrixValues[7]), z);
    				if (!isfinite(temp) || isnan(temp))
    				{
    					temp = 0.0f;
    					avgdenom--;
    				}
    				avg_temp += temp;
				}

				if (int(sampleMatrixValues[5]) != 0)
				{
					avgdenom++;
    				temp = pixels.at(rowPos + int(sampleMatrixValues[5]), y, z);
    				if (!isfinite(temp) || isnan(temp))
    				{
    					temp = 0.0f;
    					avgdenom--;
    				}
    				avg_temp += temp;
				}

				if (int(sampleMatrixValues[2]) != 0)
				{
					avgdenom++;
    				temp = pixels.at(rowPos + int(sampleMatrixValues[2]), y - int(sampleMatrixValues[2]), z);
    				if (!isfinite(temp) || isnan(temp))
    				{
    					temp = 0.0f;
    					avgdenom--;
    				}
    				avg_temp += temp;
				}

				if (int(sampleMatrixValues[8]) != 0)
				{
					avgdenom++;
    				temp = pixels.at(rowPos + int(sampleMatrixValues[8]), y + int(sampleMatrixValues[8]), z);
    				if (!isfinite(temp) || isnan(temp))
    				{
    					temp = 0.0f;
    					avgdenom--;
    				}
    				avg_temp += temp;
				}

    			avg_temp /= avgdenom;

				if (!isfinite(avg_temp) || isnan(avg_temp))
    			{
					avg_temp = default_value;
    			}

	    		*channelOut++ =  avg_temp;
			}
	    	else
	    	{
	    		*channelOut++ = value;
	    	}

	    	rowPos++;
	    }
    }
}





