#include "elc/registry.hpp"

#include <algorithm>
#include <array>
#include <map>
#include <string>

#include "catalog/helpers.hpp"
#include "elc/lists.hpp"

#ifndef ELC_VERSION
#define ELC_VERSION "dev"
#endif

namespace elc {

const std::vector<Category>& categories() {
    static const std::vector<Category> list = catalog::areas();
    return list;
}

namespace {

/// item id → area id from the central layout (catalog/layout.cpp)
const std::map<std::string, std::string, std::less<>>& areaOfItem() {
    static const std::map<std::string, std::string, std::less<>> m = [] {
        std::map<std::string, std::string, std::less<>> out;
        for (const auto& c : categories()) {
            for (const auto& g : c.groups) {
                for (const auto& id : g.items) out.emplace(id, c.id);
            }
        }
        return out;
    }();
    return m;
}

/// Sets each item's category from the layout; items missing there keep theirs (the layout test reports them).
template <typename T>
void assignAreas(std::vector<T>& items) {
    const auto& m = areaOfItem();
    for (auto& i : items) {
        if (const auto it = m.find(i.id); it != m.end()) i.category = it->second;
    }
}

}  // namespace

const std::vector<Calculator>& calculators() {
    static const std::vector<Calculator> list = [] {
        std::vector<Calculator> v;
        catalog::addBasics(v);
        catalog::addCables(v);
        catalog::addMotors(v);
        catalog::addIndustry(v);
        catalog::addAutomation(v);
        catalog::addHvac(v);
        catalog::addControls(v);
        catalog::addHydraulics(v);
        catalog::addBuilding(v);
        catalog::addPid(v);
        catalog::addProcess(v);
        catalog::addCleanroom(v);
        catalog::addDatacenter(v);
        catalog::addPv(v);
        catalog::addElectronics(v);
        catalog::addConverters(v);
        catalog::addElectricalPower(v);
        catalog::addElectricalCables(v);
        catalog::addElectricalComponents(v);
        catalog::addElectricalMotors(v);
        catalog::addElectricalUnits(v);
        catalog::addElectricalResources(v);
        catalog::addMotorFeeder(v);
        catalog::addSurge(v);
        assignAreas(v);
        return v;
    }();
    return list;
}

const std::vector<Table>& tables() {
    static const std::vector<Table> list = [] {
        std::vector<Table> v;
        catalog::addTables(v);
        catalog::addCleanroomTables(v);
        catalog::addDatacenterTables(v);
        catalog::addVdiTables(v);
        catalog::addControlTables(v);
        catalog::addHydraulicTables(v);
        catalog::addBuildingTables(v);
        catalog::addPidTables(v);
        catalog::addProcessTables(v);
        catalog::addElectricalTables(v);
        catalog::addElectricalResourceTables(v);
        catalog::addPinoutTables(v);
        catalog::addMotorFeederTables(v);
        catalog::addSurgeTables(v);
        assignAreas(v);
        for (auto& t : v) {
            if (!t.drawing) t.drawing = catalog::tableDrawing(t);
        }
        return v;
    }();
    return list;
}

std::vector<CategoryGroup> categoryGroups(std::string_view categoryId) {
    for (const auto& c : categories()) {
        if (c.id == categoryId && !c.groups.empty()) return c.groups;
    }
    CategoryGroup all;
    for (const auto& l : lists()) {
        if (l.category == categoryId) all.items.push_back(l.id);
    }
    for (const auto& c : calculators()) {
        if (c.category == categoryId) all.items.push_back(c.id);
    }
    for (const auto& t : tables()) {
        if (t.category == categoryId) all.items.push_back(t.id);
    }
    return {all};
}

const Calculator* findCalculator(std::string_view id) {
    const auto& all = calculators();
    const auto it = std::find_if(all.begin(), all.end(), [&](const Calculator& c) { return c.id == id; });
    return it == all.end() ? nullptr : &*it;
}

const Table* findTable(std::string_view id) {
    const auto& all = tables();
    const auto it = std::find_if(all.begin(), all.end(), [&](const Table& t) { return t.id == id; });
    return it == all.end() ? nullptr : &*it;
}

const std::vector<ListDef>& lists() {
    static const std::vector<ListDef> list = [] {
        std::vector<ListDef> v{catalog::consumerList(), catalog::dataPointList(), catalog::cableTrayList()};
        assignAreas(v);
        return v;
    }();
    return list;
}

const ListDef* findList(std::string_view id) {
    const auto& all = lists();
    const auto it = std::find_if(all.begin(), all.end(), [&](const ListDef& l) { return l.id == id; });
    return it == all.end() ? nullptr : &*it;
}

std::string_view currentItemId(std::string_view id) {
    static const std::array<std::pair<std::string_view, std::string_view>, 23> merged{{
        {"current_from_power", "power"},
        {"voltage_from_power", "power"},
        {"motor_current", "power"},
        {"motor_power", "power"},
        {"motor_voltage", "power"},
        {"motor_pf", "power_factor_calc"},
        {"color_from_value", "resistor_color"},
        {"resistor_colours", "resistor_color"},
        {"rtd_cu", "rtd"},
        {"conductor_resistance", "cable_rxz"},
        {"humid_air", "hx_diagram"},
        {"conv_phase", "trig_angle"},
        {"tuning_rules", "controller_tuning"},
        {"conductor_colours", "wiring_colours_intl"},
        {"mcb_characteristics", "trip_characteristics"},
        {"network_systems", "earthing_systems"},
        {"motor_terminals", "motor_wiring_6"},
        {"ip_first", "ip_codes"},
        {"ip_second", "ip_codes"},
        {"wire_size_compare", "awg_table"},
        {"fuse_colours", "fuse_d"},
        {"fuse_d_nh", "fuse_d"},
        {"led_resistor", "series_resistor"},
    }};
    for (const auto& [from, to] : merged) {
        if (from == id) return to;
    }
    return id;
}

std::vector<std::string_view> currentAreaIds(std::string_view id) {
    for (const auto& c : categories()) {
        if (c.id == id) return {c.id};
    }
    if (id == "process") return {"pid", "atex"};  // "R&I-Kennzeichnung & Ex-Schutz" was split in v1.9
    return {};
}

std::string_view version() { return ELC_VERSION; }

}  // namespace elc
