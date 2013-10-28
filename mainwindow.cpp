#include "mainwindow.h"
#include "datachart.h"
#include "ui_mainwindow.h"

#include "Timer.h"
#include "HttpServer.h"
#include "ThreadPool.h"
#include "Algorithm.h"
#include "PressureFrame.h"

#include <vector>
#include <forward_list>

const QColor hits_color = Qt::blue;
const QColor resp_color = Qt::red;
const QColor user_color = Qt::black;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_data(3),
    m_pHits(NULL),
    m_pResp(NULL)
{
    ui->setupUi(this);
    
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Hits/Sec" << "Average Response Time" << "Total Success" << "Total Fail");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setAutoScroll(true);
    
    ui->tableWidget_2->setHorizontalHeaderLabels(QStringList() << "Min Hits/Sec" << "Max Hits/Sec" << "Min Response(s)" << "Max Response(s)");
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setAutoScroll(true);
    
    ui->textEdit->setText("5");
    ui->textEdit_2->setText("5");
    ui->textEdit_3->setText("http://www.baidu.com");
    ui->textEdit_5->setText("1");
    
    ui->radioButton->setChecked(true);
    
    m_profile.isValid = false ;
}

MainWindow::~MainWindow()
{
    delete ui;
    
    if( m_pHits )
    {
        delete m_pHits;
        m_pHits = NULL;
    }
    
    if( m_pResp )
    {
        delete m_pResp;
        m_pResp = NULL;
    }
}

static std::mutex g_mutex ;
static std::mutex g_mutex_data ;

static void UpdateChartTest( MainWindow * pThis , int concurrent_connections , double average_response_time , int current_vuser )
{
//    const int nPoint = 36;
    
    QList<double> first_list ;
    QList<double> second_list ;
    QList<double> user_list ;
    
    { /// Update Data
        std::lock_guard<std::mutex> lock( g_mutex_data );
        pThis->m_data[0].push_back(concurrent_connections);
        pThis->m_data[1].push_back(average_response_time);
        pThis->m_data[2].push_back(current_vuser);
        
        /// Transport Params
        for(auto iter = (pThis->m_data[0]).begin() ; iter != (pThis->m_data[0]).end() ; ++iter)
            first_list.push_back(*iter);
        
        for(auto iter = (pThis->m_data[1]).begin() ; iter != (pThis->m_data[1]).end() ; ++iter)
            second_list.push_back(*iter * 1000);
        
        for(auto iter = (pThis->m_data[2]).begin() ; iter != (pThis->m_data[2]).end() ; ++iter )
            user_list.push_back( *iter) ;
    }
    
    /// Paint Chart
    if( pThis->m_pHits )
    {
        pThis->m_pHits->setData(first_list, user_list);
        pThis->m_pHits->repaint();
    }
    else
    {
        pThis->m_pHits = new DataChart;
        pThis->m_pHits->setCaption("Hits/Second");
        pThis->m_pHits->showAt(QPoint(10,10));
        pThis->m_pHits->setData(first_list , user_list);
        pThis->m_pHits->setColor(hits_color , user_color);
        pThis->m_pHits->repaint();
    }
    
    if( pThis->m_pResp )
    {
        pThis->m_pResp->setData(second_list , user_list);
        pThis->m_pResp->repaint();
    }
    else
    {
        QDesktopWidget desktop;
        pThis->m_pResp = new DataChart;
        pThis->m_pResp->setCaption("Average Response Time (Millisecond)");
        pThis->m_pResp->showAt(QPoint(10 , 10 + desktop.geometry().width() / 3 * 0.68 + 50 ));
        pThis->m_pResp->setData(second_list, user_list);
        pThis->m_pResp->setColor(resp_color , user_color);
        pThis->m_pResp->repaint();
    }
}

