#include "ga_pakovanjepoligona.h"

PakovanjePoligona::PakovanjePoligona(QWidget *pCrtanje,
                                     int pauzaKoraka,
                                     const bool &naivni,
                                     std::string imeDatoteke,
                                     int brojPoligona)
    : AlgoritamBaza(pCrtanje, pauzaKoraka, naivni),_c(3)
{
    if (imeDatoteke == "")
        _poligoni = generisiNasumicnePoligone(5,6,brojPoligona);
    else
        _poligoni = ucitajPodatkeIzDatotekePoligoni(imeDatoteke);

}

bool operator<(const QPointF& tacka1, const QPointF& tacka2) {
    if (tacka1.x() == tacka2.x()) {
        return tacka1.y() < tacka2.y();
    }
    return tacka1.x() < tacka2.x();
}

QLineF PakovanjePoligona::kicmaPoligona(const QPolygonF& poligon){
    qreal minY=std::numeric_limits<qreal>::max();
    qreal maxY=std::numeric_limits<qreal>::min();
    QPointF minTacka;
    QPointF maxTacka;

    for(const QPointF& tacka:poligon){
        if(tacka.y()<minY){
            minY=tacka.y();
            minTacka=tacka;
        }
        if(tacka.y()>maxY){
            maxY=tacka.y();
            maxTacka=tacka;
        }
    }

    QLineF kicma(minTacka,maxTacka);
    return kicma;
}

QLineF PakovanjePoligona::kicmaParalelograma(const QPolygonF& paralelogram){
    QPointF dl=paralelogram.at(0);
    QPointF gl=paralelogram.at(3);

    QLineF kicma(dl,gl);
    return kicma;
}

bool PakovanjePoligona::porediPoKicmi(const QPolygonF& a, const QPolygonF& b){
    QLineF kicma1=kicmaPoligona(a);
    QLineF kicma2=kicmaPoligona(b);

    qreal ugao1=kicma1.angle();
    qreal ugao2=kicma2.angle();

    return ugao1<ugao2;
}

bool PakovanjePoligona::porediPoKicmiParalelograme(const QPolygonF& a, const QPolygonF& b){
    QLineF kicma1=kicmaParalelograma(a);
    QLineF kicma2=kicmaParalelograma(b);

    qreal ugao1=kicma1.angle();
    qreal ugao2=kicma2.angle();

    return ugao1<ugao2;
}

QVector<QPolygonF> PakovanjePoligona::sortirajParalelogramePoKicmi(const QVector<QPolygonF> paralelogrami)
{
    QVector<QPolygonF> sortiraniParalelogrami=paralelogrami;

    std::sort(sortiraniParalelogrami.begin(),sortiraniParalelogrami.end(),porediPoKicmiParalelograme);

    return sortiraniParalelogrami;
}


qreal PakovanjePoligona::rastojanje(const QPointF &p, const QLineF &ivica)
{
    QPointF p1 = ivica.p1();
    QPointF p2 = ivica.p2();
    qreal x = p.x() - p1.x();
    qreal y = p.y() - p1.y();
    qreal x2 = p2.x() - p1.x();
    qreal y2 = p2.y() - p1.y();


    qreal norm = sqrt(x2*x2 + y2*y2);
    if (norm <= std::numeric_limits<int>::epsilon())
        return sqrt(x*x + y*y);

    return (x*y2 - y*x2) / norm;
}

std::tuple<qreal,qreal,qreal> PakovanjePoligona::pronadjiJednacinuPrave(const QPointF &tacka1,const QPointF &tacka2){
    qreal a = tacka1.y() - tacka2.y();
    qreal b = tacka2.x() - tacka1.x();
    qreal c = tacka1.x() * tacka2.y() - tacka2.x() * tacka1.y();

    return std::make_tuple(a,b,c);
}

QLineF PakovanjePoligona::pravaIzKoef(qreal a,qreal b,qreal c){
    QPointF prva(0,-c/b);
    QPointF druga(1,(-c-a)/b);

    return QLineF(prva,druga);
}

