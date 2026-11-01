#include "CalcBridge.h"

#include <QClipboard>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QStandardPaths>
#include <algorithm>
#include <cmath>

#include "UiStrings.h"
#include "elc/registry.hpp"

namespace {

QString q(const std::string& s) { return QString::fromStdString(s); }

QString categoryColor(const std::string& id) {
    if (id == "projects") return QStringLiteral("#1F6F5C");
    if (id == "process") return QStringLiteral("#7B5E3C");
    if (id == "building") return QStringLiteral("#B04A7A");
    if (id == "basics") return QStringLiteral("#3D7EDB");
    if (id == "cables") return QStringLiteral("#E08A1E");
    if (id == "motors") return QStringLiteral("#2E9E6B");
    if (id == "industry") return QStringLiteral("#8A5CD1");
    if (id == "automation") return QStringLiteral("#D9534F");
    if (id == "electronics") return QStringLiteral("#1BA3A6");
    if (id == "converters") return QStringLiteral("#6C7A89");
    if (id == "cleanroom") return QStringLiteral("#2A8BBF");
    if (id == "datacenter") return QStringLiteral("#4B5BA8");
    if (id == "pv") return QStringLiteral("#D9A21B");
    return QStringLiteral("#B5873A");
}

const elc::Category* findCategory(const std::string& id) {
    for (const auto& c : elc::categories()) {
        if (c.id == id) return &c;
    }
    return nullptr;
}

QString severityName(elc::Severity s) {
    switch (s) {
        case elc::Severity::Ok: return QStringLiteral("ok");
        case elc::Severity::Warning: return QStringLiteral("warning");
        case elc::Severity::Error: return QStringLiteral("error");
        case elc::Severity::Info: break;
    }
    return QStringLiteral("info");
}

int severityRank(elc::Severity s) {
    switch (s) {
        case elc::Severity::Info: return 0;
        case elc::Severity::Ok: return 1;
        case elc::Severity::Warning: return 2;
        case elc::Severity::Error: return 3;
    }
    return 0;
}

/// Worst message of a result: "none", "info", "ok", "warning" or "error"
QString statusOf(const elc::Result& r) {
    int worst = -1;
    elc::Severity sev = elc::Severity::Info;
    for (const auto& m : r.messages) {
        if (severityRank(m.severity) > worst) {
            worst = severityRank(m.severity);
            sev = m.severity;
        }
    }
    return worst < 0 ? QStringLiteral("none") : severityName(sev);
}

bool isTextLike(const elc::InputDef& d) { return d.kind != elc::InputDef::Kind::Number; }

/// UI values (text) → core inputs
elc::Inputs toInputs(const std::vector<elc::InputDef>& defs, const QVariantMap& values) {
    elc::Inputs in;
    for (const auto& def : defs) {
        const auto key = QString::fromStdString(def.id);
        if (isTextLike(def)) {
            in.setChoice(def.id, values.contains(key) ? values.value(key).toString().toStdString() : def.defaultChoice);
        } else if (values.contains(key)) {
            in.set(def.id, elc::parseNumber(values.value(key).toString().toStdString()).value_or(elc::kNaN));
        } else {
            in.set(def.id, def.defaultValue);
        }
    }
    return in;
}

QString listKey(const QString& id, const char* what) { return QStringLiteral("lists/%1/%2").arg(id, QString::fromLatin1(what)); }

}  // namespace

CalcBridge::CalcBridge(QObject* parent) : QObject(parent) {
    const QString systemLang = QLocale::system().language() == QLocale::German ? "de" : "en";
    const QString lang = qEnvironmentVariable("ELC_LANG", settings_.value("language", systemLang).toString());
    lang_ = lang == "en" ? elc::Lang::En : elc::Lang::De;
    norm_ = settings_.value("norm", "de").toString() == "ch" ? elc::NormProfile::CH : elc::NormProfile::DE;
    dark_ = qEnvironmentVariable("ELC_THEME", settings_.value("dark", false).toBool() ? "dark" : "light") == "dark";
    favorites_ = settings_.value("favorites").toStringList();
    hiddenCategories_ = settings_.value("hiddenCategories").toStringList();
    // currency: stored choice, otherwise from the norm profile; exchange rate editable in the settings
    const QString cur = settings_.value("currency", norm_ == elc::NormProfile::CH ? "CHF" : "EUR").toString();
    currency_ = cur == "CHF" ? elc::Currency::CHF : elc::Currency::EUR;
    chfPerEur_ = settings_.value("chfPerEur", 0.92).toDouble();
    if (!(chfPerEur_ > 0)) chfPerEur_ = 0.92;
    startPage_ = qEnvironmentVariable("ELC_OPEN");  // used for automated screenshots
}

