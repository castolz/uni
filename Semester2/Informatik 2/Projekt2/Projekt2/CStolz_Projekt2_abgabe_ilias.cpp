/*
Projektarbeit 2 – Quantenschach
Author: Chhistian Stolz
Ich stimme der Veröffentlichung unseres Source Code in anonymisierter Form zu.

Copyright (C) [2026] [Christian Stolz]
SPDX-License-Identifier: MIT

*/

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <SDL3/SDL.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <cstring>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

//  Kleine ASCII-Helfer 

static char ascii_klein(char ch)
{
    if (ch >= 'A' && ch <= 'Z') return (char)(ch - 'A' + 'a');
    return ch;
}
static char ascii_gross(char ch)
{
    if (ch >= 'a' && ch <= 'z') return (char)(ch - 'a' + 'A');
    return ch;
}
static int ganz_abs(int v) { return (v < 0) ? -v : v; }

//  PRNG (synchronisiert) 
// xorshift32, deterministisch: beide Peers starten mit gleichem Seed und
// rufen zufall_u32() in gleicher Reihenfolge (bei Messung/Kollaps) auf.

static unsigned int g_seed = 0;
static unsigned int g_rng = 0;

static unsigned int zufall_u32()
{
    if (g_rng == 0) g_rng = 0xA341316Cu;
    unsigned int x = g_rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    g_rng = x;
    return x;
}
static int zufall_auswahl(int anzahl)
{
    if (anzahl <= 1) return 0;
    return (int)(zufall_u32() % (unsigned int)anzahl);
}

// Schach / Quantenmodell

enum FigurenTyp { FT_LEER, FT_BAUER, FT_SPRINGER, FT_LAEUFER, FT_TURM, FT_DAME, FT_KOENIG };
enum Farbe { FARBE_KEINE, FARBE_WEISS, FARBE_SCHWARZ };

struct Feld { int x; int y; };

struct Figur {
    int id;
    FigurenTyp typ;
    Farbe farbe;
    bool lebt;

    int positionsAnzahl;   // 0,1,2
    Feld positionen[2];

    // verschraenkungId:
    // -1 => klassisch / nicht verschränkt
    // >=0 => verschränkt (Komponente). Messung kollabiert ALLE Figuren dieser Komponente mit gleichem Branch 0/1.
    int verschraenkungId;
};

static bool im_brett(int x, int y) { return x >= 0 && x < 8 && y >= 0 && y < 8; }

static char figur_zeichen(FigurenTyp t)
{

    // Bauer=B, Springer=S, Läufer=L, Turm=T, Dame=D, König=K
    if (t == FT_BAUER)    return 'B';
    if (t == FT_SPRINGER) return 'S';
    if (t == FT_LAEUFER)  return 'L';
    if (t == FT_TURM)     return 'T';
    if (t == FT_DAME)     return 'D';
    if (t == FT_KOENIG)   return 'K';
    return ' ';
}

static string feld_zu_string(int x, int y)
{
    string s = "a1";
    s[0] = (char)('a' + x);
    s[1] = (char)('8' - y);
    return s;
}
static bool string_zu_feld(const string& s, int& x, int& y)
{
    if (s.size() != 2) return false;
    char f = ascii_klein(s[0]);
    char r = s[1];
    if (f < 'a' || f > 'h') return false;
    if (r < '1' || r > '8') return false;
    x = (int)(f - 'a');
    y = (int)('8' - r);
    return true;
}

struct QuantenBrett {
    static const int MAX_FIGUREN = 32;
    static const int MAX_BELEG = 4;

    Figur figuren[MAX_FIGUREN];
    int figurenAnzahl = 0;
    int naechsteId = 1;
    int naechsteVerschraenkungId = 0;
    Farbe amZug = FARBE_WEISS;

    // Belegungstabellen
    int belegAnzahl[8][8];
    int belegIds[8][8][MAX_BELEG];

    void belegung_leeren()
    {
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {
            belegAnzahl[y][x] = 0;
            for (int k = 0; k < MAX_BELEG; k++) belegIds[y][x][k] = 0;
        }
    }

    void belegung_aktualisieren()
    {
        belegung_leeren();
        for (int i = 0; i < figurenAnzahl; i++) {
            Figur* f = &figuren[i];
            if (!f->lebt) continue;
            for (int j = 0; j < f->positionsAnzahl; j++) {
                int x = f->positionen[j].x;
                int y = f->positionen[j].y;
                if (!im_brett(x, y)) continue;
                int cnt = belegAnzahl[y][x];
                if (cnt < MAX_BELEG) {
                    belegIds[y][x][cnt] = f->id;
                    belegAnzahl[y][x] = cnt + 1;
                }
            }
        }
    }

    Figur* figur_nach_id(int id)
    {
        for (int i = 0; i < figurenAnzahl; i++) if (figuren[i].id == id) return &figuren[i];
        return NULL;
    }

    bool feld_belegt(int x, int y) const { return belegAnzahl[y][x] > 0; }

    bool feld_belegt_von_farbe(int x, int y, Farbe farbe) const
    {
        int cnt = belegAnzahl[y][x];
        for (int i = 0; i < cnt; i++) {
            int id = belegIds[y][x][i];
            for (int j = 0; j < figurenAnzahl; j++) {
                if (figuren[j].id == id && figuren[j].lebt && figuren[j].farbe == farbe) return true;
            }
        }
        return false;
    }

    void figur_hinzufuegen(FigurenTyp t, Farbe f, int x, int y)
    {
        if (figurenAnzahl >= MAX_FIGUREN) return;
        Figur* p = &figuren[figurenAnzahl++];
        p->id = naechsteId++;
        p->typ = t;
        p->farbe = f;
        p->lebt = true;
        p->positionsAnzahl = 1;
        p->positionen[0].x = x; p->positionen[0].y = y;
        p->verschraenkungId = -1;
    }

