
for i in tum_freiburg1_desk2 #tum_freiburg1_desk2 tum_freiburg1_room
do
    # /media/sdg1/rzh/
	./build/bin/stack_pcd_viewer_offline tartanair /dataset/tartanair/soulcity/Easy/P001 ./results/tartanair/rkhs/soulcity_rkhs_rgbd_kitti.txt 0 0
   #./build/bin/stack_pcd_viewer /home/rayzhang/dsm/sunny_results/${i}_$date 2000 0 0
   #./build/bin/stack_pcd_viewer /home/rayzhang/dsm/tum_fr1_desk2 2000 0 0
   #./build/bin/stack_pcd_viewer /home/rayzhang/dsm/ 2000 0 0
   #cat /home/rayzhang/dsm/$i/${i}_graph.txt
   #./build/bin/stack_pcd_viewer /home/rayzhang/dsm/ 0 0 0
done