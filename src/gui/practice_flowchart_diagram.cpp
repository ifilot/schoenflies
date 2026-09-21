#include "practice_flowchart_diagram.h"

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <QByteArray>
#include <QString>

namespace {
struct Node {
    const char* key;
    const char* label;
    bool result;
};

struct Edge {
    const char* from;
    const char* to;
    const char* answer;
};

const std::vector<Node> nodes = {
    {"start", "Is the molecule linear?", false},
    {"linear", "Does it have an inversion centre?", false},
    {"Cinfv", "C∞v", true}, {"Dinfh", "D∞h", true},
    {"higher_order", "Several Cₙ axes with n > 2?", false},
    {"high_sym", "Does it have an inversion centre?", false},
    {"Td", "Td", true}, {"C5", "Does it have a C₅ axis?", false},
    {"Oh", "Oh", true}, {"Ih", "Ih", true},
    {"C", "Does it have any proper axis?", false},
    {"no_C", "Does it have a reflection plane?", false},
    {"Cs", "Cs", true}, {"nothing", "Does it have an inversion centre?", false},
    {"C1", "C₁", true}, {"Ci", "Ci", true},
    {"yes_C", "Choose the highest-order Cₙ", false},
    {"nC2", "Are there n perpendicular C₂ axes?", false},
    {"D_horizontal", "Is there a horizontal σₕ plane?", false},
    {"Dnh", "Dₙh", true}, {"D_dihedral", "Are there dihedral σd planes?", false},
    {"Dn", "Dₙ", true}, {"Dnd", "Dₙd", true},
    {"C_horizontal", "Is there a horizontal σₕ plane?", false},
    {"Cnh", "Cₙh", true}, {"C_vertical", "Are there vertical σv planes?", false},
    {"Cnv", "Cₙv", true}, {"imp_rot", "Is there an improper S₂ₙ axis?", false},
    {"Cn", "Cₙ", true}, {"S2n", "S₂ₙ", true}
};

const std::vector<Edge> edges = {
    {"start", "higher_order", "No"}, {"start", "linear", "Yes"},
    {"linear", "Cinfv", "No"}, {"linear", "Dinfh", "Yes"},
    {"higher_order", "C", "No"}, {"higher_order", "high_sym", "Yes"},
    {"high_sym", "Td", "No"}, {"high_sym", "C5", "Yes"},
    {"C5", "Oh", "No"}, {"C5", "Ih", "Yes"},
    {"C", "no_C", "No"}, {"C", "yes_C", "Yes"},
    {"no_C", "nothing", "No"}, {"no_C", "Cs", "Yes"},
    {"nothing", "C1", "No"}, {"nothing", "Ci", "Yes"},
    {"yes_C", "nC2", "n"},
    {"nC2", "C_horizontal", "No"}, {"nC2", "D_horizontal", "Yes"},
    {"D_horizontal", "D_dihedral", "No"}, {"D_horizontal", "Dnh", "Yes"},
    {"D_dihedral", "Dn", "No"}, {"D_dihedral", "Dnd", "Yes"},
    {"C_horizontal", "C_vertical", "No"}, {"C_horizontal", "Cnh", "Yes"},
    {"C_vertical", "imp_rot", "No"}, {"C_vertical", "Cnv", "Yes"},
    {"imp_rot", "Cn", "No"}, {"imp_rot", "S2n", "Yes"}
};

QString escaped(const char* text) {
    return QString::fromUtf8(text).toHtmlEscaped();
}
}

PracticeFlowchartDiagram::PracticeFlowchartDiagram(QWidget* parent): QSvgWidget(parent) {
    this->setMinimumHeight(520);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->render({"start"});
}

void PracticeFlowchartDiagram::set_path(const std::vector<std::string>& path) {
    this->current_path = path.empty() ? std::vector<std::string>{"start"} : path;
    if (this->overview_mode) this->render_overview(this->current_path);
    else this->render(this->current_path);
}

void PracticeFlowchartDiagram::set_overview_mode(bool overview) {
    this->overview_mode = overview;
    if (overview) this->render_overview(this->current_path);
    else this->render(this->current_path);
}

const std::vector<std::string>& PracticeFlowchartDiagram::get_path() const {
    return this->current_path;
}

