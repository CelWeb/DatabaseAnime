#include "dialogs/settings.h"
#include "ui_settings.h"
#include "definespath.h"

#include "xmldbareader.h"
#include "xmldbawriter.h"

#include <QSettings>
#include <QVariant>

#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>

#include <QtXml>

FormSettings::FormSettings(MngrConnection &MngrCon, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormSettings), restoreDefault(false), MngrConnect(MngrCon)
{
    ui->setupUi(this);
    QSettings settings;
    this->restoreGeometry( settings.value("WindowSettings/Geometry").toByteArray() );
    ui->tabWidget_General->setCurrentIndex(0);
    ui->tabWidget_ImpExp->setCurrentIndex(0);
    ui->LineEdit_ExDir->setText( QStandardPaths::writableLocation( QStandardPaths::HomeLocation ) );
    ui->LineEdit_ImFile->setText( QStandardPaths::writableLocation( QStandardPaths::HomeLocation ) );

    settings.beginGroup("enableSection");
        bool b1 = settings.value( "Anime",   true ).toBool();
        bool b2 = settings.value( "Manga",  false ).toBool();
        bool b3 = settings.value( "AMV",    false ).toBool();
        bool b4 = settings.value( "Dorama", false ).toBool();
    settings.endGroup();

    settings.beginGroup("optionalField");
        settings.beginGroup("anime");
            bool a1 = settings.value( "OrigTitle",   false ).toBool();
            bool a2 = settings.value( "Director",    false ).toBool();
            bool a3 = settings.value( "PostScoring", false ).toBool();
        settings.endGroup();

        settings.beginGroup("manga");
            bool m1 = settings.value( "AltTitle",    false ).toBool();
            bool m2 = settings.value( "Author",      false ).toBool();
            bool m3 = settings.value( "Translation", false ).toBool();
        settings.endGroup();

        settings.beginGroup("dorama");
            bool d1 = settings.value( "AltTitle",   false ).toBool();
            bool d2 = settings.value( "Director",   false ).toBool();
        settings.endGroup();
    settings.endGroup();

    bool checkUpdates = settings.value("General/VerUpdate", false).toBool();

    bool c1 = settings.value( "SwitchToDirOnHoverACover", true ).toBool();

    ui->CheckBox_EnableAnime->setChecked( b1 );
    ui->CheckBox_EnableManga->setChecked( b2 );
    ui->CheckBox_EnableAMV->setChecked( b3 );
    ui->CheckBox_EnableDorama->setChecked( b4 );

    ui->CBox_Anime_AltTitle->setChecked( a1 );
    ui->CBox_Anime_Director->setChecked( a2 );
    ui->CBox_Anime_PostScoring->setChecked( a3 );

    ui->CBox_Manga_AltTitle->setChecked( m1 );
    ui->CBox_Manga_Author->setChecked( m2 );
    ui->CBox_Manga_Translation->setChecked( m3 );

    ui->CBox_Dorama_AltTitle->setChecked( d1 );
    ui->CBox_Dorama_Director->setChecked( d2 );

    ui->ChBox_Update->setChecked( checkUpdates );
    ui->CBox_SwitchToDirOnHoverCover->setChecked( c1 );

    QString set_language = settings.value( "Application/l10n", tr("<System>") ).toString();

    ui->CB_Language->addItem( tr("<System>"), 0 );


    QDir dir( DefinesPath::l10n() );
    QStringList l10n = dir.entryList( QStringList("DatabaseAnime_*.qm") );
    for(int i = 0; i < l10n.count(); ++i){
        ui->CB_Language->addItem( l10n.at(i).section('_', 1, 1).section(".", 0, 0) );
        if( set_language == l10n.at(i).section('_', 1, 1).section(".", 0, 0) )
            ui->CB_Language->setCurrentIndex(i+1);
    }
    Sort::sort sort = static_cast<Sort::sort>( settings.value( "Sorting", Sort::asc ).toInt() );
    ui->CBox_Sort->setCurrentIndex( sort );

    ui->LineEdit_WorkDirectory->setText( QDir::toNativeSeparators( DefinesPath::appData() ) );
}

FormSettings::~FormSettings()
{
    QSettings settings;
    settings.setValue("WindowSettings/Geometry", this->saveGeometry() );
    delete ui;
}

