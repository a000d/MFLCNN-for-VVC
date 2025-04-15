import argparse
parser = argparse.ArgumentParser(description='Shell Generater ')
parser.add_argument('-i', '--input', help='CTC Squences path', required=False,default="./VVCSeq/")

args = parser.parse_args()

video_list = [
    args.input + "/A1/10bit/Campfire_3840x2160_30fps_10bit_420_bt709_videoRange.yuv",
    args.input + "/A1/10bit/FoodMarket4_3840x2160_60fps_10bit_420.yuv",
    args.input + "/A1/10bit/Tango2_3840x2160_60fps_10bit_420.yuv",
    args.input + "/A2/10bit/CatRobot_3840x2160_60fps_10bit_420_jvet.yuv",
    args.input + "/A2/10bit/DaylightRoad_3840x2160_60fps_10bit_420_jvet.yuv",
    args.input + "/A2/10bit/ParkRunning3_3840x2160_50fps_10bit_420.yuv",
    args.input + "/B/BasketballDrive_1920x1080_50.yuv",
    args.input + "/B/BQTerrace_1920x1080_60.yuv",
    args.input + "/B/Cactus_1920x1080_50.yuv",
    args.input + "/B/MarketPlace_1920x1080_60fps_8bit_420.yuv",
    args.input + "/B/RitualDance_1920x1080_60fps_8bit_420.yuv",
    args.input + "/C/BasketballDrill_832x480_50.yuv",
    args.input + "/C/BQMall_832x480_60.yuv",
    args.input + "/C/PartyScene_832x480_50.yuv",
    args.input + "/C/RaceHorsesC_832x480_30.yuv",
    args.input + "/D/BasketballPass_416x240_50.yuv",
    args.input + "/D/BlowingBubbles_416x240_50.yuv",
    args.input + "/D/BQSquare_416x240_60.yuv",
    args.input + "/D/RaceHorses_416x240_30.yuv",
    args.input + "/E/FourPeople_1280x720_60.yuv",
    args.input + "/E/Johnny_1280x720_60.yuv",
    args.input + "/E/KristenAndSara_1280x720_60.yuv",
    args.input + "/F/BasketballDrillText_832x480_50.yuv",
    args.input + "/F/SlideEditing_1280x720_30.yuv",
    args.input + "/F/SlideShow_1280x720_20.yuv"
]

cfg_list =[
    "./cfg/Campfire.cfg",
    "./cfg/FoodMarket4.cfg",
    "./cfg/Tango2.cfg",
    "./cfg/CatRobot.cfg",
    "./cfg/DaylightRoad2.cfg",
    "./cfg/ParkRunning3.cfg",
    "./cfg/BasketballDrive.cfg",
    "./cfg/BQTerrace.cfg",
    "./cfg/Cactus.cfg",
    "./cfg/MarketPlace.cfg",
    "./cfg/RitualDance.cfg",
    "./cfg/BasketballDrill.cfg",
    "./cfg/BQMall.cfg",
    "./cfg/PartyScene.cfg",
    "./cfg/RaceHorsesC.cfg",
    "./cfg/BasketballPass.cfg",
    "./cfg/BlowingBubbles.cfg",
    "./cfg/BQSquare.cfg",
    "./cfg/RaceHorses.cfg",
    "./cfg/FourPeople.cfg",
    "./cfg/Johnny.cfg",
    "./cfg/KristenAndSara.cfg",
    "./cfg/BasketballDrillText.cfg",
    "./cfg/SlideEditing.cfg",
    "./cfg/SlideShow.cfg",
    
]
import os



sh_id = 0


for exe in [
            ('EncoderApp_100_fast','100_64_fast'),
            ('EncoderApp_100_medium','100_64_medium'),
            ('EncoderApp_100_slow','100_64_slow'),
            
            ]:
    
    try:
        os.makedirs(f"./{exe[1]}/bin")
    except:
        pass
    try:
        os.makedirs(f"./sh/")
    except:
        pass

    for qp in [22,27,32,37]:
        # for i in [24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1]:
        for i in range(25):

            cfg = cfg_list[i]
            video = video_list[i]
            v_name = video.split("/")[-1].split(".")[0]
            
            with open(f"./sh/{sh_id}.sh","w") as f:
                
                f.write(f'FIND_FILE="./{exe[1]}/{qp}_{v_name}.txt"\n')
                f.write(f'FIND_STR="finished"\n')
                f.write(f"if [ `grep -c \"$FIND_STR\" $FIND_FILE` -ne '0' ];then\n")
                f.write(f"  exit 0\n")
                f.write(f"fi\n")
                f.write(f"\n\n")

                f.write(f"./{exe[0]} -c 100_encoder_randomaccess_vtm_gop32.cfg -c {cfg} -q {qp} -f 64  -i {video} -b ./{exe[1]}/bin/{qp}_{v_name}.bin > ./{exe[1]}/{qp}_{v_name}.txt")
                
                
            
            sh_id+=1





