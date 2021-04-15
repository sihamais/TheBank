#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QChart>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QDebug>

#include <QtWidgets/QApplication>
#include <QGridLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtCore/QRandomGenerator>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_agenciesList(generateAgenciesList()),
      m_bankersList(generateBankersList()),
      m_clientsList(generateClientsList()),
      m_productsTable(generateProductsTable()),
      m_dataTable(generateRandomData()),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->InsightComboBox->setCurrentIndex(0);
    ui->backButton->hide();
    ui->horizontalSpacer_8->changeSize(40,20,QSizePolicy::Fixed,QSizePolicy::Fixed);
    connectSignals();

    QChart *chart1, *chart2, *chart3;
    int max = maxValue();

    generateBarSets(m_dataTable, 0);
    generateBarSeries();

    chart1 = createChart(m_barSeries[0], max);
    chart2 = createChart(m_barSeries[1], max);
    chart3 = createChart(m_barSeries[2], max);

    m_charts << chart1 << chart2 << chart3;

    ui->PBancaireChart->setChart(m_charts[0]);
    ui->PAssuranceChart->setChart(m_charts[1]);
    ui->PBoursierChart->setChart(m_charts[2]);

    m_table = ui->tableWidget;
    m_table->setColumnCount(4);
    QStringList hlabels;
    hlabels << "Nom" << "Agence" << "Volume d'affaire" << "Prospects";
    m_table->setHorizontalHeaderLabels(hlabels);
    fillTable(m_dataTable);
    m_table->setSortingEnabled(true);
    m_table->sortByColumn(3,Qt::DescendingOrder);

    updateUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::nextPage()
{
    int curr = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(curr+1);
    ui->backButton->show();
    ui->horizontalSpacer_8->changeSize(40,20,QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
}

void MainWindow::previousPage()
{
    int curr = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(curr-1);
    ui->backButton->hide();
    ui->horizontalSpacer_8->changeSize(40,20,QSizePolicy::Preferred,QSizePolicy::Preferred);
}

void MainWindow::updateUI()
{
    DataTable data;
    QDate fromDate = ui->fromDateEdit->date();
    QDate toDate = ui->toDateEdit->date();
    QString agency = ui->agenciesComboBox->currentText();

    if(agency == "Toutes les agences"){
        for(int i(0); i < m_dataTable.size(); i++){
            for(int j(0); j < m_dataTable[i].sales.size(); j++){
                if(fromDate.operator<=(m_dataTable[i].sales[j].date) && toDate.operator>=(m_dataTable[i].sales[j].date)){
                    data << m_dataTable[i];
                }
            }
        }
    }
    else{
        for(int i(0); i < m_dataTable.size(); i++){
            if(m_dataTable[i].agency == agency){
                for(int j(0); j < m_dataTable[i].sales.size(); j++){
                    if(fromDate.operator<=(m_dataTable[i].sales[j].date) && toDate.operator>=(m_dataTable[i].sales[j].date)){
                        data << m_dataTable[i];
                    }
                }
            }
        }
    }


    int insight = ui->InsightComboBox->currentIndex();
    if(insight == 2){
        generateBarSets(data,2);
        generatePercentBarSeries();
        m_charts[0] = createPercentChart(m_percentBarSeries[0]);
        m_charts[1] = createPercentChart(m_percentBarSeries[1]);
        m_charts[2] = createPercentChart(m_percentBarSeries[2]);
    }
    else{
        if(insight == 0){
            generateBarSets(data,0);
        }
        else{
            generateBarSets(data,1);
        }
        generateBarSeries();
        int max = maxValue();
        m_charts[0] = createChart(m_barSeries[0],max);
        m_charts[1] = createChart(m_barSeries[1],max);
        m_charts[2] = createChart(m_barSeries[2],max);
    }

    m_table->setSortingEnabled(false);
    fillTable(data);
    m_table->setSortingEnabled(true);

    updateTheme();

    ui->PBancaireChart->setChart(m_charts[0]);
    ui->PAssuranceChart->setChart(m_charts[1]);
    ui->PBoursierChart->setChart(m_charts[2]);

}

QChart* MainWindow::createPercentChart(QPercentBarSeries *series) const
{
    QChart *chart = new QChart();
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis();
    axisY->applyNiceNumbers();
    chart->addSeries(series);
    axisX->append("");
    chart->addAxis(axisX, Qt::AlignTop);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setAnimationDuration(3000);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    return chart;
}

void MainWindow::updateTheme(){
    QPalette pal = window()->palette();
    bool themeChecked = ui->themeCheckBox->isChecked();
    if(themeChecked == true){
        pal.setColor(QPalette::Window, QRgb(0x121218));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        for(int i(0); i< m_charts.size(); i++){
            m_charts[i]->setTheme(QChart::ChartThemeDark);
        }
    }
    else{
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        for(int i(0); i< m_charts.size(); i++){
            m_charts[i]->setTheme(QChart::ChartThemeLight);
        }
    }

    this->setPalette(pal);
}

void MainWindow::connectSignals()
{
    connect(ui->themeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateTheme()));
    connect(ui->InsightComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateUI()));
    connect(ui->fromDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(updateUI()));
    connect(ui->toDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(updateUI()));
    connect(ui->agenciesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateUI()));
    connect(ui->PbanquierBtn, SIGNAL(clicked()), this, SLOT(nextPage()));
    connect(ui->backButton, SIGNAL(clicked()), this, SLOT(previousPage()));
}