QString CalcBridge::txt(const elc::Text& t) const { return q(t.in(lang_)); }

elc::Context CalcBridge::context() const {
    elc::Context c;
    c.norm = norm_;
    c.currency = currency_;
    c.chfPerEur = chfPerEur_;
    return c;
}

void CalcBridge::setCurrency(const QString& currency) {
    const auto c = currency == "CHF" ? elc::Currency::CHF : elc::Currency::EUR;
    if (c == currency_) return;
    currency_ = c;
    settings_.setValue("currency", this->currency());
    emit currencyChanged();
}

void CalcBridge::setChfPerEur(double rate) {
    if (!(rate > 0) || !std::isfinite(rate) || rate == chfPerEur_) return;
    chfPerEur_ = rate;
    settings_.setValue("chfPerEur", rate);
    emit currencyChanged();
}

QString CalcBridge::formatNumber(double v, int decimals) const { return q(elc::formatNumber(v, decimals, lang_)); }

double CalcBridge::parseNumber(const QString& text) const { return elc::parseNumber(text.toStdString()).value_or(elc::kNaN); }

QString CalcBridge::language() const { return lang_ == elc::Lang::En ? "en" : "de"; }

void CalcBridge::setLanguage(const QString& lang) {
    const auto l = lang == "en" ? elc::Lang::En : elc::Lang::De;
    if (l == lang_) return;
    lang_ = l;
    settings_.setValue("language", language());
    emit languageChanged();
}

QString CalcBridge::norm() const { return norm_ == elc::NormProfile::CH ? "ch" : "de"; }

void CalcBridge::setNorm(const QString& norm) {
    const auto n = norm == "ch" ? elc::NormProfile::CH : elc::NormProfile::DE;
    if (n == norm_) return;
    norm_ = n;
    settings_.setValue("norm", this->norm());
    emit normChanged();
}

void CalcBridge::setDarkTheme(bool dark) {
    if (dark == dark_) return;
    dark_ = dark;
    settings_.setValue("dark", dark_);
    emit darkThemeChanged();
}

QVariantMap CalcBridge::ui() const { return uiStrings(lang_); }

QString CalcBridge::version() const { return q(std::string(elc::version())); }

QVariantList CalcBridge::categories() const {
    QVariantList list;
    for (const auto& c : elc::categories()) {
        int calcs = 0;
        int tables = 0;
        for (const auto& calc : elc::calculators()) calcs += calc.category == c.id ? 1 : 0;
        for (const auto& t : elc::tables()) tables += t.category == c.id ? 1 : 0;
        const auto lists = std::count_if(elc::lists().begin(), elc::lists().end(),
                                         [&](const elc::ListDef& l) { return l.category == c.id; });
        list.append(QVariantMap{{"id", q(c.id)},
                                {"title", txt(c.title)},
                                {"badge", q(c.badge)},
                                {"color", categoryColor(c.id)},
                                {"count", calcs + tables + static_cast<int>(lists)}});
    }
    return list;
}

QVariantMap CalcBridge::itemFor(const QString& id) const {
    const auto sid = id.toStdString();
    if (const auto* l = elc::findList(sid)) {
        return {{"id", id},
                {"kind", "list"},
                {"title", txt(l->title)},
                {"summary", txt(l->summary)},
                {"badge", QStringLiteral("Σ")},
                {"color", categoryColor(l->category)}};
    }
    if (const auto* c = elc::findCalculator(sid)) {
        const auto* cat = findCategory(c->category);
        return {{"id", id},
                {"kind", "calc"},
                {"title", txt(c->title)},
                {"summary", txt(c->summary)},
                {"badge", cat ? q(cat->badge) : QString()},
                {"color", categoryColor(c->category)}};
    }
    if (const auto* t = elc::findTable(sid)) {
        return {{"id", id},
                {"kind", "table"},
                {"title", txt(t->title)},
                {"summary", txt(t->summary)},
                {"badge", QStringLiteral("≡")},
                {"color", categoryColor(t->category)}};
    }
    return {};
}