    void initialisiere()
    {
        figurenAnzahl = 0;
        naechsteId = 1;
        naechsteVerschraenkungId = 0;

        // Schwarz
        figur_hinzufuegen(FT_TURM, FARBE_SCHWARZ, 0, 0);
        figur_hinzufuegen(FT_SPRINGER, FARBE_SCHWARZ, 1, 0);
        figur_hinzufuegen(FT_LAEUFER, FARBE_SCHWARZ, 2, 0);
        figur_hinzufuegen(FT_DAME, FARBE_SCHWARZ, 3, 0);
        figur_hinzufuegen(FT_KOENIG, FARBE_SCHWARZ, 4, 0);
        figur_hinzufuegen(FT_LAEUFER, FARBE_SCHWARZ, 5, 0);
        figur_hinzufuegen(FT_SPRINGER, FARBE_SCHWARZ, 6, 0);
        figur_hinzufuegen(FT_TURM, FARBE_SCHWARZ, 7, 0);
        for (int x = 0; x < 8; x++) figur_hinzufuegen(FT_BAUER, FARBE_SCHWARZ, x, 1);

        // Weiß
        figur_hinzufuegen(FT_TURM, FARBE_WEISS, 0, 7);
        figur_hinzufuegen(FT_SPRINGER, FARBE_WEISS, 1, 7);
        figur_hinzufuegen(FT_LAEUFER, FARBE_WEISS, 2, 7);
        figur_hinzufuegen(FT_DAME, FARBE_WEISS, 3, 7);
        figur_hinzufuegen(FT_KOENIG, FARBE_WEISS, 4, 7);
        figur_hinzufuegen(FT_LAEUFER, FARBE_WEISS, 5, 7);
        figur_hinzufuegen(FT_SPRINGER, FARBE_WEISS, 6, 7);
        figur_hinzufuegen(FT_TURM, FARBE_WEISS, 7, 7);
        for (int x = 0; x < 8; x++) figur_hinzufuegen(FT_BAUER, FARBE_WEISS, x, 6);

        amZug = FARBE_WEISS;
        belegung_aktualisieren();
    }

    void kollabiere_auf(Figur* f, int x, int y)
    {
        f->positionsAnzahl = 1;
        f->positionen[0].x = x;
        f->positionen[0].y = y;
    }

    void messe_verschraenkung(int verschId)
    {
        if (verschId < 0) return;
        int branch = zufall_auswahl(2); // 0 oder 1

        for (int i = 0; i < figurenAnzahl; i++) {
            Figur* f = &figuren[i];
            if (!f->lebt) continue;
            if (f->verschraenkungId != verschId) continue;

            if (f->positionsAnzahl == 2) {
                kollabiere_auf(f, f->positionen[branch].x, f->positionen[branch].y);
            }
            else {
                // ist eh klassisch positioniert, bleibt
            }
            f->verschraenkungId = -1;
            f->positionsAnzahl = 1;
        }
        belegung_aktualisieren();
    }

    // stellt sicher, dass der Beweger wirklich von (vonX,vonY) "existiert".
    bool stelle_ausgangsposition_sicher(Figur* f, int vonX, int vonY)
    {
        if (!f || !f->lebt) return false;

        // falls verschränkt und gesplittet => Messung der gesamten Komponente vor "Anfassen"
        if (f->positionsAnzahl == 2 && f->verschraenkungId >= 0) {
            messe_verschraenkung(f->verschraenkungId);
        }

        int gefunden = -1;
        for (int i = 0; i < f->positionsAnzahl; i++) {
            if (f->positionen[i].x == vonX && f->positionen[i].y == vonY) { gefunden = i; break; }
        }
        if (gefunden < 0) return false;

        // nicht verschränkt, aber gesplittet => lokal kollabieren
        if (f->positionsAnzahl == 2) {
            kollabiere_auf(f, vonX, vonY);
            f->verschraenkungId = -1;
            belegung_aktualisieren();
        }

        return true;
    }

    // Strahlstatus:
    // 0 = frei
    // 1 = sicher blockiert (klassische Figur)
    // 2 = unsicherer Blocker, aber NUR EIN verschId auf dem Weg (50%-Block-Fall)
    // 3 = unsicher / mehrere qids / nicht-verschraenkte Splits -> unentscheidbar => Zug ablehnen
    int strahl_status(int vonX, int vonY, int nachX, int nachY, int sx, int sy, int* outVerschId) const
    {
        *outVerschId = -1;
        int x = vonX + sx;
        int y = vonY + sy;

        int gesehenVerschId = -1;

        while (x != nachX || y != nachY) {
            int cnt = belegAnzahl[y][x];
            if (cnt > 0) {
                for (int i = 0; i < cnt; i++) {
                    int id = belegIds[y][x][i];

                    const Figur* f = NULL;
                    for (int j = 0; j < figurenAnzahl; j++) {
                        if (figuren[j].id == id) { f = &figuren[j]; break; }
                    }
                    if (!f || !f->lebt) continue;

                    if (f->positionsAnzahl == 1) return 1; // sicher blockiert

                    // gesplittet:
                    int q = f->verschraenkungId;
                    if (q < 0) return 3; // Split ohne Verschraenkung => im Modell unentscheidbar

                    if (gesehenVerschId < 0) gesehenVerschId = q;
                    else if (gesehenVerschId != q) return 3; // mehrere qids
                }
            }
            x += sx; y += sy;
        }

        if (gesehenVerschId >= 0) { *outVerschId = gesehenVerschId; return 2; }
        return 0;
    }

