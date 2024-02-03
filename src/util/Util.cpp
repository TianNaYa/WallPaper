#include <WallPaper.hpp>
#include <QFileDialog>

BOOL WallPaper::Util::Functions::EnumWindowsCallBackWorkerW( _In_ HWND hwnd, _In_ LPARAM lParam )
{
    HWND Handle = FindWindowExA( hwnd, nullptr, "SHELLDLL_DefView", nullptr );

    if ( not Handle )
    {
        return TRUE;
    }

    Handle = FindWindowExA( nullptr, hwnd, "WorkerW", nullptr );

    if ( not Handle )
    {
        return TRUE;
    }

    *( ( HWND* ) lParam ) = Handle;
    return FALSE;
}

BOOL WallPaper::Util::Functions::EnumWindowsCallBackSysListView32( _In_ HWND hwnd, _In_ LPARAM lParam )
{
    HWND Handle = FindWindowExA( hwnd, nullptr, "SHELLDLL_DefView", nullptr );

    if ( not Handle )
    {
        return TRUE;
    }

    Handle = FindWindowExA( Handle, nullptr, "SysListView32", nullptr );

    if ( not Handle )
    {
        return TRUE;
    }

    *( ( HWND* ) lParam ) = Handle;
    return FALSE;
}

BOOL WallPaper::Util::Functions::SetWallPaper( WId panel )
{
    HWND HandleProgman = FindWindowA( "Progman", "Program Manager" );
    HWND HandleWorkerW = GetWorkerW( );

    if ( not HandleProgman || not HandleWorkerW )
    {
        return FALSE;
    }

    SendMessageTimeoutA( HandleProgman, 0x0000052c, 0 , 0, SMTO_NORMAL, 10000, nullptr );

    ShowWindow( HandleWorkerW, SW_HIDE );
    SetParent((HWND)panel, HandleProgman );

    return TRUE;
}

HWND WallPaper::Util::Functions::GetWorkerW( VOID )
{
    HWND Handle = nullptr;

    EnumWindows( EnumWindowsCallBackWorkerW, (LPARAM) &Handle );

    return Handle;
}

HWND WallPaper::Util::Functions::GetSysListView32( VOID )
{
    HWND Handle = nullptr;

    EnumWindows( EnumWindowsCallBackSysListView32, (LPARAM) &Handle );

    return Handle;
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