QString CalcBridge::kindOf(const QString& id) const {
    const auto sid = id.toStdString();
    if (elc::findCalculator(sid)) return QStringLiteral("calc");
    if (elc::findTable(sid)) return QStringLiteral("table");
    if (elc::findList(sid)) return QStringLiteral("list");
    return {};
}

QVariantList CalcBridge::items(const QString& categoryId) const {
    QVariantList list;
    const auto cid = categoryId.toStdString();
    for (const auto& l : elc::lists()) {
        if (l.category == cid) list.append(itemFor(q(l.id)));
    }
    for (const auto& c : elc::calculators()) {
        if (c.category == cid) list.append(itemFor(q(c.id)));
    }
    for (const auto& t : elc::tables()) {
        if (t.category == cid) list.append(itemFor(q(t.id)));
    }
    return list;
}

QVariantList CalcBridge::search(const QString& text) const {
    QVariantList list;
    const auto terms = text.simplified().split(' ', Qt::SkipEmptyParts);
    if (terms.isEmpty()) return list;
    const auto matches = [&](const QString& hay) {
        for (const auto& t : terms) {
            if (!hay.contains(t, Qt::CaseInsensitive)) return false;
        }
        return true;
    };
    for (const auto& l : elc::lists()) {
        QString hay = q(l.id) + ' ' + q(l.title.de) + ' ' + q(l.title.en) + ' ' + q(l.summary.de) + ' ' + q(l.summary.en);
        for (const auto& k : l.keywords) hay += ' ' + q(k);
        if (matches(hay)) list.append(itemFor(q(l.id)));
    }
    for (const auto& c : elc::calculators()) {
        QString hay = q(c.id) + ' ' + q(c.title.de) + ' ' + q(c.title.en) + ' ' + q(c.summary.de) + ' ' + q(c.summary.en);
        for (const auto& k : c.keywords) hay += ' ' + q(k);
        if (matches(hay)) list.append(itemFor(q(c.id)));
    }
    for (const auto& t : elc::tables()) {
        const QString hay = q(t.id) + ' ' + q(t.title.de) + ' ' + q(t.title.en) + ' ' + q(t.summary.de) + ' ' + q(t.summary.en);
        if (matches(hay)) list.append(itemFor(q(t.id)));
    }
    return list;
}

QVariantList CalcBridge::favoriteItems() const {
    QVariantList list;
    for (const auto& id : favorites_) {
        const auto item = itemFor(id);
        if (!item.isEmpty()) list.append(item);
    }
    return list;
}

void CalcBridge::setCategoryVisible(const QString& id, bool visible) {
    if (visible == !hiddenCategories_.contains(id)) return;
    if (visible) {
        hiddenCategories_.removeAll(id);
    } else {
        hiddenCategories_.append(id);
    }
    if (hiddenCategories_.isEmpty())
        settings_.remove("hiddenCategories");
    else
        settings_.setValue("hiddenCategories", hiddenCategories_);
    emit hiddenCategoriesChanged();
}

void CalcBridge::showAllCategories() {
    if (hiddenCategories_.isEmpty()) return;
    hiddenCategories_.clear();
    settings_.remove("hiddenCategories");
    emit hiddenCategoriesChanged();
}

void CalcBridge::toggleFavorite(const QString& id) {
    if (favorites_.contains(id)) {
        favorites_.removeAll(id);
    } else {
        favorites_.append(id);
    }
    settings_.setValue("favorites", favorites_);
    emit favoritesChanged();
}

void CalcBridge::copyToClipboard(const QString& text) const {
    if (auto* cb = QGuiApplication::clipboard()) cb->setText(text);
}

