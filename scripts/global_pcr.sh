cd results
#773 816 10 0
exec ../../build/bin/cvo_global_pcr /home/link/unified_cvo/test_data/kitti_05_sample /home/link/unified_cvo/cvo_params/cvo_kitti_semantic.yaml /home/link/unified_cvo/results/cvo_intensity_img_gpu0_oct25_best/05.txt 773 816 3 1
exec ../../build/bin/cvo_global_pcr /home/link/unified_cvo/test_data/kitti_05_sample /home/link/unified_cvo/cvo_params/cvo_kitti.yaml /home/link/unified_cvo/results/cvo_intensity_img_gpu0_oct25_best/05.txt 773 816 3 0

exec ../../build/bin/cvo_global_pcr /home/link/unified_cvo/test_data/kitti_05_sample /home/link/unified_cvo/cvo_params/cvo_kitti.yaml /home/link/unified_cvo/results/cvo_intensity_img_gpu0_oct25_best/05.txt 790 795 1 0

exec ../../build/bin/cvo_global_pcr /home/link/unified_cvo/test_data/kitti_05_sample /home/link/unified_cvo/cvo_params/cvo_kitti_semantic.yaml /home/link/unified_cvo/results/cvo_intensity_img_gpu0_oct25_best/05.txt 790 795 1 0

# vino
export CUDA_VISIBLE_DEVICES=1
cd test_data/semantic && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti_semantic.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 593 1515 3 1 ../edges.txt
cd test_data/intensity && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 593 1515 3 0 ../edges.txt
#test
cd test_data/intensity && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 593 1351 757 0