    // Versuch, Mehrdeutigkeit im Strahl  zu reduzieren:
    // Misst die kleinste qid auf dem Strahl, wiederholt, bis klar oder Abbruch.
    bool strahl_aufloesen_durch_messungen(int vonX, int vonY, int nachX, int nachY, int sx, int sy)
    {
        for (int iter = 0; iter < 8; iter++) {
            int x = vonX + sx;
            int y = vonY + sy;

            int minQ = -1;
            int maxQ = -1;
            bool hatSicherenBlock = false;

            while (x != nachX || y != nachY) {
                int cnt = belegAnzahl[y][x];
                for (int i = 0; i < cnt; i++) {
                    int id = belegIds[y][x][i];
                    Figur* f = figur_nach_id(id);
                    if (!f || !f->lebt) continue;

                    if (f->positionsAnzahl == 1) { hatSicherenBlock = true; break; }

                    if (f->verschraenkungId >= 0) {
                        int q = f->verschraenkungId;
                        if (minQ < 0 || q < minQ) minQ = q;
                        if (maxQ < 0 || q > maxQ) maxQ = q;
                    }
                }
                if (hatSicherenBlock) break;
                x += sx; y += sy;
            }

            if (hatSicherenBlock) return false;
            if (minQ < 0) return true;
            if (minQ == maxQ) return true;

            messe_verschraenkung(minQ);
        }
        return false;
    }

    // eindeutiges Ziel im Zielfeld: 0=kein Gegner, 1=genau ein Gegner, 2=mehrdeutig
    int finde_eindeutiges_ziel(Farbe angreiferFarbe, int nachX, int nachY, Figur** outZiel)
    {
        *outZiel = NULL;
        int cnt = belegAnzahl[nachY][nachX];
        int gefundenId = 0;
        int gefundenAnzahl = 0;

        for (int i = 0; i < cnt; i++) {
            int id = belegIds[nachY][nachX][i];
            Figur* f = figur_nach_id(id);
            if (!f || !f->lebt) continue;
            if (f->farbe == angreiferFarbe) continue;

            gefundenId = id;
            gefundenAnzahl++;
            if (gefundenAnzahl > 1) return 2;
        }

        if (gefundenAnzahl == 0) return 0;
        *outZiel = figur_nach_id(gefundenId);
        return 1;
    }

    // Bauer diagonal: zuerst Messung am Zielfeld (falls gesplittet), dann später pseudo-legal prüfen.
    void bauer_diagonal_vormessung(Figur* bauer, int vonX, int vonY, int nachX, int nachY)
    {
        if (!bauer || !bauer->lebt) return;
        if (bauer->typ != FT_BAUER) return;

        int dir = (bauer->farbe == FARBE_WEISS) ? -1 : +1;
        int dx = nachX - vonX;
        int dy = nachY - vonY;

        if (ganz_abs(dx) == 1 && dy == dir) {
            Figur* ziel = NULL;
            int st = finde_eindeutiges_ziel(bauer->farbe, nachX, nachY, &ziel);
            if (st == 1 && ziel && ziel->lebt && ziel->positionsAnzahl == 2) {
                if (ziel->verschraenkungId >= 0) {
                    messe_verschraenkung(ziel->verschraenkungId);
                }
                else {
                    int w = zufall_auswahl(2);
                    kollabiere_auf(ziel, ziel->positionen[w].x, ziel->positionen[w].y);
                    ziel->verschraenkungId = -1;
                    belegung_aktualisieren();
                }
            }
        }
    }

    bool zug_pseudolegal(const Figur* f, int vonX, int vonY, int nachX, int nachY) const
    {
        if (!f || !f->lebt) return false;
        if (!im_brett(vonX, vonY) || !im_brett(nachX, nachY)) return false;
        if (vonX == nachX && vonY == nachY) return false;
        if (feld_belegt_von_farbe(nachX, nachY, f->farbe)) return false;

        int dx = nachX - vonX;
        int dy = nachY - vonY;

        if (f->typ == FT_BAUER) {
            int dir = (f->farbe == FARBE_WEISS) ? -1 : +1;
            int start = (f->farbe == FARBE_WEISS) ? 6 : 1;

            bool zielHatGegner = false;
            int cnt = belegAnzahl[nachY][nachX];
            for (int i = 0; i < cnt; i++) {
                int id = belegIds[nachY][nachX][i];
                for (int j = 0; j < figurenAnzahl; j++) {
                    if (figuren[j].id == id && figuren[j].lebt && figuren[j].farbe != f->farbe) { zielHatGegner = true; break; }
                }
            }

            if (dx == 0 && dy == dir && !feld_belegt(nachX, nachY)) return true;
            if (dx == 0 && dy == 2 * dir && vonY == start) {
                int mittelY = vonY + dir;
                if (!feld_belegt(nachX, nachY) && !feld_belegt(vonX, mittelY)) return true;
            }
            if ((dx == 1 || dx == -1) && dy == dir && zielHatGegner) return true;
            return false;
        }

        if (f->typ == FT_SPRINGER) {
            int adx = ganz_abs(dx), ady = ganz_abs(dy);
            return (adx == 1 && ady == 2) || (adx == 2 && ady == 1);
        }

        if (f->typ == FT_LAEUFER) {
            if (ganz_abs(dx) != ganz_abs(dy)) return false;
            int sx = (dx > 0) ? 1 : -1;
            int sy = (dy > 0) ? 1 : -1;
            int qid = -1;
            int st = strahl_status(vonX, vonY, nachX, nachY, sx, sy, &qid);
            return (st == 0) || (st == 2);
        }

        if (f->typ == FT_TURM) {
            if (!((dx == 0) ^ (dy == 0))) return false;
            int sx = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
            int sy = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
            int qid = -1;
            int st = strahl_status(vonX, vonY, nachX, nachY, sx, sy, &qid);
            return (st == 0) || (st == 2);
        }

        if (f->typ == FT_DAME) {
            if (ganz_abs(dx) == ganz_abs(dy)) {
                int sx = (dx > 0) ? 1 : -1;
                int sy = (dy > 0) ? 1 : -1;
                int qid = -1;
                int st = strahl_status(vonX, vonY, nachX, nachY, sx, sy, &qid);
                return (st == 0) || (st == 2);
            }
            if ((dx == 0) ^ (dy == 0)) {
                int sx = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
                int sy = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
                int qid = -1;
                int st = strahl_status(vonX, vonY, nachX, nachY, sx, sy, &qid);
                return (st == 0) || (st == 2);
            }
            return false;
        }

        if (f->typ == FT_KOENIG) {
            int adx = ganz_abs(dx), ady = ganz_abs(dy);
            return adx <= 1 && ady <= 1;
        }

        return false;
    }