QVariantList CalcBridge::inputList(const std::vector<elc::InputDef>& defs) const {
    QVariantList inputs;
    for (const auto& in : defs) {
        const char* kind = in.kind == elc::InputDef::Kind::Choice ? "choice" : (in.kind == elc::InputDef::Kind::Text ? "text" : "number");
        const QString unit = in.unit == elc::kCurrencyUnit ? q(elc::currencySymbol(context())) : q(in.unit);
        QVariantMap m{{"id", q(in.id)},
                      {"label", txt(in.label)},
                      {"unit", unit},
                      {"kind", kind},
                      {"optional", in.optional && in.kind == elc::InputDef::Kind::Number},
                      {"hint", txt(in.hint)},
                      {"section", txt(in.section)}};
        if (in.kind == elc::InputDef::Kind::Choice) {
            QVariantList opts;
            for (const auto& o : in.options) opts.append(QVariantMap{{"value", q(o.value)}, {"label", txt(o.label)}});
            m.insert("options", opts);
        }
        if (in.visibleIf) {
            QStringList values;
            for (const auto& v : in.visibleIf->values) values.append(q(v));
            m.insert("visibleIf", QVariantMap{{"input", q(in.visibleIf->inputId)}, {"values", values}});
        }
        inputs.append(m);
    }
    return inputs;
}

QVariantMap CalcBridge::textValues(const std::vector<elc::InputDef>& defs, const elc::Inputs& in) const {
    QVariantMap m;
    for (const auto& d : defs) {
        if (isTextLike(d)) {
            m.insert(q(d.id), q(in.choice(d.id)));
        } else {
            const double v = in.num(d.id);
            m.insert(q(d.id), std::isfinite(v) ? q(elc::formatNumber(v, 6, lang_)) : QString());
        }
    }
    return m;
}

QVariantList CalcBridge::outputList(const std::vector<elc::OutputDef>& defs, const elc::Result& r) const {
    QVariantList outputs;
    for (const auto& out : defs) {
        QString value;
        QString unit = q(out.unit);
        if (out.isText) {
            const auto it = r.texts.find(out.id);
            if (it == r.texts.end()) continue;
            value = txt(it->second);
        } else {
            const auto it = r.values.find(out.id);
            if (it == r.values.end()) continue;
            value = q(elc::formatNumber(it->second, out.decimals, lang_));
            if (const auto u = r.units.find(out.id); u != r.units.end()) unit = q(u->second);
        }
        const auto alt = r.alt.find(out.id);
        outputs.append(QVariantMap{{"id", q(out.id)},
                                   {"label", txt(out.label)},
                                   {"alt", alt == r.alt.end() ? QString() : txt(alt->second)},
                                   {"short", txt(out.shortLabel.de.empty() ? out.label : out.shortLabel)},
                                   {"value", value},
                                   {"unit", unit},
                                   {"key", out.key}});
    }
    return outputs;
}

QVariantList CalcBridge::messageList(const elc::Result& r) const {
    QVariantList messages;
    for (const auto& m : r.messages) messages.append(QVariantMap{{"severity", severityName(m.severity)}, {"text", txt(m.text)}});
    return messages;
}

QVariantMap CalcBridge::calculator(const QString& id) const {
    const auto* c = elc::findCalculator(id.toStdString());
    if (!c) return {};
    const auto* cat = findCategory(c->category);
    return {{"id", id},
            {"title", txt(c->title)},
            {"summary", txt(c->summary)},
            {"description", txt(c->description)},
            {"color", categoryColor(c->category)},
            {"category", cat ? txt(cat->title) : QString()},
            {"inputs", inputList(c->inputs)},
            {"hasChart", static_cast<bool>(c->chart)},
            {"hasPick", static_cast<bool>(c->pick)}};
}