std::pair<qreal,qreal> PakovanjePoligona::rastojanje2(const QPointF &p, const QLineF &ivica)
{
    std::tuple<qreal,qreal,qreal> prava;
    prava=pronadjiJednacinuPrave(ivica.p1(),ivica.p2());

    qreal a=std::get<0>(prava);
    qreal b=std::get<1>(prava);

    qreal normalna_a=-b;
    qreal normalna_b=a;
    qreal normalna_c=-(normalna_a*p.x()+normalna_b*p.y());

    QLineF normala=pravaIzKoef(normalna_a,normalna_b,normalna_c);

    QPointF presek;
    ivica.intersects(normala,&presek);

    return std::make_pair(p.x()-presek.x(),p.y()-presek.y());
}

QLineF PakovanjePoligona::paralelnaXOsiKrozTacku(QPointF tacka){
    QPointF prva(tacka.x()-1,tacka.y());
    QPointF druga(tacka.x()+1,tacka.y());

    return QLineF(prva,druga);
}

QPolygonF PakovanjePoligona::napraviXParalelogram(const QPolygonF& poligon){
    QPointF pb, pt;

    QLineF kicma=kicmaPoligona(poligon);
    pb=kicma.p1();
    pt=kicma.p2();
    QPointF leva, desna;
    qreal maxRastL = std::numeric_limits<qreal>::min();
    qreal maxRastD = std::numeric_limits<qreal>::min();
    for (const auto& tacka : poligon) {
        if (tacka != pb && tacka != pt) {
            qreal rast=rastojanje(tacka,kicma);
            if(rast<0 && fabs(rast)>maxRastL){
                maxRastL=fabs(rast);
                leva=tacka;
            }
            if(rast>0 && rast>maxRastD){
                maxRastD=rast;
                desna=tacka;
            }
        }
    }

    if(leva.x()==0 && leva.y()==0){
        if(pb.x()<pt.x()){
           leva.setX(pb.x());
           leva.setY(pb.y());
        }
        else{
           leva.setX(pt.x());
           leva.setY(pt.y());
        }
    }
    if(desna.x()==0 && desna.y()==0){
        if(pb.x()>=pt.x()){
           desna.setX(pb.x());
           desna.setY(pb.y());
        }
        else{
           desna.setX(pt.x());
           desna.setY(pt.y());
        }
    }

    std::pair<qreal,qreal> rastXYL=rastojanje2(leva,kicma);
    std::pair<qreal,qreal> rastXYD=rastojanje2(desna,kicma);

    QLineF sl=kicma.translated(rastXYL.first,rastXYL.second);
    QLineF sr=kicma.translated(rastXYD.first,rastXYD.second);
    QLineF lb=paralelnaXOsiKrozTacku(pb);
    QLineF lt=paralelnaXOsiKrozTacku(pt);
    QPointF dl,dd,gl,gd;
    sl.intersects(lb,&dl);
    sr.intersects(lb,&dd);
    sl.intersects(lt,&gl);
    sr.intersects(lt,&gd);


    QPolygonF paralelogram;
    paralelogram<<dl<<dd<<gd<<gl;
    return paralelogram;
}

QVector<QRectF> PakovanjePoligona::FFDHPakovanje(const QVector<QRectF>& pravougaonici, qreal stripWidth) {
    QVector<QRectF> nizPravougaonika=pravougaonici;
    std::sort(nizPravougaonika.begin(), nizPravougaonika.end(), [](const QRectF& a, const QRectF& b) {
        return a.height() > b.height();
    });
    QVector<std::tuple<qreal,qreal,qreal>> listaNivoa;
    listaNivoa.push_back(std::make_tuple(stripWidth-nizPravougaonika[0].width(),nizPravougaonika[0].height(),0));

    int i=1,j;
    while(i!=nizPravougaonika.size()){
        for(j=0;j<listaNivoa.size();j++){
           if(std::get<0>(listaNivoa[j])>=nizPravougaonika[i].width()){
                nizPravougaonika[i].moveTopLeft(QPointF(stripWidth-std::get<0>(listaNivoa[j]),std::get<2>(listaNivoa[j])));
                std::get<0>(listaNivoa[j])-=nizPravougaonika[i].width();
                break;
           }
        }
        if(j==listaNivoa.size()){
           listaNivoa.push_back(std::make_tuple(stripWidth-nizPravougaonika[i].width(),nizPravougaonika[i].height(),std::get<2>(listaNivoa[j-1])+std::get<1>(listaNivoa[j-1])));
           nizPravougaonika[i].moveTopLeft(QPointF(0,std::get<2>(listaNivoa[j])));
        }
        i++;
    }
    return nizPravougaonika;
}