Sort::sort FormSettings::getSort()
{
    return static_cast<Sort::sort>( ui->CBox_Sort->currentIndex() );
}

bool FormSettings::getSwitchToDir()
{
    return ui->CBox_SwitchToDirOnHoverCover->isChecked();
}

bool FormSettings::getRestoreDefault()
{
    return restoreDefault;
}

void FormSettings::on_BtnBox_accepted()
{
    QSettings settings;

    settings.beginGroup("enableSection");
        settings.setValue( "Anime",  ui->CheckBox_EnableAnime->isChecked() );
        settings.setValue( "Manga",  ui->CheckBox_EnableManga->isChecked() );
        settings.setValue( "AMV",    ui->CheckBox_EnableAMV->isChecked() );
        settings.setValue( "Dorama", ui->CheckBox_EnableDorama->isChecked() );
    settings.endGroup();

    settings.beginGroup("optionalField");
        settings.beginGroup("anime");
            settings.setValue( "OrigTitle",   ui->CBox_Anime_AltTitle->isChecked() );
            settings.setValue( "Director",    ui->CBox_Anime_Director->isChecked() );
            settings.setValue( "PostScoring", ui->CBox_Anime_PostScoring->isChecked() );
        settings.endGroup();

        settings.beginGroup("manga");
            settings.setValue( "AltTitle",    ui->CBox_Manga_AltTitle->isChecked() );
            settings.setValue( "Author",      ui->CBox_Manga_Author->isChecked() );
            settings.setValue( "Translation", ui->CBox_Manga_Translation->isChecked() );
        settings.endGroup();

        settings.beginGroup("dorama");
            settings.setValue( "AltTitle",   ui->CBox_Dorama_AltTitle->isChecked() );
            settings.setValue( "Director",   ui->CBox_Dorama_Director->isChecked() );
        settings.endGroup();
    settings.endGroup();

    settings.beginGroup("Application");
        settings.setValue( "l10n", ui->CB_Language->currentText() );
        settings.setValue( "l10n_index", ui->CB_Language->currentIndex() );
    settings.endGroup();
    settings.setValue( "Sorting", ui->CBox_Sort->currentIndex() );

    settings.setValue( "VerUpdate", ui->ChBox_Update->isChecked() );
    settings.setValue( "SwitchToDirOnHoverACover", ui->CBox_SwitchToDirOnHoverCover->isChecked() );

    if( QDir::isAbsolutePath(ui->LineEdit_WorkDirectory->text()) )
        settings.setValue( "WorkDirectory", QDir(ui->LineEdit_WorkDirectory->text()).path() );
    else
        settings.remove("WorkDirectory");
}

void FormSettings::BtnBox_resetDefaults(){
    restoreDefault = true;
    ui->CheckBox_EnableAnime->setChecked( true );
    ui->CheckBox_EnableManga->setChecked( false );
    ui->CheckBox_EnableAMV->setChecked( false );
    ui->CheckBox_EnableDorama->setChecked( false );

    ui->CBox_Anime_AltTitle->setChecked( false );
    ui->CBox_Anime_Director->setChecked( false );
    ui->CBox_Anime_PostScoring->setChecked( false );

    ui->CBox_Manga_AltTitle->setChecked( false );
    ui->CBox_Manga_Author->setChecked( false );
    ui->CBox_Manga_Translation->setChecked( false );

    ui->CBox_Dorama_AltTitle->setChecked( false );
    ui->CBox_Dorama_Director->setChecked( false );

    ui->CB_Language->setCurrentIndex(0);
    ui->CBox_Sort->setCurrentIndex(1);

    ui->ChBox_Update->setChecked(false);
    ui->LineEdit_WorkDirectory->setText( DefinesPath::appData(true) );
}

void FormSettings::on_BtnBox_clicked(QAbstractButton *button)
{
    switch( ui->BtnBox->buttonRole( button ) ){
        case  QDialogButtonBox::ApplyRole:
            on_BtnBox_accepted();
            this->close();
            break;
        case QDialogButtonBox::ResetRole:
            BtnBox_resetDefaults();
            break;
        default:
            this->close();
    }
}