QVariantMap CalcBridge::chart(const QString& id, const QVariantMap& values) const {
    const auto* c = elc::findCalculator(id.toStdString());
    if (!c) return {};
    const auto ch = elc::evaluateChart(*c, toInputs(c->inputs, values), context());
    if (!ch) return {};
    QVariantList series;
    for (const auto& s : ch->series) {
        QVariantList pts;
        pts.reserve(static_cast<qsizetype>(2 * s.pts.size()));
        for (const auto& p : s.pts) {
            pts.append(p[0]);
            pts.append(p[1]);
        }
        series.append(QVariantMap{{"style", q(s.style)}, {"pts", pts}});
    }
    QVariantList labels;
    for (const auto& l : ch->labels) labels.append(QVariantMap{{"x", l.x}, {"y", l.y}, {"text", txt(l.text)}, {"style", q(l.style)}});
    QVariantList markers;
    for (const auto& m : ch->markers) markers.append(QVariantMap{{"x", m.x}, {"y", m.y}, {"label", q(m.label)}, {"style", q(m.style)}});
    return {{"xmin", ch->x.min},     {"xmax", ch->x.max},     {"xstep", ch->x.step},       {"xtitle", txt(ch->x.title)},
            {"ymin", ch->y.min},     {"ymax", ch->y.max},     {"ystep", ch->y.step},       {"ytitle", txt(ch->y.title)},
            {"note", txt(ch->note)}, {"series", series},      {"labels", labels},          {"markers", markers},
            {"diagram", ch->diagram}, {"aspect", ch->aspect}};
}

QVariantMap CalcBridge::chartPick(const QString& id, const QVariantMap& values, double x, double y) const {
    const auto* c = elc::findCalculator(id.toStdString());
    if (!c) return {};
    const auto picked = elc::pickChart(*c, toInputs(c->inputs, values), x, y, context());
    if (picked.empty()) return {};
    QVariantMap out = values;
    for (const auto& v : picked) {
        out.insert(q(v.id), v.choice.empty() ? q(elc::formatNumber(v.number, 6, lang_)) : q(v.choice));
    }
    return out;
}

QVariantMap CalcBridge::defaults(const QString& id) const {
    const auto* c = elc::findCalculator(id.toStdString());
    if (!c) return {};
    QVariantMap m;
    for (const auto& in : c->inputs) {
        if (isTextLike(in)) {
            m.insert(q(in.id), q(in.defaultChoice));
        } else {
            m.insert(q(in.id), std::isfinite(in.defaultValue) ? q(elc::formatNumber(in.defaultValue, 6, lang_)) : QString());
        }
    }
    return m;
}

QVariantMap CalcBridge::compute(const QString& id, const QVariantMap& values) const {
    const auto* c = elc::findCalculator(id.toStdString());
    if (!c) return {};
    const auto in = toInputs(c->inputs, values);
    const auto r = elc::evaluate(*c, in, context());

    QVariantList outputs;
    QString copy = txt(c->title) + '\n';
    for (const auto& def : c->inputs) {
        if (!elc::isVisible(def, in)) continue;
        QString value = values.value(q(def.id)).toString();
        if (def.kind == elc::InputDef::Kind::Choice) {
            for (const auto& o : def.options) {
                if (o.value == value.toStdString()) value = txt(o.label);
            }
        } else if (value.isEmpty()) {
            continue;
        }
        const QString unit = def.unit == elc::kCurrencyUnit ? q(elc::currencySymbol(context())) : q(def.unit);
        copy += "  " + QStringLiteral("%1: %2 %3").arg(txt(def.label), value, unit).trimmed() + '\n';
    }
    copy += QStringLiteral("→\n");
    for (const auto& out : c->outputs) {
        QString value;
        QString unit = q(out.unit);
        if (out.isText) {
            const auto it = r.texts.find(out.id);
            if (it == r.texts.end()) continue;
            value = txt(it->second);
        } else {
            const auto it = r.values.find(out.id);
            if (it == r.values.end()) continue;
            value = q(elc::formatNumber(it->second, out.decimals, lang_));
            if (const auto u = r.units.find(out.id); u != r.units.end()) unit = q(u->second);
        }
        const auto altIt = r.alt.find(out.id);
        const QString alt = altIt == r.alt.end() ? QString() : txt(altIt->second);
        outputs.append(QVariantMap{{"id", q(out.id)},
                                   {"label", txt(out.label)},
                                   {"value", value},
                                   {"unit", unit},
                                   {"alt", alt},
                                   {"key", out.key},
                                   {"short", txt(out.shortLabel.de.empty() ? out.label : out.shortLabel)}});
        copy += QStringLiteral("  %1: %2 %3%4\n").arg(txt(out.label), value, unit, alt.isEmpty() ? QString() : "  (" + alt + ")");
    }
    QVariantList messages;
    for (const auto& m : r.messages) {
        messages.append(QVariantMap{{"severity", severityName(m.severity)}, {"text", txt(m.text)}});
        copy += QStringLiteral("  • %1\n").arg(txt(m.text));
    }
    return {{"outputs", outputs}, {"messages", messages}, {"hasError", r.hasError()}, {"copyText", copy.trimmed()}};
}

