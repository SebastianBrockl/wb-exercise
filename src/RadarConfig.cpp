#include "RadarConfig.h"
#include "util.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>

bool RadarConfig::loadFromFile(const std::string& filePath) {
    std::string configStr = read_file_to_string(filePath);

    ensure_unix_line_endings(configStr);
    remove_comments(configStr);
    remove_empty_lines(configStr);

    return loadFromString(configStr);
}

bool RadarConfig::loadFromString(const std::string &configStr) {
    std::istringstream iss(configStr);
    std::string line;
    while (std::getline(iss, line)) {
        parseLine(line);
    }

    return true;
}

void RadarConfig::parseLine(const std::string& line) {
    if (line.empty() || line[0] == '%') {
        return; // Skip empty lines and comments
    }

    std::istringstream iss(line);
    std::string key;
    iss >> key;

    if (key == "dfeDataOutputMode") {
        iss >> dfeDataOutputMode;
    } else if (key == "channelCfg") {
        channelCfg = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "adcCfg") {
        adcCfg = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "adcbufCfg") {
        adcbufCfg = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "lowPower") {
        lowPower = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "profileCfg") {
        profileCfg = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "chirpCfg" && line.find("chirpCfg 0") != std::string::npos) {
        chirpCfg0 = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "chirpCfg" && line.find("chirpCfg 1") != std::string::npos) {
        chirpCfg1 = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "chirpCfg" && line.find("chirpCfg 2") != std::string::npos) {
        chirpCfg2 = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "frameCfg") {
        frameCfg = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "dynamicRACfarCfg") {
        dynamicRACfarCfg = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "staticRACfarCfg") {
        staticRACfarCfg = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "dynamicRangeAngleCfg") {
        dynamicRangeAngleCfg = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "dynamic2DAngleCfg") {
        dynamic2DAngleCfg = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "staticRangeAngleCfg") {
        staticRangeAngleCfg = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "antGeometry0") {
        antGeometry0 = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "antGeometry1") {
        antGeometry1 = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "antPhaseRot") {
        antPhaseRot = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "fovCfg") {
        fovCfg = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "compRangeBiasAndRxChanPhase") {
        compRangeBiasAndRxChanPhase = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "staticBoundaryBox") {
        staticBoundaryBox = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "boundaryBox") {
        boundaryBox = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "sensorPosition") {
        sensorPosition = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "gatingParam") {
        gatingParam = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "stateParam") {
        stateParam = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "allocationParam") {
        allocationParam = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "maxAcceleration") {
        maxAcceleration = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } else if (key == "trackingCfg") {
        trackingCfg = std::vector<int>(std::istream_iterator<int>(iss), std::istream_iterator<int>());
    } else if (key == "presenceBoundaryBox") {
        presenceBoundaryBox = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    } 
}

