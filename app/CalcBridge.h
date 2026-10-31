#pragma once
// Bridge between the C++ calculation core and QML. Exposed to QML as context property "Calc".
// Everything the UI shows is generated from the core's data model, so new calculators
// appear in the app without touching QML.

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

#include "elc/lists.hpp"
#include "elc/types.hpp"

class CalcBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString norm READ norm WRITE setNorm NOTIFY normChanged)
    Q_PROPERTY(bool darkTheme READ darkTheme WRITE setDarkTheme NOTIFY darkThemeChanged)
    Q_PROPERTY(QVariantMap ui READ ui NOTIFY languageChanged)
    Q_PROPERTY(QVariantList categories READ categories NOTIFY languageChanged)
    Q_PROPERTY(QStringList favorites READ favorites NOTIFY favoritesChanged)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString startPage READ startPage CONSTANT)

public:
    explicit CalcBridge(QObject* parent = nullptr);

    [[nodiscard]] QString language() const;
    void setLanguage(const QString& lang);
    [[nodiscard]] QString norm() const;
    void setNorm(const QString& norm);
    [[nodiscard]] bool darkTheme() const { return dark_; }
    void setDarkTheme(bool dark);
    [[nodiscard]] QVariantMap ui() const;
    [[nodiscard]] QVariantList categories() const;
    [[nodiscard]] QStringList favorites() const { return favorites_; }
    [[nodiscard]] QString version() const;
    [[nodiscard]] QString startPage() const { return startPage_; }

    /// Calculators and tables of one category: [{id, kind, title, summary, badge, color}]
    Q_INVOKABLE QVariantList items(const QString& categoryId) const;
    Q_INVOKABLE QVariantList search(const QString& text) const;
    Q_INVOKABLE QVariantList favoriteItems() const;
    Q_INVOKABLE QString kindOf(const QString& id) const;  // "calc", "table", "list" or ""

    /// Definition for the generic calculator page
    Q_INVOKABLE QVariantMap calculator(const QString& id) const;
    /// Input values as text keyed by input id -> {outputs, messages, hasError, copyText}
    Q_INVOKABLE QVariantMap compute(const QString& id, const QVariantMap& values) const;
    /// Default input values as text
    Q_INVOKABLE QVariantMap defaults(const QString& id) const;
    Q_INVOKABLE QVariantMap table(const QString& id) const;
    /// Chart of a calculator: {xmin, xmax, xstep, ymin, ymax, ystep, xtitle, ytitle, note,
    /// series: [{style, pts: [x0, y0, x1, y1, …]}], labels: [{x, y, text, style}], markers: [{x, y, label, style}]}
    Q_INVOKABLE QVariantMap chart(const QString& id, const QVariantMap& values) const;
    /// Values after tapping at (x, y) in the chart (data units); empty map if nothing changes
    Q_INVOKABLE QVariantMap chartPick(const QString& id, const QVariantMap& values, double x, double y) const;

    // ---- Project lists (consumer list, data point list) ----
    /// {id, title, summary, description, color, category, settingsInputs, rowInputs, hasDetail, detailTitle}
    Q_INVOKABLE QVariantMap listDef(const QString& id) const;
    /// Saved rows (values as text) – the example project if nothing has been saved yet
    Q_INVOKABLE QVariantList listRows(const QString& id) const;
    Q_INVOKABLE QVariantMap listSettings(const QString& id) const;
    Q_INVOKABLE void saveList(const QString& id, const QVariantList& rows, const QVariantMap& settings);
    Q_INVOKABLE QVariantMap defaultListRow(const QString& id) const;
    Q_INVOKABLE QVariantList exampleRows(const QString& id) const;
    /// {rows: [{title, subtitle, status, outputs, keyOutputs}], totals: [...], status, nok}
    Q_INVOKABLE QVariantMap computeList(const QString& id, const QVariantList& rows, const QVariantMap& settings) const;
    /// One row like compute(): {outputs, messages, hasError}
    Q_INVOKABLE QVariantMap computeRow(const QString& id, const QVariantMap& row, const QVariantMap& settings) const;
    /// CSV (';', for files) or tab separated (for pasting into Excel); detail = expanded data point list
    Q_INVOKABLE QString exportList(const QString& id, const QVariantList& rows, const QVariantMap& settings, bool detail,
                                   bool tabs) const;
    /// Writes a UTF-8 CSV (with BOM for Excel) to Documents/ElektroRechner, returns the path or "" on failure
    Q_INVOKABLE QString saveCsv(const QString& baseName, const QString& content) const;
    /// Rows from a pasted table (Excel copy, CSV); empty if no column matched
    Q_INVOKABLE QVariantList importList(const QString& id, const QString& text) const;
    Q_INVOKABLE QString clipboardText() const;

    Q_INVOKABLE bool isFavorite(const QString& id) const { return favorites_.contains(id); }
    Q_INVOKABLE void toggleFavorite(const QString& id);
    Q_INVOKABLE void copyToClipboard(const QString& text) const;

signals:
    void languageChanged();
    void normChanged();
    void darkThemeChanged();
    void favoritesChanged();

private:
    [[nodiscard]] QString txt(const elc::Text& t) const;
    [[nodiscard]] QVariantMap itemFor(const QString& id) const;
    [[nodiscard]] QVariantList inputList(const std::vector<elc::InputDef>& defs) const;
    [[nodiscard]] QVariantMap textValues(const std::vector<elc::InputDef>& defs, const elc::Inputs& in) const;
    [[nodiscard]] QVariantList outputList(const std::vector<elc::OutputDef>& defs, const elc::Result& r) const;
    [[nodiscard]] QVariantList messageList(const elc::Result& r) const;
    [[nodiscard]] std::vector<elc::Inputs> rowsFrom(const elc::ListDef& def, const QVariantList& rows) const;

    QSettings settings_;
    elc::Lang lang_ = elc::Lang::De;
    elc::NormProfile norm_ = elc::NormProfile::DE;
    bool dark_ = false;
    QStringList favorites_;
    QString startPage_;
};