    bool fuehre_klassischen_zug_aus(int figurenId, int vonX, int vonY, int nachX, int nachY)
    {
        Figur* beweg = figur_nach_id(figurenId);
        if (!beweg || !beweg->lebt) return false;
        if (beweg->farbe != amZug) return false;

        if (!stelle_ausgangsposition_sicher(beweg, vonX, vonY)) return false;

        // Bauer diagonal: erst Messung am Zielfeld (Fall 1/2), dann Legalisierung
        bauer_diagonal_vormessung(beweg, vonX, vonY, nachX, nachY);

        int dx = nachX - vonX;
        int dy = nachY - vonY;

        // Slider: Strahl ggf. durch Messungen vereinheitlichen
        bool istSlider = false;
        int sx = 0, sy = 0;

        if (beweg->typ == FT_TURM) {
            if (!((dx == 0) ^ (dy == 0))) return false;
            sx = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
            sy = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
            istSlider = true;
        }
        else if (beweg->typ == FT_LAEUFER) {
            if (ganz_abs(dx) != ganz_abs(dy)) return false;
            sx = (dx > 0) ? 1 : -1;
            sy = (dy > 0) ? 1 : -1;
            istSlider = true;
        }
        else if (beweg->typ == FT_DAME) {
            if (ganz_abs(dx) == ganz_abs(dy)) {
                sx = (dx > 0) ? 1 : -1;
                sy = (dy > 0) ? 1 : -1;
                istSlider = true;
            }
            else if ((dx == 0) ^ (dy == 0)) {
                sx = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
                sy = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
                istSlider = true;
            }
            else return false;
        }

        if (istSlider) {
            if (!strahl_aufloesen_durch_messungen(vonX, vonY, nachX, nachY, sx, sy)) return false;

            int qid = -1;
            int st = strahl_status(vonX, vonY, nachX, nachY, sx, sy, &qid);

            if (st == 1 || st == 3) return false;

            // "50%-Block"-Fall: genau ein verschId liegt im Strahl.
            // => beweg wird an diesen qid gekoppelt: Branch0: zieht, Branch1: bleibt
            if (st == 2 && qid >= 0) {
                if (feld_belegt_von_farbe(nachX, nachY, beweg->farbe)) return false;

                beweg->verschraenkungId = qid;
                beweg->positionsAnzahl = 2;
                beweg->positionen[0].x = nachX; beweg->positionen[0].y = nachY; // "zieht"
                beweg->positionen[1].x = vonX;  beweg->positionen[1].y = vonY;  // "bleibt"

                belegung_aktualisieren();
                amZug = (amZug == FARBE_WEISS) ? FARBE_SCHWARZ : FARBE_WEISS;
                return true;
            }
        }

        // Normale Legalität (nach Messungen)
        if (!zug_pseudolegal(beweg, vonX, vonY, nachX, nachY)) return false;

        // Schlagen: eindeutiges Ziel, bei gesplittetem Ziel => Messung (inkl. Verschraenkung) zuerst
        Figur* ziel = NULL;
        int stZ = finde_eindeutiges_ziel(beweg->farbe, nachX, nachY, &ziel);
        if (stZ == 2) return false;

        if (stZ == 1 && ziel && ziel->lebt) {
            if (ziel->positionsAnzahl == 2) {
                if (ziel->verschraenkungId >= 0) {
                    messe_verschraenkung(ziel->verschraenkungId);
                }
                else {
                    int w = zufall_auswahl(2);
                    kollabiere_auf(ziel, ziel->positionen[w].x, ziel->positionen[w].y);
                    ziel->verschraenkungId = -1;
                    belegung_aktualisieren();
                }

                // Nach Messung erneut prüfen, ob wirklich ein Ziel auf (nachX,nachY) steht
                Figur* ziel2 = NULL;
                int stZ2 = finde_eindeutiges_ziel(beweg->farbe, nachX, nachY, &ziel2);
                if (stZ2 == 2) return false;
                if (stZ2 == 1 && ziel2 && ziel2->lebt) {
                    ziel2->lebt = false;
                    ziel2->positionsAnzahl = 0;
                    ziel2->verschraenkungId = -1;
                    belegung_aktualisieren();
                }
            }
            else {
                ziel->lebt = false;
                ziel->positionsAnzahl = 0;
                ziel->verschraenkungId = -1;
                belegung_aktualisieren();
            }
        }

        // Bewegung ausführen
        beweg->positionsAnzahl = 1;
        beweg->verschraenkungId = -1;
        beweg->positionen[0].x = nachX;
        beweg->positionen[0].y = nachY;

        // Bauer-Umwandlung (vereinfachte Regel: zur Dame)
        if (beweg->typ == FT_BAUER) {
            if (beweg->farbe == FARBE_WEISS && nachY == 0) beweg->typ = FT_DAME;
            if (beweg->farbe == FARBE_SCHWARZ && nachY == 7) beweg->typ = FT_DAME;
        }

        belegung_aktualisieren();
        amZug = (amZug == FARBE_WEISS) ? FARBE_SCHWARZ : FARBE_WEISS;
        return true;
    }