void FormSettings::on_PBtn_Export_clicked()
{
    bool exAnime  = ui->CBox_ExAnime->isChecked();
    bool exManga  = ui->CBox_ExManga->isChecked();
    bool exAmv    = ui->CBox_ExAmv->isChecked();
    bool exDorama = ui->CBox_ExDorama->isChecked();
    bool exImages = ui->CBox_ExportImages->isChecked();

    QString exportDir = ui->LineEdit_ExDir->text();
            exportDir = QDir( exportDir ).path() + QDir::separator();

    if( exportDir.isEmpty() ){
        QMessageBox::warning(this, tr("Warning"), tr("The directory for export isn't choose"));
        ui->LineEdit_ExDir->setFocus();
        return;
    }

    if( QDir::isAbsolutePath( exportDir ) == false ){
        QMessageBox::warning( this, tr("Warning"), tr("The path shall be absolute") );
        return;
    }

    this->setDisabled( true );

    QDir().mkpath( exportDir );
    QFile file( exportDir + "DatabaseAnime.xml" );
    if( ! file.open( QIODevice::WriteOnly | QIODevice::Text ) ){
        return;
    }
    XmlDbaWriter writer(&file);
    QSqlQuery query;
    if ( !query.exec( QString("SELECT * FROM %1").arg( MngrQuerys::getTableName(sections::anime)) ) ){
        qCritical() << QString("Cannot select data from table %1").arg( MngrQuerys::getTableName(sections::anime) );
        QMessageBox::critical(this, tr("Critical"), tr("Cannot export data") );
        return;
    }

    writer.startDocument();
    writer.startElement("DatabaseAnime");

    if( exAnime ){
        if ( !query.exec( QString("SELECT * FROM %1").arg( MngrQuerys::getTableName(sections::anime) ) ) ){
            qCritical() << QString("Cannot select data from table %1").arg( MngrQuerys::getTableName(sections::anime) );
            QMessageBox::critical(this, tr("Critical"), tr("Cannot export data") );
            return;
        }

        writer.startSection(sections::anime);
        while( query.next() ){
            QMap<QString, QString> data;

            data["isHaveLooked"]    = query.value("isHaveLooked"    ).toString();
            data["isEditingDone"]   = query.value("isEditingDone"   ).toString();
            data["isAdult"]         = query.value("isAdult"         ).toString();
            data["Title"]           = query.value("Title"           ).toString();
            data["OrigTitle"]       = query.value("OrigTitle"       ).toString();
            data["Director"]        = query.value("Director"        ).toString();
            data["PostScoring"]     = query.value("PostScoring"     ).toString();
            data["SeriesTV"]        = query.value("SeriesTV"        ).toString();
            data["SeriesOVA"]       = query.value("SeriesOVA"       ).toString();
            data["SeriesONA"]       = query.value("SeriesONA"       ).toString();
            data["SeriesSpecial"]   = query.value("SeriesSpecial"   ).toString();
            data["SeriesMovie"]     = query.value("SeriesMovie"     ).toString();
            data["vSeriesTV"]       = query.value("vSeriesTV"       ).toString();
            data["vSeriesOVA"]      = query.value("vSeriesOVA"      ).toString();
            data["vSeriesONA"]      = query.value("vSeriesONA"      ).toString();
            data["vSeriesSpecial"]  = query.value("vSeriesSpecial"  ).toString();
            data["vSeriesMovie"]    = query.value("vSeriesMovie"    ).toString();
            data["Score"]           = query.value("Score"           ).toString();
            data["Year"]            = query.value("Year"            ).toString();
            data["Season"]          = query.value("Season"          ).toString();
            data["Studios"]         = query.value("Studios"         ).toString();
            data["Tags"]            = query.value("Tags"            ).toString();
            data["Description"]     = query.value("Description"     ).toString();
            data["URL"]             = query.value("URL"             ).toString();
            data["Dir"]             = query.value("Dir"             ).toString();
            data["ImagePath"]       = query.value("ImagePath"       ).toString();

            writer.writeNext(data);
            QCoreApplication::processEvents();
        }
        writer.endSection(/*Anime*/);
    }
    if( exManga ){
        if ( !query.exec( QString("SELECT * FROM %1").arg( MngrQuerys::getTableName(sections::manga)) ) ){
            qCritical() << QString("Cannot select data from table %1").arg( MngrQuerys::getTableName(sections::manga) );
            QMessageBox::critical(this, tr("Critical"), tr("Cannot export data") );
            return;
        }

        writer.startSection(sections::manga);
        while (query.next()) {
            QMap<QString, QString> data;

            data["isHaveLooked"]    = query.value("isHaveLooked"    ).toString();
            data["isEditingDone"]   = query.value("isEditingDone"   ).toString();
            data["isAdult"]         = query.value("isAdult"         ).toString();
            data["Title"]           = query.value("Title"           ).toString();
            data["AltTitle"]        = query.value("AltTitle"        ).toString();
            data["Author"]          = query.value("Author"          ).toString();
            data["Translation"]     = query.value("Translation"     ).toString();
            data["Vol"]             = query.value("Vol"             ).toString();
            data["Ch"]              = query.value("Ch"              ).toString();
            data["Pages"]           = query.value("Pages"           ).toString();
            data["vVol"]            = query.value("vVol"            ).toString();
            data["vCh"]             = query.value("vCh"             ).toString();
            data["vPages"]          = query.value("vPages"          ).toString();
            data["Score"]           = query.value("Score"           ).toString();
            data["Year"]            = query.value("Year"            ).toString();
            data["Tags"]            = query.value("Tags"            ).toString();
            data["Description"]     = query.value("Description"     ).toString();
            data["URL"]             = query.value("URL"             ).toString();
            data["Dir"]             = query.value("Dir"             ).toString();
            data["ImagePath"]       = query.value("ImagePath"       ).toString();

            writer.writeNext(data);
            QCoreApplication::processEvents();
        }
        writer.endSection(/*Manga*/);
    }
    if( exAmv ){
        if ( !query.exec( QString("SELECT * FROM %1").arg( MngrQuerys::getTableName(sections::amv)) ) ){
            qCritical() << QString("Cannot select data from table %1").arg( MngrQuerys::getTableName(sections::amv) );
            QMessageBox::critical(this, tr("Critical"), tr("Cannot export data") );
            return;
        }

        writer.startSection(sections::amv);
        while (query.next()) {
            QMap<QString, QString> data;

            data["isEditingDone"]   = query.value("isEditingDone"   ).toString();
            data["isAdult"]         = query.value("isAdult"         ).toString();
            data["Title"]           = query.value("Title"           ).toString();
            data["Author"]          = query.value("Author"          ).toString();
            data["Сontestant"]      = query.value("Сontestant"      ).toString();
            data["Score"]           = query.value("Score"           ).toString();
            data["Year"]            = query.value("Year"            ).toString();
            data["Tags"]            = query.value("Tags"            ).toString();
            data["ContainingMusic"] = query.value("ContainingMusic" ).toString();
            data["ContainingAnime"] = query.value("ContainingAnime" ).toString();
            data["AuthorComment"]   = query.value("AuthorComment"   ).toString();
            data["URL"]             = query.value("URL"             ).toString();
            data["Dir"]             = query.value("Dir"             ).toString();
            data["ImagePath"]       = query.value("ImagePath"       ).toString();

            writer.writeNext(data);
            QCoreApplication::processEvents();
        }
        writer.endSection(/*Amv*/);
    }
    if( exDorama ){
        if ( !query.exec( QString("SELECT * FROM %1").arg( MngrQuerys::getTableName(sections::dorama)) ) ){
            qCritical() << QString("Cannot select data from table %1").arg( MngrQuerys::getTableName(sections::dorama) );
            QMessageBox::critical(this, tr("Critical"), tr("Cannot export data") );
            return;
        }

        writer.startSection(sections::dorama);
        while (query.next()) {
            QMap<QString, QString> data;

            data["isHaveLooked"]    = query.value("isHaveLooked"    ).toString();
            data["isEditingDone"]   = query.value("isEditingDone"   ).toString();
            data["isAdult"]         = query.value("isAdult"         ).toString();
            data["Title"]           = query.value("Title"           ).toString();
            data["AltTitle"]        = query.value("AltTitle"        ).toString();
            data["Director"]        = query.value("Director"        ).toString();
            data["SeriesTV"]        = query.value("SeriesTV"        ).toString();
            data["SeriesSpecial"]   = query.value("SeriesSpecial"   ).toString();
            data["SeriesMovie"]     = query.value("SeriesMovie"     ).toString();
            data["vSeriesTV"]       = query.value("vSeriesTV"       ).toString();
            data["vSeriesSpecial"]  = query.value("vSeriesSpecial"  ).toString();
            data["vSeriesMovie"]    = query.value("vSeriesMovie"    ).toString();
            data["Score"]           = query.value("Score"           ).toString();
            data["Year"]            = query.value("Year"            ).toString();
            data["Season"]          = query.value("Season"          ).toString();
            data["Tags"]            = query.value("Tags"            ).toString();
            data["Description"]     = query.value("Description"     ).toString();
            data["Actors"]          = query.value("Actors"          ).toString();
            data["URL"]             = query.value("URL"             ).toString();
            data["Dir"]             = query.value("Dir"             ).toString();
            data["ImagePath"]       = query.value("ImagePath"       ).toString();

            writer.writeNext(data);
            QCoreApplication::processEvents();
        }
        writer.endSection(/*Dorama*/);
    }
    writer.endElement(/*DatabaseAnime*/);

    file.close();

    if( exAnime  && exImages ){
        QDirIterator it( DefinesPath::animeCovers() );
        QString animeCoversExportPath( DefinesPath::animeCovers( exportDir ) );
        QDir().mkpath( animeCoversExportPath );
        while( it.hasNext() ){
            it.next();
            if( it.fileName() == "." || it.fileName() == ".." )
                continue;
            QFile( it.filePath() ).copy( animeCoversExportPath + it.fileName() );
            QCoreApplication::processEvents();
        }
    }
    if( exManga  && exImages ){
        QDirIterator it( DefinesPath::mangaCovers() );
        QString mangaCoversExportPath( DefinesPath::mangaCovers( exportDir ) );
        QDir().mkpath( mangaCoversExportPath );
        while( it.hasNext() ){
            it.next();
            if( it.fileName() == "." || it.fileName() == ".." )
                continue;
            QFile( it.filePath() ).copy( mangaCoversExportPath + it.fileName() );
            QCoreApplication::processEvents();
        }
    }
    if( exAmv && exImages ){
        QDirIterator it( DefinesPath::amvCovers() );
        QString amvCoversExportPath( DefinesPath::amvCovers( exportDir ) );
        QDir().mkpath( amvCoversExportPath );
        while( it.hasNext() ){
            it.next();
            if( it.fileName() == "." || it.fileName() == ".." )
                continue;
            QFile( it.filePath() ).copy( amvCoversExportPath + it.fileName() );
            QCoreApplication::processEvents();
        }
    }
    if( exDorama && exImages ){
        QDirIterator it( DefinesPath::doramaCovers() );
        QString doramaCoversExportPath( DefinesPath::doramaCovers( exportDir ) );
        QDir().mkpath( doramaCoversExportPath );
        while( it.hasNext() ){
            it.next();
            if( it.fileName() == "." || it.fileName() == ".." )
                continue;
            QFile( it.filePath() ).copy( doramaCoversExportPath + it.fileName() );
            QCoreApplication::processEvents();
        }
    }

    this->setEnabled( true );
    QMessageBox::information( this, tr("Export"), tr("Export is successfully finished") );
}

