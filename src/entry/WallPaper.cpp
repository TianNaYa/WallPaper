#include <WallPaper.hpp>
#include <QScreen>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMenu>
#include <QTimer>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMouseEvent>
#include <QApplication>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>

WallPaper::UserInterface::WallPaperUI::WallPaperUI( const QIcon& Icon ) :
    TrayIcon( new QSystemTrayIcon( ) ),
    Menu( new QMenu( ) )
{
    // 初始化右键菜单
    this->InitMenu( );

    // 初始化系统托盘
    this->TrayIcon->setIcon( Icon );
    this->TrayIcon->setToolTip( WINDOW_TITLE );
    this->TrayIcon->show( );
    this->TrayIcon->setContextMenu( this->Menu );

    // 监听屏幕变化事件
    connect(QApplication::primaryScreen( ), &QScreen::geometryChanged, this, &WallPaperUI::RefreshScreenSize);
}

WallPaper::UserInterface::WallPaperUI::~WallPaperUI( VOID )
{
    // 显示我们的桌面
    ShowDesktop( );

    delete this->Menu;
    delete this->Close;
    delete this->Reset;
    delete this->Refresh;
    delete this->TrayIcon;
    delete this->EngineView;
}

VOID WallPaper::UserInterface::WallPaperUI::RepairParentWindow( VOID )
{
    QTimer* timer = { };

    timer = new QTimer( );
    timer->setInterval( 5000 );

    connect( timer, &QTimer::timeout, [=] {
        if ( not this->EngineView )
        {
            return;
        }

        HWND Parnet = FindWindowA( "Progman", "Program Manager" );
        HWND Handle = FindWindowExA( Parnet, nullptr, nullptr, WALLPAPER_CLASS_NAME );

        if ( not Handle )
        {
            WallPaper::Util::Functions::SetWallPaper( this->EngineView->winId() );
        }
    });

    timer->start( );
}

VOID WallPaper::UserInterface::WallPaperUI::InstallWallPaper( VOID )
{
    QString       Context       = { };
    QJsonObject   JsonObject    = { };
    QJsonDocument JsonDocument  = { };

    // 读取我们的配置文件
    if ( not WallPaper::Util::Functions::FileExists( CONFIG_FILE_NAME ) )
    {
        ResetWallPaper( ); return;
    }

    Context      = WallPaper::Util::Functions::ReadFileAllBytesAsQString( CONFIG_FILE_NAME );
    JsonDocument = QJsonDocument::fromJson( Context.toUtf8( ) );
    JsonObject   = JsonDocument.object( );

    // 如果没有我们的键，或者文件不存在
    if ( not JsonObject.contains( CURRENT_DESKTOP_WALLPAPER ) || not WallPaper::Util::Functions::FileExists( JsonObject[ CURRENT_DESKTOP_WALLPAPER ].toString( ) ) )
    {
        ResetWallPaper( ); return;
    }

    Context     = WallPaper::Util::Functions::ReadFileAllBytesAsQString( JsonObject[ CURRENT_DESKTOP_WALLPAPER ].toString( ) );
    CurrentFile = QString( JsonObject[ CURRENT_DESKTOP_WALLPAPER ].toString( ) );

    WebEngineViewReLoad( Context );
}

VOID WallPaper::UserInterface::WallPaperUI::ResetWallPaper( VOID )
{
    QString       Path          = { };
    QJsonObject   JsonObject    = { };
    QJsonDocument JsonDocument  = { };

    Path = WallPaper::Util::Functions::DialogChooseFile( this );

    if ( Path.isEmpty() || not WallPaper::Util::Functions::FileExists( Path ) )
    {
        return;
    }
    else
    {
        this->CurrentFile = Path;
    }

    // 显示html，并且刷新显示
    WebEngineViewReLoad( WallPaper::Util::Functions::ReadFileAllBytesAsQString( Path ) );

    // 刷新我们的配置文件
    JsonObject[ CURRENT_DESKTOP_WALLPAPER ] = Path;
    JsonDocument.setObject( JsonObject );

    // 写入配置文件
    WallPaper::Util::Functions::SaveFile( CONFIG_FILE_NAME, JsonDocument.toJson( ) );
}

VOID WallPaper::UserInterface::WallPaperUI::InitMenu( VOID )
{
    this->Menu->setStyleSheet(
            "QMenu {"
            "    background-color: #f5f5f5;"
            "    color: #121212;"
            "}"
            "QMenu::separator {"
            "    height: 1px;"
            "    background: #303030;"
            "}"
            "QMenu::item:selected {"
            "    background: #d3cbcb;"
            "}"
            "QAction {"
            "    background-color: #f5f5f5;"
            "    color: #121212;"
            "}"
    );

    this->Close   = new QAction( QString( "Exit" ), this );
    this->Refresh = new QAction( QString( "Refresh" ), this );
    this->Reset   = new QAction( QString( "Reset WallPaper" ), this );
    this->Start   = new QAction( QString( "Start" ), this );
    this->Stop    = new QAction( QString( "Stop" ), this );

    this->Menu->addAction( this->Start );
    this->Menu->addAction( this->Stop );
    this->Menu->addSeparator( );
    this->Menu->addAction( this->Reset );
    this->Menu->addSeparator( );
    this->Menu->addAction( this->Refresh );
    this->Menu->addSeparator( );
    this->Menu->addAction( this->Close );

    connect( this->Reset,   &QAction::triggered, this, &WallPaperUI::ResetWallPaperAction );
    connect( this->Refresh, &QAction::triggered, this, &WallPaperUI::RefreshAction );
    connect( this->Close,   &QAction::triggered, this, &WallPaperUI::CloseAction );
    connect( this->Start,   &QAction::triggered, this, &WallPaperUI::StartAction );
    connect( this->Stop,    &QAction::triggered, this, &WallPaperUI::StopAction );
}