    bool fuehre_quanten_zug_aus(int figurenId, int vonX, int vonY, int ziel1X, int ziel1Y, int ziel2X, int ziel2Y)
    {
        Figur* beweg = figur_nach_id(figurenId);
        if (!beweg || !beweg->lebt) return false;
        if (beweg->farbe != amZug) return false;

        if (!im_brett(ziel1X, ziel1Y) || !im_brett(ziel2X, ziel2Y)) return false;
        if (ziel1X == ziel2X && ziel1Y == ziel2Y) return false;

        if (!stelle_ausgangsposition_sicher(beweg, vonX, vonY)) return false;

        // Cantwell-Folien (wie von dir zitiert): Bauern splitten nicht
        if (beweg->typ == FT_BAUER) return false;

        if (!zug_pseudolegal(beweg, vonX, vonY, ziel1X, ziel1Y)) return false;
        if (!zug_pseudolegal(beweg, vonX, vonY, ziel2X, ziel2Y)) return false;

        // Stabilitätsregel: nicht auf besetzte Felder splitten
        if (feld_belegt(ziel1X, ziel1Y)) return false;
        if (feld_belegt(ziel2X, ziel2Y)) return false;

        beweg->positionsAnzahl = 2;
        beweg->positionen[0].x = ziel1X; beweg->positionen[0].y = ziel1Y;
        beweg->positionen[1].x = ziel2X; beweg->positionen[1].y = ziel2Y;

        beweg->verschraenkungId = naechsteVerschraenkungId++; // neue Komponente

        belegung_aktualisieren();
        amZug = (amZug == FARBE_WEISS) ? FARBE_SCHWARZ : FARBE_WEISS;
        return true;
    }
};

//  5x7 Schrift

struct Glyph { unsigned char zeilen[7]; };

static Glyph glyph_fuer(char c)
{
    c = ascii_gross(c);

    Glyph g = { {0,0,0,0,0,0,0} };

    // Ziffern
    if (c == '0') { Glyph t = { {14,17,19,21,25,17,14} }; return t; }
    if (c == '1') { Glyph t = { {4,12,4,4,4,4,14} }; return t; }
    if (c == '2') { Glyph t = { {14,17,1,2,4,8,31} }; return t; }
    if (c == '3') { Glyph t = { {30,1,1,14,1,1,30} }; return t; }
    if (c == '4') { Glyph t = { {2,6,10,18,31,2,2} }; return t; }
    if (c == '5') { Glyph t = { {31,16,16,30,1,1,30} }; return t; }
    if (c == '6') { Glyph t = { {14,16,16,30,17,17,14} }; return t; }
    if (c == '7') { Glyph t = { {31,1,2,4,8,8,8} }; return t; }
    if (c == '8') { Glyph t = { {14,17,17,14,17,17,14} }; return t; }
    if (c == '9') { Glyph t = { {14,17,17,15,1,1,14} }; return t; }

    // Buchstaben (für Overlay + Figuren: B,S,L,T,D,K usw.)
    if (c == 'A') { Glyph t = { {14,17,17,31,17,17,17} }; return t; }
    if (c == 'B') { Glyph t = { {30,17,17,30,17,17,30} }; return t; }
    if (c == 'C') { Glyph t = { {14,17,16,16,16,17,14} }; return t; }
    if (c == 'D') { Glyph t = { {30,17,17,17,17,17,30} }; return t; }
    if (c == 'E') { Glyph t = { {31,16,16,30,16,16,31} }; return t; }
    if (c == 'F') { Glyph t = { {31,16,16,30,16,16,16} }; return t; }
    if (c == 'G') { Glyph t = { {14,17,16,23,17,17,14} }; return t; }
    if (c == 'H') { Glyph t = { {17,17,17,31,17,17,17} }; return t; }
    if (c == 'I') { Glyph t = { {14,4,4,4,4,4,14} }; return t; }
    if (c == 'J') { Glyph t = { {7,2,2,2,2,18,12} }; return t; }
    if (c == 'K') { Glyph t = { {17,18,20,24,20,18,17} }; return t; }
    if (c == 'L') { Glyph t = { {16,16,16,16,16,16,31} }; return t; }
    if (c == 'M') { Glyph t = { {17,27,21,17,17,17,17} }; return t; }
    if (c == 'N') { Glyph t = { {17,25,21,19,17,17,17} }; return t; }
    if (c == 'O') { Glyph t = { {14,17,17,17,17,17,14} }; return t; }
    if (c == 'P') { Glyph t = { {30,17,17,30,16,16,16} }; return t; }
    if (c == 'Q') { Glyph t = { {14,17,17,17,21,18,13} }; return t; }
    if (c == 'R') { Glyph t = { {30,17,17,30,20,18,17} }; return t; }
    if (c == 'S') { Glyph t = { {15,16,16,14,1,1,30} }; return t; }
    if (c == 'T') { Glyph t = { {31,4,4,4,4,4,4} }; return t; }
    if (c == 'U') { Glyph t = { {17,17,17,17,17,17,14} }; return t; }
    if (c == 'V') { Glyph t = { {17,17,17,17,17,10,4} }; return t; }
    if (c == 'W') { Glyph t = { {17,17,17,21,21,21,10} }; return t; }
    if (c == 'X') { Glyph t = { {17,17,10,4,10,17,17} }; return t; }
    if (c == 'Y') { Glyph t = { {17,17,10,4,4,4,4} }; return t; }
    if (c == 'Z') { Glyph t = { {31,1,2,4,8,16,31} }; return t; }

    // Satzzeichen
    if (c == ':') { Glyph t = { {0,4,0,0,4,0,0} }; return t; }
    if (c == '-') { Glyph t = { {0,0,0,31,0,0,0} }; return t; }
    if (c == '/') { Glyph t = { {1,2,4,8,16,0,0} }; return t; }
    if (c == '+') { Glyph t = { {0,4,4,31,4,4,0} }; return t; }
    if (c == '.') { Glyph t = { {0,0,0,0,0,6,6} }; return t; }
    if (c == '(') { Glyph t = { {2,4,8,8,8,4,2} }; return t; }
    if (c == ')') { Glyph t = { {8,4,2,2,2,4,8} }; return t; }
    if (c == '%') { Glyph t = { {17,2,4,8,16,0,17} }; return t; }
    if (c == ' ') { return g; }

    return g;
}