QVariantMap CalcBridge::table(const QString& id) const {
    const auto* t = elc::findTable(id.toStdString());
    if (!t) return {};
    QStringList columns;
    for (const auto& c : t->columns) columns.append(txt(c));
    QVariantList rows;
    for (const auto& row : t->rows) {
        QStringList cells;
        for (const auto& cell : row) cells.append(txt(cell));
        rows.append(cells);
    }
    return {{"id", id},
            {"title", txt(t->title)},
            {"summary", txt(t->summary)},
            {"description", txt(t->description)},
            {"color", categoryColor(t->category)},
            {"columns", columns},
            {"rows", rows}};
}

// ---------------------------------------------------------------- project lists

std::vector<elc::Inputs> CalcBridge::rowsFrom(const elc::ListDef& def, const QVariantList& rows) const {
    std::vector<elc::Inputs> out;
    out.reserve(static_cast<std::size_t>(rows.size()));
    for (const auto& r : rows) out.push_back(toInputs(def.rowInputs, r.toMap()));
    return out;
}

QVariantMap CalcBridge::listDef(const QString& id) const {
    const auto* l = elc::findList(id.toStdString());
    if (!l) return {};
    const auto* cat = findCategory(l->category);
    QVariantList rowOutputs;
    for (const auto& o : l->rowOutputs) rowOutputs.append(QVariantMap{{"id", q(o.id)}, {"label", txt(o.label)}, {"key", o.key}});
    return {{"id", id},
            {"title", txt(l->title)},
            {"summary", txt(l->summary)},
            {"description", txt(l->description)},
            {"color", categoryColor(l->category)},
            {"category", cat ? txt(cat->title) : QString()},
            {"settingsInputs", inputList(l->settingsInputs)},
            {"rowInputs", inputList(l->rowInputs)},
            {"rowOutputs", rowOutputs},
            {"titleInput", q(l->rowInputs.front().id)},
            {"hasDetail", static_cast<bool>(l->expand)},
            {"detailTitle", txt(l->expandTitle)}};
}

QVariantList CalcBridge::exampleRows(const QString& id) const {
    const auto* l = elc::findList(id.toStdString());
    if (!l) return {};
    QVariantList rows;
    for (const auto& r : elc::exampleRows(*l)) rows.append(textValues(l->rowInputs, r));
    return rows;
}

QVariantList CalcBridge::listRows(const QString& id) const {
    const auto stored = settings_.value(listKey(id, "rows"));
    if (!stored.isValid()) return exampleRows(id);
    return QJsonDocument::fromJson(stored.toByteArray()).array().toVariantList();
}

QVariantMap CalcBridge::listSettings(const QString& id) const {
    const auto* l = elc::findList(id.toStdString());
    if (!l) return {};
    auto values = textValues(l->settingsInputs, elc::defaultSettings(*l));
    const auto stored = QJsonDocument::fromJson(settings_.value(listKey(id, "settings")).toByteArray()).object().toVariantMap();
    for (auto it = stored.begin(); it != stored.end(); ++it) {
        if (values.contains(it.key())) values.insert(it.key(), it.value());
    }
    return values;
}

void CalcBridge::saveList(const QString& id, const QVariantList& rows, const QVariantMap& settings) {
    if (!elc::findList(id.toStdString())) return;
    // QSettings stores the rows as compact JSON text – portable between Windows registry and Android ini files
    settings_.setValue(listKey(id, "rows"), QJsonDocument(QJsonArray::fromVariantList(rows)).toJson(QJsonDocument::Compact));
    settings_.setValue(listKey(id, "settings"), QJsonDocument(QJsonObject::fromVariantMap(settings)).toJson(QJsonDocument::Compact));
}