int MainWindow::maxValue()
{
    int max = 0;
    for (int k(0); k < m_barSets.size(); k++){
        if (m_barSets[k]->at(0) > max){
            max = m_barSets[k]->at(0);
        }
    }
    return max;
}

void MainWindow::fillTable(DataTable data)
{
    int rowCount;
    QList<QTableWidgetItem *> curr;
    for(int i(0); i<data.size(); i++){
        for(int j(0); j<data[i].sales.size(); j++){
            if(data[i].sales[j].product.channel == "Banque"){
                QTableWidgetItem * name = new QTableWidgetItem();
                QTableWidgetItem * agency = new QTableWidgetItem();
                QTableWidgetItem * turnover = new QTableWidgetItem();
                QTableWidgetItem * prospects = new QTableWidgetItem();
                curr = m_table->findItems(data[i].banker,Qt::MatchFixedString);
                if(curr.size() > 0){
                    rowCount = curr[0]->row();
                    turnover->setText(QString::number(m_table->itemAt(rowCount,2)->text().toInt()+data[i].sales[j].product.benefice));
                    m_table->setItem(rowCount,2,turnover);
                    prospects->setText(QString::number(m_table->itemAt(rowCount,3)->text().toInt()+1));
                    m_table->setItem(rowCount,3,prospects);
                }
                else{
                    rowCount = m_table->rowCount();
                    m_table->insertRow(rowCount);
                    name->setText(data[i].banker);
                    m_table->setItem(rowCount, 0, name);
                    agency->setText(data[i].agency);
                    m_table->setItem(rowCount, 1, agency);
                    turnover->setText(QString::number(data[i].sales[j].product.benefice));
                    m_table->setItem(rowCount, 2, turnover);
                    prospects->setText(QString::number(1));
                    m_table->setItem(rowCount, 3, prospects);
                }
            }
        }
    }
}

QChart *MainWindow::createChart(QBarSeries *series, int max) const
{
    QChart *chart = new QChart();
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, max);
    axisY->applyNiceNumbers();
    chart->addSeries(series);
    axisX->append("");
    chart->addAxis(axisX, Qt::AlignTop);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setAnimationDuration(3000);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    return chart;
}

