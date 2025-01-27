#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <boost/filesystem.hpp>
//#include "dataset_handler/KittiHandler.hpp"
#include "utils/CvoPoint.hpp"
#include "utils/ImageStereo.hpp"
#include "utils/Calibration.hpp"
#include "utils/CvoPointCloud.hpp"
#include "cvo/CvoGPU.hpp"
#include "cvo/CvoParams.hpp"
#include "cvo/IRLS_State_CPU.hpp"
#include "cvo/IRLS_State.hpp"
#include "utils/PointSegmentedDistribution.hpp"
#include "utils/VoxelMap.hpp"
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
//#include <boost/shared_ptr.hpp>
#include <pcl/impl/point_types.hpp>



using namespace std;
using namespace boost::filesystem;

extern template class cvo::VoxelMap<pcl::PointXYZRGB>;
extern template class cvo::Voxel<pcl::PointXYZRGB>;





Eigen::Vector3f get_pc_mean(const cvo::CvoPointCloud & pc) {
  Eigen::Vector3f p_mean_tmp = Eigen::Vector3f::Zero();
  for (int k = 0; k < pc.num_points(); k++)
    p_mean_tmp = (p_mean_tmp + pc.positions()[k]).eval();
  p_mean_tmp = (p_mean_tmp) / pc.num_points();    
  return p_mean_tmp;
}


void read_semantics_pc(pcl::PointCloud<pcl::PointXYZ> & pc,
                       float * semantics,
                       std::shared_ptr<cvo::CvoPointCloud> pc_out) {
  if (semantics == nullptr || semantics + pc.size() -1 == nullptr ) {
    std::cout<<"semantics file ptr is nullptr\n";
    return;
  }
    
  if (pc.size() == 0) {
    std::cout<<"read emtpy pc file\n";
  }

  pc_out->reserve(pc.size(), FEATURE_DIMENSIONS, NUM_CLASSES);
  for (int i = 0; i < pc.size(); i++) {
    float * curr_semantic_ptr = semantics + i * NUM_CLASSES;
    Eigen::VectorXf semantic_vector(NUM_CLASSES);
    memcpy(semantic_vector.data(), curr_semantic_ptr, sizeof(float)*NUM_CLASSES);
    if ( pc.size() -1 == i) {
      std::cout<<"p[-1]: raw semantic is "<<*(curr_semantic_ptr+NUM_CLASSES-1)<<" and copied to"<<  semantic_vector(NUM_CLASSES-1)<<"\n";
    }
    Eigen::VectorXf feature_vec(FEATURE_DIMENSIONS);
    feature_vec.setZero();
    Eigen::Vector2f geoType;
    geoType(0) = 0;
    geoType(1) = 1;
    Eigen::Vector3f xyz = pc[i].getVector3fMap();
    pc_out->add_point(i, xyz, feature_vec, semantic_vector, geoType);
  }
  return;
}

