/*
Projektarbeit 1 – Dijkstra‘s Algorithmus
Gruppe:
author: Christian Stolz (102597)
Wir stimmen der Veröffentlichung unseres Source Code in anonymisierter Form zu.
Copyright (C) [2025] [Christian Stolz (102597)]
SPDX-License-Identifier: MIT
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <limits>
#include <functional>
//#include <algorithm>
#include <SDL3/SDL.h>
#include <random>

// Grundgerüst

struct node {
    long long id;
    double lat;
    double lon;
};

struct way {
    long long id;
    std::string name;
    std::string type;          
    std::vector<long long> node_ids;
};

struct Edge {
    long long to;          // ID Nachbar
    double weight;         // Abstand 
};

// Graph als Liste
using Graph = std::unordered_map<long long, std::vector<Edge>> ;

long long randomstartNode(const Graph& g) {
    if (g.empty()) return -1;

    static std::mt19937_64 rng(std::random_device{}());

    std::uniform_int_distribution<size_t> dist(0, g.size() - 1);

    size_t index = dist(rng);

    auto it = g.begin();
    std::advance(it, index);

    return it->first;
}

std::unordered_map<long long, node> buildNodeMap(const std::vector<node>& nodes) {
    std::unordered_map<long long, node> map;
    map.reserve(nodes.size());
    for (const auto& n : nodes) {
        map[n.id] = n;
    }
    return map;
}

struct Bounds {
    double minLat, maxLat;
    double minLon, maxLon;
};

Bounds computeBounds(const std::vector<node>& nodes) {
    Bounds b;
    b.minLat = b.minLon = 1e30;
    b.maxLat = b.maxLon = -1e30;

    for (const auto& n : nodes) {
        b.minLat = std::min(b.minLat, n.lat);
        b.maxLat = std::max(b.maxLat, n.lat);
        b.minLon = std::min(b.minLon, n.lon);
        b.maxLon = std::max(b.maxLon, n.lon);
    }
    return b;
}

inline void project(const node& n, const Bounds& b, int width, int height, float& x, float& y) {
    double u = (n.lon - b.minLon) / (b.maxLon - b.minLon + 1e-12);
    double v = (n.lat - b.minLat) / (b.maxLat - b.minLat + 1e-12);

    // bisschen Rand (10 px)
    x = static_cast<float>(10 + u * (width - 20));
    // y-Achse ist in Grafiken nach unten positiv -> invertieren
    y = static_cast<float>(height - (10 + v * (height - 20)));
}

constexpr double DEG_TO_RAD = 3.14159265358979323846 / 180.0;

constexpr double EARTH_RADIUS_M = 6371000.0; // mittlerer Erdradius in m

double haversine(const node& a, const node& b) {
    double lat1 = a.lat * DEG_TO_RAD;
    double lon1 = a.lon * DEG_TO_RAD;
    double lat2 = b.lat * DEG_TO_RAD;
    double lon2 = b.lon * DEG_TO_RAD;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double sin_dlat = std::sin(dlat / 2.0);
    double sin_dlon = std::sin(dlon / 2.0);

    double h = sin_dlat * sin_dlat +
        std::cos(lat1) * std::cos(lat2) * sin_dlon * sin_dlon;

    double c = 2.0 * std::atan2(std::sqrt(h), std::sqrt(1.0 - h));
    return EARTH_RADIUS_M * c;
}

bool initSDL(SDL_Window** window, SDL_Renderer** renderer, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {  // SDL3 gibt bool zurück :contentReference[oaicite:0]{index=0}
        SDL_Log("SDL_Init fehlgeschlagen: %s", SDL_GetError());
        return false;
    }

    *window = SDL_CreateWindow("OSM Map + Route", width, height, 0);
    if (!*window) {
        SDL_Log("SDL_CreateWindow fehlgeschlagen: %s", SDL_GetError());
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, nullptr);
    if (!*renderer) {
        SDL_Log("SDL_CreateRenderer fehlgeschlagen: %s", SDL_GetError());
        return false;
    }

    return true;
}

void drawWays(SDL_Renderer* renderer, const std::vector<way>& ways, const std::unordered_map<long long, node>& nodeMap, const Bounds& bounds, int width, int height) {
    // Straßenfarbe: hellgrau
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);

    for (const auto& w : ways) {
        if (w.node_ids.size() < 2) continue;

        for (size_t i = 0; i + 1 < w.node_ids.size(); ++i) {
            auto itA = nodeMap.find(w.node_ids[i]);
            auto itB = nodeMap.find(w.node_ids[i + 1]);
            if (itA == nodeMap.end() || itB == nodeMap.end()) continue;

            float x1, y1, x2, y2;
            project(itA->second, bounds, width, height, x1, y1);
            project(itB->second, bounds, width, height, x2, y2);

            SDL_RenderLine(renderer, x1, y1, x2, y2);
        }
    }
}

void drawRoute(SDL_Renderer* renderer, const std::vector<long long>& routeNodeIds, const std::unordered_map<long long, node>& nodeMap, const Bounds& bounds, int width, int height) {
    if (routeNodeIds.size() < 2) return;

    // Route in rot
    SDL_SetRenderDrawColor(renderer, 220, 30, 30, SDL_ALPHA_OPAQUE);

    for (size_t i = 0; i + 1 < routeNodeIds.size(); ++i) {
        auto itA = nodeMap.find(routeNodeIds[i]);
        auto itB = nodeMap.find(routeNodeIds[i + 1]);
        if (itA == nodeMap.end() || itB == nodeMap.end()) continue;

        float x1, y1, x2, y2;
        project(itA->second, bounds, width, height, x1, y1);
        project(itB->second, bounds, width, height, x2, y2);

        SDL_RenderLine(renderer, x1, y1, x2, y2);
    }
}

Graph buildGraph(const std::vector<node>& nodes, const std::vector<way>& ways){
    Graph graph;
    auto nodeMap = buildNodeMap(nodes);
    for (const auto& w : ways) {
        if (w.node_ids.size() < 2) continue; // braucht mind. 2 Nodes

        for (size_t i = 0; i + 1 < w.node_ids.size(); ++i) {
            long long idA = w.node_ids[i];
            long long idB = w.node_ids[i + 1];

            auto itA = nodeMap.find(idA);
            auto itB = nodeMap.find(idB);
            if (itA == nodeMap.end() || itB == nodeMap.end()) {
                // Node nicht gefunden -> überspringen
                continue;
            }

            const node& nA = itA->second;
            const node& nB = itB->second;

            double dist = haversine(nA, nB);

            // ungerichteter Graph: beide Richtungen einfügen
            graph[idA].push_back(Edge{ idB, dist });
            graph[idB].push_back(Edge{ idA, dist });
        }
    }
    return graph;
}

std::string getAttr(const std::string& line, const std::string& key) {
    size_t pos = line.find(key);
    if (pos == std::string::npos) return "";
    pos += key.size();
    if (line[pos] == '"') pos++;
    size_t end = line.find("\"", pos);
    if (end == std::string::npos) return "";
    return line.substr(pos, end - pos);
}

std::vector<long long> dijkstra(const Graph& g, long long startId, long long zielId) {
    const double INF = std::numeric_limits<double>::infinity();

    // Distanz und Vorgänger
    std::unordered_map<long long, double> dist;
    std::unordered_map<long long, long long>    prev;

    // Initialisieren
    for (const auto& [nodeId, edges] : g) {
        dist[nodeId] = INF;
    }

    // Falls Start/Ziel gar nicht im Graphen sind
    if (dist.find(startId) == dist.end() || dist.find(zielId) == dist.end()) {
        return {}; // leerer Pfad
    }

    dist[startId] = 0.0;

    using PQItem = std::pair<double, long long>; // (Distanz, NodeID)
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;
    pq.push({ 0.0, startId });

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // Falls wir schon eine bessere Distanz kennen → ignorieren
        if (d > dist[u]) continue;

        // Früh abbrechen, wenn Ziel erreicht
        if (u == zielId) break;

        auto it = g.find(u);
        if (it == g.end()) continue;

        for (const auto& e : it->second) {
            long long v = e.to;
            double nd = d + e.weight;

            if (nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
                pq.push({ nd, v });
            }
        }
    }

    // Kein Pfad gefunden
    if (dist[zielId] == INF) {
        return {};
    }

    // Pfad rekonstrieren: von zielId zurück zu startId
    std::vector<long long> path;
    for (long long at = zielId; ; ) {
        path.push_back(at);
        if (at == startId) break;
        auto it = prev.find(at);
        if (it == prev.end()) { // sollte nicht passieren, aber falls doch
            path.clear();
            return path;
        }
        at = it->second;
    }

    // umdrehen: von Start → Ziel
    std::reverse(path.begin(), path.end());
    return path;
}

int main() {
    // Datei einlesen
    std::ifstream file("maposm.osm");
    if (!file.is_open()) {
        std::cout << "Datei konnte nicht geöffnet werden!\n";
        return 1;
    }

    std::vector<node> nodes;
    std::vector<way>  ways;

    std::string line;

    // --------- Variablen für way-Parsing ----------
    bool insideWay = false;
    bool hasHighway = false;
    way currentWay;

    while (std::getline(file, line)) {

        // ---------- NODE einlesen (alle!) ----------
        if (line.find("<node") != std::string::npos && line.find("lat=") != std::string::npos) {
            std::string fullNode = line;

            // Falls der Node über mehrere Zeilen geht, bis "/>" oder "</node>" zusammenfassen
            while (fullNode.find("/>") == std::string::npos &&
                fullNode.find("</node>") == std::string::npos) {

                std::string next;
                if (!std::getline(file, next)) break;
                fullNode += next;
            }

            std::string idStr = getAttr(fullNode, "id=\"");
            std::string latStr = getAttr(fullNode, "lat=\"");
            std::string lonStr = getAttr(fullNode, "lon=\"");

            if (!idStr.empty() && !latStr.empty() && !lonStr.empty()) {
                node n;
                n.id = std::stoll(idStr);
                n.lat = std::stod(latStr);
                n.lon = std::stod(lonStr);
                nodes.push_back(n);
            }
        }

        // ---------- WAY START ----------
        if (line.find("<way") != std::string::npos) {
            insideWay = true;
            hasHighway = false;
            currentWay = way();

            std::string idStr = getAttr(line, "id=\"");
            if (!idStr.empty()) currentWay.id = std::stoll(idStr);
        }

        // ---------- ND (Node-Referenzen in Ways) ----------
        if (insideWay && line.find("<nd") != std::string::npos) {
            std::string refStr = getAttr(line, "ref=\"");
            if (!refStr.empty()) {
                currentWay.node_ids.push_back(std::stoll(refStr));
            }
        }

        // ---------- TAGS im WAY ----------
        if (insideWay && line.find("<tag") != std::string::npos) {
            std::string key = getAttr(line, "k=\"");
            std::string val = getAttr(line, "v=\"");

            if (key == "highway") {
                currentWay.type = val;
                hasHighway = true;
            }
            if (key == "name") {
                currentWay.name = val;
            }
        }

        // ---------- WAY ENDE ----------
        if (insideWay && line.find("</way>") != std::string::npos) {
            insideWay = false;
            if (hasHighway) {
                ways.push_back(currentWay); // nur highways speichern
            }
        }
    }

    file.close();

    std::cout << "Nodes insgesamt: " << nodes.size() << std::endl;
    std::cout << "Highway-Ways: " << ways.size() << std::endl;

    // Graph bauen
    Graph g = buildGraph(nodes, ways);

    std::cout << "Graph-Knoten (mit mindestens einer Kante): " << g.size() << "\n";

    auto nodeMap = buildNodeMap(nodes);
   // Dijekstra Algorithmus 
    long long startId = randomstartNode(g);   // z.B. irgendein Node aus deiner Datei
    long long zielId = 310217202; // HKA auf Highway



    auto path = dijkstra(g, startId, zielId);
    if (path.empty()) {
        std::cout << "Kein Pfad gefunden.\n";
    }
    else {
        std::cout << "Pfad (" << path.size() << " Knoten):\n";
        std::cout << "Start-Node auf Highway: " << startId << "  (lat=" << nodeMap[startId].lat << ", lon=" << nodeMap[startId].lon << ")" << std::endl;
		int i = 1;
        for (int id : path) {
            std::cout << "Knoten" << i << ": " << id << std::endl;
            i++;
        }
    }
    if (nodes.empty() || ways.empty()) {
        std::cout << "Keine Nodes oder Ways geladen!\n";
        return 0;
    }

    
    Bounds bounds = computeBounds(nodes);
    std::vector<long long> routeNodeIds = dijkstra(g, startId, zielId);
    //std::vector<int> routeNodeIds; // hier dummy, damit es kompiliert
    std::cout << "Ziel-Node HKA auf Highway:  " << zielId << "  (lat=" << nodeMap[zielId].lat << ", lon=" << nodeMap[zielId].lon << ")" << std::endl;
    // 3. SDL initialisieren
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    const int WIDTH = 1080;
    const int HEIGHT = 720;

    if (!initSDL(&window, &renderer, WIDTH, HEIGHT)) {
        return 1;
    }

    bool running = true;
    SDL_Event e;

    while (running) {
        // Events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = false;
            }
            // optional: ESC zum Beenden
            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        // Hintergrund schwarz
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Karte
        drawWays(renderer, ways, nodeMap, bounds, WIDTH, HEIGHT);

        // Route (falls vorhanden)
        if (!routeNodeIds.empty()) {
            drawRoute(renderer, routeNodeIds, nodeMap, bounds, WIDTH, HEIGHT);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}