static void UpdateListTest( MainWindow * pThis , QTableWidget * pListWidget ,int hit_per_sec , double response , int total_success , int total_fail , int current_user)
{
    int row = pListWidget->rowCount();
    pListWidget->insertRow(row);
    
    auto pHit = new QTableWidgetItem(QString::number(hit_per_sec));
    auto pRes =  new QTableWidgetItem(QString::number(response));
    auto pSuc = new QTableWidgetItem(QString::number(total_success));
    auto pFail = new QTableWidgetItem(QString::number(total_fail));
    
    pHit->setTextAlignment(Qt::AlignRight);
    pRes->setTextAlignment(Qt::AlignRight);
    pSuc->setTextAlignment(Qt::AlignRight);
    pFail->setTextAlignment(Qt::AlignRight);
    
    pListWidget->setItem(row , 0 , pHit);
    pListWidget->setItem(row , 1 , pRes);
    pListWidget->setItem(row , 2 , pSuc );
    pListWidget->setItem(row , 3 , pFail);
    
    pListWidget->scrollToBottom();
    
    UpdateChartTest(pThis , hit_per_sec , response , current_user);
}

static void UpdateResultTest( MainWindow * pThis , QTableWidget * pResultWidget , int hits_min , int hits_max , double response_min , double response_max )
{
    int row = pResultWidget->rowCount();
    pResultWidget->insertRow(row);
    
    auto pHit = new QTableWidgetItem(QString::number(hits_min));
    auto pRes =  new QTableWidgetItem(QString::number(hits_max));
    auto pSuc = new QTableWidgetItem(QString::number(response_min));
    auto pFail = new QTableWidgetItem(QString::number(response_max));
    
    pHit->setTextAlignment(Qt::AlignRight);
    pRes->setTextAlignment(Qt::AlignRight);
    pSuc->setTextAlignment(Qt::AlignRight);
    pFail->setTextAlignment(Qt::AlignRight);
    
    pResultWidget->setItem(row , 0 , pHit);
    pResultWidget->setItem(row , 1 , pRes);
    pResultWidget->setItem(row , 2 , pSuc );
    pResultWidget->setItem(row , 3 , pFail);
    
    pResultWidget->scrollToBottom();
}

//static void UpdateList( MainWindow * pThis , QTableWidget * pListWidget , int hit_per_sec , double response , int total_success , int total_fail)
//{
//    int row = pListWidget->rowCount();
//    pListWidget->insertRow(row);
//    
//    auto pHit = new QTableWidgetItem(QString::number(hit_per_sec));
//    auto pRes =  new QTableWidgetItem(QString::number(response));
//    auto pSuc = new QTableWidgetItem(QString::number(total_success));
//    auto pFail = new QTableWidgetItem(QString::number(total_fail));
//    
//    pHit->setTextAlignment(Qt::AlignRight);
//    pRes->setTextAlignment(Qt::AlignRight);
//    pSuc->setTextAlignment(Qt::AlignRight);
//    pFail->setTextAlignment(Qt::AlignRight);
//    
//    pListWidget->setItem(row , 0 , pHit);
//    pListWidget->setItem(row , 1 , pRes);
//    pListWidget->setItem(row , 2 , pSuc );
//    pListWidget->setItem(row , 3 , pFail);
//    
//    pListWidget->scrollToBottom();
//}

//static void UpdateResult( QTableWidget * pResultWidget , int hits_min , int hits_max , double response_min , double response_max )
//{
//    int row = pResultWidget->rowCount();
//    pResultWidget->insertRow(row);
//    
//    auto pHit = new QTableWidgetItem(QString::number(hits_min));
//    auto pRes =  new QTableWidgetItem(QString::number(hits_max));
//    auto pSuc = new QTableWidgetItem(QString::number(response_min));
//    auto pFail = new QTableWidgetItem(QString::number(response_max));
//    
//    pHit->setTextAlignment(Qt::AlignRight);
//    pRes->setTextAlignment(Qt::AlignRight);
//    pSuc->setTextAlignment(Qt::AlignRight);
//    pFail->setTextAlignment(Qt::AlignRight);
//    
//    pResultWidget->setItem(row , 0 , pHit);
//    pResultWidget->setItem(row , 1 , pRes);
//    pResultWidget->setItem(row , 2 , pSuc );
//    pResultWidget->setItem(row , 3 , pFail);
//    
//    pResultWidget->scrollToBottom();
//}

