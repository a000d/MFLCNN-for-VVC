#pragma once

#include "TypeDef.h"
#include <vector>
// #include "../App/EncoderApp/fast_code/include/onnxruntime_cxx_api.h"
#include <algorithm>
#include <iostream>
#include <CodingStructure.h>
// #include <EncModeCtrl.h>
#include <dlfcn.h>

#include <chrono>
// #pragma comment(lib, "onnxruntime.lib")
// #pragma comment(lib, "onnxruntime_providers_shared.lib")





typedef std::chrono::high_resolution_clock Clock;

using namespace std;

struct cu_Size_Info
{
  int w;
  int h;
  int c;

  char         *w_char_buf = nullptr;
  vector<float> size_code;

  cu_Size_Info(int w, int h, int c, vector<float> size_code) : w(w), h(h), c(c), size_code(size_code) {}

  int Buf_Size() const { return w * h; }

  ~cu_Size_Info()
  {
    if (w_char_buf != nullptr)
    {
      delete (w_char_buf);
    }
  }
};

struct Net_Result
{
  bool mode_permit[6] = { 0, 0, 0, 0, 0, 0 };

  Net_Result() {}
  Net_Result(bool m0, bool m1, bool m2, bool m3, bool m4, bool m5)
  {
    mode_permit[0] = m0;
    mode_permit[1] = m1;
    mode_permit[2] = m2;
    mode_permit[3] = m3;
    mode_permit[4] = m4;
    mode_permit[5] = m5;
  }
  void Set_Permit(bool m0, bool m1, bool m2, bool m3, bool m4, bool m5)
  {
    mode_permit[0] = m0;
    mode_permit[1] = m1;
    mode_permit[2] = m2;
    mode_permit[3] = m3;
    mode_permit[4] = m4;
    mode_permit[5] = m5;
  }
};

enum MY_Split_Mode
{
  NONE  = -1,
  QT    = 0,
  VER_2 = 1,
  HOR_2 = 2,
  VER_3 = 3,
  HOR_3 = 4,
  NO    = 5
};

class Fast_Tool
{
    int qp_group = -1;
    std::string video;
    int         frames_tobe_encoded;

public:
  // 
  float *resi_buf;
  // 
  float *fea_20;
  // 
  float *result_buf;
private:
  void *handle;
  void (*Net_Run)(float *, float *, float *, int, int);
  void (*Net_Init)(int);

  vector<cu_Size_Info> cu_size_list = {
    { 128,  128,    4, { 0, 0, 0, 0 } }, 
    { 64,   128,    2, { 0, 0, 1, 0 } }, 
    { 128,  64,     2, { 0, 0, 0, 1 } },
    { 64,   64,     6, { 0, 0, 1, 1 } },   
    { 64,   32,     5, { 0, 1, 0, 0 } },  
    { 32,   64,     5, { 0, 1, 0, 1 } },
    { 16,   64,     5, { 0, 1, 1, 0 } },   
    { 64,   16,     5, { 0, 1, 1, 1 } },  
    { 32,   32,     6, { 1, 0, 0, 0 } },
    { 32,   16,     5, { 1, 0, 0, 1 } },   
    { 16,   32,     5, { 1, 0, 1, 0 } },  
    { 32,   8,      4, { 1, 0, 1, 1 } },
    { 8,    32,     4, { 1, 1, 0, 0 } },    
    { 16,   16,     6, { 1, 1, 0, 1 } },

  };
  vector<vector<int>> size_level_call_times;
  vector<vector<size_t>> size_level_time_spend;
 
public:
  Fast_Tool()
  {

    //-------------------------------------------------------
    for(int i=0;i<14;i++){
        size_level_call_times.push_back(vector<int>());
        for(int t=0;t<6;t++){
            size_level_call_times[i].push_back(0);
        }

    }
    for (int i = 0; i < 14; i++)
    {
        // size_level_time_spend.push_back({ 0, 0, 0, 0, 0, 0 });
        size_level_time_spend.push_back(vector<size_t>());
        for(int t=0;t<6;t++){
            size_level_time_spend[i].push_back(0);
        }
    }
    //
    fea_20 = new float[20];
    //
    resi_buf = new float[128 * 128];
    //
    result_buf = new float[6];

    handle = dlopen("libnet_lib2.so", RTLD_LAZY);
    if (!handle)
    {
      cout << "dll open faild " << endl;
      throw 0;
    }
    Net_Run  = (void (*)(float *, float *, float *, int, int)) dlsym(handle, "Net_Run");
    
    //-----------------------------------------------

    // Net_Init(22);

    // for(int i=0;i<20;i++){
    //     fea_20[i] = i;
    // }
    // for(int i=0;i<64 * 64;i++){
    //     resi_buf[i] = i;
    // }

    // Net_Run(resi_buf,fea_20,result_buf,3,16);

    //-----------------------------------------------
    cout<<"Fast Code Init Done"<<endl;
  }
  ~Fast_Tool()
  {
    delete (resi_buf);
    delete (result_buf);
    delete (fea_20);

    dlclose(handle);

    cout << endl;
    cout << "call times:" << endl;
    for(int i=0;i<14;i++){

        for(int g=1;g<6;g++){
            cout<<size_level_call_times[i][g]<<"\t\t";
        }
        cout<<endl;
        //   <<size_level_call_times[i][2]<<"\t\t"
        //   <<size_level_call_times[i][3]<<"\t\t"
        //   <<size_level_call_times[i][4]<<"\t\t"
        //   <<size_level_call_times[i][5]<<"\t\t"<<endl;

    }
    cout << endl;
    cout << "time_spend:" << endl;
    for (int i = 0; i < 14; i++)
    {
        for(int g=1;g<6;g++){
            cout<< size_level_time_spend[i][g] << "\t\t" ;
        }
        cout << endl;
        cout << endl;
        //   cout 
        //     << size_level_time_spend[i][1] << "\t\t" 
        //     << size_level_time_spend[i][2] << "\t\t"
        //     << size_level_time_spend[i][3] << "\t\t" 
        //     << size_level_time_spend[i][4] << "\t\t"
        //     << size_level_time_spend[i][5] << "\t\t" 
        //     << endl;
    }
   
  }


