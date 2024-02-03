#include <WallPaper.hpp>
#include <QFileDialog>

BOOL WallPaper::Util::Functions::EnumWindowsCallBack( _In_ HWND hwnd, _In_ LPARAM lParam )
{
    HWND Handle               = nullptr;
    CHAR HandleName[MAX_PATH] = { 0 };

    // 检查是否出现在我们的列表中的整理软件
    for ( const QString& name : DesktopOrganizationSoftwareList->toList() )
    {
        if ( not GetClassNameA( hwnd, HandleName, sizeof( HandleName ) ) )
        {
            continue;
        }

        if ( QString(HandleName) == name )
        {
            Handle = hwnd;
        }
        else
        {
            Handle = FindWindowExA( hwnd, nullptr, name.toLocal8Bit().data(), nullptr );
        }

        if ( Handle )
        {
            ShowWindow( Handle, SW_HIDE );
        }
    }

    Handle = FindWindowExA( hwnd, nullptr, "SHELLDLL_DefView", nullptr );

    if ( not Handle )
    {
        return TRUE;
    }

    Handle = FindWindowExA( nullptr, hwnd, "WorkerW", nullptr );

    if ( not Handle )
    {
        return TRUE;
    }

    ShowWindow( ( WallPaper::Util::HandleWorkerW = Handle ), SW_HIDE );

    return FALSE;
}

BOOL WallPaper::Util::Functions::SetWallPaper( WId panel )
{
    HWND parent = FindWindowA( "Progman", "Program Manager" );

    if ( not parent )
    {
        return FALSE;
    }

    SendMessageTimeoutA( parent, 0x0000052c, 0 , 0, SMTO_NORMAL, 10000, nullptr );

    EnumWindows( EnumWindowsCallBack, NULL );

    SetParent((HWND)panel, parent );

    return TRUE;
}

QString WallPaper::Util::Functions::ReadFileAllBytesAsQString( const QString& Path )
{
    QString Result = { };
    QFile*  File   = { };

    if ( Path.isEmpty( ) )
    {
        return { };
    }

    File = new QFile( Path );

    if( not File->open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        delete File;
        return { };
    }

    Result = QString( File->readAll( ) );
    File->close( );

    delete File;
    return Result;
}

VOID WallPaper::Util::Functions::SaveFile( const QString& path , const QByteArray& data )
{
    QFile file(path);

    if (not file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.close();
        return;
    }

    file.write(data);
    file.close();
}

QString WallPaper::Util::Functions::DialogChooseFile( QWidget* parent )
{
    return QFileDialog::getOpenFileName( parent, QString( "Choose File" ), QDir::homePath( ) );
}

BOOL WallPaper::Util::Functions::FileExists( const QString& path )
{
    return QFile::exists( path );
}

VOID WallPaper::Util::Functions::InitDesktopOrganizationSoftwareList( VOID )
{
    // 初始化
    DesktopOrganizationSoftwareList = new QList<QString>( );

    DesktopOrganizationSoftwareList->append( "TXMiniSkin" ); // 腾讯桌面整理
}