static DataChart * GenerateChart(const QList<qreal>& list , const QList<qreal>& user  , const QString& caption , const QPoint& point , const QColor& color , const QColor& user_c)
{
    DataChart * pDataChart = new DataChart;
    
    pDataChart->setCaption(caption);
    pDataChart->setData(list , user);
    pDataChart->showAt(point);
    pDataChart->setColor(color , user_c);
    
    return pDataChart ;
}

//static void worker_proc( MainWindow * pThis ,
//                        QPushButton * pThisButton , QTableWidget * pListWidget, QTableWidget * pResultWidget ,
//                        QPushButton * pHitsButton , QPushButton * pRespButton ,
//                        std::vector<std::vector<double>>& chart_data ,
//                        const int vuser , const int time,
//                        const QString url , const QString data,
//                        const bool isRendezvous)
//{
//    std::atomic<int> currentRendezvousNumber(0) ;
//    
//    std::atomic<bool> shouldContinue(true) ;
//    
//    std::vector<std::vector<std::tuple<double,bool,double>>> result(vuser);
//    timer countTime ;
//    
//    /// Work Thread To Count Time
//    std::thread countTimeThread([&shouldContinue , time , &countTime]() -> void
//                                {
//                                    countTime.Reset();
//                                    
//                                    while (true) {
//                                        if( countTime.ElapsedSeconds() >= double(time) ) {
//                                            /// <shouldContinue> Only Write Here
//                                            shouldContinue = false;
//                                            break ;
//                                        }
//                                    }
//                                } );
//    
//    /// Work Function to Rendezvous Point
//    auto arrivedNotify = [&currentRendezvousNumber]() -> void
//    {
//        ++currentRendezvousNumber;
//    };
//    
//    /// Work Function to Rendezvous Point
//    auto waitForLeave = [&currentRendezvousNumber , vuser]() -> bool
//    {
//        return currentRendezvousNumber < vuser ;
//    };
//    
//    /// Work Function to Rendezvous Point
//    auto hasGone = [&currentRendezvousNumber]() -> void
//    {
//        --currentRendezvousNumber ;
//    };
//    
//    /// Create VUSER's  Thread
//    std::forward_list<std::thread> vusr_list ;
//    for (int i = 0 ; i < vuser ; ++i)
//    {
//        auto& result_ref = result[i];
//        vusr_list.push_front(std::thread( [
//                                           &shouldContinue , &result_ref , &countTime , &currentRendezvousNumber , &url , &data , &vuser , &isRendezvous ,
//                                           &arrivedNotify , &waitForLeave , &hasGone
//                                           ]() -> void
//                                         {
//                                             auto request ( std::make_shared<HttpRequest>( (url + data).toStdString() , HttpRequestMethod::GET ));
//                                             auto response( std::make_shared<HttpResponse>());
//                                             auto error   ( std::make_shared<std::string>());
//                                             
//                                             timer use_timer;
//                                             
//                                             /// All Read Here
//                                             while (shouldContinue)
//                                             {
//                                                 /// Should Rendezvous
//                                                 if( isRendezvous )
//                                                 {
//                                                     /// Arrived Rendezvous
//                                                     arrivedNotify();
//                                                     
//                                                     /// Wait for leave
//                                                     while ( waitForLeave() ) {
//                                                         if(shouldContinue)
//                                                             continue;
//                                                         else
//                                                             break;
//                                                     }
//                                                     
//                                                     /// Sleep for 0.0001 s so that all threads have executed "waitForLeave"
//                                                     std::this_thread::sleep_for(std::chrono::microseconds(100));
//                                                     
//                                                     /// Notify Gone
//                                                     hasGone();
//                                                 }
//                                                 
//                                                 /// Send Request
//                                                 HttpConnection::SendAsyncHttpRequestWithoutBody(request, response , error);
//                                                 
//                                                 double seconds  = countTime.ElapsedSeconds() ;
//                                                 
//                                                 std::lock_guard<std::mutex> lock ( g_mutex );
//                                                 result_ref.push_back(std::make_tuple(response->GetTotalTime() ,
//                                                                                      response->GetStatusCode() < 400 ,
//                                                                                      seconds ));
//                                                 
//                                             }
//                                         } )
//                             );
//        
//    }
//    
//    /// Work Thread to Analysis
//    std::thread analysisThread( [&countTime , &result , &chart_data , pThis , time , vuser , pListWidget , pResultWidget]() -> void
//                               {
//                                   int current = 2;
//                                   std::pair<int,int>       record(0 , 0) ;   /// ( success , error )
//                                   std::pair<double,double> response_time_range  (std::numeric_limits<double>::max() , 0.0);
//                                   std::pair<int,int>       concurrent_conn_range(std::numeric_limits<int>::max() , 0);
//                                   std::pair<int,int>       response_time_range_second (1,1);
//                                   std::pair<int,int>       concurrent_conn_range_second(1,1);
//                                   
//                                   while (countTime.ElapsedSeconds() <= time )
//                                   {
//                                       if( countTime.ElapsedSeconds() >= current )
//                                       {
//                                           /// Concurrent connections in second i
//                                           int concurrent_connections = 0;
//                                           double average_response_time = 0.0;
//                                           
//                                           {  /// Safety Begin
//                                               std::lock_guard<std::mutex> lock(g_mutex);
//                                               
//                                               for(auto& user : result)
//                                               {
//                                                   for(auto& res : user)
//                                                   {
//                                                       /// Current Second
//                                                       if( std::get<2>(res) == current - 1 )
//                                                       {
//                                                           bool isSuccess = std::get<1>(res);
//                                                           
//                                                           concurrent_connections  += isSuccess ? 1 : 0;
//                                                           average_response_time += isSuccess ? std::get<0>(res) : 0.0 ;
//                                                           
//                                                           isSuccess ? ++record.first : ++record.second ;
//                                                       }
//                                                   }
//                                               }
//                                           } /// Safety End
//                                           
//                                           if( concurrent_connections > 0 )
//                                               average_response_time /= concurrent_connections;
//                                           
//                                           std::less<double> comparePred ;
//                                           
//                                           /// Get (min,max) of concurrent connections
//                                           if( comparePred(concurrent_connections , concurrent_conn_range.first) )
//                                           {
//                                               concurrent_conn_range.first = concurrent_connections ;
//                                               concurrent_conn_range_second.first = current - 1;
//                                           }
//                                           
//                                           if( !comparePred(concurrent_connections , concurrent_conn_range.second) )
//                                           {
//                                               concurrent_conn_range.second = concurrent_connections ;
//                                               concurrent_conn_range_second.second = current - 1;
//                                           }
//                                           
//                                           /// Get (min,max) of response time
//                                           if( comparePred(average_response_time , response_time_range.first ) )
//                                           {
//                                               response_time_range.first = average_response_time ;
//                                               response_time_range_second.first = current - 1;
//                                           }
//                                           
//                                           if( !comparePred(average_response_time , response_time_range.second ) )
//                                           {
//                                               response_time_range.second = average_response_time ;
//                                               response_time_range_second.second = current - 1;
//                                           }
//                                           
//                                           /// Output
////                                           std::cout << "Second : " << current - 1 << "       VUSER : " << VUSER << std::endl;
////                                           std::cout << "Hits       Per         Second : " << concurrent_connections << std::endl;
////                                           std::cout << "Average    Response    Time   : " << average_response_time << " s" << std::endl;
////                                           std::cout << "Total      Success     Conns  : " << record.first << std::endl;
////                                           std::cout << "Total      Fail        Conns  : " << record.second << std::endl;
////                                           std::cout << std::endl;
//                                           
//                                           UpdateList(pThis , pListWidget, concurrent_connections , average_response_time , record.first , record.second);
//                                                                                
//                                           { /// Update Data
//                                               std::lock_guard<std::mutex> lock( g_mutex_data );
//                                               chart_data[0].push_back(concurrent_connections);
//                                               chart_data[1].push_back(average_response_time);
//                                           }
//                                           
//                                           /// Show Data
//                                           QList<double> first_list ;
//                                           for(auto i : chart_data[0])
//                                               first_list.push_back(i);
//                                           
//                                           if( pThis->m_pHits )
//                                           {
//                                               pThis->m_pHits->setData(first_list);
//                                               pThis->m_pHits->repaint();
//                                           }
//                                           else
//                                           {
//                                               pThis->m_pHits = new DataChart;
//                                               pThis->m_pHits->setCaption("Hits/Second");
//                                               pThis->m_pHits->showAt(QPoint(10,10));
//                                               pThis->m_pHits->setData(first_list);
//                                               pThis->m_pHits->setColor(hits_color);
//                                               pThis->m_pHits->repaint();
//                                           }
//                                           
//                                           QList<double> second_list ;
//                                           for(auto i : chart_data[1])
//                                               second_list.push_back(i * 1000);
//                                           
//                                           if( pThis->m_pResp )
//                                           {
//                                               pThis->m_pResp->setData(second_list);
//                                               pThis->m_pResp->repaint();
//                                           }
//                                           else
//                                           {
//                                               QDesktopWidget desktop;
//                                               pThis->m_pResp = new DataChart;
//                                               pThis->m_pResp->setCaption("Average Response Time (Millisecond)");
//                                               pThis->m_pResp->showAt(QPoint(10 , 10 + desktop.geometry().width() / 3 * 0.68 + 50 ));
//                                               pThis->m_pResp->setData(second_list);
//                                               pThis->m_pResp->setColor(resp_color);
//                                               pThis->m_pResp->repaint();
//                                           }
//                                           
//                                           //// Next Second
//                                           ++current;
//                                       }
//                                   }
//                                  
//                                   UpdateResult(pResultWidget, concurrent_conn_range.first , concurrent_conn_range.second, response_time_range.first , response_time_range.second);
//                               } );
//    
//    analysisThread.join();
//    
////    std::cout << "Task success : analysis" << std::endl;
//    
//    /// Wait For End
//    for(auto& vusr : vusr_list )
//        vusr.join();
//    
////    std::cout << "Task success : virtual user" << std::endl;
//    
//    /// End Time Counter
//    countTimeThread.join() ;
//    
////    std::cout << "Task success : time counter" << std::endl;
//    
//    pThisButton->setEnabled(true);
//    pThisButton->setText("Start Test");
//}