  void Set_Info(int qp_group, int frames_tobe_encoded, std::string video) 
  { 
    this->qp_group = qp_group; 
    this->video    = video;
    this->frames_tobe_encoded = frames_tobe_encoded;
  }
  
  static int Get_Level(const int pos)
  {
    const int level_list[32] = { -1, 5, 4, 5, 3, 5, 4, 5, 2, 5, 4, 5, 3, 5, 4, 5,
                                 1,  5, 4, 5, 3, 5, 4, 5, 2, 5, 4, 5, 3, 5, 4, 5 };
    int       local_pos      = pos % 32;
    if (local_pos == 0)
    {
      throw -1;
    }

    return level_list[local_pos];
  }


  static vector<int> Sort_Index(const float *list, const int size)
  {
    vector<int> id_list(size);

    for (int i = 0; i < size; i++)
    {
      id_list[i] = i;
    }

    std::sort(id_list.begin(), id_list.end(), [&](const int &a, const int &b) { return list[a] > list[b]; });

    return id_list;
  }

  static Net_Result Net_Id_to_VTM_Mode_Id(int size_index, const vector<int> &sort_id_list, int top_n,int level)
  {
    Net_Result result;
    switch (size_index)
    {
    case 0:   // 128x128
    {
        //if (sort_id_list[0] == 0)
        //{
        //    result.Set_Permit(1, 1, 1, 1, 1, 0);
        //}
        //else
        //{
        //    result.Set_Permit(0, 0, 0, 0, 0, 1);
        //}
        constexpr int remap[] = { 0, 1, 2, 5 };

        for (int i = 0; i < min(top_n,4); i++)
        {
            result.mode_permit[remap[sort_id_list[i]]] = true;
        }

        break;



      break;
    }
    case 1:   // 64x128
    {
      if (sort_id_list[0] == 0)
      {
        result.Set_Permit(1, 1, 1, 1, 1, 0);
      }
      else
      {
        result.Set_Permit(0, 0, 0, 0, 0, 1);
      }
     
      break;
    }
    case 2:   // 128x64
    {
      if (sort_id_list[0] == 0)
      {
        result.Set_Permit(1, 1, 1, 1, 1, 0);
      }
      else
      {
        result.Set_Permit(0, 0, 0, 0, 0, 1);
      }
      
      break;
    }
    case 3:   // 64x64
    {
        for (int i = 0; i < top_n; i++)
        {
            result.mode_permit[sort_id_list[i]] = true;
        }

        break;
    }
    case 4:   // 64x32
    {
      for (int i = 0; i < min(top_n,5); i++)     
      {
        result.mode_permit[sort_id_list[i] + 1] = true;
      }
      // result.mode_permit[sort_id_list[0] + 1] = 1;
      // result.mode_permit[sort_id_list[1] + 1] = 1;
      break;
    }
    case 5:   // 32x64
    {
      for (int i = 0; i < min(top_n,5); i++)
      {
        result.mode_permit[sort_id_list[i] + 1] = true;
      }
      // result.mode_permit[sort_id_list[0] + 1] = 1;
      // result.mode_permit[sort_id_list[1] + 1] = 1;
      break;
    }
    case 6:   // 16x64
    {
      for (int i = 0; i < min(top_n,5); i++)
      {
        result.mode_permit[sort_id_list[i] + 1] = true;
      }
      break;
    }
    case 7:   // 64x16
    {
      for (int i = 0; i < min(top_n,5); i++)
      {
        result.mode_permit[sort_id_list[i] + 1] = true;
      }
      break;
    }
    case 8:   // 32x32
    {
      for (int i = 0; i < top_n; i++)
      {
        result.mode_permit[sort_id_list[i]] = true;
      }
      break;
    }
    case 9:   // 32x16
    {
      for (int i = 0; i < min(top_n,5); i++)
      {
        result.mode_permit[sort_id_list[i] + 1] = true;
      }
      break;
    }
    case 10:   // 16x32
    {
      for (int i = 0; i < min(top_n,5); i++)
      {
        result.mode_permit[sort_id_list[i] + 1] = true;
      }
      break;
    }
    case 11:   // 32x8
    {
      constexpr int remap[] = { 1, 2, 3, 5 };

      for (int i = 0; i < min(top_n,4); i++)
      {
        result.mode_permit[remap[sort_id_list[i]]] = true;
      }

      break;
    }
    case 12:   // 8x32
    {
      constexpr int remap[] = { 1, 2, 4, 5 };   

      for (int i = 0; i < min(top_n,4); i++)
      {
        result.mode_permit[remap[sort_id_list[i]]] = true;
      }

      break;
    }
    case 13:   // 16x16
    {
      for (int i = 0; i < top_n; i++)
      {
        result.mode_permit[sort_id_list[i]] = true;
      }
      break;

    }

    default:
    {
      cout << "not tgt size " << size_index << endl;
      throw -1;
      break;
    }
    }

    return result;
  }