static void farbe_setzen(SDL_Renderer* r, unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa)
{
    SDL_SetRenderDrawColor(r, rr, gg, bb, aa);
}

static void glyph_zeichnen(SDL_Renderer* r, char c, int x, int y, int scale)
{
    Glyph g = glyph_fuer(c);
    for (int zeile = 0; zeile < 7; zeile++) {
        for (int spalte = 0; spalte < 5; spalte++) {
            int an = (g.zeilen[zeile] >> (4 - spalte)) & 1;
            if (an) {
                SDL_FRect px;
                px.x = (float)(x + spalte * scale);
                px.y = (float)(y + zeile * scale);
                px.w = (float)scale;
                px.h = (float)scale;
                SDL_RenderFillRect(r, &px);
            }
        }
    }
}


static void text_zeichnen(SDL_Renderer* r, const string& s, int x, int y, int scale)
{
    int zeilenHoehe = 9 * scale;
    int cx = x;
    for (size_t i = 0; i < s.size(); i++) {
        char ch = s[i];
        if (ch == '\n') { cx = x; y += zeilenHoehe; continue; }
        glyph_zeichnen(r, ch, cx, y, scale);
        cx += 6 * scale;
    }
}

//Socket-Helfer

static SOCKET g_socket = INVALID_SOCKET;

static bool netz_sende_zeile(const string& zeile)
{
    if (g_socket == INVALID_SOCKET) return false;
    string out = zeile + "\n";
    int sent = send(g_socket, out.c_str(), (int)out.size(), 0);
    return sent != SOCKET_ERROR;
}

static bool netz_starte_server(unsigned short port)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) return false;

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) { closesocket(listenSock); return false; }
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) { closesocket(listenSock); return false; }

    cout << "Warte auf Verbindung auf Port " << port << "...\n";
    SOCKET clientSock = accept(listenSock, NULL, NULL);
    closesocket(listenSock);

    if (clientSock == INVALID_SOCKET) return false;

    g_socket = clientSock;
    return true;
}

static bool netz_starte_client(const string& ip, unsigned short port)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return false;

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    cout << "Verbinde zu " << ip << ":" << port << "...\n";
    if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) { closesocket(s); return false; }

    g_socket = s;
    return true;
}

static void netz_setze_nonblocking(SOCKET s)
{
    u_long mode = 1;
    ioctlsocket(s, FIONBIO, &mode);
}

static void netz_schliessen()
{
    if (g_socket != INVALID_SOCKET) {
        shutdown(g_socket, SD_BOTH);
        closesocket(g_socket);
        g_socket = INVALID_SOCKET;
    }
    WSACleanup();
}

// Tokenizer: bis zu 6 Tokens, ohne <sstream>
static int tokens_splitten(const string& zeile, string tok[6])
{
    int n = 0;
    size_t i = 0;

    while (i < zeile.size()) {
        while (i < zeile.size() && (zeile[i] == ' ' || zeile[i] == '\t')) i++;
        if (i >= zeile.size()) break;
        if (n >= 6) break;

        size_t j = i;
        while (j < zeile.size() && zeile[j] != ' ' && zeile[j] != '\t') j++;

        tok[n++] = zeile.substr(i, j - i);
        i = j;
    }
    return n;
}

static unsigned int string_zu_u32(const string& s)
{
    unsigned int v = 0;
    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];
        if (c < '0' || c > '9') break;
        v = v * 10u + (unsigned int)(c - '0');
    }
    return v;
}

static int string_zu_int(const string& s)
{
    int sign = 1;
    size_t i = 0;
    if (!s.empty() && s[0] == '-') { sign = -1; i = 1; }
    int v = 0;
    for (; i < s.size(); i++) {
        char c = s[i];
        if (c < '0' || c > '9') break;
        v = v * 10 + (c - '0');
    }
    return v * sign;
}

//  Main 