void PracticeFlowchartDiagram::render(const std::vector<std::string>& path) {
    std::unordered_map<std::string, Node> by_key;
    std::unordered_map<std::string, std::vector<Edge>> children;
    for (const Node& node : nodes) by_key.emplace(node.key, node);
    for (const Edge& edge : edges) children[edge.from].push_back(edge);

    const std::string active_key = path.empty() ? "start" : path.back();
    const Node& active_node = by_key.at(active_key);
    const double route_gap = std::min(72.0, 470.0 / std::max<std::size_t>(path.size(), 1));
    const double active_y = 132.0 + (path.empty() ? 0.0 : (path.size() - 1) * route_gap);

    auto node_width = [](const Node& node, int minimum, int maximum) {
        int estimate = 34 + static_cast<int>(QString::fromUtf8(node.label).size()) * 8;
        return std::max(minimum, std::min(maximum, estimate));
    };

    QString svg = QStringLiteral(
        "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 620 900'>"
        "<defs>"
        "<linearGradient id='done' x1='0' y1='0' x2='1' y2='1'>"
        "<stop offset='0' stop-color='#dcfce7'/><stop offset='1' stop-color='#86efac'/></linearGradient>"
        "</defs><rect x='2' y='2' width='616' height='896' rx='24' fill='#fffdf7' stroke='#d9d5c9' stroke-width='3'/>"
        "<text x='30' y='42' fill='#20242b' font-family='Segoe UI' font-size='25' font-weight='700'>"
        "Your route</text>"
        "<text x='30' y='69' fill='#667085' font-family='Segoe UI' font-size='14'>"
        "Only the relevant part of the decision tree is expanded.</text>");

    // Completed decisions form a compact, readable breadcrumb down the centre.
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
        const Node& node = by_key.at(path[i]);
        const double y = 112.0 + i * route_gap;
        const int width = node_width(node, 150, 230);
        svg += QString("<rect x='%1' y='%2' width='%3' height='38' rx='19' fill='#dcfce7' "
                       "stroke='#15803d' stroke-width='2'/>")
            .arg(310 - width / 2).arg(y).arg(width);
        svg += QString("<text x='310' y='%1' text-anchor='middle' dominant-baseline='middle' "
                       "fill='#14532d' font-family='Segoe UI' font-size='13' font-weight='600'>%2</text>")
            .arg(y + 20).arg(escaped(node.label));

        const std::string& next_key = path[i + 1];
        QString answer;
        const Edge* rejected = nullptr;
        for (const Edge& edge : children[path[i]]) {
            if (edge.to == next_key) answer = edge.answer;
            else rejected = &edge;
        }
        svg += QString("<path d='M310 %1 V %2' stroke='#15803d' stroke-width='3'/>")
            .arg(y + 38).arg(y + route_gap);
        svg += QString("<text x='322' y='%1' fill='#15803d' font-family='Segoe UI' font-size='12' "
                       "font-weight='700'>%2</text>").arg(y + 53).arg(answer);

        // The unchosen sibling stays visible as quiet context, without its subtree.
        if (rejected != nullptr && route_gap >= 52.0) {
            const Node& sibling = by_key.at(rejected->to);
            int sibling_width = node_width(sibling, 86, 170);
            svg += QString("<path d='M %1 %2 H %3' stroke='#c7cbd1' stroke-width='2' "
                           "stroke-dasharray='4 5'/>")
                .arg(310 - width / 2).arg(y + 19).arg(105 + sibling_width / 2);
            svg += QString("<rect x='%1' y='%2' width='%3' height='30' rx='15' fill='#f3f4f6' "
                           "stroke='#d1d5db'/>")
                .arg(105 - sibling_width / 2).arg(y + 4).arg(sibling_width);
            svg += QString("<text x='105' y='%1' text-anchor='middle' dominant-baseline='middle' "
                           "fill='#9ca3af' font-family='Segoe UI' font-size='10'>%2: %3</text>")
                .arg(y + 20).arg(rejected->answer).arg(escaped(sibling.label));
        }
    }

    // Match the application's restrained card language: blue for an action in progress,
    // and green only when the route has reached a result.
    const int active_width = node_width(active_node, active_node.result ? 100 : 230,
                                        active_node.result ? 180 : 330);
    const int active_height = active_node.result ? 48 : 58;
    const QString active_fill = active_node.result ? "#ecfdf5" : "#ffffff";
    const QString active_stroke = active_node.result ? "#15803d" : "#0284c7";
    const QString active_text = active_node.result ? "#14532d" : "#172033";
    svg += QString("<rect x='%1' y='%2' width='%3' height='%4' rx='12' fill='%5' "
                   "stroke='%6' stroke-width='2'/>")
        .arg(310 - active_width / 2).arg(active_y).arg(active_width).arg(active_height)
        .arg(active_fill).arg(active_stroke);
    svg += QString("<text x='310' y='%1' text-anchor='middle' dominant-baseline='middle' fill='%2' "
                   "font-family='Segoe UI' font-size='%3' font-weight='700'>%4</text>")
        .arg(active_y + active_height / 2 + 1).arg(active_text)
        .arg(active_node.result ? 15 : 16).arg(escaped(active_node.label));
    svg += QString("<text x='310' y='%1' text-anchor='middle' fill='%2' font-family='Segoe UI' "
                   "font-size='11' font-weight='700' letter-spacing='1'>%3</text>")
        .arg(active_y - 13).arg(active_stroke)
        .arg(active_node.result ? "RESULT" : "CURRENT STEP");

    const auto& options = children[active_key];
    if (!options.empty()) {
        const double option_y = std::min(active_y + 150.0, 800.0);
        for (std::size_t i = 0; i < options.size(); ++i) {
            const Edge& edge = options[i];
            const Node& child = by_key.at(edge.to);
            const double x = options.size() == 1 ? 310.0 : (i == 0 ? 165.0 : 455.0);
            const int width = node_width(child, child.result ? 100 : 155, 230);
            svg += QString("<path d='M310 %1 C310 %2, %3 %2, %3 %4' fill='none' stroke='#9ca3af' "
                           "stroke-width='2' stroke-dasharray='6 6'/>")
                .arg(active_y + active_height).arg((active_y + option_y) / 2).arg(x).arg(option_y);
            svg += QString("<text x='%1' y='%2' text-anchor='middle' fill='#667085' font-family='Segoe UI' "
                           "font-size='13' font-weight='700'>%3</text>")
                .arg((310 + x) / 2).arg((active_y + option_y) / 2 - 5).arg(edge.answer);
            svg += QString("<rect x='%1' y='%2' width='%3' height='48' rx='14' fill='%4' stroke='#8b8172' "
                           "stroke-width='2'/>")
                .arg(x - width / 2).arg(option_y).arg(width).arg(child.result ? "#dcebd8" : "#f3e3b5");
            svg += QString("<text x='%1' y='%2' text-anchor='middle' dominant-baseline='middle' fill='#30343b' "
                           "font-family='Segoe UI' font-size='13' font-weight='600'>%3</text>")
                .arg(x).arg(option_y + 25).arg(escaped(child.label));
        }
    } else {
        svg += QString("<text x='310' y='%1' text-anchor='middle' fill='#15803d' font-family='Segoe UI' "
                       "font-size='13' font-weight='600'>Route complete</text>")
            .arg(active_y + 76);
    }

    svg += QStringLiteral(
        "<rect x='30' y='840' width='560' height='34' rx='17' fill='#f3f4f6'/>"
        "<text x='310' y='858' text-anchor='middle' dominant-baseline='middle' fill='#667085' "
        "font-family='Segoe UI' font-size='12'>Faded pills show alternatives you have already ruled out.</text>");

    svg += "</svg>";
    this->load(svg.toUtf8());
}

