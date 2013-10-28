#ifndef DATACHART_H
#define DATACHART_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QPushButton>

class DataChartCaption;
class DataChartPrivate;
class DataChart : public QWidget
{
    Q_OBJECT
//private:
//    explicit DataChart(QWidget *parent = 0);
public:
    explicit DataChart(QWidget *parent = 0);
    
public Q_SLOTS:
    void exportImage();

public:
    
    void setColor(const QColor& , const QColor&);
    
    void setData(const QList<qreal>& data , const QList<qreal>& data2);
    
    void repaint();
    
    void clear();

    void showAt(const QPoint& showPoint);
    
    void setCaption(const QString& strCaption);
    QString caption() const;

protected:
    QSize sizeHint() const
    {
        return QSize(300,300);
    }

    void showEvent(QShowEvent *);

    void mousePressEvent(QMouseEvent *);

    void mouseMoveEvent(QMouseEvent *);


//private:
//    static DataChart* _instance;

protected:

private:
    void setThisSize();
//    void initTimer();
//    void resetTimer();
    QPushButton* createButton(const QString& strText,const QIcon& icon = QIcon(),const QString& strTip = QString());

private:
    QPushButton* _pinButton;
    QPushButton* _closeButton;
    QPushButton* _exportButton;
    DataChartPrivate* _d;
    DataChartCaption* _caption;

//    QTimer* _hideTimer;
    bool _pressed;
    QPoint _offset;

private Q_SLOTS:
    void slot_pinThis();
    void slot_buttonHandler();
};

class DataChartCaption : public QWidget
{
    Q_OBJECT
private:
    explicit DataChartCaption(QWidget* parent = 0);

public:
    void setCaption(const QString& strCaption);
    QString caption() const;

protected:
    void paintEvent(QPaintEvent *);

private:
    void draw(QPainter* painter);

private:
    QString _strCaption;

    friend class DataChart;
};

class DataChartPrivate : public QWidget
{
    Q_OBJECT
public:
    explicit DataChartPrivate(QWidget* parent = 0);

public:
    void setColor(const QColor& , const QColor&);
    void setData(const QList<qreal>& data,const QList<qreal>& data2);
    void clear();

protected:
    void paintEvent(QPaintEvent *);
    
private:
    void drawBackground(QPainter* painter);
    void drawLegend(QPainter* painter);
    void drawGraph(QPainter* painter);
    void drawLittleAngle(QPainter* painter,const QPoint& vertex,Qt::Orientation o);
    void drawRotatedLittleRect(QPainter* painter,const QPoint& center);

    void findMaxValue();
    void findMaxValue2();
    
    int getVertCount();
    int getVertCount2();
    
private:
    typedef QPair<QRectF,qreal> RectValue;
    QList<qreal> _data  ;
    QList<qreal> _data2 ;
    qreal _maxValue  ;
    qreal _maxValue2 ;
    qreal _maxLeftSpace ;
    qreal _maxLeftSpace2 ;
    QColor m_graph_color ;
    QColor m_graph_color2 ;
};

#endif // DATACHART_H
