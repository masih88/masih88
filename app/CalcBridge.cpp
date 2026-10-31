#include "CalcBridge.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QLocale>
#include <cmath>

#include "UiStrings.h"
#include "elc/registry.hpp"

namespace {

QString q(const std::string& s) { return QString::fromStdString(s); }

QString categoryColor(const std::string& id) {
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

}  // namespace

CalcBridge::CalcBridge(QObject* parent) : QObject(parent) {
    const QString systemLang = QLocale::system().language() == QLocale::German ? "de" : "en";
    const QString lang = qEnvironmentVariable("ELC_LANG", settings_.value("language", systemLang).toString());
    lang_ = lang == "en" ? elc::Lang::En : elc::Lang::De;
    norm_ = settings_.value("norm", "de").toString() == "ch" ? elc::NormProfile::CH : elc::NormProfile::DE;
    dark_ = qEnvironmentVariable("ELC_THEME", settings_.value("dark", false).toBool() ? "dark" : "light") == "dark";
    favorites_ = settings_.value("favorites").toStringList();
    startPage_ = qEnvironmentVariable("ELC_OPEN");  // used for automated screenshots
}

QString CalcBridge::txt(const elc::Text& t) const { return q(t.in(lang_)); }

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
        list.append(QVariantMap{{"id", q(c.id)},
                                {"title", txt(c.title)},
                                {"badge", q(c.badge)},
                                {"color", categoryColor(c.id)},
                                {"count", calcs + tables}});
    }
    return list;
}

QVariantMap CalcBridge::itemFor(const QString& id) const {
    const auto sid = id.toStdString();
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
    return {};
}

QVariantList CalcBridge::items(const QString& categoryId) const {
    QVariantList list;
    const auto cid = categoryId.toStdString();
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

QVariantMap CalcBridge::calculator(const QString& id) const {
    const auto* c = elc::findCalculator(id.toStdString());
    if (!c) return {};
    QVariantList inputs;
    for (const auto& in : c->inputs) {
        QVariantMap m{{"id", q(in.id)},
                      {"label", txt(in.label)},
                      {"unit", q(in.unit)},
                      {"kind", in.kind == elc::InputDef::Kind::Choice ? "choice" : "number"},
                      {"optional", in.optional},
                      {"hint", txt(in.hint)}};
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
    const auto* cat = findCategory(c->category);
    return {{"id", id},
            {"title", txt(c->title)},
            {"summary", txt(c->summary)},
            {"description", txt(c->description)},
            {"color", categoryColor(c->category)},
            {"category", cat ? txt(cat->title) : QString()},
            {"inputs", inputs}};
}

QVariantMap CalcBridge::defaults(const QString& id) const {
    const auto* c = elc::findCalculator(id.toStdString());
    if (!c) return {};
    QVariantMap m;
    for (const auto& in : c->inputs) {
        if (in.kind == elc::InputDef::Kind::Choice) {
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
    elc::Inputs in;
    for (const auto& def : c->inputs) {
        const auto text = values.value(q(def.id)).toString().toStdString();
        if (def.kind == elc::InputDef::Kind::Choice) {
            in.setChoice(def.id, text);
        } else {
            in.set(def.id, elc::parseNumber(text).value_or(elc::kNaN));
        }
    }
    const auto r = elc::evaluate(*c, in, elc::Context{norm_});

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
        copy += QStringLiteral("  %1: %2 %3\n").arg(txt(def.label), value, q(def.unit));
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
        outputs.append(QVariantMap{{"id", q(out.id)}, {"label", txt(out.label)}, {"value", value}, {"unit", unit}, {"key", out.key}});
        copy += QStringLiteral("  %1: %2 %3\n").arg(txt(out.label), value, unit);
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
