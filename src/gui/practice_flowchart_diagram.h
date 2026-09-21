/**
 * Interactive progress map for the point-group decision tree.
 */
#ifndef GUI_PRACTICE_FLOWCHART_DIAGRAM_H
#define GUI_PRACTICE_FLOWCHART_DIAGRAM_H

#include <string>
#include <vector>
#include <QSvgWidget>

class PracticeFlowchartDiagram: public QSvgWidget {
    Q_OBJECT

public:
    explicit PracticeFlowchartDiagram(QWidget* parent = nullptr);

    /** Update the highlighted route through the decision tree. */
    void set_path(const std::vector<std::string>& path);

    /** Switch between the focused route lens and the complete tree. */
    void set_overview_mode(bool overview);

    /** Return the route currently highlighted by the diagram. */
    const std::vector<std::string>& get_path() const;

private:
    bool overview_mode = false;
    std::vector<std::string> current_path = {"start"};

    void render(const std::vector<std::string>& path);
    void render_overview(const std::vector<std::string>& path);
};

#endif  // GUI_PRACTICE_FLOWCHART_DIAGRAM_H
