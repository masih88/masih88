#include "elc/formulas/datacenter.hpp"

namespace elc::datacenter {

PueBalance pueBalance(const PueInputs& in) {
    PueBalance b;
    b.upsLoss = in.itPower * (1.0 / in.upsEfficiency - 1.0);
    b.distLoss = (in.itPower + b.upsLoss) * in.distributionLoss;
    b.coolingLoad = in.itPower + b.upsLoss + in.otherPower;
    b.coolingPower = b.coolingLoad * (1.0 - in.freeCooling) / in.eer + b.coolingLoad * in.auxShare;
    b.total = in.itPower + b.upsLoss + b.distLoss + b.coolingPower + in.otherPower;
    b.pue = b.total / in.itPower;
    b.pPueCooling = (in.itPower + b.coolingPower) / in.itPower;
    b.pPuePower = (in.itPower + b.upsLoss + b.distLoss) / in.itPower;
    return b;
}

}  // namespace elc::datacenter
