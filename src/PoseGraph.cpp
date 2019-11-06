#include <iostream>
#include <cstdio>
#include <cassert>
// Graphs
#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/inference/Symbol.h>
#include <gtsam/nonlinear/Values.h>
#include <gtsam/nonlinear/ISAM2.h>
#include <gtsam/geometry/Pose3.h>
// Factors
#include <gtsam/slam/PriorFactor.h>
#include <gtsam/slam/BetweenFactor.h>

#include "graph_optimizer/PoseGraph.hpp"
#include "utils/data_type.hpp"
#include "utils/conversions.hpp"
namespace cvo {

  using gtsam::symbol_shorthand::X; // Pose3 (x,y,z,r,p,y)

  
  PoseGraph::PoseGraph():
    isam2_(nullptr){
    // ISAM2 solver
    gtsam::ISAM2Params isam_params;
    isam_params.relinearizeThreshold = 0.01;
    isam_params.relinearizeSkip = 1;
    isam_params.cacheLinearizedFactors = false;
    isam_params.enableDetailedResults = true;
    isam_params.print();
    this->isam2_ .reset( new gtsam::ISAM2(isam_params));
    
  }

  PoseGraph::~PoseGraph() {
    
    
  }

  static Eigen::Affine3f read_tracking_init_guess() {
    FILE * tracking_init_guess_f = fopen("cvo_init.txt", "r");
    Eigen::Affine3f init_guess;
    auto & m = init_guess.matrix();
    if (tracking_init_guess_f) {
      fscanf(tracking_init_guess_f,
             "%f %f %f %f %f %f %f %f %f %f %f %f",
             m(0,0), m(0,1), m(0,2), m(0,3),
             m(1,0), m(1,1), m(1,2), m(1,3),
             m(2,0), m(2,1), m(2,2), m(2,3));
      fclose(tracking_init_guess_f);
    } else {
      printf("No tracking init guess file found! use identity\n");
      m.setIdentity();
    }
    return init_guess;
  }

  void PoseGraph::add_new_frame(std::shared_ptr<Frame> new_frame) {
    std::cout<<"add_new_frame: id "<<new_frame->id<<std::endl;
    std::cout<<"---- number of points is "<<new_frame->points().num_points()<<std::endl;
    //new_frame->points().write_to_color_pcd(std::to_string(new_frame->id)+".pcd"  );
    bool is_keyframe = false;
    Eigen::Affine3f tracking_pose;
    if (tracking_relative_transforms_.size() == 0) {
      is_keyframe = true;
      all_frames_since_last_keyframe_ = {};
      
      //new_frame->set_relative_transform();
    } else {

      //auto & last_keyframe = *all_frames_since_last_keyframe.front();
      //auto & last_frame = *all_frames_since_last_keyframe.back();
    
    
    // align with 
    //cvo.set_pcd(last_keyframe.points(), new_frame.points())
      //new_frame->set_relative_transform();
    }

    //all_frames_since_last_keyframe_.push(new_frame);
    tracking_relative_transforms_.push_back(new_frame->tracking_relative_transform());

    if(is_keyframe) {
      if (keyframes_.size() == 0) {
        init_pose_graph(new_frame);
      } else {
        pose_graph_optimize(new_frame);
      }
      keyframes_.push_back (new_frame);
      id2keyframe_[new_frame->id] = new_frame;
      all_frames_since_last_keyframe_.clear();
      
    }
    all_frames_since_last_keyframe_.push_back(new_frame);
    last_two_frames_.push(new_frame);
    if (last_two_frames_.size() > 2)
      last_two_frames_.pop();

  }

  void PoseGraph::init_pose_graph(std::shared_ptr<Frame> new_frame) {
    //fill config values
    gtsam::Vector4 q_WtoC;
    q_WtoC << 0,0,0,1;
    gtsam::Vector3 t_WtoC;
    t_WtoC << 0,0,0;
    gtsam::Vector6 prior_pose_noise;
    prior_pose_noise << gtsam::Vector3::Constant(0.1), gtsam::Vector3::Constant(0.1);

    // prior state and noise
    gtsam::Pose3 prior_state(gtsam::Quaternion(q_WtoC(3), q_WtoC(0), q_WtoC(1), q_WtoC(2)),
                             t_WtoC);
    auto pose_noise = gtsam::noiseModel::Diagonal::Sigmas( prior_pose_noise);
    
    factor_graph_.add(gtsam::PriorFactor<gtsam::Pose3>(X(new_frame->id),
                                                       prior_state, pose_noise));
    graph_values_.insert(X(new_frame->id), prior_state);
  }