void FormSettings::on_TBtn_ChooseDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Choose a directory for export file"),
                                                    ui->LineEdit_ExDir->text()
                                                    );
    if( dir.isEmpty() )
        ui->LineEdit_ExDir->setText( QStandardPaths::writableLocation( QStandardPaths::HomeLocation ) );
    else
        ui->LineEdit_ExDir->setText( dir );
}

void FormSettings::on_TBtn_ImFile_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,
                                                tr("Choose a xml file for import"),
                                                ui->LineEdit_ImFile->text()
                                                );
    if( file.isEmpty() )
        ui->LineEdit_ImFile->setText( QStandardPaths::writableLocation( QStandardPaths::HomeLocation ) );
    else
        ui->LineEdit_ImFile->setText( file );
}

void FormSettings::on_PBtn_ImAppend_clicked()
{
    if( ui->LineEdit_ImFile->text().isEmpty() ){
        QMessageBox::warning(this, tr("Warning"), tr("The imported file isn't selected"));
        ui->LineEdit_ImFile->setFocus();
        this->setEnabled( true );
        return;
    }

    this->setCursor( QCursor(Qt::WaitCursor) );
    this->setDisabled(true);

    int countImportRecords = on_actionImport_triggered();
    if( countImportRecords > 0 ){
        QMessageBox::information(this, tr("Import"),"<b>" + tr("Import is successfully finished") + "</b><br>"
                                                    + tr("Records it is imported:")+ " " + QString::number(countImportRecords) + "   "
                                                        );
    }else{
        QMessageBox::critical(this, tr("Import"),"<b>" + tr("Import failed") + "</b>");
    }

    this->setEnabled(true);
    this->setCursor( QCursor(Qt::ArrowCursor) );
}

