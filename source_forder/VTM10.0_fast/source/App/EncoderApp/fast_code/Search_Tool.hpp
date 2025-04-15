
/*
#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <string>

using namespace std;

#define B_Size 4

//typedef       int16_t           Pel;
template<typename Pel> 
class Search_Tool {

private:
	vector<vector<int>> search_offset;
	//----------------------------------------------------------------------------------------------
	Pel* ref_frame_buf;
	int ref_frame_width;
	int ref_frame_height;
	int ref_frame_stride;

	Pel* orig_frame_buf;
	int orig_frame_width;
	int orig_frame_height;
	int orig_frame_stride;
	//----------------------------------------------------------------------------------------------
	
	Pel* b_4x4;
	int b_x;
	int b_y;
	
public:
	Search_Tool() {
		search_offset = {
			{0,0},
			{0,1},
			{0,-1},
			{1,0},
			{-1,0},
		};
		vector<vector<int>> tmp_search_offset{
			{0,2},
			{0,-2},
			{2,0},
			{-2,0},
			{1,1},
			{1,-1},
			{-1,1},
			{-1,-1},
		};
		for (int r = 0; r < 5; r++) {
			for (int i = 0; i < 8; i++) {
				search_offset.push_back(tmp_search_offset[i]);
			}
			for (int i = 0; i < 8; i++) {
				tmp_search_offset[i][0] *= 2;
				tmp_search_offset[i][1] *= 2;
			}

		}
	}
	~Search_Tool() {

	}
private:
	Pel& Get_Block_Buf(int x, int y){

		//if (b_x + x>= orig_frame_width) {
		//	return 0;
		//}
		//if (b_y + y >= orig_frame_height) {
		//	return 0;
		//}
		return b_4x4[y * orig_frame_stride + x];

	};
	Pel& Get_Pic_Buf(int x, int y){

		return ref_frame_buf[y * ref_frame_stride + x];

	};

	float Calu_Err(int pic_x, int pic_y){
		float sum = 0;

		for (int y = 0; y < B_Size+8; y+=2) {
			for (int x = 0; x < B_Size+8; x+=2) {

				auto a = Get_Block_Buf(x, y);

				auto b = Get_Pic_Buf(pic_x + x, pic_y + y);

				sum += abs(a - b);

			}
		}
		return sum;
	};

	float Search(int start_x, int start_y, int* best_pos_x, int* best_pos_y) {
		float best_err = FLT_MAX;


		int offset_count = search_offset.size();


		for (int i = 0; i < offset_count; i++) {
			int pos_y = search_offset[i][1] + start_y;
			int pos_x = search_offset[i][0] + start_x;

			if (pos_y < 0 || pos_y + B_Size+8 >= ref_frame_height) {
				continue;
			}
			if (pos_x < 0 || pos_x + B_Size+8 >= ref_frame_width) {
				continue;
			}
			int err = Calu_Err(pos_x, pos_y);

			if (best_err > err) {
				best_err = err;
				*best_pos_y = pos_y;
				*best_pos_x = pos_x;

			}

		}
		return best_err;

	}
	float Search_Proc(int start_x, int start_y, int* best_pos_x, int* best_pos_y){

		int err;

		int current_best_pos_x = start_x;
		int current_best_pos_y = start_y;

		for (int round = 0; round < 5;round++) {

			int new_pos_x;
			int new_pos_y;

			err = Search(current_best_pos_x, current_best_pos_y, &new_pos_x, &new_pos_y);

			if (current_best_pos_x == new_pos_x && current_best_pos_y == new_pos_y) {
				break;
			}

			current_best_pos_x = new_pos_x; 
			current_best_pos_y = new_pos_y;
		}
		*best_pos_x = current_best_pos_x;
		*best_pos_y = current_best_pos_y;

		return err;

	}

public:
	void Set_Ref_Info(Pel* buf,int w,int h,int s) {
		this->ref_frame_buf = buf;
		this->ref_frame_width = w;
		this->ref_frame_height = h;
		this->ref_frame_stride = s;

	}
	void Set_Orig_Info(Pel* buf, int w, int h, int s) {
		this->orig_frame_buf = buf;
		this->orig_frame_width = w;
		this->orig_frame_height = h;
		this->orig_frame_stride = s;

	}

	void Get_CTU_4x4_Motion(int ctu_x,int ctu_y,float* ver_buf, float* hor_buf, float* err_buf) {
	
		for (int y_id = 0; y_id < 32;y_id++) {
			for (int x_id = 0; x_id < 32;x_id++) {
	
				b_4x4 = orig_frame_buf + (ctu_y + y_id * 4) * orig_frame_stride + ctu_x + x_id * 4;
				int b_4x4_x = x_id * 4 + ctu_x;
				int b_4x4_y = y_id * 4 + ctu_y;
	
				int best_pos_x;
				int best_pos_y;
	
				float err = Search_Proc(b_4x4_x, b_4x4_y, &best_pos_x, &best_pos_y);
	
				ver_buf[y_id * 32 + x_id] = best_pos_y - b_4x4_y;
				hor_buf[y_id * 32 + x_id] = best_pos_x - b_4x4_x;
				err_buf[y_id * 32 + x_id] = err;
			}
	
	
		}
	}

	void Get_4x4_Motion(int x,int y,float* ver,float* hor,float* err) {
		
		b_4x4 = orig_frame_buf + y * orig_frame_stride + x;
		b_x = x;
		b_y = y;


		int best_pos_x=0;
		int best_pos_y=0;

		*err = Search_Proc(x, y, &best_pos_x, &best_pos_y);

		*ver = best_pos_y - y;
		*hor = best_pos_x - x;

	}






};
*/