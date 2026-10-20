#ifndef GUI_SETTINGS_DIALOG_H
#define GUI_SETTINGS_DIALOG_H
#include <QDialog>

class QSettings;

class SettingsDialog : public QDialog {
public:
    explicit SettingsDialog(QWidget* parent = nullptr);

    // Whether irreducible representation exercises include point groups with
    // complex-conjugate pairs of irreps (E+ and E-); off by default.
    static bool load_include_complex_irreps(QSettings& settings);
    static bool save_include_complex_irreps(QSettings& settings, bool include);
};
#endif