void FormSettings::on_PBtn_ImReplace_clicked()
{
    QMessageBox* pmbx =
    new QMessageBox(QMessageBox::Question,
        tr("Warning"),
        tr("<b>Delete</b> your database?<br>"
           "<i>Fields marked by a checkbox will be deleted</i>"),
        QMessageBox::Yes | QMessageBox::No
    );
    int n = pmbx->exec();
    delete pmbx;

    this->setCursor( QCursor(Qt::WaitCursor) );
    this->setDisabled(true);

    if (n == QMessageBox::Yes) {
        on_actionDeleteRecords_triggered();
        on_PBtn_ImAppend_clicked();
    }

    this->setCursor( QCursor(Qt::ArrowCursor) );
    this->setEnabled(true);
}

unsigned long long FormSettings::on_actionImport_triggered()
{
    bool imAnime  = ui->CBox_ImAnime->isChecked();
    bool imManga  = ui->CBox_ImManga->isChecked();
    bool imAmv    = ui->CBox_ImAmv->isChecked();
    bool imDorama = ui->CBox_ImDorama->isChecked();
    bool imImages = ui->CBox_ImportImages->isChecked();
    QString filePath = ui->LineEdit_ImFile->text();

    QFile file( QDir(filePath).path() );
    if( ! file.open(QIODevice::ReadOnly | QIODevice::Text) ){
        qCritical() << file.errorString()
                    << "\nFileName: " << file.fileName()
                    << "\nFileError: " << file.error();
        QMessageBox::critical(this, tr("Critical"), tr("File is not open"));
        return 0;
    }

    unsigned long long n(0);
    XmlDbaReader reader(&file);
    QMap<QString, QVariant> data;

    MngrConnect.transaction();
    while( ! reader.atEnd() && ! reader.hasError() ){
        data = reader.readNext();
        if( data.isEmpty() )
            continue;

        if( reader.currentSection() == sections::anime
            && imAnime == false)
            continue;
        if( reader.currentSection() == sections::manga
            && imManga == false)
            continue;
        if( reader.currentSection() == sections::amv
            && imAmv == false)
            continue;
        if( reader.currentSection() == sections::dorama
            && imDorama == false)
            continue;

        switch ( reader.currentSection() ) {
        case sections::anime :
            MngrQuerys::insertAnime(data);
            break;
        case sections::manga :
            MngrQuerys::insertManga(data);
            break;
        case sections::amv :
            MngrQuerys::insertAmv(data);
            break;
        case sections::dorama :
            MngrQuerys::insertDorama(data);
            break;
        default:
            qCritical() << "[FormSettings::importAppend] uncorrect section: " << reader.currentSection();
        }
        n++;
        QCoreApplication::processEvents();
    }
    if( reader.hasError() )
        MngrConnect.rollback();
    else
        MngrConnect.commit();

    QString importPath( QFileInfo( filePath ).path() );

    if( imAnime  && imImages ){
        QDirIterator it( DefinesPath::animeCovers( importPath ) );
        QDir().mkpath( DefinesPath::animeCovers() );
        while( it.hasNext() ){
            it.next();
            if( it.fileName() == "." || it.fileName() == ".." )
                continue;
            QFile( it.filePath() ).copy( DefinesPath::animeCovers() + it.fileName() );
            QCoreApplication::processEvents();
        }
    }
    if( imManga  && imImages ){
        QDirIterator it( DefinesPath::mangaCovers( importPath ) );
        QDir().mkpath( DefinesPath::mangaCovers() );
        while( it.hasNext() ){
            it.next();
            if( it.fileName() == "." || it.fileName() == ".." )
                continue;
            QFile( it.filePath() ).copy( DefinesPath::mangaCovers() + it.fileName() );
            QCoreApplication::processEvents();
        }
    }
    if( imAmv  && imImages ){
        QDirIterator it( DefinesPath::amvCovers( importPath ) );
        QDir().mkpath( DefinesPath::amvCovers() );
        while( it.hasNext() ){
            it.next();
            if( it.fileName() == "." || it.fileName() == ".." )
                continue;
            QFile( it.filePath() ).copy( DefinesPath::amvCovers() + it.fileName() );
            QCoreApplication::processEvents();
        }
    }
    if( imDorama && imImages ){
        QDirIterator it( DefinesPath::doramaCovers( importPath ) );
        QDir().mkpath( DefinesPath::doramaCovers() );
        while( it.hasNext() ){
            it.next();
            if( it.fileName() == "." || it.fileName() == ".." )
                continue;
            QFile( it.filePath() ).copy( DefinesPath::doramaCovers() + it.fileName() );
            QCoreApplication::processEvents();
        }
    }

    file.close();

    return n;
}

