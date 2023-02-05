#include "dataset_handler/TumHandler.hpp"

#include <pcl/io/pcd_io.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <algorithm>
#include <boost/filesystem.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <opencv2/imgcodecs.hpp>
#include <string>

#include "utils/debug_visualization.hpp"
#include "utils/def_assert.hpp"
using namespace std;
using namespace boost::filesystem;

namespace cvo {
TumHandler::TumHandler(std::string tum_folder) {
  curr_index = 0;
  folder_name = tum_folder;
  debug_plot = true;

  // load associate file
  string assoc_pth = tum_folder + "/assoc.txt";
  std::cout << "assoc path: " << assoc_pth << std::endl;
  load_file_name(assoc_pth, vstrRGBName, vstrRGBPth, vstrDepPth);
  cout << "Tum contains " << vstrRGBName.size() << " files\n";
}

TumHandler::TumHandler(std::string tum_folder, const std::string &assoc_file) {
  curr_index = 0;
  folder_name = tum_folder;
  debug_plot = true;

  // load associate file
  string assoc_pth = tum_folder + "/" + assoc_file;
  std::cout << "assoc path: " << assoc_pth << std::endl;
  load_file_name(assoc_pth, vstrRGBName, vstrRGBPth, vstrDepPth);
  cout << "Tum contains " << vstrRGBName.size() << " files\n";
}

TumHandler::~TumHandler() {}

int TumHandler::read_next_rgbd(cv::Mat &rgb_img, cv::Mat &dep_img) {
  if (curr_index >= vstrRGBName.size()) {
    cerr << "Can't read images. Image index out of range!"
         << "\n";
    return -1;
  }

  string rgb_pth = folder_name + "/" + vstrRGBPth[curr_index];
  string dep_pth = folder_name + "/" + vstrDepPth[curr_index];

  rgb_img = cv::imread(rgb_pth);
  // dep_img = cv::imread(dep_pth,CV_LOAD_IMAGE_ANYDEPTH);
  dep_img = cv::imread(dep_pth, cv::IMREAD_UNCHANGED);
  if (rgb_img.data == nullptr || dep_img.data == nullptr) {
    cerr << "Image doesn't read successfully: " << rgb_pth << ", " << dep_pth
         << "\n";
    return -1;
  }
  return 0;
}
int TumHandler::read_next_rgbd(cv::Mat &rgb_img, std::vector<uint16_t> &dep) {
  if (curr_index >= vstrRGBName.size()) {
    cerr << "Can't read images. Image index out of range!"
         << "\n";
    return -1;
  }

  string rgb_pth = folder_name + "/" + vstrRGBPth[curr_index];
  string dep_pth = folder_name + "/" + vstrDepPth[curr_index];

  rgb_img = cv::imread(rgb_pth);
  // dep_img = cv::imread(dep_pth,CV_LOAD_IMAGE_ANYDEPTH);
  cv::Mat dep_img = cv::imread(dep_pth, cv::IMREAD_UNCHANGED);
  if (rgb_img.data == nullptr || dep_img.data == nullptr) {
    cerr << "Image doesn't read successfully: " << rgb_pth << ", " << dep_pth
         << "\n";
    return -1;
  }

  std::copy(dep_img.begin<uint16_t>(), dep_img.end<uint16_t>(),
            std::back_inserter(dep));
  assert(dep.size() == dep_img.total());

  return 0;
}

void TumHandler::next_frame_index() { curr_index++; }

void TumHandler::set_start_index(int start) { curr_index = start; }

int TumHandler::get_current_index() { return curr_index; }

int TumHandler::get_total_number() { return vstrRGBName.size(); }

void TumHandler::load_file_name(string assoc_pth, vector<string> &vstrRGBName,
                                vector<string> &vstrRGBPth,
                                vector<string> &vstrDepPth) {
  std::ifstream fAssociation;
  assert(std::filesystem::exists(assoc_pth));

  fAssociation.open(assoc_pth.c_str());
  while (!fAssociation.eof()) {
    string s;
    getline(fAssociation, s);
    if (!s.empty()) {
      stringstream ss;
      ss << s;
      string RGB;
      ss >> RGB;
      vstrRGBName.push_back(RGB);
      string RGB_pth;
      ss >> RGB_pth;
      vstrRGBPth.push_back(RGB_pth);
      string dep;
      ss >> dep;
      string depPth;
      ss >> depPth;
      vstrDepPth.push_back(depPth);
    }
  }
  fAssociation.close();
}

}  // namespace cvo
