#pragma once
#include "onnxruntime_cxx_api.h"
#include <string.h>
#include <algorithm>
#include <iostream>

using namespace std;
struct cu_Size_Info
{
    int w;
    int h;
    int c;

    char* w_char_buf = nullptr;
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
    NONE = -1,
    QT = 0,
    VER_2 = 1,
    HOR_2 = 2,
    VER_3 = 3,
    HOR_3 = 4,
    NO = 5
};


class Fast_Tool
{
    // 残差
    float* resi_buf;
    // 辅助特征
    float* fea_20;
    // 传递特征空间
    float* fea_trans;
    // 结果值
    float* result_buf;
    // 结果传递特征空间
    float* fea_trans_out;

    Ort::Env* env;
    Ort::SessionOptions session_options;

    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    Ort::Session* session_list[6][16];

    std::vector<const char*> input_node_names{ "cu", "fea" ,"fea_trans"};
    std::vector<const char*> output_node_names{ "result" ,"trans" };

    std::vector<std::vector<std::vector<int64_t>>> input_node_dims_vector_list;
    std::vector<std::vector<std::vector<int64_t>>> output_node_dims_vector_list;

    vector<cu_Size_Info> cu_size_list = {
      { 128, 128, 2, { 0, 0, 0, 0 } },
      { 64, 128, 2,  { 0, 0, 1, 0 } },
      { 128, 64, 2,  { 0, 0, 0, 1 } },
      { 64, 64, 6,   { 0, 0, 1, 1 } },
      { 64, 32, 5,   { 0, 1, 0, 0 } },
      { 32, 64, 5,   { 0, 1, 0, 1 } },
      { 16, 64, 5,   { 0, 1, 1, 0 } },
      { 64, 16, 5,   { 0, 1, 1, 1 } },
      { 32, 32, 6,   { 1, 0, 0, 0 } },
      { 32, 16, 5,   { 1, 0, 0, 1 } },
      { 16, 32, 5,   { 1, 0, 1, 0 } },
      { 32, 8, 4,    { 1, 0, 1, 1 } },
      { 8, 32, 4,    { 1, 1, 0, 0 } },
      { 16, 16, 6,   { 1, 1, 0, 1 } },
      { 64, 8, 4,    { 1, 1, 0, 1 } },
      { 8, 64, 4,    { 1, 1, 0, 1 } },

    };

public:
    Fast_Tool()
    {
        env = new Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_ERROR, "test");
        session_options.SetIntraOpNumThreads(1);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

        for (int i = 0; i < 24; i++)
        {
            input_node_dims_vector_list.push_back({ { 1, 1, cu_size_list[i].h, cu_size_list[i].w }, { 1, 20 }, { 1, 32 } });
            output_node_dims_vector_list.push_back({ { 1, cu_size_list[i].c }, { 1, 32 } });
        };

        for (int g = 1; g <= 5; g++)
        {
            for (int i = 0; i < 14; i++)
            {
                //wchar_t* name = cu_size_list[i].Onnx_File("./weight/", l);

                string name = "./weight/nn_" + std::to_string(g) + "_" + std::to_string(cu_size_list[i].w) + "_" + std::to_string(cu_size_list[i].h) + ".onnx";

                session_list[g][i] = new Ort::Session(*env, name.c_str(), session_options);
            };

        }

        fea_trans = new float[32];
        fea_trans_out = new float[32];
        for (int i = 0; i < 32;i++) {
            fea_trans[i] = 0;
            fea_trans_out[i] = 0;
        }
    }

    static int Get_Level(const int pos)
    {
        const int level_list[32] = { -1, 5, 4, 5, 3, 5, 4, 5, 2, 5, 4, 5, 3, 5, 4, 5,
                                     1,  5, 4, 5, 3, 5, 4, 5, 2, 5, 4, 5, 3, 5, 4, 5 };
        int       local_pos = pos % 32;
        if (local_pos == 0)
        {
            throw - 1;
        }
    
        return level_list[local_pos];
    }
    /*
    static int Get_Ref_Group(const int pos) {

        int local_pos = pos % 32;

        const vector<vector<int>> group_frame_list = {
            {16},
            {8},
            {24},
            {4},
            {12,20},
            {28},
            {2},
            {6,14,22},
            {10,18,26},
            {30},
            {1},
            {3,11,19,27},
            {5,13,21,29},
            {7,15,23},
            {9,17,25},
            {31},
        };

        for (int i = 0; i < 16; i++) {
            const vector<int>& frame_list = group_frame_list[i];

            for (int v : frame_list) {
                if (v == local_pos) {
                    return i;
                }
            }
        }
        cout << "can not find the group.  pos:" << pos << endl;
        throw 0;



    }
    */


    void Run(int size_id, int frame_index)
    {

        const int group = Get_Level(frame_index);

        Ort::Session* session = session_list[group][size_id];

        const std::vector<std::vector<int64_t>>& input_node_dims_vector = input_node_dims_vector_list[size_id];
        const std::vector<std::vector<int64_t>>& output_node_dims_vector = output_node_dims_vector_list[size_id];

        std::vector<Ort::Value> input_list;

        input_list.push_back(
            Ort::Value::CreateTensor<float>(
                memory_info, resi_buf, (size_t)cu_size_list[size_id].Buf_Size() * 4,
                input_node_dims_vector[0].data(), input_node_dims_vector[0].size()));

        input_list.push_back(Ort::Value::CreateTensor<float>(
                memory_info, fea_20, (size_t)(20 * 4), 
                input_node_dims_vector[1].data(), input_node_dims_vector[1].size()));

        input_list.push_back(Ort::Value::CreateTensor<float>(
                memory_info, fea_trans, (size_t)(32 * 4),
                input_node_dims_vector[2].data(), input_node_dims_vector[2].size()));



        std::vector<Ort::Value> output_list;

        output_list.push_back(
            Ort::Value::CreateTensor<float>(memory_info, result_buf, (size_t)(cu_size_list[size_id].c * 4),
                output_node_dims_vector[0].data(), output_node_dims_vector[0].size()));

        output_list.push_back(
            Ort::Value::CreateTensor<float>(memory_info, fea_trans_out, (size_t)(32 * 4),
                output_node_dims_vector[1].data(), output_node_dims_vector[1].size()));



        session->Run(Ort::RunOptions{ nullptr }, input_node_names.data(), input_list.data(), input_list.size(),
            output_node_names.data(), output_list.data(), output_list.size());


  
    }

    void Get_Mode(float* resi_buf, float* fea_20,float* result,int size_id, int pic_poc)
    {
        this->resi_buf = resi_buf;
        this->fea_20 = fea_20;
        this->result_buf = result;

        Run(size_id, pic_poc);


    }


    ~Fast_Tool()
    {
        delete(fea_trans_out);
        delete(fea_trans);
        //delete (resi_buf);
        //delete (result_buf);
        //delete (fea_20);
    }



};