void PakovanjePoligona::pokreniAlgoritam()
{
    _brPoligona=_poligoni.count();

    QVector<QPolygonF> IQ;

    for(const QPolygonF& poligon:_poligoni){
        QPolygonF q=napraviXParalelogram(poligon);
        IQ.append(q);
    }

    QVector<QPolygonF> prvobitniPoligoni = _poligoni;

    _paralelogrami.clear();
    for(QPolygonF poligon:IQ){
        _paralelogrami.push_back(poligon);
        AlgoritamBaza_updateCanvasAndBlock()
    }

    QVector<QRectF> IR;
    for(QPolygonF& q:IQ){
        QRectF r(0,0,q.boundingRect().width(),q.boundingRect().height());
        IR.push_back(r);
    }

    qreal sirinaTrake=_c*500;

    QVector<QRectF> PR = FFDHPakovanje(IR, sirinaTrake);

    QMap<qreal,QVector<QPolygonF>> grupisaniParalelogrami;


    for(QRectF& pravougaonik:PR){
        qreal y=pravougaonik.y();

        if(!grupisaniParalelogrami.contains(y)){
           grupisaniParalelogrami[y]=QVector<QPolygonF>();
        }
        for(QPolygonF& q:IQ){
           if(q.boundingRect().width()==pravougaonik.width() && q.boundingRect().height()==pravougaonik.height()){
                grupisaniParalelogrami[y].push_back(q);
           }
        }
    }

    int kljucBrojac=0;
    _paralelogrami.clear();
    for(qreal kljuc:grupisaniParalelogrami.keys()){
        QVector<QPolygonF> sortiraniPoKicmi=sortirajParalelogramePoKicmi(grupisaniParalelogrami[kljuc]);
        for (const QPolygonF& poligon : sortiraniPoKicmi) {
           _paralelogramiKraj[kljucBrojac].push_back(poligon);
           _paralelogrami.push_back(poligon);
        }
        kljucBrojac++;
    }

    for(qreal kljuc:_paralelogramiKraj.keys()){
        QVector<QPolygonF> paralelogramiKraj=_paralelogramiKraj[kljuc];
        for(QPolygonF paralelogram:paralelogramiKraj){
           auto it=std::find_if(prvobitniPoligoni.begin(),prvobitniPoligoni.end(), [&](const QPolygonF& poligon) {
               return (paralelogram.boundingRect().bottom()-1 <= poligon.boundingRect().bottom() && poligon.boundingRect().bottom() <= paralelogram.boundingRect().bottom()+1) && (paralelogram.boundingRect().top()-1 <= poligon.boundingRect().top() && poligon.boundingRect().top() <= paralelogram.boundingRect().top()+1);
           });

           if(it!=prvobitniPoligoni.end()){
                _poligoniKraj[kljuc].push_back(*it);
           }
        }
    }


    QVector<qreal> xOffsets;
    QVector<qreal> yOffsets;
    for(qreal kljuc:_paralelogramiKraj.keys()){
        for(QPolygonF& paralelogram:_paralelogramiKraj[kljuc]){
           QRectF boundingRect=paralelogram.boundingRect();

           qreal xOffset=-boundingRect.left();
           qreal yOffset=-boundingRect.top();
           xOffsets.push_back(xOffset);
           yOffsets.push_back(yOffset);

           QTransform translacijaMatrica;
           translacijaMatrica.translate(xOffset,yOffset);
           paralelogram=translacijaMatrica.map(paralelogram);
        }
    }

    for(qreal kljuc:_poligoniKraj.keys()){
        for(QPolygonF& poligon:_poligoniKraj[kljuc]){

           qreal xOffset=xOffsets.front();
           xOffsets.pop_front();
           qreal yOffset=yOffsets.front();
           yOffsets.pop_front();

           QTransform translacijaMatrica;
           translacijaMatrica.translate(xOffset,yOffset);
           poligon=translacijaMatrica.map(poligon);
        }
    }
    kraj=true;

    AlgoritamBaza_updateCanvasAndBlock();
    emit animacijaZavrsila();
}