  int Size_Index(const int w, const int h) const
  {
    if (w == 128 && h == 128)
    {
      return 0;
    }
    if (w == 64 && h == 128)
    {
      return 1;
    }
    if (w == 128 && h == 64)
    {
      return 2;
    }
    if (w == 64 && h == 64)
    {
      return 3;
    }
    if (w == 64 && h == 32)
    {
      return 4;
    }
    if (w == 32 && h == 64)
    {
      return 5;
    }
    if (w == 16 && h == 64)
    {
      return 6;
    }
    if (w == 64 && h == 16)
    {
      return 7;
    }
    if (w == 32 && h == 32)
    {
      return 8;
    }
    //----
    if (w == 32 && h == 16)
    {
      return 9;
    }
    if (w == 16 && h == 32)
    {
      return 10;
    }
    if (w == 32 && h == 8)
    {
      return 11;
    }
    if (w == 8 && h == 32)
    {
      return 12;
    }
    if (w == 16 && h == 16)
    {
      return 13;
    }
    return 0;
    std::cout << "size not found  " << w << "  " << h << endl;
    throw 0;
  }

  Net_Result Run(int size_id, int frame_index)
  {
    
    int level = Get_Level(frame_index);

    Net_Run(resi_buf, fea_20, result_buf, size_id, frame_index);

    int         out_count    = cu_size_list[size_id].c;
    vector<int> sort_id_list = Sort_Index(result_buf, out_count);

    // constexpr float threshold = 0.9f;

    // int top_n = 0;
    // float reliability = 0;

    // for(int pos=0;pos<out_count;pos++){
    //     top_n++;
    //     reliability+=result_buf[sort_id_list[pos]];

    //     if(reliability>=threshold){
    //         break;
    //     }

    // }
    // top_n = max(top_n,2);
    // int top_n = 2;
    // if(level<=4){
    //     top_n = 3;
    // }
    // int top_n_list[] = {-1,3,3,3,3,3};
    // int top_n_list[] = {-1,3,3,3,3,2};
    int top_n_list[] = {-1,2,2,2,2,2};

    int top_n = top_n_list[level];

    Net_Result res = Net_Id_to_VTM_Mode_Id(size_id, sort_id_list, top_n,level);

    return res;
  }

