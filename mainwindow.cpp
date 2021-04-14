#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QChart>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QValueAxis>
#include <QBarCategoryAxis>

#include <QtWidgets/QApplication>
#include <QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_agenciesList(generateAgenciesList()),
      m_bankersList(generateBankersList()),
      m_clientsList(generateClientsList()),
      m_productsTable(generateProductsTable()),
      m_dataTable(generateRandomData()),
      m_barSets(generateBarSets(m_dataTable,0)),
      m_barSeries(generateBarSeries()),
      m_percentBarSeries(generatePercentBarSeries()),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->InsightComboBox->setCurrentIndex(0);
    connectSignals();

    QChart *chart1, *chart2, *chart3;
    int max = maxValue();

    chart1 = createChart(m_barSeries[0],"Produits bancaires", max);
    chart2 = createChart(m_barSeries[1],"Produits d'assurance", max);
    chart3 = createChart(m_barSeries[2],"Produits boursiers", max);

    m_charts << chart1 << chart2 << chart3;

    ui->PBanquierChart->setChart(m_charts[0]);
    ui->PAssuranceChart->setChart(m_charts[1]);
    ui->PBoursierChart->setChart(m_charts[2]);

    updateUI();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::nextPage()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::updateUI()
{
    DataTable dataTableAgency;
    DataTable dataTableDate;

    QDate fromDate = ui->fromDateEdit->date();
    QDate toDate = ui->toDateEdit->date();

    for(int i(0); i < m_dataTable.size(); i++){
        for(int j(0); j < m_dataTable[i].sales.size(); j++){
            if(fromDate.operator<=(m_dataTable[i].sales[j].date) && toDate.operator>=(m_dataTable[i].sales[j].date))
            {
                dataTableDate << m_dataTable[i];
            }
        }
    }

    int agency = ui->agenciesComboBox->currentIndex();
    if(agency == 0){
        dataTableAgency = dataTableDate;
    }
    else
    {
        for(int i(0); i < dataTableDate.size(); i++)
        {
            if(dataTableDate[i].agency == ui->agenciesComboBox->currentText())
            {
                dataTableAgency << dataTableDate[i];
            }
        }
    }

    int insight = ui->InsightComboBox->currentIndex();
    if(insight == 0)
    {
        m_barSets = generateBarSets(dataTableAgency,0);
        m_barSeries = generateBarSeries();
        int max = maxValue();
        m_charts[0] = createChart(m_barSeries[0],"Produits bancaires", max);
        m_charts[1] = createChart(m_barSeries[1],"Produits d'assurance", max);
        m_charts[2] = createChart(m_barSeries[2],"Produits boursiers", max);
    }
    else if(insight == 1)
    {
        m_barSets = generateBarSets(dataTableAgency,1);
        m_barSeries = generateBarSeries();
        int max = maxValue();
        m_charts[0] = createChart(m_barSeries[0],"Produits bancaires", max);
        m_charts[1] = createChart(m_barSeries[1],"Produits d'assurance", max);
        m_charts[2] = createChart(m_barSeries[2],"Produits boursiers", max);

    }
    else
    {
        m_barSets = generateBarSets(dataTableAgency,2);
        m_percentBarSeries = generatePercentBarSeries();
        m_charts[0] = createPercentChart(m_percentBarSeries[0],"Produits bancaires");
        m_charts[1] = createPercentChart(m_percentBarSeries[1],"Produits d'assurance");
        m_charts[2] = createPercentChart(m_percentBarSeries[2],"Produits boursiers");
    }

    ui->PBanquierChart->setChart(m_charts[0]);
    ui->PAssuranceChart->setChart(m_charts[1]);
    ui->PBoursierChart->setChart(m_charts[2]);

    QPalette pal = window()->palette();
    bool themeChecked = ui->themeCheckBox->isChecked();
    if(themeChecked == true)
    {
        pal.setColor(QPalette::Window, QRgb(0x121218));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        for(int i(0); i< m_charts.size(); i++)
        {
            m_charts[i]->setTheme(QChart::ChartThemeDark);
        }
    }
    else
    {
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        for(int i(0); i< m_charts.size(); i++)
        {
            m_charts[i]->setTheme(QChart::ChartThemeLight);
        }
    }

    this->setPalette(pal);

}

void MainWindow::connectSignals()
{
    connect(ui->themeCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(updateUI()));
    connect(ui->InsightComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateUI()));
    connect(ui->fromDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(updateUI()));
    connect(ui->toDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(updateUI()));
    connect(ui->agenciesComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateUI()));
}