static void work_proc_test(const int vuser , const int time , const int secs_per_vuser ,
                           std::vector<TestCaseInfo> test_case_list ,
                           MainWindow * pThis , QTableWidget * pListWidget , QTableWidget * pResultWidget)
{
    PressureTest test ;
    
    PressureTest::CallBackType callBackFunc = [pThis,pListWidget]( int concurrent , double response , int success , int fail , int current_user ) -> void
    {
        UpdateListTest(pThis , pListWidget , concurrent , response , success , fail , current_user);
    };
    
    PressureTest::ResultCallBackType resultCallBackFunc = [pThis , pResultWidget]( int hits_min , int hits_max, double response_min, double response_max) -> void
    {
        UpdateResultTest(pThis, pResultWidget ,hits_min , hits_max, response_min, response_max);
    };
    
    test.InitializePressureTest(vuser , time , secs_per_vuser);
    
    test.SetTestCase(test_case_list);
    
    test.RegisiterPerSecondCallBack(callBackFunc);
    test.RegisiterResultCallBack(resultCallBackFunc);
    
    test.StartAsync();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    test.WaitForTaskCompleted();
    
    pThis->GetUI()->pushButton->setEnabled(true);
    pThis->GetUI()->pushButton->setText("Start Test");
}

void MainWindow::on_pushButton_clicked(bool checked)
{
    auto pThisButton = ui->pushButton;
    auto pVuserText = ui->textEdit;
    auto pTimeText  = ui->textEdit_2;
    auto pUrlText   = ui->textEdit_3;
    auto pPostText  = ui->textEdit_4;
    auto pSecsPerVuser = ui->textEdit_5;
    auto pHitsButton = ui->pushButton_2;
    auto pRespButton = ui->pushButton_3;
    
    auto pListWidget = ui->tableWidget;
    auto pResultWidget = ui->tableWidget_2;
    
    /// Lock
    pThisButton->setText("Testing");
    pThisButton->setEnabled(false);
    
    /// Clear list
    pListWidget->clearContents();
    pListWidget->setRowCount(0);
    
    /// Dont clear result list
//    pResultWidget->clearContents();
//    pResultWidget->setRowCount(0);
    
    int vuser = pVuserText->toPlainText().toInt();
    int time  = pTimeText->toPlainText().toInt();
    int secs_per_vuser = pSecsPerVuser->toPlainText().toInt();
    
    QString url = pUrlText->toPlainText();
    QString data = pPostText->toPlainText();
    
    bool isRendezvous = ui->radioButton->isChecked() ;
    
    //////////////////////////////////////////////////////////////////
    
    if(m_data.size() < 3)
        m_data.resize(3);
    
    m_data[0].clear();
    m_data[1].clear();
    m_data[2].clear();
    
    std::vector<TestCaseInfo> test_case_list ;
    
    if( m_profile.isValid )
    {
        for(const auto& test_case : m_profile.url_list)
            test_case_list.push_back(test_case);
    }
    else
    {
        TestCaseInfo test_case ;
        
        test_case.SetUrl(url.toStdString());
        test_case.SetPost(data.toStdString());
        test_case.SetRendezvous(isRendezvous);
        
        test_case_list.push_back(test_case);
    }
    
//    std::thread( worker_proc ,this, pThisButton , pListWidget, pResultWidget , pHitsButton, pRespButton , std::ref(m_data) , vuser , time , url , data , isRendezvous ).detach();
    std::thread( work_proc_test , vuser , time , secs_per_vuser , test_case_list , this , pListWidget , pResultWidget).detach();
}