std::string RadarConfig::toString() const {
    std::ostringstream oss;

    oss << "dfeDataOutputMode: " << dfeDataOutputMode << "\n";
    oss << "channelCfg: ";
    for (const auto& val : channelCfg) oss << val << " ";
    oss << "\nadcCfg: ";
    for (const auto& val : adcCfg) oss << val << " ";
    oss << "\nadcbufCfg: ";
    for (const auto& val : adcbufCfg) oss << val << " ";
    oss << "\nlowPower: ";
    for (const auto& val : lowPower) oss << val << " ";
    oss << "\nprofileCfg: ";
    for (const auto& val : profileCfg) oss << val << " ";
    oss << "\nchirpCfg0: ";
    for (const auto& val : chirpCfg0) oss << val << " ";
    oss << "\nchirpCfg1: ";
    for (const auto& val : chirpCfg1) oss << val << " ";
    oss << "\nchirpCfg2: ";
    for (const auto& val : chirpCfg2) oss << val << " ";
    oss << "\nframeCfg: ";
    for (const auto& val : frameCfg) oss << val << " ";
    oss << "\ndynamicRACfarCfg: ";
    for (const auto& val : dynamicRACfarCfg) oss << val << " ";
    oss << "\nstaticRACfarCfg: ";
    for (const auto& val : staticRACfarCfg) oss << val << " ";
    oss << "\ndynamicRangeAngleCfg: ";
    for (const auto& val : dynamicRangeAngleCfg) oss << val << " ";
    oss << "\ndynamic2DAngleCfg: ";
    for (const auto& val : dynamic2DAngleCfg) oss << val << " ";
    oss << "\nstaticRangeAngleCfg: ";
    for (const auto& val : staticRangeAngleCfg) oss << val << " ";
    oss << "\nantGeometry0: ";
    for (const auto& val : antGeometry0) oss << val << " ";
    oss << "\nantGeometry1: ";
    for (const auto& val : antGeometry1) oss << val << " ";
    oss << "\nantPhaseRot: ";
    for (const auto& val : antPhaseRot) oss << val << " ";
    oss << "\nfovCfg: ";
    for (const auto& val : fovCfg) oss << val << " ";
    oss << "\ncompRangeBiasAndRxChanPhase: ";
    for (const auto& val : compRangeBiasAndRxChanPhase) oss << val << " ";
    oss << "\nstaticBoundaryBox: ";
    for (const auto& val : staticBoundaryBox) oss << val << " ";
    oss << "\nboundaryBox: ";
    for (const auto& val : boundaryBox) oss << val << " ";
    oss << "\nsensorPosition: ";
    for (const auto& val : sensorPosition) oss << val << " ";
    oss << "\ngatingParam: ";
    for (const auto& val : gatingParam) oss << val << " ";
    oss << "\nstateParam: ";
    for (const auto& val : stateParam) oss << val << " ";
    oss << "\nallocationParam: ";
    for (const auto& val : allocationParam) oss << val << " ";
    oss << "\nmaxAcceleration: ";
    for (const auto& val : maxAcceleration) oss << val << " ";
    oss << "\ntrackingCfg: ";
    for (const auto& val : trackingCfg) oss << val << " ";
    oss << "\npresenceBoundaryBox: ";
    for (const auto& val : presenceBoundaryBox) oss << val << " ";

    return oss.str();
}

nlohmann::json RadarConfig::to_json() const {
    nlohmann::json json;

    json["dfeDataOutputMode"] = dfeDataOutputMode;
    json["channelCfg"] = channelCfg;
    json["adcCfg"] = adcCfg;
    json["adcbufCfg"] = adcbufCfg;
    json["lowPower"] = lowPower;
    json["profileCfg"] = profileCfg;
    json["chirpCfg0"] = chirpCfg0;
    json["chirpCfg1"] = chirpCfg1;
    json["chirpCfg2"] = chirpCfg2;
    json["frameCfg"] = frameCfg;
    json["dynamicRACfarCfg"] = dynamicRACfarCfg;
    json["staticRACfarCfg"] = staticRACfarCfg;
    json["dynamicRangeAngleCfg"] = dynamicRangeAngleCfg;
    json["dynamic2DAngleCfg"] = dynamic2DAngleCfg;
    json["staticRangeAngleCfg"] = staticRangeAngleCfg;
    json["antGeometry0"] = antGeometry0;
    json["antGeometry1"] = antGeometry1;
    json["antPhaseRot"] = antPhaseRot;
    json["fovCfg"] = fovCfg;
    json["compRangeBiasAndRxChanPhase"] = compRangeBiasAndRxChanPhase;
    json["staticBoundaryBox"] = staticBoundaryBox;
    json["boundaryBox"] = boundaryBox;
    json["sensorPosition"] = sensorPosition;
    json["gatingParam"] = gatingParam;
    json["stateParam"] = stateParam;
    json["allocationParam"] = allocationParam;
    json["maxAcceleration"] = maxAcceleration;
    json["trackingCfg"] = trackingCfg;
    json["presenceBoundaryBox"] = presenceBoundaryBox;

    return json;
}

std::string RadarConfig::to_json_pretty() const {
    return to_json().dump(4); // Pretty print with 4 spaces indentation
}
