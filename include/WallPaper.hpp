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
            static BOOL CALLBACK EnumWindowsCallBack( _In_ HWND hwnd, _In_ LPARAM lParam );

        public:
            static BOOL    SetWallPaper( WId panel );
            static QString ReadFileAllBytesAsQString( const QString& Path );
            static QString DialogChooseFile( QWidget* parent );
            static BOOL    FileExists( const QString& path );
            static VOID    SaveFile( const QString& path, const QByteArray& data );
            static VOID    InitDesktopOrganizationSoftwareList( VOID );

        };

        static HWND            HandleWorkerW;
        static QList<QString>* DesktopOrganizationSoftwareList;
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
            QString          CurrentFile   = { };

        private:
            QAction*         Close         = { };
            QAction*         Reset         = { };
            QAction*         Refresh       = { };
            QAction*         Start         = { };
            QAction*         Stop          = { };

        public slots:
            void CloseAction( );
            void ResetWallPaperAction( );
            void RefreshAction( );
            void StartAction( );
            void StopAction( );
            void RefreshScreenSize( const QRect& newGeometry );

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
