#pragma once
// Drawing helper for schematics (Chart::diagram = true): data units, y up, series drawn in order.
// Styles see elc/chart.hpp; additionally "fill:#RRGGBB" (filled with that colour, outlined) and
// "line:#RRGGBB" (thick line in that colour, e.g. copper bridges).

#include <array>
#include <cmath>
#include <numbers>
#include <string>
#include <utility>
#include <vector>

#include "elc/chart.hpp"
#include "elc/text.hpp"

namespace elc::catalog {

struct Sketch {
    using P = std::array<double, 2>;
    Chart ch;

    void frame(double width, double height, double aspect) {
        ch.x = {"", 0, width, 10};
        ch.y = {"", 0, height, 10};
        ch.diagram = true;
        ch.aspect = aspect;
    }
    void line(const std::string& style, std::vector<P> pts) { ch.series.push_back({style, std::move(pts)}); }
    void closed(const std::string& style, std::vector<P> pts) {
        pts.push_back(pts.front());
        line(style, std::move(pts));
    }
    void circle(const std::string& style, double x, double y, double r, int n = 28) {
        std::vector<P> pts;
        for (int i = 0; i <= n; ++i) {
            const double a = 2.0 * std::numbers::pi * i / n;
            pts.push_back({x + r * std::cos(a), y + r * std::sin(a)});
        }
        line(style, std::move(pts));
    }
    void label(double x, double y, Text t, const char* style = "c") { ch.labels.push_back({x, y, std::move(t), style}); }
    void rect(const std::string& style, double x0, double y0, double x1, double y1) {
        closed(style, {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}});
    }
    /// Rectangle of width w centred on the segment a → b (coil or resistor body)
    void bar(const std::string& style, P a, P b, double w) {
        const double dx = b[0] - a[0];
        const double dy = b[1] - a[1];
        const double len = std::hypot(dx, dy);
        const double nx = -dy / len * w / 2.0;
        const double ny = dx / len * w / 2.0;
        closed(style, {{a[0] + nx, a[1] + ny}, {b[0] + nx, b[1] + ny}, {b[0] - nx, b[1] - ny}, {a[0] - nx, a[1] - ny}});
    }
    /// Point at fraction t on a → b
    static P along(P a, P b, double t) { return {a[0] + (b[0] - a[0]) * t, a[1] + (b[1] - a[1]) * t}; }
};

}  // namespace elc::catalog