void MainWindow::on_pushButton_2_clicked(bool checked)
{
    QList<double> first_list ;
    QList<double> user_list ;
    
    {///
        std::lock_guard<std::mutex> lock(g_mutex_data);
        
        for(auto iter = m_data[0].begin() ; iter != m_data[0].end() ; ++iter)
            first_list.push_back(*iter);
        
        for(auto iter = m_data[2].begin() ; iter != m_data[2].end() ; ++iter)
            user_list.push_back(*iter);
    }
    
    if( m_pHits )
    {
        delete m_pHits;
        m_pHits = NULL;
    }
    
    m_pHits = GenerateChart(first_list , user_list , "Hits/Second" , QPoint(10, 10) , hits_color , user_color);
}

void MainWindow::on_pushButton_3_clicked(bool checked)
{
    QList<double> second_list ;
    QList<double> user_list ;
    { ///
        std::lock_guard<std::mutex> lock(g_mutex_data);
        for(auto iter = m_data[1].begin() ; iter != m_data[1].end() ; ++iter)
            second_list.push_back(*iter * 1000);
        
        for(auto iter = m_data[2].begin() ; iter != m_data[2].end() ; ++iter)
            user_list.push_back(*iter);
    }
    
    if( m_pResp )
    {
        delete m_pResp;
        m_pResp = NULL;
    }
    
    QDesktopWidget desktop;
    m_pResp = GenerateChart(second_list ,
                            user_list ,
                            "Average Response Time (Millisecond)" ,
                            QPoint(10 , 10 + desktop.geometry().width() / 3 * 0.68 + 50 ) ,
                            resp_color ,
                            user_color);
    
}

