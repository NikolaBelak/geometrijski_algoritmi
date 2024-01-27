#ifndef PAKOVANJEPOLIGONA_H
#define PAKOVANJEPOLIGONA_H

#include "algoritambaza.h"
#include <iostream>
#include <set>
#include <optional>
#include <QFileDialog>
#include <cmath>
#include <QVariant>
#include <QtTest>


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
    static QLineF kicmaPoligona(const QPolygonF& poligon);
    static QLineF kicmaParalelograma(const QPolygonF& paralelogram);
    static bool porediPoKicmi(const QPolygonF& a, const QPolygonF& b);
    static bool porediPoKicmiParalelograme(const QPolygonF& a, const QPolygonF& b);
    static QVector<QPolygonF> sortirajParalelogramePoKicmi(const QVector<QPolygonF> paralelogrami);
    static qreal rastojanje(const QPointF &p, const QLineF &ivica);
    static std::tuple<qreal,qreal,qreal> pronadjiJednacinuPrave(const QPointF &tacka1,const QPointF &tacka2);
    static qreal distanca(qreal a,qreal b,qreal c,QPointF& tacka);
    static QLineF pravaIzKoef(qreal a,qreal b,qreal c);
    static std::pair<qreal,qreal> rastojanje2(const QPointF &p, const QLineF &ivica);
    static QLineF paralelnaXOsiKrozTacku(QPointF tacka);
    static QPolygonF napraviXParalelogram(const QPolygonF& poligon);
    static QVector<QRectF> FFDHPakovanje(const QVector<QRectF>& pravougaonici, qreal stripWidth);
    static bool daLiSeLinijeSeku(const QLineF& linija1, const QLineF& linija2);
    static bool daLiSePoligoniSeku(const QPolygonF& poligon1, const QPolygonF& poligon2);
    static void translirajPoligon(QPolygonF& poligon, qreal dx, qreal dy);
    static bool daLiSeNizPoligonaPreseca(QPolygonF& poligon1, const QVector<QPolygonF>& poligoni);
    static std::pair<QRectF,QVector<QPolygonF>> packPolygons(QVector<QPolygonF>& poligoni,int g);


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
