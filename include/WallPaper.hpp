#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QListView>
#include <QMainWindow>
#include <QSplitter>
#include <QStringList>
#include <QString>
#include <QStringListModel>
#include <QSystemTrayIcon>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QWebEngineProfile>
#include <iostream>
#include <Windows.h>

#ifndef WINDOW_TITLE
#define WINDOW_TITLE "WallPaper"
#endif

#ifndef CONFIG_FILE_NAME
#define CONFIG_FILE_NAME            ".wallpaper.json"
#endif

#ifndef CURRENT_DESKTOP_WALLPAPER
#define CURRENT_DESKTOP_WALLPAPER   "Current_WallPaper"
#endif

#ifndef WALLPAPER_CLASS_NAME
#define WALLPAPER_CLASS_NAME        "Desktop_WallPaper_Windows_Title"
#endif

namespace WallPaper
{
    namespace Util
    {
        class Functions
        {

        private:
            static BOOL CALLBACK EnumWindowsCallBackWorkerW( _In_ HWND hwnd, _In_ LPARAM lParam );
            static BOOL CALLBACK EnumWindowsCallBackSysListView32( _In_ HWND hwnd, _In_ LPARAM lParam );

        public:
            static BOOL    SetWallPaper( WId panel );
            static QString ReadFileAllBytesAsQString( const QString& Path );
            static QString DialogChooseFile( QWidget* parent );
            static BOOL    FileExists( const QString& path );
            static VOID    SaveFile( const QString& path, const QByteArray& data );
            static HWND    GetSysListView32( VOID );
            static HWND    GetWorkerW( VOID );

        };
    }

    namespace UserInterface
    {
        class WallPaperUI : public QMainWindow
        {
        Q_OBJECT

        private:
            QSystemTrayIcon* TrayIcon      = { };
            QWebEngineView*  EngineView    = { };
            QMenu*           Menu          = { };
            QMenu*           Icon          = { };
            QString          CurrentFile   = { };
            QTimer*          RepairTimer   = { };

        private:
            QAction*         Close         = { };
            QAction*         Reset         = { };
            QAction*         Refresh       = { };
            QAction*         Start         = { };
            QAction*         Stop          = { };
            QAction*         HideIcons     = { };
            QAction*         ShowIcons     = { };

        public slots:
            void CloseAction( );
            void ResetWallPaperAction( );
            void RefreshAction( );
            void StartAction( );
            void StopAction( );
            void RefreshScreenSize( const QRect& newGeometry );

        public slots:
            static void HideDesktopIcons( );
            static void ShowDesktopIcons( );

        public:
            WallPaperUI( const QIcon& Icon );
            ~WallPaperUI( VOID );

        public:
            VOID    InstallWallPaper( );
            VOID    ResetWallPaper( );
            VOID    InitMenu( );
            VOID    WebEngineViewReLoad( const QString& html );
            VOID    RepairParentWindow( VOID );

        private:
            VOID FreeWebEngineView( VOID );

        private:
            static VOID HideDesktop( VOID );
            static VOID ShowDesktop( VOID );

        };
    }

    VOID WallPaperMainWindowStart( const QIcon& Icon, QApplication* application );
}
