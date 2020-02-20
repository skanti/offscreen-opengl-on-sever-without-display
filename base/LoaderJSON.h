#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <json/json.h>

#include <eigen3/Eigen/Dense>

inline void jsonvalue2mat4(Json::Value &trans0, Json::Value &rot0, Json::Value &scale0, Eigen::Matrix4f &trans1, Eigen::Matrix4f &rot1, Eigen::Matrix4f &scale1) {

	Eigen::Vector3f trans(trans0[0].asFloat(), trans0[1].asFloat(), trans0[2].asFloat());
	Eigen::Quaternionf rot(rot0[3].asFloat(), rot0[0].asFloat(), rot0[1].asFloat(), rot0[2].asFloat());
	Eigen::Vector3f scale(scale0[0].asFloat(), scale0[1].asFloat(), scale0[2].asFloat());

	trans1 = Eigen::Matrix4f::Identity();
	trans1.block(0, 3, 3, 1) = trans;

	rot1 = Eigen::Matrix4f::Identity();
	rot1.block(0, 0, 3, 3) = rot.toRotationMatrix();

	scale1 = Eigen::Matrix4f::Identity();
	scale1.block(0, 0, 3, 3) = scale.asDiagonal();
}

inline static void load_cad_bbox_from_json(std::string filename, Eigen::Matrix4f &trs_root, std::vector<Eigen::Matrix4f> &trs_list, std::vector<Eigen::Vector3f> &bbox_list) {
	Json::Value root;

	std::ifstream file(filename);
	assert(file.is_open() && "Cannot open json file.");
	file >> root;
	
	
	Json::Value aligned_models = root["aligned_models"];	
	assert(aligned_models.size() > 0 && "No models found.");
	
	Json::Value jtrs0 = root["trs"];
	Json::Value jtrans0 = jtrs0["translation"];
	Json::Value jrot0 = jtrs0["rotation"];
	Json::Value jscale0 = jtrs0["scale"];

	Eigen::Matrix4f trans_root, rot_root, scale_root;
	jsonvalue2mat4(jtrans0, jrot0, jscale0, trans_root, rot_root, scale_root);
	trs_root = trans_root*rot_root*scale_root;
		
	for (int i = 0; i < (int) aligned_models.size(); i++) {
		Json::Value model = aligned_models[i];
		Json::Value jbbox = model["bbox"];
		Json::Value jtrs = model["trs"];
		Json::Value jtrans = jtrs["translation"];
		Json::Value jrot = jtrs["rotation"];
		Json::Value jscale = jtrs["scale"];

		Eigen::Matrix4f trans, rot, scale, trs;
		jsonvalue2mat4(jtrans, jrot, jscale, trans, rot, scale);
		trs = trs_root.inverse()*trans*rot*scale;

		Eigen::Vector3f bbox(jbbox[0].asFloat(), jbbox[1].asFloat(), jbbox[2].asFloat());
		
		trs_list.push_back(trs);
		bbox_list.push_back(bbox);
	}
}

inline static void load_scan_keypoints_from_json(std::string filename, std::vector<Eigen::MatrixXf> &keypoints, Eigen::Vector3f &trans, Eigen::Quaternionf &rot, Eigen::Vector3f &scale) {
	Json::Value root;

	std::ifstream file(filename);
	assert(file.is_open() && "Cannot open json file.");
	file >> root;
	Json::Value trs = root["trs"];
	Json::Value trans0 = trs["translation"];
	Json::Value rot0 = trs["rotation"];
	Json::Value scale0 = trs["scale"];
	
	trans = Eigen::Vector3f(trans0[0].asFloat(), trans0[1].asFloat(), trans0[2].asFloat());
	rot = Eigen::Quaternionf(rot0[3].asFloat(), rot0[0].asFloat(), rot0[1].asFloat(), rot0[2].asFloat());
	scale = Eigen::Vector3f(scale0[0].asFloat(), scale0[1].asFloat(), scale0[2].asFloat());

	Eigen::Matrix3f rotmat = rot.toRotationMatrix();

	Json::Value aligned_models = root["aligned_models"];	
	assert(aligned_models.size() > 0 && "No models found.");
	keypoints.resize(aligned_models.size());
	int n_models0 = aligned_models.size();
	int n_keypoints0 = 0;
	for (int i = 0; i < (int) aligned_models.size(); i++) {
		Json::Value model = aligned_models[i];
		Json::Value kp1 = model["keypoint1"];
		int n_keypoints = kp1["n_keypoints"].asInt();
		keypoints[i].resize(3, n_keypoints);
		int j = 0;
		std::generate(keypoints[i].data(), keypoints[i].data() + 3*n_keypoints, [&] () {return kp1["position"][j++].asFloat(); });
		keypoints[i] = rotmat.transpose()*(keypoints[i].colwise() - trans);
		n_keypoints0 += n_keypoints;
	}
	printf("n_models: %d n_keypoints: %d\n", n_models0, n_keypoints0);
}

inline static void load_cad_keypoints_from_json(std::string filename, std::vector<Eigen::MatrixXf> &keypoints, Eigen::Vector3f &trans, Eigen::Quaternionf &rot, Eigen::Vector3f &scale, int &n_keypoints0) {
	Json::Value root;

	std::ifstream file(filename);
	assert(file.is_open() && "Cannot open json file.");
	file >> root;

	Json::Value aligned_models = root["aligned_models"];	
	assert(aligned_models.size() > 0 && "No models found.");
	keypoints.resize(aligned_models.size());
	int n_models0 = aligned_models.size();
	for (int i = 0; i < (int) aligned_models.size(); i++) {
		Json::Value model = aligned_models[i];
		//std::cout << "i-cad: " << model["id_shapenet"].asString() << std::endl;
		
		Json::Value jtrs = model["trs"];
		Json::Value jcenter = model["center"];
		Json::Value jtrans = jtrs["translation"];
		Json::Value jrot = jtrs["rotation"];
		Json::Value jscale = jtrs["scale"];
		//std::cout << rot << std::endl;
		Eigen::Vector3f trans(jtrans[0].asFloat(), jtrans[1].asFloat(), jtrans[2].asFloat());
		Eigen::Vector3f center(jcenter[0].asFloat(), jcenter[1].asFloat(), jcenter[2].asFloat());
		//std::cout << "center: " << center.transpose() << std::endl;
		Eigen::Matrix3f rot = (Eigen::Quaternionf(jrot[3].asFloat(), jrot[0].asFloat(), jrot[1].asFloat(), jrot[2].asFloat())).toRotationMatrix();
		Eigen::Matrix3f scale = (Eigen::Matrix3f) Eigen::Vector3f(jscale[0].asFloat(), jscale[1].asFloat(), jscale[2].asFloat()).asDiagonal();


		Json::Value kp0 = model["keypoint0"];
		int n_keypoints = kp0["n_keypoints"].asInt();
		keypoints[i].resize(3, n_keypoints);
		int j = 0;
		std::generate(keypoints[i].data(), keypoints[i].data() + 3*n_keypoints, [&] () {return kp0["position"][j++].asFloat(); });
		//printf("n_kp: %d\n", n_keypoints);
		keypoints[i] = scale.inverse()*rot.inverse()*(keypoints[i].colwise() - trans);
		keypoints[i] = keypoints[i].colwise() + center;
		n_keypoints0 += n_keypoints;
	}
	printf("n_models: %d n_keypoints: %d\n", n_models0, n_keypoints0);
}

