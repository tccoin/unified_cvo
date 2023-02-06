
for i in tum_freiburg1_desk2 #tum_freiburg1_desk2 tum_freiburg1_room
do
    # /media/sdg1/rzh/
    ./build/bin/stack_pcd_viewer_offline tartanair /dataset/tartanair/soulcity/Easy/P001 ./results/ tartanair/rkhs/soulcity_rkhs_rgbd_kitti.txt 0 10
    #./build/bin/stack_pcd_viewer /home/rayzhang/dsm/sunny_results/${i}_$date 2000 0 0
    #./build/bin/stack_pcd_viewer /home/rayzhang/dsm/tum_fr1_desk2 2000 0 0
    #./build/bin/stack_pcd_viewer /home/rayzhang/dsm/ 2000 0 0
    #cat /home/rayzhang/dsm/$i/${i}_graph.txt
    #./build/bin/stack_pcd_viewer /home/rayzhang/dsm/ 0 0 0
done

# docker run -it --net=host --gpus all  -e DISPLAY=${DISPLAY} \
#   -e QT_GRAPHICSSYSTEM=native \
#   -e NVIDIA_DRIVER_CAPABILITIES=all \
#   -e XAUTHORITY \
#   -v /etc/group:/etc/group:ro \
#   -u $(id -u):$(id -g) --workdir="/home/$USER/" \
#   -v "/tmp/.X11-unix:/tmp/.X11-unix:rw"  -v "/etc/passwd:/etc/passwd:rw" -v  "/etc/group:/etc/group:ro"  -v "/etc/shadow:/etc/shadow:ro" -e "TERM=xterm-256color" \
#   --device /dev/nvidia0 --device /dev/nvidia-modeset --device /dev/nvidiactl --device /dev/nvidia-uvm --device /dev/nvidia-uvm-tools  \
#   -v "/home/link/Projects/:/home/$USER/" \
#   -v /run/media/link/Samsung_T5/:/dataset \
#   --name=cvo_stack  umcurly/cvo:latest