bool FormSettings::on_actionDeleteRecords_triggered()
{
    bool imAnime  = ui->CBox_ImAnime->isChecked();
    bool imManga  = ui->CBox_ImManga->isChecked();
    bool imAmv    = ui->CBox_ImAmv->isChecked();
    bool imDorama = ui->CBox_ImDorama->isChecked();
    bool imImages = ui->CBox_ImportImages->isChecked();
    QString filePath = ui->LineEdit_ImFile->text();

    MngrConnect.transaction();
    QSqlQuery query;
    if( imAnime ){
        if( false == query.exec( QString("DROP TABLE IF EXISTS %1").arg( MngrQuerys::getTableName( sections::anime ) ) ) ){
            qCritical() << QString("Error when deleting a table %1").arg( MngrQuerys::getTableName( sections::anime ) )
                        << query.lastError();
            MngrConnect.rollback();
            return false;
        }

        if( imImages ){
            QDirIterator it( DefinesPath::animeCovers() );
            while( it.hasNext() ){
                it.next();
                if( it.fileName() == "." || it.fileName() == ".." )
                    continue;
                QFile( it.filePath() ).remove();
                QCoreApplication::processEvents();
            }
        }
    }
    if( imManga ){
        if( false == query.exec( QString("DROP TABLE IF EXISTS %1").arg( MngrQuerys::getTableName( sections::manga ) ) ) ){
            qCritical() << QString("Error when deleting a table %1").arg( MngrQuerys::getTableName( sections::manga ) )
                        << query.lastError();
            MngrConnect.rollback();
            return false;
        }

        if( imImages ){
            QDirIterator it( DefinesPath::mangaCovers() );
            while( it.hasNext() ){
                it.next();
                if( it.fileName() == "." || it.fileName() == ".." )
                    continue;
                QFile( it.filePath() ).remove();
                QCoreApplication::processEvents();
            }
        }
    }
    if( imAmv ){
        if( false == query.exec( QString("DROP TABLE IF EXISTS %1").arg( MngrQuerys::getTableName( sections::amv ) ) ) ){
            qCritical() << QString("Error when deleting a table %1").arg( MngrQuerys::getTableName( sections::amv ) )
                        << query.lastError();
            MngrConnect.rollback();
            return false;
        }

        if( imImages ){
            QDirIterator it( DefinesPath::amvCovers() );
            while( it.hasNext() ){
                it.next();
                if( it.fileName() == "." || it.fileName() == ".." )
                    continue;
                QFile( it.filePath() ).remove();
                QCoreApplication::processEvents();
            }
        }
    }
    if( imDorama ){
        if( false == query.exec( QString("DROP TABLE IF EXISTS %1").arg( MngrQuerys::getTableName( sections::dorama ) ) ) ){
            qCritical() << QString("Error when deleting a table %1").arg( MngrQuerys::getTableName( sections::dorama ) )
                        << query.lastError();
            MngrConnect.rollback();
            return false;
        }

        if( imImages ){
            QDirIterator it( DefinesPath::doramaCovers() );
            while( it.hasNext() ){
                it.next();
                if( it.fileName() == "." || it.fileName() == ".." )
                    continue;
                QFile( it.filePath() ).remove();
                QCoreApplication::processEvents();
            }
        }
    }

    MngrQuerys::createTable_Anime();
    MngrQuerys::createTable_Manga();
    MngrQuerys::createTable_Amv();
    MngrQuerys::createTable_Dorama();
    MngrConnect.commit();
    return true;
}
