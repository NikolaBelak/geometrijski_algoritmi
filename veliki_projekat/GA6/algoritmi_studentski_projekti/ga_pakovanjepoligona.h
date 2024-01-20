#ifndef PAKOVANJEPOLIGONA_H
#define PAKOVANJEPOLIGONA_H

#include "algoritambaza.h"
#include <iostream>
#include <set>
#include <optional>
#include <QFileDialog>
#include <cmath>
#include <QVariant>

class PakovanjePoligona : public AlgoritamBaza
{
public:
    PakovanjePoligona(QWidget *pCrtanje,
                      int pauzaKoraka,
                      const bool &naivni = false,
                      std::string imeDatoteke = "",
                      int brojPoligona = BROJ_SLUCAJNIH_OBJEKATA);

    void pokreniAlgoritam() final;
    void crtajAlgoritam(QPainter *painter) const final;
    void pokreniNaivniAlgoritam() final;
    void crtajNaivniAlgoritam(QPainter *painter) const final;

private:
    QVector<QPolygonF> _poligoni;
    QVector<QPolygonF> _poligoniKrajNaivni;
    QVector<QPolygonF> _paralelogrami;
    QMap<qreal,QVector<QPolygonF>> _paralelogramiKraj;
    QMap<qreal,QVector<QPolygonF>> _poligoniKraj;
    int _c;
    int _brPoligona;
    bool kraj=false;
};

#endif // PAKOVANJEPOLIGONA_H