void PakovanjePoligona::crtajAlgoritam(QPainter *painter) const
{
    if (!painter) return;

    QPen p = painter->pen();
    p.setColor(Qt::red);
    p.setWidth(3);


    QPen p2 = painter->pen();
    p2.setColor(Qt::black);
    p2.setWidth(1);

    painter->setPen(p);


    int brPol=_poligoni.size();
    int k=0;

    if(!kraj){
        for(int i=0;i<_poligoni.size();i++){
           QPolygonF poligon=_poligoni.at(i);
           poligon.translate((i%(static_cast<int>(std::ceil(brPol/2.0))))*_pCrtanje->width()/(std::ceil(brPol/2.0)),(k/(static_cast<int>(std::ceil(brPol/2.0))))*_pCrtanje->height()/2);
           painter->drawPolygon(poligon);
           k++;
        }

        k=0;
        painter->setPen(p2);
        for(int i=0;i<_paralelogrami.size();i++){
           QPolygonF poligon=_paralelogrami.at(i);
           poligon.translate((i%(static_cast<int>(std::ceil(brPol/2.0))))*_pCrtanje->width()/(std::ceil(brPol/2.0)),(k/(static_cast<int>(std::ceil(brPol/2.0))))*_pCrtanje->height()/2);
           painter->drawPolygon(poligon);
           k++;
        }
    }
    else{
        qreal yOffset = 0.0;
        qreal xOffset=0.0;
        qreal xMin;
        qreal xMin2;
        qreal maxPoligonVisina;
        QPolygonF pravougaonik;
        QRectF boundingRectPoli;
        QRectF boundingRectParal;
        pravougaonik<<QPointF(0,0)<<QPointF(100,0)<<QPointF(100,0)<<QPointF(0,100);
        for (int kljuc:_paralelogramiKraj.keys()) {
           QVector<QPolygonF> paralelogrami = _paralelogramiKraj[kljuc];
           QVector<QPolygonF> poligoni = _poligoniKraj[kljuc];


           if (kljuc != 0) {
                maxPoligonVisina = 0.0;
                for (const QPolygonF &poligon : _paralelogramiKraj[kljuc-1]) {
                    qreal visina = poligon.boundingRect().height();
                    if (visina > maxPoligonVisina) {
                        maxPoligonVisina = visina;
                    }
                }
                yOffset += maxPoligonVisina;
           }

           xOffset = 0.0;
           for (int i=0;i<paralelogrami.size();i++) {
                QPolygonF paralelogram=paralelogrami.at(i);
                QPolygonF polygon=poligoni.at(i);
                xMin = std::numeric_limits<qreal>::max();
                xMin2 = std::numeric_limits<qreal>::max();
                if(!porediPoKicmiParalelograme(paralelogram,pravougaonik)){
                    for (const QPointF &tacka : paralelogram) {

                        if (qFuzzyCompare(tacka.y(),polygon.boundingRect().bottom())) {
                            if (tacka.x() < xMin2) {
                                xMin2 = tacka.x();
                            }
                        }
                    }
                }
                else{
                    if(i!=0){
                        for (const QPointF &tacka : paralelogram) {
                            if (tacka.y() <=0.1 && tacka.y()>=-0.1) {
                                if (tacka.x() < xMin) {
                                    xMin = tacka.x();
                                }
                            }
                        }
                        xOffset = xOffset - xMin;
                    }
                }

                paralelogram.translate(xOffset, yOffset);
                polygon.translate(xOffset,yOffset);
                xOffset += paralelogram.boundingRect().width();
                boundingRectPoli=boundingRectPoli.united(polygon.boundingRect());
                boundingRectParal=boundingRectParal.united(paralelogram.boundingRect());
                painter->setPen(p);
                painter->drawPolygon(polygon);
                painter->setPen(p2);
                painter->drawPolygon(paralelogram);
                if(xMin2!=std::numeric_limits<qreal>::max()){
                    xOffset=xOffset-xMin2;
                }

           }
        }
        qreal xPoli,yPoli;
        xPoli=boundingRectPoli.right()-boundingRectPoli.left();
        yPoli=boundingRectPoli.bottom();

        painter->setPen(p);
        painter->drawRect(boundingRectPoli);
        painter->setPen(p2);
        painter->save();
        painter->scale(1,-1);
        painter->drawText(QPointF(boundingRectPoli.bottomRight().x(),-boundingRectPoli.bottomRight().y()+yPoli/2),QString::number(yPoli));
        painter->drawText(QPointF(boundingRectPoli.bottomRight().x()-xPoli/2,-boundingRectPoli.bottomRight().y()-1),QString::number(xPoli));
        painter->restore();
        painter->drawRect(boundingRectParal);

    }
}

