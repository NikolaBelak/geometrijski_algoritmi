#include "algoritambaza.h"
#include <fstream>

void AlgoritamBaza::timerEvent(QTimerEvent */* unused */)
{
     _semafor.release();
}

AlgoritamBaza::AlgoritamBaza(QWidget *pCrtanje, int pauzaKoraka, const bool &naivni)
    : QObject{}, _pauzaKoraka{pauzaKoraka},
      _timerId{INVALID_TIMER_ID}, _semafor(0),
      _unistiAnimaciju(false), _pNit(nullptr),
      _pCrtanje{pCrtanje}, _naivni(naivni)
{}

bool AlgoritamBaza::naivni() const
{
    return _naivni;
}

void AlgoritamBaza::crtaj(QPainter *painter) const
{
    if (_naivni)
        crtajNaivniAlgoritam(painter);
    else
        crtajAlgoritam(painter);
}

void AlgoritamBaza::pokreniAnimaciju()
{
    delete _pNit;
    _pNit = new AnimacijaNit(this);
    _pNit->start();
    _timerId = startTimer(_pauzaKoraka);
    _pCrtanje->update();
}

void AlgoritamBaza::pauzirajIliNastaviAnimaciju()
{
    if (INVALID_TIMER_ID != _timerId)
    {
        killTimer(_timerId);
        _timerId = INVALID_TIMER_ID;
    }
    else
        _timerId = startTimer(_pauzaKoraka);
}

void AlgoritamBaza::sledeciKorakAnimacije()
{
    if (INVALID_TIMER_ID != _timerId)
    {
        killTimer(_timerId);
        _timerId = INVALID_TIMER_ID;
    }

    _semafor.release();
}

void AlgoritamBaza::zaustaviAnimaciju()
{
    _unistiAnimaciju = true;
    _semafor.release();
    _pNit->quit();
    _pNit->wait();

    delete _pNit;
    _pNit = nullptr;
    _unistiAnimaciju = false;
}

void AlgoritamBaza::promeniDuzinuPauze(int duzinaPauze)
{
    if (INVALID_TIMER_ID != _timerId)
        killTimer(_timerId);
    _pauzaKoraka = duzinaPauze;
    _timerId = startTimer(_pauzaKoraka);
}

bool AlgoritamBaza::updateCanvasAndBlock()
{
    if (_pCrtanje) {
        _pCrtanje->update();
        _semafor.acquire();
    }
    return _unistiAnimaciju;
}

std::vector<QPoint> AlgoritamBaza::generisiNasumicneTacke(int brojTacaka) const
{
    srand(static_cast<unsigned>(time(nullptr)));
    int xMax;
    int yMax;

    if (_pCrtanje)
    {
        xMax = _pCrtanje->width() - DRAWING_BORDER;
        yMax = _pCrtanje->height() - DRAWING_BORDER;
    }
    else
    {
        xMax = CANVAS_WIDTH;
        yMax = CANVAS_HEIGHT;
    }

    int xMin = DRAWING_BORDER;
    int yMin = DRAWING_BORDER;

    std::vector<QPoint> randomPoints;

    int xDiff = xMax-xMin;
    int yDiff = yMax-yMin;
    for(int i=0; i < brojTacaka; i++)
        randomPoints.emplace_back(xMin + rand()%xDiff, yMin + rand()%yDiff);

    return randomPoints;
}

std::vector<QPoint> AlgoritamBaza::ucitajPodatkeIzDatoteke(std::string imeDatoteke) const
{
    std::ifstream inputFile(imeDatoteke);
    std::vector<QPoint> points;
    int x, y;
    while(inputFile >> x >> y)
        points.emplace_back(x, y);
    return points;
}

bool isConvex(const QPolygonF &polygon) {
    int size = polygon.size();
    if (size < 3) {
        // Poligon s manje od tri točke nije konveksan
        return false;
    }

    bool hasNegative = false;
    bool hasPositive = false;

    for (int i = 0; i < size; ++i) {
        QPointF p1 = polygon[i];
        QPointF p2 = polygon[(i + 1) % size];
        QPointF p3 = polygon[(i + 2) % size];

        int crossProduct = (p2.x() - p1.x()) * (p3.y() - p1.y()) - (p2.y() - p1.y()) * (p3.x() - p1.x());

        if (crossProduct < 0) {
            hasNegative = true;
        } else if (crossProduct > 0) {
            hasPositive = true;
        }

        if (hasNegative && hasPositive) {
            // Poligon nije konveksan
            return false;
        }
    }

    // Ako su svi proizvodi usmjereni u istom smjeru (ili su svi nula), poligon je konveksan
    return true;
}

bool isPolygonSimple(const QPolygonF& polygon) {
    int n = polygon.size();
    for (int i = 0; i < n; ++i) {
        QPointF current = polygon[i];
        QPointF next = polygon[(i + 1) % n];
        for (int j = i + 2; j < n; ++j) {
            QPointF other1 = polygon[j];
            QPointF other2 = polygon[(j + 1) % n];
            QLineF line1(current, next);
            QLineF line2(other1, other2);
            if (line1.intersects(line2, nullptr) == QLineF::BoundedIntersection) {
                return false;
            }
        }
    }
    return true;
}

QVector<QPolygonF> AlgoritamBaza::generisiNasumicnePoligone(int minVertices, int maxVertices,int numPolygons) const{
    srand(static_cast<unsigned>(time(nullptr)));
    int xMax;
    int yMax;

    if (_pCrtanje)
    {
        xMax = _pCrtanje->width() - DRAWING_BORDER;
        yMax = _pCrtanje->height() - DRAWING_BORDER;
    }
    else
    {
        xMax = CANVAS_WIDTH;
        yMax = CANVAS_HEIGHT;
    }

    int xMin = DRAWING_BORDER;
    int yMin = DRAWING_BORDER;

    QVector<QPolygonF> polygons;


    int xDiff = xMax-xMin;
    int yDiff = yMax-yMin;
    int numVertices;

    for (int i = 0; i < numPolygons; ++i) {
        QPolygonF polygon;
        numVertices = rand() % (maxVertices-minVertices + 1) + minVertices;

        for (int j = 0; j < numVertices; ++j) {
            int x = xMin + rand() % xDiff / (numPolygons/1.5);
            int y = yMin + rand() % yDiff / (numPolygons/(numPolygons/2));
            if (j > 0) {
                while (qFuzzyCompare(x, polygon[j - 1].x()) || qFuzzyCompare(y, polygon[j - 1].y())) {
                    x = xMin + rand() % xDiff;
                    y = yMin + rand() % yDiff;
                }
            }
            polygon << QPointF(x, y);
        }

        if (isPolygonSimple(polygon) && isConvex(polygon)){

            polygons.append(polygon);
        }
        else{
            --i;
        }
    }
    return polygons;
}

QVector<QPolygonF> AlgoritamBaza::ucitajPodatkeIzDatotekePoligoni(std::string imeDatoteke) const
{
    std::ifstream inputFile(imeDatoteke);
    QVector<QPolygonF> polygons;
    int x, y;
    int brPoligona;
    int brTacaka;
    inputFile>>brPoligona;
    for(int i=0;i<brPoligona;i++){
        QPolygonF polygon;
        inputFile>>brTacaka;
        for(int j=0;j<brTacaka;j++){
            inputFile>>x>>y;
            polygon<<QPointF(x,y);
        }
        polygons.append(polygon);
    }

    return polygons;
}