QVariantMap CalcBridge::defaultListRow(const QString& id) const {
    const auto* l = elc::findList(id.toStdString());
    if (!l) return {};
    return textValues(l->rowInputs, elc::defaultRow(*l));
}

QVariantMap CalcBridge::computeList(const QString& id, const QVariantList& rows, const QVariantMap& settings) const {
    const auto* l = elc::findList(id.toStdString());
    if (!l) return {};
    const auto in = rowsFrom(*l, rows);
    const auto cfg = toInputs(l->settingsInputs, settings);
    const auto res = elc::evaluateList(*l, in, cfg, context());

    // second line of a row card: the remaining text inputs (e.g. trade · panel · room) or the first key text output
    QVariantList outRows;
    int nok = 0;
    for (std::size_t i = 0; i < in.size(); ++i) {
        const auto& r = res.rows[i];
        QStringList sub;
        for (std::size_t k = 1; k < l->rowInputs.size(); ++k) {
            const auto& d = l->rowInputs[k];
            if (d.kind == elc::InputDef::Kind::Text && !in[i].choice(d.id).empty()) sub.append(q(in[i].choice(d.id)));
            if (sub.size() >= 3) break;
        }
        const auto status = statusOf(r);
        nok += r.hasError() ? 1 : 0;
        const auto outputs = outputList(l->rowOutputs, r);
        QVariantList keys;
        for (const auto& o : outputs) {
            if (o.toMap().value("key").toBool()) keys.append(o);
        }
        outRows.append(QVariantMap{{"title", q(in[i].choice(l->rowInputs.front().id))},
                                   {"subtitle", sub.join(QStringLiteral(" · "))},
                                   {"status", status},
                                   {"hasError", r.hasError()},
                                   {"outputs", outputs},
                                   {"keyOutputs", keys},
                                   {"messages", messageList(r)}});
    }
    return {{"rows", outRows},
            {"totals", outputList(l->totalOutputs, res.totals)},
            {"totalMessages", messageList(res.totals)},
            {"nok", nok}};
}

QVariantMap CalcBridge::computeRow(const QString& id, const QVariantMap& row, const QVariantMap& settings) const {
    const auto* l = elc::findList(id.toStdString());
    if (!l) return {};
    const auto r = elc::evaluateRow(*l, toInputs(l->rowInputs, row), toInputs(l->settingsInputs, settings), context());
    return {{"outputs", outputList(l->rowOutputs, r)}, {"messages", messageList(r)}, {"hasError", r.hasError()}, {"status", statusOf(r)}};
}

QString CalcBridge::exportList(const QString& id, const QVariantList& rows, const QVariantMap& settings, bool detail,
                               bool tabs) const {
    const auto* l = elc::findList(id.toStdString());
    if (!l) return {};
    return q(elc::exportCsv(*l, rowsFrom(*l, rows), toInputs(l->settingsInputs, settings), context(), lang_, detail,
                            tabs ? '\t' : ';'));
}

QString CalcBridge::saveCsv(const QString& baseName, const QString& content) const {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (dir.isEmpty()) dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    dir += QStringLiteral("/ElektroRechner");
    if (!QDir().mkpath(dir)) return {};
    const QString path = QStringLiteral("%1/%2_%3.csv").arg(dir, baseName, QDate::currentDate().toString(Qt::ISODate));
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return {};
    f.write("\xEF\xBB\xBF");  // BOM: Excel then reads UTF-8 (umlauts, ², Δ) correctly
    f.write(content.toUtf8());
    return f.error() == QFile::NoError ? QDir::toNativeSeparators(path) : QString();
}

QVariantList CalcBridge::importList(const QString& id, const QString& text) const {
    const auto* l = elc::findList(id.toStdString());
    if (!l) return {};
    QVariantList rows;
    for (const auto& r : elc::importTable(*l, text.toStdString())) rows.append(textValues(l->rowInputs, r));
    return rows;
}

QString CalcBridge::clipboardText() const {
    if (const auto* cb = QGuiApplication::clipboard()) return cb->text();
    return {};
}
