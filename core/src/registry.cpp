#include "elc/registry.hpp"

#include <algorithm>

#include "catalog/helpers.hpp"

#ifndef ELC_VERSION
#define ELC_VERSION "dev"
#endif

namespace elc {

const std::vector<Category>& categories() {
    static const std::vector<Category> list{
        {"basics", {"Grundlagen", "Basics"}, "Ω"},
        {"cables", {"Leitungen & Schutz", "Cables & protection"}, "mm²"},
        {"motors", {"Motoren & Antriebe", "Motors & drives"}, "M~"},
        {"industry", {"Industrie & Netze", "Industry & power"}, "kVA"},
        {"automation", {"Gebäudeautomation & MSR", "Building automation & I&C"}, "mA"},
        {"cleanroom", {"Reinraum & Pharma", "Cleanroom & pharma"}, "ISO"},
        {"datacenter", {"Rechenzentrum", "Data centre"}, "IT"},
        {"pv", {"Photovoltaik & Speicher", "PV & storage"}, "PV"},
        {"electronics", {"Elektronik", "Electronics"}, "RC"},
        {"converters", {"Umrechner", "Converters"}, "⇄"},
        {"tables", {"Tabellen & Pinouts", "Tables & pinouts"}, "≡"},
    };
    return list;
}

const std::vector<Calculator>& calculators() {
    static const std::vector<Calculator> list = [] {
        std::vector<Calculator> v;
        catalog::addBasics(v);
        catalog::addCables(v);
        catalog::addMotors(v);
        catalog::addIndustry(v);
        catalog::addAutomation(v);
        catalog::addHvac(v);
        catalog::addCleanroom(v);
        catalog::addDatacenter(v);
        catalog::addPv(v);
        catalog::addElectronics(v);
        catalog::addConverters(v);
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
        return v;
    }();
    return list;
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

std::string_view version() { return ELC_VERSION; }

}  // namespace elc