void MainWindow::generateBarSets(DataTable data, int insight)
{
    QList<QBarSet *> barSets;
    QBarSet *prets= new QBarSet("Prêts");
    QBarSet *comptes= new QBarSet("Comptes");
    QBarSet *chequiers= new QBarSet("Chéquiers");
    QBarSet *cartes= new QBarSet("Cartes");
    QBarSet *individuelle = new QBarSet("Individuelle");
    QBarSet *collective= new QBarSet("Collective");
    QBarSet *action= new QBarSet("Action");

    double pretsCount = 0, comptesCount = 0, chequiersCount = 0, cartesCount = 0, indiviCount = 0, collectiCount = 0, actionCount = 0;

    if(insight == 0){
        for(int i(0); i < data.size(); i++){
            for(int j(0); j < data[i].sales.size(); j++){
                if(data[i].sales[j].product.family == "Prêts")
                    pretsCount++;
                else if(data[i].sales[j].product.family == "Comptes")
                    comptesCount++;
                else if(data[i].sales[j].product.family == "Chéquiers")
                    chequiersCount++;
                else if(data[i].sales[j].product.family == "Cartes")
                    cartesCount++;
                else if(data[i].sales[j].product.family == "Individuelle")
                    indiviCount++;
                else if(data[i].sales[j].product.family == "Collective")
                    collectiCount++;
                else
                    actionCount++;
            }
        }
    }
    else{
        for(int i(0); i < data.size(); i++){
            for(int j(0); j < data[i].sales.size(); j++){
                if(data[i].sales[j].product.family == "Prêts")
                    pretsCount += data[i].sales[j].product.benefice;
                else if(data[i].sales[j].product.family == "Comptes")
                    comptesCount += data[i].sales[j].product.benefice;
                else if(data[i].sales[j].product.family == "Chéquiers")
                    chequiersCount += data[i].sales[j].product.benefice;
                else if(data[i].sales[j].product.family == "Cartes")
                    cartesCount += data[i].sales[j].product.benefice;
                else if(data[i].sales[j].product.family == "Individuelle")
                    indiviCount += data[i].sales[j].product.benefice;
                else if(data[i].sales[j].product.family == "Collective")
                    collectiCount += data[i].sales[j].product.benefice;
                else
                    actionCount += data[i].sales[j].product.benefice;
            }
        }
    }
    prets->append(pretsCount);
    comptes->append(comptesCount);
    chequiers->append(chequiersCount);
    cartes->append(cartesCount);
    individuelle->append(indiviCount);
    collective->append(collectiCount);
    action->append(actionCount);
    barSets << prets << comptes << chequiers << cartes << individuelle << collective << action;
    m_barSets = barSets;
}

void MainWindow::generateBarSeries()
{
    QList<QBarSeries *> series;
    QBarSeries * pbancaire = new QBarSeries();
    QBarSeries * passurance = new QBarSeries();
    QBarSeries * pboursier = new QBarSeries();

    pbancaire->append(m_barSets[0]);
    pbancaire->append(m_barSets[1]);
    pbancaire->append(m_barSets[2]);
    pbancaire->append(m_barSets[3]);
    passurance->append(m_barSets[4]);
    passurance->append(m_barSets[5]);
    pboursier->append(m_barSets[6]);

    series << pbancaire << passurance << pboursier;
    m_barSeries = series;
}

void MainWindow::generatePercentBarSeries()
{
    QList<QPercentBarSeries *> series;
    QPercentBarSeries *pbancaire = new QPercentBarSeries();
    QPercentBarSeries *passurance = new QPercentBarSeries();
    QPercentBarSeries *pboursier = new QPercentBarSeries();

    pbancaire->append(m_barSets[0]);
    pbancaire->append(m_barSets[1]);
    pbancaire->append(m_barSets[2]);
    pbancaire->append(m_barSets[3]);
    passurance->append(m_barSets[4]);
    passurance->append(m_barSets[5]);
    pboursier->append(m_barSets[6]);

    series << pbancaire << passurance << pboursier;
    m_percentBarSeries = series;
}

DataTable MainWindow::generateRandomData() const
{
    DataTable dataTable;

    BankersList bankersListCopy = m_bankersList;
    for (int i(0); i < m_agenciesList.size(); i ++){
        for (int j(0); j < 5; j++){
            DataList dataList;
            dataList.agency = m_agenciesList[i];
            int bankerIdx = QRandomGenerator::global()->bounded(bankersListCopy.size());
            dataList.banker = bankersListCopy[bankerIdx];
            for (int k(0); k < QRandomGenerator::global()->bounded(10,30); k++){
                Sale sale;
                sale.product = m_productsTable[QRandomGenerator::global()->bounded(m_productsTable.size())];
                sale.client = m_clientsList[QRandomGenerator::global()->bounded(m_clientsList.size())];
                sale.date.setDate(2021,QRandomGenerator::global()->bounded(1,4),QRandomGenerator::global()->bounded(1,30));
                dataList.sales << sale;
            }
            bankersListCopy.removeAt(bankerIdx);
            dataTable << dataList;
        }
    }
    return dataTable;
}

