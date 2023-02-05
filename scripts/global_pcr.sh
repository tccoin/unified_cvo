cd results
#773 816 10 0
exec ../../build/bin/cvo_global_pcr /home/link/unified_cvo/test_data/kitti_05_sample /home/link/unified_cvo/cvo_params/cvo_kitti_semantic.yaml /home/link/unified_cvo/results/cvo_intensity_img_gpu0_oct25_best/05.txt 773 816 3 1
exec ../../build/bin/cvo_global_pcr /home/link/unified_cvo/test_data/kitti_05_sample /home/link/unified_cvo/cvo_params/cvo_kitti.yaml /home/link/unified_cvo/results/cvo_intensity_img_gpu0_oct25_best/05.txt 773 816 3 0

exec ../../build/bin/cvo_global_pcr /home/link/unified_cvo/test_data/kitti_05_sample /home/link/unified_cvo/cvo_params/cvo_kitti.yaml /home/link/unified_cvo/results/cvo_intensity_img_gpu0_oct25_best/05.txt 790 795 1 0

exec ../../build/bin/cvo_global_pcr /home/link/unified_cvo/test_data/kitti_05_sample /home/link/unified_cvo/cvo_params/cvo_kitti_semantic.yaml /home/link/unified_cvo/results/cvo_intensity_img_gpu0_oct25_best/05.txt 790 795 1 0

# vino
export CUDA_VISIBLE_DEVICES=1
cd test_data/semantic && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti_semantic.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 593 1515 3 1 ../edges.txt
cd test_data/intensity && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 593 1515 3 0 ../edges_3.txt > log.txt
#test
cd test_data/test && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 614 1372 757 0 ../edges.txt > log.txt

cd test_data/test && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti_semantic.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 614 1372 757 1 ../edges.txt > log.txt

593 1350

cd test_data/test && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 593 1515 3 0 ../edges.txt > log.txt

# vino int=2
export CUDA_VISIBLE_DEVICES=1
cd test_data/semantic && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti_semantic.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 593 1515 2 1 ../edges_2.txt > log.txt
export CUDA_VISIBLE_DEVICES=1
cd test_data/test && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 593 1515 2 0 ../edges_2.txt > log.txt

# vino 4frame
cd test_data/test_4frames && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti_semantic.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 593 1515 3 1 ../edges_3.txt > log.txt
# vino 30frame
cd test_data/30frame && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti_semantic.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 30 1515 3 1 ../edges_3.txt > ../log.txt
cd test_data/8frame && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti_semantic.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 8 1515 3 1 ../edges_3.txt > ../log.txt
cd test_data/4frame && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti_semantic.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 4 1515 3 1 ../edges_3.txt > ../log.txt
cd test_data/2frame && exec ../../build/bin/cvo_global_pcr /kitti/05 ~/cvo_params/cvo_kitti_semantic.yaml ~/results/cvo_intensity_img_gpu0_oct25_best/05.txt 2 1515 3 1 ../edges_3.txt > ../log.txt

bash scripts/run_loop.sh 16 > test_data/16_intensity_c_ell_0.025_to_0.01_log.txt