  void PoseGraph::pose_graph_optimize(std::shared_ptr<Frame> new_frame) {

    assert(tracking_relative_transforms_.size() > 1);
    
    std::cout<<"optimize the pose graph with gtsam\n";
    int new_id = new_frame->id;
    int last_kf_id = all_frames_since_last_keyframe_[0]->id;
    auto last_kf = keyframes_[last_kf_id];
    
    auto tf_last_frame_to_last_keyframe = tracking_relative_transforms_.back().ref_frame_to_curr_frame().inverse();
    auto tf_last_keyframe_to_newframe = tf_last_frame_to_last_keyframe * tracking_relative_transforms_[new_id].ref_frame_to_curr_frame();
    Eigen::Affine3f tf_WtoNew_eigen = last_kf->pose_in_graph() * tf_last_keyframe_to_newframe;
    gtsam::Pose3 tf_WtoNew = affine3f_to_pose3(tf_WtoNew_eigen);
    gtsam::Pose3 odom_last_kf_to_new = affine3f_to_pose3(tf_last_keyframe_to_newframe);
    // TODO? use the noise from inner product??
    gtsam::Vector6 prior_pose_noise;
    prior_pose_noise << gtsam::Vector3::Constant(0.1), gtsam::Vector3::Constant(0.1);
    auto pose_noise = gtsam::noiseModel::Diagonal::Sigmas( prior_pose_noise);
    factor_graph_.add(gtsam::BetweenFactor<gtsam::Pose3>(X(last_kf_id), X(new_id),
                                                         odom_last_kf_to_new, pose_noise));
    // TOOD: add init value for this state
    graph_values_.insert(X(new_id), tf_WtoNew);
    //TODO align two functions to get another between factor

    if (keyframes_.size()>2) {
      auto kf_second_last = keyframes_[keyframes_.size()-2];
      auto kf_second_last_id = kf_second_last->id;
      std::unique_ptr<CvoPointCloud> map_points_kf_second_last = kf_second_last->export_points_from_map();
      std::unique_ptr<CvoPointCloud> map_points_kf_last = keyframes_[keyframes_.size()-1]->export_points_from_map();
      Eigen::Affine3f init_guess = kf_second_last->pose_in_graph().inverse() * last_kf->pose_in_graph();
      cvo_align_.set_pcd(*map_points_kf_second_last, *map_points_kf_last,
                         init_guess, true);

      Eigen::Affine3f cvo_result = cvo_align_.get_transform();
      // TODO: check cvo align quality
      gtsam::Pose3 tf_slast_kf_to_last_kf = affine3f_to_pose3(cvo_result);
      factor_graph_.add(gtsam::BetweenFactor<gtsam::Pose3>(X(kf_second_last_id ), X(last_kf_id ),
                                                           tf_slast_kf_to_last_kf, pose_noise));
      graph_values_.print("\ngraph init values\n");
      
    }
    try {
      factor_graph_.print("factor graph");
      gtsam::ISAM2Result result = isam2_->update(factor_graph_, graph_values_ ); // difference from optimize()?
      graph_values_ = isam2_->calculateEstimate();
      update_optimized_poses_to_frames();
      factor_graph_.resize(0);
      graph_values_.clear();
    } catch(gtsam::IndeterminantLinearSystemException &e) {
      std::cerr<<("FORSTER2 gtsam indeterminate linear system exception!\n");
      std::cerr << e.what() << std::endl;
      exit(EXIT_FAILURE);
    }


  }

  void PoseGraph::update_optimized_poses_to_frames() {
    for (int key : graph_values_.keys()) {
      Eigen::Matrix4d pose_mat= graph_values_.at<gtsam::Pose3>(X(key)).matrix() ;
      Eigen::Affine3f pose;
      pose.linear() = pose_mat.block(0,0,3,3).cast<float>();
      pose.translation() = pose_mat.block(0,3,3,1).cast<float>();
      id2keyframe_[key]->set_pose_in_graph(pose);
    }
    
  }
  
}