ProductsTable MainWindow::generateProductsTable() const {

    ProductsTable productsTable;
    Product pret_habitation, pret_auto, compte_courant, compte_epargne, chequier, carte_bancaire, carte_credit, assurance_auto, assurance_ordi, assurance_habitation, action_LaBanque;

    pret_habitation.channel = "Banque"; pret_habitation.family = "Prêts"; pret_habitation.version = "Habitation"; pret_habitation.price = 1000; pret_habitation.duration = 36; pret_habitation.benefice = 100;
    pret_auto.channel = "Banque"; pret_auto.family = "Prêts"; pret_auto.version = "Auto"; pret_auto.price = 200; pret_auto.duration = 36; pret_auto.benefice = 50;
    compte_courant.channel = "Banque"; compte_courant.family = "Comptes"; compte_courant.version = "Courant"; compte_courant.price = 8.99; compte_courant.duration = 12; compte_courant.benefice = 107.88;
    compte_epargne.channel = "Banque"; compte_epargne.family = "Comptes"; compte_epargne.version = "Épargne"; compte_epargne.price = 5.99; compte_epargne.duration = 24; compte_epargne.benefice = 71.88;
    chequier.channel = "Banque"; chequier.family = "Chéquiers"; chequier.version = "Chéquier"; chequier.price = 10; chequier.duration = 12; chequier.benefice = 120;
    carte_credit.channel = "Banque"; carte_credit.family = "Cartes"; carte_credit.version = "Crédit"; carte_credit.price = 10; carte_credit.duration = 36; carte_credit.benefice = 120;
    carte_bancaire.channel = "Banque"; carte_bancaire.family = "Cartes"; carte_bancaire.version = "Bancaire"; carte_bancaire.price = 10; carte_bancaire.duration = 36; carte_bancaire.benefice = 120;
    assurance_auto.channel = "Assurance"; assurance_auto.family = "Individuelle"; assurance_auto.version = "Auto"; assurance_auto.price = 144; assurance_auto.duration = 12; assurance_auto.benefice = 144;
    assurance_habitation.channel = "Assurance"; assurance_habitation.family = "Collective"; assurance_habitation.version = "Habitation"; assurance_habitation.price = 120; assurance_habitation.duration = 12; assurance_habitation.benefice = 120;
    assurance_ordi.channel = "Assurance"; assurance_ordi.family = "Individuelle"; assurance_ordi.version = "Ordinateur"; assurance_ordi.price = 71.88; assurance_ordi.duration = 12; assurance_ordi.benefice = 71.88;
    action_LaBanque.channel = "Bourse"; action_LaBanque.family = "Action"; action_LaBanque.version = "LaBanque"; action_LaBanque.price = 62; action_LaBanque.duration = 12; action_LaBanque.benefice = 150;

    productsTable << pret_auto << pret_habitation << chequier << carte_credit << carte_bancaire << compte_courant << compte_epargne << assurance_auto << assurance_ordi << action_LaBanque << assurance_habitation;
    return productsTable;
}

BankersList MainWindow::generateBankersList() const
{
    BankersList bankersList;
    bankersList << "Megan Vance" << "Declan Martinez" << "Jonas Sawyer" << "Nash Robertson" << "Kevin Lee" << "Conan Weeks" << "Gabriel Patrick" << "Mohammad Carey" << "Evangeline Riggs" << "August Hahn" << "Malachi Caldwell" << "Matthew Dale" << "Gregory Velez" << "Sophia Head" << "Dillon Bauer" << "Kaitlin Jenkins" << "Phoebe Nieves" << "Mark Hale" << "Edward Hodges" << "Allistair Goodwin" << "MacKensie Curry" << "Jeremy Terry" << "Patricia Conrad" << "Sigourney Rasmussen" << "Hamilton Pitts";
    return bankersList;
}

ClientsList MainWindow::generateClientsList() const
{
    ClientsList clientsList;
    clientsList << "Ingrid Camacho" << "Nayda Rodriguez" << "Cadman Cohen" << "Audra Aguilar" << "Ethan Glenn" << "Herrod Blankenship" << "Keaton Fernandez" << "Carissa Ferrell" << "Jakeem Walls" << "Leandra Baldwin" << "Demetrius Ellis" << "Vernon Mcgowan" << "Knox Glenn" << "Lester Keller" << "Dominic Morton" << "Naida Patrick" << "Isaiah Parsons" << "Alexandra Vargas" << "Kirby Young" << "George Jefferson" << "Graham Schwartz" << "Natalie Holmes" << "Alfreda Ware" << "Garrett Soto" << "Gannon Foreman" << "Amber Nixon";
    return clientsList;
}

AgenciesList MainWindow::generateAgenciesList() const
{
    AgenciesList agenciesList;
    agenciesList << "Agence de Strasbourg" << "Agence de Paris" << "Agence de Lille" << "Agence de Nice" << "Agence de Lyon";
    return agenciesList;
}