int main()
{
    cout << "Spielen als (Name egal): ";
    string name;
    cin >> name;

    cout << "Host (1) oder Client (0)? ";
    int hostInt = 0; cin >> hostInt;
    bool istHost = (hostInt != 0);

    const unsigned short PORT = 26000;

    bool netOK = false;
    if (istHost) {
        cout << "Fuehre 'ipconfig' aus und teile IPv4.\n";
        netOK = netz_starte_server(PORT);
    }
    else {
        cout << "IP des Hosts: ";
        string ip; cin >> ip;
        netOK = netz_starte_client(ip, PORT);
    }

    if (!netOK) {
        cout << "Netzwerk-Setup fehlgeschlagen.\n";
        return 1;
    }

    netz_setze_nonblocking(g_socket);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cout << "SDL_Init fehlgeschlagen: " << SDL_GetError() << "\n";
        netz_schliessen();
        return 1;
    }

    // Seed-Synchronisation
    bool seedVorhanden = istHost;
    if (istHost) {
        g_seed = (unsigned int)(SDL_GetTicks() ^ 0xA5A5A5A5u);
        g_rng = g_seed ^ 0x9E3779B9u;
        netz_sende_zeile(string("SEED ") + to_string(g_seed));
    }

    SDL_Window* fenster = SDL_CreateWindow("Quantenschach Option2 (H: Hilfe)", 920, 680, 0);
    if (!fenster) {
        cout << "SDL_CreateWindow fehlgeschlagen: " << SDL_GetError() << "\n";
        netz_schliessen();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(fenster, NULL);
    if (!renderer) {
        cout << "SDL_CreateRenderer fehlgeschlagen: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(fenster);
        netz_schliessen();
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    QuantenBrett brett;
    brett.initialisiere();

    Farbe meineFarbe = istHost ? FARBE_WEISS : FARBE_SCHWARZ;
    unsigned long long zugIndex = 0;

    // UI
    const int brettX = 20, brettY = 20;
    const int feldGroesse = 70;
    const int brettGroesse = feldGroesse * 8;

    bool quantenModus = false;
    bool hilfeAnzeigen = true;

    enum EingabeModus { EM_FIGUR_WAEHLEN, EM_ZIEL1, EM_ZIEL2 };
    EingabeModus modus = EM_FIGUR_WAEHLEN;

    int ausgewaehlteId = -1;
    int vonX = -1, vonY = -1;
    int ziel1X = -1, ziel1Y = -1;

    // RX Buffer als string
    string rxbuf;

    bool beenden = false;

    while (!beenden) {

        //  Netzwerk: Bytes empfangen 
        if (g_socket != INVALID_SOCKET) {
            char tmp[512];
            int r = recv(g_socket, tmp, 512, 0);
            if (r > 0) rxbuf.append(tmp, (size_t)r);
        }

        //  Netzwerk: Zeilen verarbeiten
        for (;;) {
            size_t nl = rxbuf.find('\n');
            if (nl == string::npos) break;

            string zeile = rxbuf.substr(0, nl);
            rxbuf.erase(0, nl + 1);

            if (!zeile.empty() && zeile.back() == '\r') zeile.pop_back();

            string tok[6];
            int tn = tokens_splitten(zeile, tok);

            if (tn >= 2 && tok[0] == "SEED") {
                if (!seedVorhanden) {
                    g_seed = string_zu_u32(tok[1]);
                    g_rng = g_seed ^ 0x9E3779B9u;
                    seedVorhanden = true;
                    zugIndex = 0;
                }
                continue;
            }

            if (!seedVorhanden) continue;

            // M <id> <von> <nach>
            if (tn == 4 && tok[0] == "M") {
                int id = string_zu_int(tok[1]);
                int fx, fy, tx, ty;
                if (string_zu_feld(tok[2], fx, fy) && string_zu_feld(tok[3], tx, ty)) {
                    if (brett.fuehre_klassischen_zug_aus(id, fx, fy, tx, ty)) zugIndex++;
                }
                continue;
            }

            // Q <id> <von> <ziel1> <ziel2>
            if (tn == 5 && tok[0] == "Q") {
                int id = string_zu_int(tok[1]);
                int fx, fy, ax, ay, bx, by;
                if (string_zu_feld(tok[2], fx, fy) &&
                    string_zu_feld(tok[3], ax, ay) &&
                    string_zu_feld(tok[4], bx, by)) {
                    if (brett.fuehre_quanten_zug_aus(id, fx, fy, ax, ay, bx, by)) zugIndex++;
                }
                continue;
            }
        }

        //  SDL Events 
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) beenden = true;

            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_H) hilfeAnzeigen = !hilfeAnzeigen;

                if (e.key.key == SDLK_Q) {
                    quantenModus = !quantenModus;
                    modus = EM_FIGUR_WAEHLEN;
                    ausgewaehlteId = -1;
                    vonX = vonY = ziel1X = ziel1Y = -1;
                }

                if (e.key.key == SDLK_ESCAPE) {
                    modus = EM_FIGUR_WAEHLEN;
                    ausgewaehlteId = -1;
                    vonX = vonY = ziel1X = ziel1Y = -1;
                }
            }

            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
                if (!seedVorhanden) break;
                if (brett.amZug != meineFarbe) break;

                int mx = (int)e.button.x - brettX;
                int my = (int)e.button.y - brettY;
                if (mx < 0 || my < 0 || mx >= brettGroesse || my >= brettGroesse) break;

                int x = mx / feldGroesse;
                int y = my / feldGroesse;

                if (modus == EM_FIGUR_WAEHLEN) {
                    int cnt = brett.belegAnzahl[y][x];
                    int gefunden = -1;
                    for (int i = 0; i < cnt; i++) {
                        int fid = brett.belegIds[y][x][i];
                        Figur* f = brett.figur_nach_id(fid);
                        if (f && f->lebt && f->farbe == meineFarbe) { gefunden = fid; break; }
                    }
                    if (gefunden >= 0) {
                        ausgewaehlteId = gefunden;
                        vonX = x; vonY = y;
                        modus = EM_ZIEL1;
                    }
                }
                else if (!quantenModus) {
                    if (ausgewaehlteId < 0) { modus = EM_FIGUR_WAEHLEN; break; }

                    bool ok = brett.fuehre_klassischen_zug_aus(ausgewaehlteId, vonX, vonY, x, y);
                    if (ok) {
                        string a = feld_zu_string(vonX, vonY);
                        string b = feld_zu_string(x, y);
                        netz_sende_zeile("M " + to_string(ausgewaehlteId) + " " + a + " " + b);
                        zugIndex++;

                        modus = EM_FIGUR_WAEHLEN;
                        ausgewaehlteId = -1;
                        vonX = vonY = -1;
                    }
                    else {
                        modus = EM_FIGUR_WAEHLEN;
                        ausgewaehlteId = -1;
                        vonX = vonY = -1;
                    }
                }
                else {
                    if (modus == EM_ZIEL1) {
                        ziel1X = x; ziel1Y = y;
                        modus = EM_ZIEL2;
                    }
                    else if (modus == EM_ZIEL2) {
                        int ziel2X = x, ziel2Y = y;

                        bool ok = brett.fuehre_quanten_zug_aus(ausgewaehlteId, vonX, vonY, ziel1X, ziel1Y, ziel2X, ziel2Y);
                        if (ok) {
                            string a = feld_zu_string(vonX, vonY);
                            string b = feld_zu_string(ziel1X, ziel1Y);
                            string c = feld_zu_string(ziel2X, ziel2Y);
                            netz_sende_zeile("Q " + to_string(ausgewaehlteId) + " " + a + " " + b + " " + c);
                            zugIndex++;
                        }

                        modus = EM_FIGUR_WAEHLEN;
                        ausgewaehlteId = -1;
                        vonX = vonY = ziel1X = ziel1Y = -1;
                    }
                }
            }
        }

        //  Render 
        farbe_setzen(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        // Brettfelder
        for (int yy = 0; yy < 8; yy++) for (int xx = 0; xx < 8; xx++) {
            bool hell = ((xx + yy) % 2 == 0);
            if (hell) farbe_setzen(renderer, 230, 230, 230, 255);
            else      farbe_setzen(renderer, 120, 120, 120, 255);

            SDL_FRect sq;
            sq.x = (float)(brettX + xx * feldGroesse);
            sq.y = (float)(brettY + yy * feldGroesse);
            sq.w = (float)feldGroesse;
            sq.h = (float)feldGroesse;
            SDL_RenderFillRect(renderer, &sq);

            if (ausgewaehlteId >= 0 && vonX == xx && vonY == yy) {
                farbe_setzen(renderer, 255, 215, 0, 160);
                SDL_RenderFillRect(renderer, &sq);
            }
            if (quantenModus && modus == EM_ZIEL2 && ziel1X == xx && ziel1Y == yy) {
                farbe_setzen(renderer, 0, 180, 255, 140);
                SDL_RenderFillRect(renderer, &sq);
            }
        }

        // Figuren zeichnen
        for (int i = 0; i < brett.figurenAnzahl; i++) {
            Figur* f = &brett.figuren[i];
            if (!f->lebt) continue;

            for (int j = 0; j < f->positionsAnzahl; j++) {
                int xx = f->positionen[j].x;
                int yy = f->positionen[j].y;

                unsigned char alpha = (unsigned char)((f->positionsAnzahl == 2) ? (j == 0 ? 255 : 140) : 255);

                SDL_FRect cell;
                cell.x = (float)(brettX + xx * feldGroesse);
                cell.y = (float)(brettY + yy * feldGroesse);
                cell.w = (float)feldGroesse;
                cell.h = (float)feldGroesse;

                if (f->farbe == FARBE_WEISS) farbe_setzen(renderer, 245, 245, 245, alpha);
                else                        farbe_setzen(renderer, 30, 30, 30, alpha);

                SDL_FRect pad;
                pad.x = cell.x + 10; pad.y = cell.y + 10;
                pad.w = cell.w - 20; pad.h = cell.h - 20;
                SDL_RenderFillRect(renderer, &pad);

                char L = figur_zeichen(f->typ);
                if (f->farbe == FARBE_WEISS) farbe_setzen(renderer, 30, 30, 30, alpha);
                else                        farbe_setzen(renderer, 245, 245, 245, alpha);

                int scale = 6;
                int gx = (int)cell.x + (feldGroesse - 5 * scale) / 2;
                int gy = (int)cell.y + (feldGroesse - 7 * scale) / 2;
                glyph_zeichnen(renderer, L, gx, gy, scale);
            }
        }

        // Seitenpanel
        int panelX = brettX + brettGroesse + 20;
        int panelY = brettY;

        farbe_setzen(renderer, 40, 40, 40, 255);
        SDL_FRect panel;
        panel.x = (float)panelX; panel.y = (float)panelY; panel.w = 240.0f; panel.h = 600.0f;
        SDL_RenderFillRect(renderer, &panel);

        bool ichAmZug = (brett.amZug == meineFarbe);
        farbe_setzen(renderer, ichAmZug ? 0 : 180, ichAmZug ? 180 : 0, 0, 255);
        SDL_FRect zugRect;
        zugRect.x = (float)panelX + 20; zugRect.y = (float)panelY + 20; zugRect.w = 200.0f; zugRect.h = 30.0f;
        SDL_RenderFillRect(renderer, &zugRect);

        farbe_setzen(renderer, 0, 120, 220, quantenModus ? 255 : 90);
        SDL_FRect qRect;
        qRect.x = (float)panelX + 20; qRect.y = (float)panelY + 70; qRect.w = 200.0f; qRect.h = 30.0f;
        SDL_RenderFillRect(renderer, &qRect);

        // Hilfe-Overlay
        if (hilfeAnzeigen) {
            farbe_setzen(renderer, 0, 0, 0, 175);
            SDL_FRect ov;
            ov.x = 40.0f; ov.y = 40.0f; ov.w = 700.0f; ov.h = 330.0f;
            SDL_RenderFillRect(renderer, &ov);

            farbe_setzen(renderer, 255, 255, 255, 255);
            int scale = 3;

            text_zeichnen(renderer, "TASTENBELEGUNG", 60, 60, scale);
            text_zeichnen(renderer,
                "H: HILFE EIN/AUS\n"
                "Q: QUANTEN-MODUS EIN/AUS\n"
                "ESC: AUSWAHL ABBRECHEN\n"
                "MAUS LINKS:\n"
                "  NORMAL: FIGUR -> ZIEL\n"
                "  QUANTEN: FIGUR -> ZIEL1 -> ZIEL2\n"
                "\n"
                "REGELN (KERN):\n"
                "  BAUER DIAGONAL: ERST MESSUNG, DANN LEGALITAET\n"
                "  VERSCHRAENKUNG: MESSUNG KOLLABIERT DIE GANZE GRUPPE\n"
                "  SLIDER UEBER 50%-BLOCK: 50% ZIEHT / 50% BLEIBT\n",
                60, 95, scale);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenster);
    netz_schliessen();
    SDL_Quit();
    return 0;
}
