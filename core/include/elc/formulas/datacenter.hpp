#pragma once
// Data centre energy balance (PUE from the consumer breakdown).

namespace elc::datacenter {

struct PueInputs {
    double itPower = 0;           // kW, annual average
    double upsEfficiency = 0.95;  // 0…1, all IT load on UPS
    double distributionLoss = 0;  // 0…1, transformer/cabling losses upstream of the UPS
    double otherPower = 0;        // kW, lighting, security, offices in the data centre
    double eer = 4;               // annual EER of mechanical cooling
    double freeCooling = 0;       // 0…1, share of the cooling load covered without compressors
    double auxShare = 0;          // 0…1, fans/pumps/dry coolers relative to the cooling load
};

struct PueBalance {
    double upsLoss = 0;       // kW
    double distLoss = 0;      // kW
    double coolingLoad = 0;   // kW heat to remove: IT + UPS loss + other
    double coolingPower = 0;  // kW electrical: compressors + fans/pumps
    double total = 0;         // kW
    double pue = 0;
    double pPueCooling = 0;   // (IT + cooling) / IT
    double pPuePower = 0;     // (IT + UPS + distribution losses) / IT
};

/// PUE = P_total / P_IT with P_total = IT + UPS loss + distribution loss + cooling + other
[[nodiscard]] PueBalance pueBalance(const PueInputs& in);

}  // namespace elc::datacenter
