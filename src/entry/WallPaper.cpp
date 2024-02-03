#include <WallPaper.hpp>
#include <QScreen>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMenu>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMouseEvent>
#include <QApplication>
#include <QFileDialog>

WallPaper::UserInterface::WallPaperUI::WallPaperUI( const QIcon& Icon ) :
    EngineView( new QWebEngineView( ) ),
    TrayIcon( new QSystemTrayIcon( ) ),
    Menu( new QMenu( ) )
{
    // 初始化右键菜单
    this->InitMenu( );

    // 初始化系统托盘
    this->TrayIcon->setIcon( Icon );
    this->TrayIcon->setToolTip( WINDOW_TITLE );
    this->TrayIcon->show();
    this->TrayIcon->setContextMenu( this->Menu );

    // 设置样式
    this->setWindowIcon( Icon );
    this->setWindowFlags( Qt::FramelessWindowHint );

    // 添加我们的窗口到主界面
    this->setCentralWidget( this->EngineView );

    // 监听屏幕变化事件
    connect(QApplication::primaryScreen(), &QScreen::geometryChanged, this, &WallPaperUI::RefreshScreenSize);
}

WallPaper::UserInterface::WallPaperUI::~WallPaperUI( VOID )
{
    // 显示我们的桌面
    ShowWindow( WallPaper::Util::HandleWorkerW, SW_SHOW );

    // 刷新壁纸
    SystemParametersInfo( SPI_SETDESKWALLPAPER, 0, nullptr, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE );

    delete this->Menu;
    delete this->Close;
    delete this->Reset;
    delete this->Refresh;
    delete this->TrayIcon;
    delete this->EngineView;
}

VOID WallPaper::UserInterface::WallPaperUI::InstallWallPaper( )
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

VOID WallPaper::UserInterface::WallPaperUI::ResetWallPaper( )
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

VOID WallPaper::UserInterface::WallPaperUI::InitMenu( )
{
    this->Close   = new QAction( QString( "Exit" ), this );
    this->Refresh = new QAction( QString( "Refresh" ), this );
    this->Reset   = new QAction( QString( "Reset WallPaper" ), this );

    this->Menu->addAction( this->Reset );
    this->Menu->addAction( this->Refresh );
    this->Menu->addAction( this->Close );

    connect( this->Reset,   &QAction::triggered, this, &WallPaperUI::ResetWallPaperAction );
    connect( this->Refresh, &QAction::triggered, this, &WallPaperUI::RefreshAction );
    connect( this->Close,   &QAction::triggered, this, &WallPaperUI::CloseAction );
}

VOID WallPaper::UserInterface::WallPaperUI::ResetWallPaperAction()
{
    ResetWallPaper( );
}

VOID WallPaper::UserInterface::WallPaperUI::RefreshAction( )
{
    if ( this->CurrentFile.isEmpty( ) || not WallPaper::Util::Functions::FileExists( this->CurrentFile ) )
    {
        QMessageBox::critical( this, "Error", "No html file was selected, or the selected file was deleted." );
        WebEngineViewReLoad( { } );
        return;
    }

    WebEngineViewReLoad( WallPaper::Util::Functions::ReadFileAllBytesAsQString( this->CurrentFile ) );
}

VOID WallPaper::UserInterface::WallPaperUI::CloseAction( )
{
    /* 隐藏我们的html渲染器 */
    this->EngineView->hide( );
    this->EngineView->stop( );

    /* 关闭主窗口 */
    this->hide();
    this->close();

    /* 调用退出事件 */
    QCoreApplication::exit( );
}

VOID WallPaper::UserInterface::WallPaperUI::WebEngineViewReLoad( const QString& html )
{
    if ( not this->EngineView )
    {
        return;
    }
    else
    {
        // 关闭我们的web舞台
        this->EngineView->stop();
        this->EngineView->hide();
        this->EngineView->close();

        // 别忘记删除实例
        delete this->EngineView;
    }

    // 我们重新创建一个舞台
    this->EngineView = new QWebEngineView( );
    this->EngineView->setHtml( html );
    this->EngineView->update( );

    // 更新我们的主窗口
    this->setCentralWidget( this->EngineView );
    this->update();
}

VOID WallPaper::UserInterface::WallPaperUI::RefreshScreenSize( const QRect& rect )
{
    /* 窗口变化，我们应该刷新我们的大小 */
    this->resize( rect.size( ) );
    this->update( );

    /* 我觉得有必要设置一下我们的Web舞台 */
    this->EngineView->resize( rect.size( ) );
    this->EngineView->update();
}

QString WallPaper::UserInterface::WallPaperUI::GetCurrentDesktopFile( VOID )
{
    return this->CurrentFile;
}

VOID WallPaper::WallPaperMainWindowStart( const QIcon& Icon, QApplication* application )
{
    WallPaper::UserInterface::WallPaperUI* ui     = nullptr;
    QScreen*                               screen = nullptr;

    /* 初始化我们要隐藏的窗口 */
    WallPaper::Util::Functions::InitDesktopOrganizationSoftwareList( );

    /* 初始化我们的变量 */
    ui     = new WallPaper::UserInterface::WallPaperUI( Icon );
    screen = QGuiApplication::primaryScreen( );

    ui->resize( screen->size( ) );

    ui->InstallWallPaper( );

    if ( not WallPaper::Util::Functions::SetWallPaper( ui->winId( ) ) )
    {
        QMessageBox::critical( ui, "Error", "Fatal error occurred" );

        ui->CloseAction( ); return;
    }

    QObject::connect( application, &QApplication::aboutToQuit, [ & ] {
        delete ui;
    });

    // 确定选择了文件
    if ( not ui->GetCurrentDesktopFile( ).isEmpty( ) )
    {
        ui->show( );
    }
}