  Net_Result Get_Mode(const CodingStructure *cs)
  {
    auto t1 = Clock::now();
    const int pic_poc  = cs->picture->poc;
    const int level    = Get_Level(pic_poc);

    const int cu_height = cs->area.lheight();
    const int cu_width  = cs->area.lwidth();
    //const int x         = cs->area.lx();
    //const int y         = cs->area.ly();

    const int parent_width  = cs->parent == nullptr ? 0 : cs->parent->area.lwidth();
    const int parent_height = cs->parent == nullptr ? 0 : cs->parent->area.lheight();

    const int size_id        = Size_Index(cu_width, cu_height);
    const int parent_size_id = Size_Index(parent_width, parent_height);

    CPelBuf   pred_buf    = cs->getPredBuf(COMPONENT_Y);
    const int pred_stride = pred_buf.stride;

    const CPelBuf    orig_buf    = cs->getOrgBuf(COMPONENT_Y);
    const int        orig_stride = orig_buf.stride;

    const Pel *p_pred_buf     = pred_buf.buf;
    const Pel *local_orig_buf = orig_buf.buf;

    
    int      QP       = *cs->currQP;
    double   cost     = cs->cost;
    uint64_t fracBits = cs->fracBits;
    uint64_t dist     = cs->dist;

#define f(c) ((float) c)

    float cu_depth             = (float) cs->cus[0]->depth;
    float cu_qt_depth          = (float) cs->cus[0]->qtDepth;
    float cu_bt_depth          = (float) cs->cus[0]->btDepth;
    float cu_mt_depth          = (float) cs->cus[0]->mtDepth;
    float cu_skip              = cs->cus[0]->skip ? f(1) : f(0);
    float cu_mmvd_skip         = cs->cus[0]->mmvdSkip ? f(1) : f(0);
    float cu_affine            = cs->cus[0]->affine ? f(1) : f(0);
    float cu_geoflag           = cs->cus[0]->geoFlag ? f(1) : f(0);
    float ipd_mergeflag        = cs->pus[0]->mergeFlag ? f(1) : f(0);
    float ipd_regularmergeflag = cs->pus[0]->regularMergeFlag ? f(1) : f(0);
    float ipd_mmvdmergeflag    = cs->pus[0]->mmvdMergeFlag ? f(1) : f(0);

    double dt = (double) dist;
    double ft = (double) fracBits;

    //
    fea_20[0]  = ((float) QP / 51) + 0.5f;
    fea_20[1]  = f(dt / 10000000);
    fea_20[2]  = f(ft / 40000000);
    fea_20[3]  = ((float) qp_group / 51) + 0.5f;
    fea_20[4]  = f(cost / 1000000000);

    fea_20[5]  = cu_depth;
    fea_20[6]  = cu_qt_depth;
    fea_20[7]  = cu_bt_depth;
    fea_20[8]  = cu_mt_depth;
    
    fea_20[9]  = cu_skip;
    fea_20[10] = cu_mmvd_skip;
    fea_20[11] = cu_affine;
    fea_20[12] = cu_geoflag;
    fea_20[13] = ipd_mergeflag;
    fea_20[14] = ipd_regularmergeflag;
    fea_20[15] = ipd_mmvdmergeflag;

    const vector<float> &parent_size_code = cu_size_list[parent_size_id].size_code;
    //fea_20[16]                            = parent_size_code[0];
    //fea_20[17]                            = parent_size_code[1];
    //fea_20[18]                            = parent_size_code[2];
    //fea_20[19]                            = parent_size_code[3];
    fea_20[16] = 0;
    fea_20[17] = 0;
    fea_20[18] = 0;
    fea_20[19] = 0;


    for (int y = 0; y < cu_height; y++)
    {
      for (int x = 0; x < cu_width; x++)
      {
        resi_buf[y * cu_width + x] = 
        abs(
            f(
                local_orig_buf[y * orig_stride + x] - p_pred_buf[y * pred_stride + x]
                
            )
        )/ 4;
        
    
      }
    }

    Net_Result res = Run(size_id, pic_poc);
    size_level_call_times[size_id][level]++;

    auto t2 = Clock::now();
    size_t time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    size_level_time_spend[size_id][level] += time;

    return res;
  }

  static bool Is_Tgt_Size(int w, int h)
  {

    if (w >= 16 && h >= 16)
    {
      return true;
    }
    //---
    else if (w == 32 && h == 8)
    {
      return true;
    }
    else if (w == 8 && h == 32)
    {
      return true;
    }
    //---
    return false;
  }

  static MY_Split_Mode Get_External_Mode(EncTestModeType mode)
  {
    switch (mode)
    {
    case (ETM_SPLIT_QT):
    {
      return MY_Split_Mode::QT;
    }
    case (ETM_SPLIT_BT_H):
    {
      return MY_Split_Mode::HOR_2;
    }
    case (ETM_SPLIT_BT_V):
    {
      return MY_Split_Mode::VER_2;
    }
    case (ETM_SPLIT_TT_H):
    {
      return MY_Split_Mode::HOR_3;
    }
    case (ETM_SPLIT_TT_V):
    {
      return MY_Split_Mode::VER_3;
    }
    default:
    {
      return MY_Split_Mode::NO;
    }
    }
  }

};