bool PakovanjePoligona::daLiSeLinijeSeku(const QLineF& linija1, const QLineF& linija2) {
    qreal x1 = linija1.p1().x(), y1 = linija1.p1().y();
    qreal x2 = linija1.p2().x(), y2 = linija1.p2().y();
    qreal x3 = linija2.p1().x(), y3 = linija2.p1().y();
    qreal x4 = linija2.p2().x(), y4 = linija2.p2().y();

    qreal det = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (det == 0) {
        return false;
    }

    qreal t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / det;
    qreal u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / det;

    return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}

bool PakovanjePoligona::daLiSePoligoniSeku(const QPolygonF& poligon1, const QPolygonF& poligon2) {
    int n1 = poligon1.size();
    int n2 = poligon2.size();

    for (int i = 0; i < n1; ++i) {
        QLineF ivica1(poligon1.at(i), poligon1.at((i + 1) % n1));

        for (int j = 0; j < n2; ++j) {
            QLineF ivica2(poligon2.at(j), poligon2.at((j + 1) % n2));

            if (daLiSeLinijeSeku(ivica1, ivica2)) {
                return true;
            }
        }
    }

    return false;
}

void PakovanjePoligona::translirajPoligon(QPolygonF& poligon, qreal dx, qreal dy) {
    for (QPointF& tacka : poligon) {
        tacka.setX(tacka.x() + dx);
        tacka.setY(tacka.y() + dy);
    }
}

bool PakovanjePoligona::daLiSeNizPoligonaPreseca(QPolygonF& poligon1, const QVector<QPolygonF>& poligoni) {

    for(const QPolygonF& poli:poligoni){
        if(daLiSePoligoniSeku(poligon1,poli)){
            return true;
        }
    }
    return false;
}

std::pair<QRectF,QVector<QPolygonF>> PakovanjePoligona::packPolygons(QVector<QPolygonF>& poligoni,int g) {
    QVector<QPolygonF> nizPoligona=poligoni;
    QRectF boundingRect = nizPoligona.first().boundingRect();

    QVector<QPolygonF> trenutniPoligoni;
    trenutniPoligoni.push_back(nizPoligona.first());

    for (int i = 1; i < nizPoligona.size(); ++i) {
        QPolygonF& poligon = nizPoligona[i];

        qreal dx = 5;
        qreal dy = 0.0;
        while (daLiSeNizPoligonaPreseca(poligon, trenutniPoligoni)) {
            dy=0;

            if(dx<0){
                dx=5;
            }

            if(poligon.boundingRect().topRight().x()+dx>1500){
                dx=-poligon.boundingRect().left();
                dy=5;
            }

            translirajPoligon(poligon, dx, dy);
        }
        trenutniPoligoni.push_back(poligon);

        boundingRect = boundingRect.united(poligon.boundingRect());
    }

    return std::make_pair(boundingRect,trenutniPoligoni);
}


