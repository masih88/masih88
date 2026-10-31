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
    Q_INVOKABLE QString kindOf(const QString& id) const;  // "calc", "table" or ""

    /// Definition for the generic calculator page
    Q_INVOKABLE QVariantMap calculator(const QString& id) const;
    /// Input values as text keyed by input id -> {outputs, messages, hasError, copyText}
    Q_INVOKABLE QVariantMap compute(const QString& id, const QVariantMap& values) const;
    /// Default input values as text
    Q_INVOKABLE QVariantMap defaults(const QString& id) const;
    Q_INVOKABLE QVariantMap table(const QString& id) const;

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

    QSettings settings_;
    elc::Lang lang_ = elc::Lang::De;
    elc::NormProfile norm_ = elc::NormProfile::DE;
    bool dark_ = false;
    QStringList favorites_;
    QString startPage_;
};
