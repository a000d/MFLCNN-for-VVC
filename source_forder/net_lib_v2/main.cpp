#include <cstdio>
#include "onnxruntime_cxx_api.h"
#include "Fast_Tool.hpp"

#define DLL_EXP extern "C"

Fast_Tool fast_tool;
 

DLL_EXP void Net_Run(float* resi, float* fea_20,float* result_buf,int size_id,int pic_poc) {


    fast_tool.Get_Mode(resi, fea_20, result_buf, size_id, pic_poc);


}



int main()
{

    return 0;
}