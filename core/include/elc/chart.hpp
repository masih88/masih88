#pragma once
// Simple 2-D chart model (lines, labels, markers) produced by the core and drawn by the UI.
// Coordinates are in data units of the axes; the UI only scales and clips.

#include <array>
#include <string>
#include <vector>

#include "elc/text.hpp"

namespace elc {

struct ChartAxis {
    Text title;
    double min = 0;
    double max = 1;
    double step = 0.1;  // tick spacing
};

struct ChartSeries {
    std::string style;  // e.g. "sat", "rh", "t", "h", "process", "aux" – the UI picks pen and colour
    std::vector<std::array<double, 2>> pts;
};

struct ChartLabel {
    double x = 0;
    double y = 0;
    Text text;
    std::string style;
};

struct ChartMarker {
    double x = 0;
    double y = 0;
    std::string label;  // short, language independent (e.g. "1", "2", "M", "τ")
    std::string style;  // "state", "aux"
};

struct Chart {
    ChartAxis x;
    ChartAxis y;
    std::vector<ChartSeries> series;
    std::vector<ChartLabel> labels;
    std::vector<ChartMarker> markers;
    Text note;  // e.g. pressure, hint for tapping
};

/// Value set by tapping into a chart: a number or (if choice is not empty) a choice
struct PickValue {
    std::string id;
    double number = 0;
    std::string choice;
};

}  // namespace elc
