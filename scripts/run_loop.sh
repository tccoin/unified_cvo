# cd test_data/$1frame && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti_semantic.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt $1 1515 3 1 ../edges_3.txt
cd test_data/$1frame && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt $1 1515 3 0 ../edges_3.txt
