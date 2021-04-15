#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartGlobal>
#include <QBarSeries>
#include <QBarSet>
#include <QDate>
#include <QPercentBarSeries>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

typedef QList<QString> BankersList;
typedef QList<QString> AgenciesList;
typedef QList<QString> ClientsList;

typedef struct Product{
    QString channel;
    QString family;
    QString version;
    qreal price;
    qreal duration;
    qreal benefice;
}Product;

typedef struct Sale{
    Product product;
    QString client;
    QDate date;
}Sale;

typedef struct DataList{
    QString agency;
    QString banker;
    QList<Sale> sales;
}Datalist;

typedef QList<DataList> DataTable;
typedef QList<Product> ProductsTable;

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
    void updateUI();
    void updateTheme();
    void nextPage();
    void previousPage();

private:
    BankersList generateBankersList() const;
    AgenciesList generateAgenciesList() const;
    ClientsList generateClientsList() const;
    DataTable generateRandomData() const;
    ProductsTable generateProductsTable() const;
    void fillTable(DataTable data);
    QChart* createChart(QBarSeries *series, int max) const;
    QChart* createPercentChart(QPercentBarSeries *series) const;
    void generateBarSeries();
    void generatePercentBarSeries();
    void generateBarSets(DataTable data,int insight);
    int maxValue();
    void connectSignals();

private:
    AgenciesList m_agenciesList;
    BankersList m_bankersList;
    ClientsList m_clientsList;
    ProductsTable m_productsTable;
    DataTable m_dataTable;
    DataTable m_curr_dataTable;
    QList<QChart *> m_charts;
    QList<QBarSet *> m_barSets;
    QList<QBarSeries *> m_barSeries;
    QList<QPercentBarSeries *> m_percentBarSeries;
    QTableWidget * m_table;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
