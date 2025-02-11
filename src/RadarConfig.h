#ifndef RADAR_CONFIG_H
#define RADAR_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

class RadarConfig {
public:
    RadarConfig() = default;
    bool loadFromFile(const std::string& filePath);
    bool loadFromString(const std::string& configStr);
    const std::string& getConfig() const { return m_config; }
    std::string toString() const;
    nlohmann::json to_json() const;
    std::string to_json_pretty() const;

    // Configuration parameters
    std::string sensorStop;
    std::string flushCfg;
    int dfeDataOutputMode;
    std::vector<int> channelCfg;
    std::vector<int> adcCfg;
    std::vector<int> adcbufCfg;
    std::vector<int> lowPower;
    std::vector<double> profileCfg;
    std::vector<int> chirpCfg0;
    std::vector<int> chirpCfg1;
    std::vector<int> chirpCfg2;
    std::vector<double> frameCfg;
    std::vector<double> dynamicRACfarCfg;
    std::vector<double> staticRACfarCfg;
    std::vector<double> dynamicRangeAngleCfg;
    std::vector<double> dynamic2DAngleCfg;
    std::vector<int> staticRangeAngleCfg;
    std::vector<int> antGeometry0;
    std::vector<int> antGeometry1;
    std::vector<int> antPhaseRot;
    std::vector<double> fovCfg;
    std::vector<int> compRangeBiasAndRxChanPhase;
    std::vector<double> staticBoundaryBox;
    std::vector<double> boundaryBox;
    std::vector<int> sensorPosition;
    std::vector<int> gatingParam;
    std::vector<int> stateParam;
    std::vector<double> allocationParam;
    std::vector<double> maxAcceleration;
    std::vector<int> trackingCfg;
    std::vector<double> presenceBoundaryBox;
    std::string sensorStart;

private:
    void parseLine(const std::string& line);
    std::string m_config;
};

#endif // RADAR_CONFIG_H