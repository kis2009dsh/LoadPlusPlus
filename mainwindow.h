#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include "datachart.h"
#include "TestCase.h"

struct PressureProfile
{
    bool isValid = false ;
    
    QString vuser;
    QString time ;
    QString secs_per_vuser;
    QString rendevzous;
    QList<TestCaseInfo> url_list ;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    
    ///
    /// @brief Start Test
    ///
    void on_pushButton_clicked(bool checked);
    
    ///
    /// @brief Hits/Sec Chart
    ///
    void on_pushButton_2_clicked(bool checked);
    
    ///
    /// @brief Response Chart
    ///
    void on_pushButton_3_clicked(bool checked);
    
    ///
    /// @brief Import XML Profile
    ///
    void on_pushButton_4_clicked(bool checked);
    
    ///
    /// @brief Clear Profile
    ///
    void on_pushButton_5_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    
public:
    inline Ui::MainWindow * GetUI() const {
        return ui;
    }
    
    std::vector<std::vector<double>> m_data ;
    DataChart * m_pHits;
    DataChart * m_pResp;
    PressureProfile m_profile ;
};

#endif // MAINWINDOW_H