VOID WallPaper::UserInterface::WallPaperUI::StartAction( VOID )
{
    // 隐藏我们的桌面
    HideDesktop( );

    // 刷新并且显示我们的web界面
    this->RefreshAction( );
}

VOID WallPaper::UserInterface::WallPaperUI::StopAction( VOID )
{
    // 显示桌面
    ShowDesktop();

    this->FreeWebEngineView( );
}

VOID WallPaper::UserInterface::WallPaperUI::FreeWebEngineView( VOID )
{
    if ( not this->EngineView )
    {
        return;
    }

    // 关闭我们的web舞台
    this->EngineView->hide();
    this->EngineView->close();

    // 清理实例
    delete( this->EngineView );

    // 我们需要将我们的指针指向空 [ 防止出现野指针 ]
    this->EngineView = nullptr;
}

VOID WallPaper::UserInterface::WallPaperUI::HideDesktop( VOID )
{
    // 显示我们的桌面
    ShowWindow( WallPaper::Util::HandleWorkerW, SW_HIDE );
}

VOID WallPaper::UserInterface::WallPaperUI::ShowDesktop( VOID )
{
    // 显示我们的桌面
    ShowWindow( WallPaper::Util::HandleWorkerW, SW_SHOW );

    // 刷新壁纸
    SystemParametersInfo( SPI_SETDESKWALLPAPER, 0, nullptr, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE );
}

VOID WallPaper::UserInterface::WallPaperUI::ResetWallPaperAction( VOID )
{
    this->ResetWallPaper( );
}

VOID WallPaper::UserInterface::WallPaperUI::RefreshAction( VOID )
{
    if ( this->CurrentFile.isEmpty( ) || not WallPaper::Util::Functions::FileExists( this->CurrentFile ) )
    {
        QMessageBox::critical( this, "Error", "No html file was selected, or the selected file was deleted." );
        this->WebEngineViewReLoad( { } );
        return;
    }

    this->WebEngineViewReLoad( WallPaper::Util::Functions::ReadFileAllBytesAsQString( this->CurrentFile ) );
}

VOID WallPaper::UserInterface::WallPaperUI::CloseAction( VOID )
{
    /* 隐藏我们的html渲染器 */
    this->EngineView->hide( );
    this->EngineView->close( );

    /* 调用退出事件 */
    QCoreApplication::exit( );
}

VOID WallPaper::UserInterface::WallPaperUI::WebEngineViewReLoad( const QString& html )
{
    QWebEngineView* WebEngineView = nullptr;

    // 我们重新创建一个舞台
    WebEngineView = new QWebEngineView( );
    WebEngineView->setWindowTitle( WALLPAPER_CLASS_NAME );
    WebEngineView->setWindowFlags( Qt::FramelessWindowHint );
    WebEngineView->resize( QGuiApplication::primaryScreen( )->size( ) );
    WebEngineView->setHtml( html );
    WebEngineView->update( );

    if ( not WallPaper::Util::Functions::SetWallPaper( WebEngineView->winId( ) ) )
    {
        QMessageBox::critical( WebEngineView, "Error", "Fatal error occurred" );
    }
    else
    {
        connect( WebEngineView, &QWebEngineView::loadFinished, [=] {
            WebEngineView->show( );

            if ( this->EngineView )
            {
                this->FreeWebEngineView( );
            }

            this->EngineView = WebEngineView;
        });
    }
}

VOID WallPaper::UserInterface::WallPaperUI::RefreshScreenSize( const QRect& rect )
{
    this->EngineView->resize( rect.size( ) );
    this->EngineView->update();
}

VOID WallPaper::WallPaperMainWindowStart( const QIcon& Icon, QApplication* application )
{
    WallPaper::UserInterface::WallPaperUI* ui = nullptr;

    /* 初始化我们要隐藏的窗口 */
    WallPaper::Util::Functions::InitDesktopOrganizationSoftwareList( );

    /* 初始化我们的变量 */
    ui = new WallPaper::UserInterface::WallPaperUI( Icon );
    ui->RepairParentWindow( );
    ui->InstallWallPaper( );

    QObject::connect( application, &QApplication::aboutToQuit, [&] {
        delete ui;
    });
}