void MainWindow::on_pushButton_4_clicked(bool checked)
{
    QString filename = QFileDialog::getOpenFileName(this , tr("Open XML") , "" , tr("XML files (*.xml)"));
    
    if( !filename.isNull() )
    {
        m_profile.url_list.clear();
        
        QFile file(filename);
        QString errorStr;
        int errorLine;
        int errorCol;
        
        if (!file.open(QIODevice::ReadOnly))
            return ;
        
        QDomDocument doc ;
        if(!doc.setContent(&file,true,&errorStr,
                           &errorLine,&errorCol))
        {
            file.close();
            qDebug()<<"errorStr:"<<errorStr<<"\n";
            qDebug()<<"errorLine:"<<errorLine<<
            "  errorCol:"<<errorCol;
            return ;
        }
        
        file.close();
        
        QDomElement root = doc.documentElement();
        QDomElement ele = root.firstChildElement();
        
        int i = 1;
        for( ; !ele.isNull() ; ele = ele.nextSiblingElement())
        {
            qDebug() << i << " : " << ele.nodeName() << " " << ele.text() << "\n";
            ++i;
            
            if( ele.nodeName() == "vuser" )
            {
                m_profile.vuser = ele.text();
            }
            else if( ele.nodeName() == "time" )
            {
                m_profile.time = ele.text();
            }
            else if( ele.nodeName() == "secs_per_vuser" )
            {
                m_profile.secs_per_vuser = ele.text();
            }
            else if( ele.nodeName() == "url")
            {
//                m_profile.url_list.push_back(qMakePair(ele.text(), ele.attribute("post")));
                TestCaseInfo url_case( ele.text().toStdString() ) ;
                
                if( ele.hasAttribute("post") )
                {
                    url_case.SetPost(ele.attribute("post").toStdString());
                }
                else if( ele.hasAttribute("record") )
                {
                    if( ele.attribute("record") == "true" )
                        url_case.SetRecord(true);
                    else
                        url_case.SetRecord(false);
                }
                else if( ele.hasAttribute("rend") )
                {
                    if( ele.attribute("rend") == "true" )
                        url_case.SetRendezvous(true);
                    else
                        url_case.SetRendezvous(false);
                }
                else if( ele.hasAttribute("expect") )
                {
                    std::string status_code = ele.attribute("expect").toStdString();
                    int code = std::stoi(status_code);
                    
                    if( code >= 100 && code <= 600)
                        url_case.SetExpectValue(code);
                }
                
                m_profile.url_list.push_back(url_case);
            }
        }
        
        m_profile.isValid = true;
        
        ui->textEdit->setText(m_profile.vuser);
        ui->textEdit_2->setText(m_profile.time);
        ui->textEdit_5->setText(m_profile.secs_per_vuser);
        ui->textEdit_3->setText("Read From Profile");
        ui->textEdit_4->setText("Read From Profile");
    }
}