int MainWindow::maxValue() const
{
    int max = 0;
    for (int k(0); k < m_barSets.size(); k++){
        if (m_barSets[k]->at(0) > max){
            max = m_barSets[k]->at(0);
        }

    }
    return max;
}

QChart *MainWindow::createChart(QBarSeries *series, QString title, int max) const
{
    QChart *chart = new QChart();
    chart->setTitle(title);


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
    connect(series, SIGNAL(clicked()),
            this, SLOT(nextPage()));

    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setAnimationDuration(3000);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    return chart;
}

QChart *MainWindow::createPercentChart(QPercentBarSeries *series, QString title) const
{
    QChart *chart = new QChart();
    chart->setTitle(title);


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


QBarSet *MainWindow::fillSetVolumes(DataTable data,QString setName) const
{
    QBarSet *barSet = new QBarSet(setName);
    int count = 0;
    for(int i(0); i < data.size(); i++){
        for(int j(0); j< data[i].sales.size(); j++){
            if(data[i].sales[j].product.family == setName)
            {
                count++;
            }
        }
    }
    *barSet << count;
    return barSet;
}

QBarSet *MainWindow::fillSetCA(DataTable data,QString setName) const
{

    QBarSet *barSet = new QBarSet(setName);
    int count = 0;
    for(int i(0); i < data.size(); i++){
        for(int j(0); j < data[i].sales.size(); j++){
            if(data[i].sales[j].product.family == setName){
                count+= data[i].sales[j].product.benefice;
            }
        }
    }
    *barSet << count;
    return barSet;
}

QList<QBarSet *> MainWindow::generateBarSets(DataTable data, int insight) const
{
    QList<QBarSet *> barSetList;
    QBarSet *prets, *comptes, *chequiers, *cartes, *individuelle, *collective, *action;

    if(insight == 0){
        prets = fillSetVolumes(data,"Prêts");
        comptes = fillSetVolumes(data,"Comptes");
        chequiers = fillSetVolumes(data,"Chéquiers");
        cartes = fillSetVolumes(data,"Cartes");
        individuelle = fillSetVolumes(data,"Individuelle");
        collective = fillSetVolumes(data,"Collective");
        action = fillSetVolumes(data,"Action");
    }
    else
    {
        prets = fillSetCA(data,"Prêts");
        comptes = fillSetCA(data,"Comptes");
        chequiers = fillSetCA(data,"Chéquiers");
        cartes = fillSetCA(data,"Cartes");
        individuelle = fillSetCA(data,"Individuelle");
        collective = fillSetCA(data,"Collective");
        action = fillSetCA(data,"Action");
    }

    barSetList << prets << comptes << chequiers << cartes << individuelle << collective << action;
    return barSetList;

}

QList<QBarSeries *> MainWindow::generateBarSeries() const
{
    QList<QBarSeries *> barSeries;
    QBarSeries *serie1 = new QBarSeries();
    QBarSeries *serie2 = new QBarSeries();
    QBarSeries *serie3 = new QBarSeries();


    serie1->append(m_barSets[0]);
    serie1->append(m_barSets[1]);
    serie1->append(m_barSets[2]);
    serie1->append(m_barSets[3]);
    serie2->append(m_barSets[4]);
    serie2->append(m_barSets[5]);
    serie3->append(m_barSets[6]);

    barSeries << serie1 << serie2 << serie3;
    return barSeries;
}

QList<QPercentBarSeries *> MainWindow::generatePercentBarSeries() const
{
    QList<QPercentBarSeries *> barSeries;
    QPercentBarSeries *serie1 = new QPercentBarSeries();
    QPercentBarSeries *serie2 = new QPercentBarSeries();
    QPercentBarSeries *serie3 = new QPercentBarSeries();


    serie1->append(m_barSets[0]);
    serie1->append(m_barSets[1]);
    serie1->append(m_barSets[2]);
    serie1->append(m_barSets[3]);
    serie2->append(m_barSets[4]);
    serie2->append(m_barSets[5]);
    serie3->append(m_barSets[6]);

    barSeries << serie1 << serie2 << serie3;
    return barSeries;
}

DataTable MainWindow::generateRandomData() const
{
    DataTable dataTable;

    BankersList bankersListCopy = m_bankersList;
    for (int i(0); i < m_agenciesList.size(); i ++){
        for (int j(0); j < 20; j++){
            DataList dataList;
            dataList.agency = m_agenciesList[i];
            dataList.banker = bankersListCopy[QRandomGenerator::global()->bounded(bankersListCopy.size())];

            for (int y(2015); y < 2022; y++){
                if (y == 2021){
                    for (int k(0); k < QRandomGenerator::global()->bounded(10,30); k++)
                    {
                        Sale sale;
                        sale.product = m_productsTable[QRandomGenerator::global()->bounded(m_productsTable.size())];
                        sale.client = m_clientsList[QRandomGenerator::global()->bounded(m_clientsList.size())];
                        sale.date.setDate(y,QRandomGenerator::global()->bounded(1,4),QRandomGenerator::global()->bounded(1,16));
                        dataList.sales << sale;
                    }
                }
                else {
                    for (int k(0); k < QRandomGenerator::global()->bounded(100,200); k++){
                        Sale sale;
                        sale.product = m_productsTable[QRandomGenerator::global()->bounded(m_productsTable.size())];
                        sale.client = m_clientsList[QRandomGenerator::global()->bounded(m_clientsList.size())];
                        sale.date.setDate(QRandomGenerator::global()->bounded(y),QRandomGenerator::global()->bounded(1,12),QRandomGenerator::global()->bounded(1,28));
                        dataList.sales << sale;
                    }
                }
            }
            bankersListCopy.removeAt(bankersListCopy.indexOf(dataList.banker));
            dataTable << dataList;
        }
    }

    return dataTable;
}

ProductsTable MainWindow::generateProductsTable() const {

    ProductsTable productsTable;

    Product pret_habitation;
    Product pret_auto;
    Product compte_courant;
    Product compte_epargne;
    Product chequier;
    Product carte_bancaire;
    Product carte_credit;
    Product assurance_auto;
    Product assurance_ordi;
    Product assurance_habitation;
    Product action_LaBanque;

    pret_habitation.channel = "Banque";
    pret_habitation.family = "Prêts";
    pret_habitation.version = "Habitation";
    pret_habitation.price = 1000;
    pret_habitation.duration = 36;
    pret_habitation.benefice = 100;

    pret_auto.channel = "Banque";
    pret_auto.family = "Prêts";
    pret_auto.version = "Auto";
    pret_auto.price = 200;
    pret_auto.duration = 36;
    pret_auto.benefice = 50;

    compte_courant.channel = "Banque";
    compte_courant.family = "Comptes";
    compte_courant.version = "Courant";
    compte_courant.price = 8.99;
    compte_courant.duration = 12;
    compte_courant.benefice = 107.88;

    compte_epargne.channel = "Banque";
    compte_epargne.family = "Comptes";
    compte_epargne.version = "Épargne";
    compte_epargne.price = 5.99;
    compte_epargne.duration = 24;
    compte_epargne.benefice = 71.88;

    chequier.channel = "Banque";
    chequier.family = "Chéquiers";
    chequier.version = "Chéquier";
    chequier.price = 10;
    chequier.duration = 12;
    chequier.benefice = 120;

    carte_credit.channel = "Banque";
    carte_credit.family = "Cartes";
    carte_credit.version = "Crédit";
    carte_credit.price = 10;
    carte_credit.duration = 36;
    carte_credit.benefice = 120;

    carte_bancaire.channel = "Banque";
    carte_bancaire.family = "Cartes";
    carte_bancaire.version = "Bancaire";
    carte_bancaire.price = 10;
    carte_bancaire.duration = 36;
    carte_bancaire.benefice = 120;

    assurance_auto.channel = "Assurance";
    assurance_auto.family = "Individuelle";
    assurance_auto.version = "Auto";
    assurance_auto.price = 144;
    assurance_auto.duration = 12;
    assurance_auto.benefice = 144;

    assurance_habitation.channel = "Assurance";
    assurance_habitation.family = "Collective";
    assurance_habitation.version = "Habitation";
    assurance_habitation.price = 120;
    assurance_habitation.duration = 12;
    assurance_habitation.benefice = 120;

    assurance_ordi.channel = "Assurance";
    assurance_ordi.family = "Individuelle";
    assurance_ordi.version = "Ordinateur";
    assurance_ordi.price = 71.88;
    assurance_ordi.duration = 12;
    assurance_ordi.benefice = 71.88;

    action_LaBanque.channel = "Bourse";
    action_LaBanque.family = "Action";
    action_LaBanque.version = "LaBanque";
    action_LaBanque.price = 62;
    action_LaBanque.duration = 12;
    action_LaBanque.benefice = 150;

    productsTable << pret_auto << pret_habitation << chequier << carte_credit << carte_bancaire << compte_courant << compte_epargne << assurance_auto << assurance_ordi << action_LaBanque << assurance_habitation;

    return productsTable;
}

BankersList MainWindow::generateBankersList() const
{
    BankersList bankersList;
    bankersList <<
                   "Megan Vance" <<
                   "Declan Martinez" <<
                   "Jonas Sawyer" <<
                   "Nash Robertson" <<
                   "Kevin Lee" <<
                   "Conan Weeks" <<
                   "Gabriel Patrick" <<
                   "Mohammad Carey" <<
                   "Evangeline Riggs" <<
                   "August Hahn" <<
                   "Malachi Caldwell" <<
                   "Matthew Dale" <<
                   "Gregory Velez" <<
                   "Sophia Head" <<
                   "Dillon Bauer" <<
                   "Kaitlin Jenkins" <<
                   "Phoebe Nieves" <<
                   "Mark Hale" <<
                   "Edward Hodges" <<
                   "Allistair Goodwin" <<
                   "MacKensie Curry" <<
                   "Jeremy Terry" <<
                   "Patricia Conrad" <<
                   "Sigourney Rasmussen" <<
                   "Hamilton Pitts" <<
                   "Yen Fischer" <<
                   "Cruz Hess" <<
                   "April Hoover" <<
                   "Kitra Santos" <<
                   "Beatrice Franklin" <<
                   "Andrew Washington" <<
                   "Colton Duncan" <<
                   "Heidi Huffman" <<
                   "Irene Reed" <<
                   "Barclay Dotson" <<
                   "Yoshio Byers" <<
                   "Austin Vasquez" <<
                   "Bert King" <<
                   "Hedda Riggs" <<
                   "Xyla Waters" <<
                   "Desiree Little" <<
                   "Miranda Deleon" <<
                   "Ferdinand Wells" <<
                   "Tallulah Hutchinson" <<
                   "Riley Travis" <<
                   "Ian Burnett" <<
                   "Brynne Hall" <<
                   "Zenaida Hatfield" <<
                   "Lamar Raymond" <<
                   "Diana Hunter" <<
                   "Adrian Baird" <<
                   "McKenzie Mccarty" <<
                   "Travis Mccarty" <<
                   "Leandra Hopper" <<
                   "Buckminster Strickland" <<
                   "Jack Orr" <<
                   "Ora Whitehead" <<
                   "Medge Ochoa" <<
                   "Lee Beasley" <<
                   "Marvin Noel" <<
                   "Jenna Carroll" <<
                   "Ruby Manning" <<
                   "Karly Alston" <<
                   "Eliana Neal" <<
                   "Zephania Adams" <<
                   "Nyssa Mcknight" <<
                   "Norman Moran" <<
                   "Callie Keith" <<
                   "Nyssa Clay" <<
                   "Tanner Dixon" <<
                   "Daniel Mckee" <<
                   "Jasmine Raymond" <<
                   "Debra Wall" <<
                   "Clark Mcbride" <<
                   "Madaline May" <<
                   "Adele Rivers" <<
                   "Nomlanga Knox" <<
                   "Chastity Woods" <<
                   "Cooper Wyatt" <<
                   "Chester Wynn" <<
                   "Danielle Foreman" <<
                   "Cadman Koch" <<
                   "Linus Rice" <<
                   "Brian Villarreal" <<
                   "Jarrod Holland" <<
                   "Kyra Peterson" <<
                   "Fiona Foster" <<
                   "Shelley Waters" <<
                   "Hop Workman" <<
                   "Ursa Lucas" <<
                   "Chancellor Reid" <<
                   "Shoshana Kramer" <<
                   "Rigel Townsend" <<
                   "Amaya Walls" <<
                   "Hanae Woods" <<
                   "Edward Levy" <<
                   "Sarah Carr" <<
                   "Murphy Gray" <<
                   "Harlan Blevins" <<
                   "Yoko Little";

    return bankersList;
}

ClientsList MainWindow::generateClientsList() const
{
    ClientsList clientsList;
    clientsList <<
                   "Ingrid Camacho"
                <<
                   "Nayda Rodriguez"
                <<
                   "Cadman Cohen"
                <<
                   "Audra Aguilar"
                <<
                   "Ethan Glenn"
                <<
                   "Herrod Blankenship"
                <<
                   "Keaton Fernandez"
                <<
                   "Carissa Ferrell"
                <<
                   "Jakeem Walls"
                <<
                   "Leandra Baldwin"
                <<
                   "Demetrius Ellis"
                <<
                   "Vernon Mcgowan"
                <<
                   "Knox Glenn"
                <<
                   "Lester Keller"
                <<
                   "Dominic Morton"
                <<
                   "Naida Patrick"
                <<
                   "Isaiah Parsons"
                <<
                   "Alexandra Vargas"
                <<
                   "Kirby Young"
                <<
                   "George Jefferson"
                <<
                   "Graham Schwartz"
                <<
                   "Natalie Holmes"
                <<
                   "Alfreda Ware"
                <<
                   "Garrett Soto"
                <<
                   "Gannon Foreman"
                <<
                   "Amber Nixon"
                <<
                   "Brent Hutchinson"
                <<
                   "Megan Ortiz"
                <<
                   "Remedios Hayden"
                <<
                   "Garrett Allison"
                <<
                   "Austin Paul"
                <<
                   "Matthew Kim"
                <<
                   "Macon Gross"
                <<
                   "Anthony Mills"
                <<
                   "Brooke Lott"
                <<
                   "Amela Mcintyre"
                <<
                   "Wade Williams"
                <<
                   "Illana Mcintosh"
                <<
                   "Kathleen Snyder"
                <<
                   "Harper Knowles"
                <<
                   "Eric Leach"
                <<
                   "Wilma Black"
                <<
                   "Nadine Mcintyre"
                <<
                   "Ezekiel Stuart"
                <<
                   "Sydney Booker"
                <<
                   "Dean Berry"
                <<
                   "Curran Mcbride"
                <<
                   "Francis Camacho"
                <<
                   "Kaye Jacobs"
                <<
                   "Rosalyn Mills"
                <<
                   "Inga Montoya"
                <<
                   "Abraham Garza"
                <<
                   "Nathan Richardson"
                <<
                   "Oscar Gillespie"
                <<
                   "Glenna Rodgers"
                <<
                   "Theodore Perry"
                <<
                   "Garth Crawford"
                <<
                   "Dolan Dorsey"
                <<
                   "Nyssa Parks"
                <<
                   "Melanie Meadows"
                <<
                   "Conan Fitzgerald"
                <<
                   "Omar Love"
                <<
                   "Ima Steele"
                <<
                   "Alyssa Merritt"
                <<
                   "Tatyana Christian"
                <<
                   "Xyla Callahan"
                <<
                   "Sylvester Medina"
                <<
                   "Brady Sharpe"
                <<
                   "Serina Gamble"
                <<
                   "Quentin Todd"
                <<
                   "Norman Jacobson"
                <<
                   "Bernard Bean"
                <<
                   "Hyacinth Floyd"
                <<
                   "Cassidy Harper"
                <<
                   "Maya Peterson"
                <<
                   "Aurora Mcgowan"
                <<
                   "Aladdin Noel"
                <<
                   "Uma Petty"
                <<
                   "Yetta Hyde"
                <<
                   "Zorita Bell"
                <<
                   "Nola Cochran"
                <<
                   "Dolan Becker"
                <<
                   "Alea Castaneda"
                <<
                   "Rose Burgess"
                <<
                   "Russell Underwood"
                <<
                   "Renee Clark"
                <<
                   "Andrew Reyes"
                <<
                   "Jada Morgan"
                <<
                   "Hammett Perkins"
                <<
                   "Dale Atkinson"
                <<
                   "Eve Malone"
                <<
                   "Leonard Britt"
                <<
                   "Tad Fitzgerald"
                <<
                   "Jenette Battle"
                <<
                   "Palmer Crane"
                <<
                   "Jerome Savage"
                <<
                   "Brynn Wiley"
                <<
                   "Quinlan Rush"
                <<
                   "Derek Spencer"
                <<
                   "Jordan Pacheco";

    return clientsList;
}

AgenciesList MainWindow::generateAgenciesList() const
{
    AgenciesList agenciesList;
    agenciesList <<
                    "Agence de Strasbourg" <<
                    "Agence de Paris" <<
                    "Agence de Lille" <<
                    "Agence de Nice" <<
                    "Agence de Lyon";

    return agenciesList;
}
