#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include <QCoreApplication>
#include <QtTest>
#include "algoritmi_studentski_projekti/ga_pakovanjepoligona.h"

class TestRunner : public QObject
{
    Q_OBJECT

private slots:


    void testFuncKicmaPoligona(){
        QPolygonF polygon;
        polygon << QPointF(100, 50) << QPointF(250, 55) << QPointF(350, 200) << QPointF(175, 300) << QPointF(50, 200);
        QLineF result=PakovanjePoligona::kicmaPoligona(polygon);
        QCOMPARE(result,QLineF(QPointF(100,50),QPointF(175,300)));
    }

    void testFuncKicmaPoligona2(){
        QPolygonF polygon;
        polygon<<QPointF(210, 15) << QPointF(190, 95) << QPointF(115, 140) << QPointF(75, 70) << QPointF(125, 20);
        QLineF result=PakovanjePoligona::kicmaPoligona(polygon);
        QCOMPARE(result,QLineF(QPointF(210,15),QPointF(115,140)));
    }

    void testFuncKicmaParalelograma(){
        QPolygonF polygon;
        polygon<<QPointF(0, 0) << QPointF(100, 0) << QPointF(100, 100) << QPointF(0, 100);
        QLineF result=PakovanjePoligona::kicmaParalelograma(polygon);
        QCOMPARE(result,QLineF(QPointF(0,0),QPointF(0,100)));
    }

    void testFuncRastojanje(){
        QPointF p=QPointF(0,0);
        QLineF ivica(QPointF(10,0),QPointF(10,10));
        qreal result=PakovanjePoligona::rastojanje(p,ivica);
        QCOMPARE(result,-10.0);
    }


    void testFuncParalelnaXOsiKrozTacku(){
        QPointF p=QPointF(15,20);
        QLineF result=PakovanjePoligona::paralelnaXOsiKrozTacku(p);
        QCOMPARE(result,QLineF(QPointF(14,20),QPointF(16,20)));
    }


    void testFuncNapraviXParalelogram(){
        QPolygonF polygon,expected;
        expected<<QPointF(116.8, 15) << QPointF(250.8, 15) << QPointF(155.8, 140) << QPointF(21.8, 140);
        polygon<<QPointF(210, 15) << QPointF(190, 95) << QPointF(115, 140) << QPointF(75, 70) << QPointF(125, 20);
        QPolygonF result=PakovanjePoligona::napraviXParalelogram(polygon);
        QCOMPARE(result,expected);
    }

    void testFuncDaLiSeLinijeSeku(){
        QLineF linija1(QPointF(3,20),QPointF(15,22));
        QLineF linija2(QPointF(5,25),QPointF(15,20));
        bool result=PakovanjePoligona::daLiSeLinijeSeku(linija1,linija2);
        QCOMPARE(result,true);
    }

    void testFuncDaLiSeLinijeSeku2(){
        QLineF linija1(QPointF(130,125),QPointF(265,175));
        QLineF linija2(QPointF(265,235),QPointF(310,135));
        bool result=PakovanjePoligona::daLiSeLinijeSeku(linija1,linija2);
        QCOMPARE(result,false);
    }

    void testFuncDaLiSePoligoniSeku(){
        QPolygonF polygon1;
        polygon1 << QPointF(0, 0) << QPointF(150, 0) << QPointF(150, 30) << QPointF(0, 30);
        QPolygonF polygon2;
        polygon2 << QPointF(100, 20) << QPointF(200, 20) << QPointF(200, 80) << QPointF(100, 80);
        bool result=PakovanjePoligona::daLiSePoligoniSeku(polygon1,polygon2);
        QCOMPARE(result,true);
    }

    void testFuncDaLiSePoligoniSeku2(){
        QPolygonF polygon1;
        polygon1 << QPointF(0, 0) << QPointF(100, 0) << QPointF(100, 30) << QPointF(0, 30);
        QPolygonF polygon2;
        polygon2 << QPointF(150, 20) << QPointF(200, 20) << QPointF(200, 80) << QPointF(150, 80);
        bool result=PakovanjePoligona::daLiSePoligoniSeku(polygon1,polygon2);
        QCOMPARE(result,false);
    }
};


#endif // TESTRUNNER_H