//void MainWindow::on_pushButton_5_clicked(bool checked)
//{
//    QString filename = QFileDialog::getSaveFileName(this, tr("Save XML"), "", tr("XML files (*.xml)"));
//    
//    if( !filename.isNull() )
//    {
//        QDomDocument document ;
//        auto instruction = document.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
//        document.appendChild(instruction);
//        
//        QDomElement root = document.createElement("setting");
//        document.appendChild(root);
//        
//        QDomElement vuser = document.createElement("vuser");
//        root.appendChild(vuser);
//        QDomText vuser_text = document.createTextNode(ui->textEdit->toPlainText());
//        vuser.appendChild(vuser_text);
//        
//        QDomElement time = document.createElement("time");
//        root.appendChild(time);
//        QDomText time_text = document.createTextNode(ui->textEdit_2->toPlainText());
//        time.appendChild(time_text);
//        
//        if( m_profile.isValid )
//        {
//            for(const auto& i : m_profile.url_list)
//            {
//                QDomElement url_node  = document.createElement("url");
//                QDomText    url_text  = document.createTextNode(i.first);
//                
//                url_node.appendChild(url_text);
//                url_node.setAttribute("post" , i.second);
//                
//                root.appendChild(url_node);
//            }
//        }
//        else
//        {
//            QDomElement url_node  = document.createElement("url");
//            QDomText    url_text  = document.createTextNode(ui->textEdit_3->toPlainText());
//            
//            url_node.appendChild(url_text);
//            url_node.setAttribute("post", ui->textEdit_4->toPlainText());
//            
//            root.appendChild(url_node);
//        }
//        
//        
//        QFile file( filename );
//        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text))
//            return ;
//        
//        QTextStream out(&file);
//        out.setCodec("UTF-8");
//        document.save( out , 4 , QDomNode::EncodingFromTextStream);
//        
//        file.close();
//    }
//}

void MainWindow::on_pushButton_5_clicked(bool checked)
{
    m_profile.isValid = false ;
}