int main(int argc, char *argv[]) {
  // list all files in current directory.
  //You could put any file path in here, e.g. "/home/me/mwah" to list that directory
  //cvo::KittiHandler kitti(argv[1], 0);
  std::string source_file(argv[1]);
  std::string target_file(argv[2]);
  std::string source_semantics(argv[3]);
  std::string target_semantics(argv[4]);
  string cvo_param_file(argv[5]);
  float ell = -1;
  if (argc > 6)
	  ell = std::stof(argv[6]);
  pcl::PointCloud<pcl::PointXYZ>::Ptr source_xyz_pcd(new pcl::PointCloud<pcl::PointXYZ>);
  std::shared_ptr<cvo::CvoPointCloud> source(new cvo::CvoPointCloud(FEATURE_DIMENSIONS, NUM_CLASSES));  
  pcl::io::loadPCDFile(source_file, *source_xyz_pcd);
  std::ifstream source_input_semantics( source_semantics, std::ios::binary );
  std::vector<char> source_semantic_ptr(std::istreambuf_iterator<char>(source_input_semantics), {});
  //assert(source_semantic_ptr.size() * sizeof(char) == source_xyz_pcd->size() * NUM_CLASSES * sizeof(float));
  read_semantics_pc(*source_xyz_pcd, reinterpret_cast<float*>(source_semantic_ptr.data()),
                    source);
  Eigen::Vector3f source_mean = get_pc_mean(*source);

  pcl::PointCloud<pcl::PointXYZ>::Ptr target_xyz_pcd(new pcl::PointCloud<pcl::PointXYZ>);
  std::shared_ptr<cvo::CvoPointCloud> target(new cvo::CvoPointCloud(FEATURE_DIMENSIONS, NUM_CLASSES));  
  pcl::io::loadPCDFile(target_file, *target_xyz_pcd);
  std::ifstream target_input_semantics( target_semantics, std::ios::binary );
  std::vector<char> target_semantic_ptr(std::istreambuf_iterator<char>(target_input_semantics), {});
  read_semantics_pc(*target_xyz_pcd, reinterpret_cast<float*>(target_semantic_ptr.data()),
                    target);
  //assert(target_semantic_ptr.size() * sizeof(char) == target_xyz_pcd->size() * NUM_CLASSES * sizeof(float));
  Eigen::Vector3f target_mean = get_pc_mean(*target);
  

  float dist = (source_mean - target_mean).norm();
  std::cout<<"source mean is "<<source_mean<<", target mean is "<<target_mean<<", dist is "<<dist<<std::endl;
  cvo::CvoGPU cvo_align(cvo_param_file );
  cvo::CvoParams & init_param = cvo_align.get_params();
  init_param.ell_init = dist; //init_param.ell_init_first_frame;
  if (argc > 6)
    init_param.ell_init = ell;
  init_param.ell_decay_rate = init_param.ell_decay_rate_first_frame;
  init_param.ell_decay_start  = init_param.ell_decay_start_first_frame;
  init_param.is_using_intensity = 0;
  init_param.is_using_semantics = 1;
  cvo_align.write_params(&init_param);

  std::cout<<"write ell! ell init is "<<cvo_align.get_params().ell_init<<std::endl;

  Eigen::Matrix4f init_guess = Eigen::Matrix4f::Identity();  // from source frame to the target frame


  Eigen::Matrix4f result, init_guess_inv;
  Eigen::Matrix4f identity_init = Eigen::Matrix4f::Identity();
  init_guess_inv = init_guess.inverse();    

  printf("Start align... num_fixed is %d, num_moving is %d\n", source->num_points(), target->num_points());
  std::cout<<std::flush;

  double this_time = 0;
  cvo_align.align(*source, *target, init_guess_inv, result, nullptr,&this_time);


  
  //cvo_align.align(*source, *target, init_guess, result);
    
  std::cout<<"Transform is "<<result <<"\n\n";
  pcl::PointCloud<pcl::PointXYZ> pcd_old, pcd_new;
  cvo::CvoPointCloud new_pc(FEATURE_DIMENSIONS, NUM_CLASSES), old_pc(FEATURE_DIMENSIONS, NUM_CLASSES);
  cvo::CvoPointCloud::transform(init_guess, * target, old_pc);
  cvo::CvoPointCloud::transform(result, *target, new_pc);
  std::cout<<"Just finished transform\n";
  cvo::CvoPointCloud sum_old = old_pc + *source;
  cvo::CvoPointCloud sum_new = new_pc  + *source ;
  std::cout<<"Just finished CvoPointCloud concatenation\n";
  std::cout<<"num of points before and after alignment is "<<sum_old.num_points()<<", "<<sum_new.num_points()<<"\n";
  sum_old.export_to_pcd(pcd_old);
  sum_new.export_to_pcd(pcd_new);
  std::cout<<"Just export to pcd\n";
  std::string fname("before_align.pcd");
  pcl::io::savePCDFileASCII(fname, pcd_old);
  fname= "after_align.pcd";
  pcl::io::savePCDFileASCII(fname, pcd_new);
  // append accum_tf_list for future initialization
  std::cout<<"Average registration time is "<<this_time<<std::endl;


  return 0;
}