void PracticeFlowchartDiagram::render_overview(const std::vector<std::string>& path) {
    using Position = std::pair<int, int>;
    const std::unordered_map<std::string, Position> positions = {
        {"start", {700, 95}},
        {"linear", {205, 190}}, {"higher_order", {760, 190}},
        {"Cinfv", {80, 285}}, {"Dinfh", {310, 285}},
        {"C", {535, 285}}, {"high_sym", {1000, 285}},
        {"no_C", {160, 380}}, {"yes_C", {520, 380}},
        {"Td", {900, 380}}, {"C5", {1130, 380}},
        {"Cs", {55, 475}}, {"nothing", {220, 475}}, {"nC2", {520, 475}},
        {"Oh", {1050, 475}}, {"Ih", {1220, 475}},
        {"C1", {150, 570}}, {"Ci", {265, 570}},
        {"C_horizontal", {430, 570}}, {"D_horizontal", {800, 570}},
        {"Cnh", {310, 665}}, {"C_vertical", {475, 665}},
        {"D_dihedral", {780, 665}}, {"Dnh", {1000, 665}},
        {"Cnv", {370, 760}}, {"imp_rot", {535, 760}},
        {"Dn", {720, 760}}, {"Dnd", {840, 760}},
        {"Cn", {480, 855}}, {"S2n", {600, 855}}
    };

    std::unordered_map<std::string, Node> by_key;
    std::unordered_map<std::string, std::vector<std::string>> child_keys;
    for (const Node& node : nodes) by_key.emplace(node.key, node);
    for (const Edge& edge : edges) child_keys[edge.from].push_back(edge.to);

    std::unordered_set<std::string> visited(path.begin(), path.end());
    std::unordered_set<std::string> inaccessible;
    std::function<void(const std::string&)> block_subtree = [&](const std::string& key) {
        if (!inaccessible.insert(key).second) return;
        for (const std::string& child : child_keys[key]) block_subtree(child);
    };
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
        for (const std::string& child : child_keys[path[i]]) {
            if (child != path[i + 1]) block_subtree(child);
        }
    }

    auto is_selected_edge = [&path](const Edge& edge) {
        for (std::size_t i = 0; i + 1 < path.size(); ++i) {
            if (path[i] == edge.from && path[i + 1] == edge.to) return true;
        }
        return false;
    };
    auto node_width = [](const Node& node) {
        int estimate = 28 + static_cast<int>(QString::fromUtf8(node.label).size()) * 7;
        return std::max(node.result ? 62 : 112, std::min(node.result ? 90 : 210, estimate));
    };

    QString svg = QStringLiteral(
        "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 1400 930'>"
        "<defs><linearGradient id='done' x1='0' y1='0' x2='1' y2='1'>"
        "<stop offset='0' stop-color='#dcfce7'/><stop offset='1' stop-color='#86efac'/></linearGradient>"
        "</defs><rect x='2' y='2' width='1396' height='926' rx='22' fill='#fffdf7' "
        "stroke='#d9d5c9' stroke-width='3'/>"
        "<text x='28' y='38' fill='#20242b' font-family='Segoe UI' font-size='24' font-weight='700'>"
        "Complete point-group decision tree</text>"
        "<text x='28' y='62' fill='#667085' font-family='Segoe UI' font-size='13'>"
        "Blue marks the current decision; green marks the completed route and results.</text>");

    // Connections are painted first so nodes remain crisp and legible.
    for (const Edge& edge : edges) {
        const Position& from = positions.at(edge.from);
        const Position& to = positions.at(edge.to);
        bool selected = is_selected_edge(edge);
        bool blocked = inaccessible.count(edge.to) > 0;
        const QString colour = selected ? "#15803d" : (blocked ? "#d7d9dc" : "#9ca3af");
        const QString dash = selected ? "" : " stroke-dasharray='6 6'";
        int middle_y = (from.second + to.second) / 2;
        svg += QString("<path d='M %1 %2 C %1 %3, %4 %3, %4 %5' fill='none' stroke='%6' "
                       "stroke-width='%7'%8/>")
            .arg(from.first).arg(from.second + 23).arg(middle_y)
            .arg(to.first).arg(to.second - 23).arg(colour).arg(selected ? 4 : 2).arg(dash);
        svg += QString("<text x='%1' y='%2' text-anchor='middle' fill='%3' font-family='Segoe UI' "
                       "font-size='11' font-weight='700'>%4</text>")
            .arg((from.first + to.first) / 2).arg(middle_y - 4).arg(colour).arg(edge.answer);
    }

    const std::string active = path.empty() ? "start" : path.back();
    for (const Node& node : nodes) {
        const Position& position = positions.at(node.key);
        bool is_active = active == node.key;
        bool is_visited = visited.count(node.key) > 0 && !is_active;
        bool blocked = inaccessible.count(node.key) > 0;
        int width = node_width(node);
        QString fill = is_active ? (node.result ? "#ecfdf5" : "#ffffff") :
            (is_visited ? "url(#done)" : (node.result ? "#dcebd8" : "#f3e3b5"));
        QString stroke = is_active ? (node.result ? "#15803d" : "#0284c7") :
            (is_visited ? "#15803d" : "#81796d");
        QString opacity = blocked ? "0.25" : "1";
        int radius = node.result ? 22 : 12;
        svg += QString("<rect x='%1' y='%2' width='%3' height='46' rx='%4' fill='%5' stroke='%6' "
                       "stroke-width='%7' opacity='%8'/>")
            .arg(position.first - width / 2).arg(position.second - 23).arg(width).arg(radius)
            .arg(fill).arg(stroke).arg(is_active ? 3 : 2).arg(opacity);
        svg += QString("<text x='%1' y='%2' text-anchor='middle' dominant-baseline='middle' fill='#25282d' "
                       "font-family='Segoe UI' font-size='%3' font-weight='%4' opacity='%5'>%6</text>")
            .arg(position.first).arg(position.second + 1).arg(node.result ? 14 : 12)
            .arg(is_active ? 700 : 600).arg(opacity).arg(escaped(node.label));
    }

    svg += "</svg>";
    this->load(svg.toUtf8());
}