void PakovanjePoligona::pokreniNaivniAlgoritam() {
    QVector<QPolygonF> poligoni=_poligoni;

    for(QPolygonF& poli:poligoni){
        QRectF boundingRect=poli.boundingRect();

        qreal xOffset=-boundingRect.left();
        qreal yOffset=-boundingRect.top();

        QTransform translacijaMatrica;
        translacijaMatrica.translate(xOffset,yOffset);
        poli=translacijaMatrica.map(poli);
    }

    std::sort(poligoni.begin(),poligoni.end());

    std::pair<QRectF,QVector<QPolygonF>> minimalRes=packPolygons(poligoni,0);
    QVector<QPolygonF> minPoligoni=minimalRes.second;
    QRectF minimalPravougaonik=minimalRes.first;
    int i=1;

    while(std::next_permutation(poligoni.begin(),poligoni.end())){

        std::pair<QRectF,QVector<QPolygonF>> trenutniRes=packPolygons(poligoni,i);
        QRectF currentRect=trenutniRes.first;
        i++;

        if((currentRect.width()*currentRect.height())<(minimalPravougaonik.width()*minimalPravougaonik.height())){
            minimalPravougaonik=currentRect;
            minPoligoni.clear();
            minPoligoni=trenutniRes.second;
        }
    }

    for(QPolygonF &poli:minPoligoni){
        _poligoniKrajNaivni.push_back(poli);
    }



    kraj=true;
    AlgoritamBaza_updateCanvasAndBlock();
    emit animacijaZavrsila();
}

void PakovanjePoligona::crtajNaivniAlgoritam(QPainter *painter) const {
    if (!painter) return;

    QPen p = painter->pen();
    p.setColor(Qt::red);
    p.setWidth(3);

    painter->setPen(p);


    int brPol=_poligoni.size();
    QRectF boundingRectPoli;
    int k=0;

    if(!kraj){
        for(int i=0;i<_poligoni.size();i++){
            QPolygonF poligon=_poligoni.at(i);
            poligon.translate((i%(static_cast<int>(std::ceil(brPol/2.0))))*_pCrtanje->width()/(std::ceil(brPol/2.0)),(k/(static_cast<int>(std::ceil(brPol/2.0))))*_pCrtanje->height()/2);
            k++;
            painter->drawPolygon(poligon);
        }
    }
    else{
        for(int i=0;i<_poligoniKrajNaivni.size();i++){
            QPolygonF polygon=_poligoniKrajNaivni.at(i);
            boundingRectPoli=boundingRectPoli.united(polygon.boundingRect());
            painter->drawPolygon(polygon);
        }

        qreal xPoli,yPoli;
        xPoli=boundingRectPoli.right()-boundingRectPoli.left();
        yPoli=boundingRectPoli.bottom();

        painter->drawRect(boundingRectPoli);
        painter->save();
        painter->scale(1,-1);
        if(boundingRectPoli.right()<1490){
            painter->drawText(QPointF(boundingRectPoli.bottomRight().x(),-boundingRectPoli.bottomRight().y()+yPoli/2),QString::number(yPoli));
        }
        else{
            painter->drawText(QPointF(boundingRectPoli.bottomRight().x()-30,-boundingRectPoli.bottomRight().y()+yPoli/2),QString::number(yPoli));
        }
        painter->drawText(QPointF(boundingRectPoli.bottomRight().x()-xPoli/2,-boundingRectPoli.bottomRight().y()-1),QString::number(xPoli));
        painter->restore();
    }
}
