#include <QApplication>
#include <QMessageBox>
#include <WallPaper.hpp>

int main( int argc , char *argv[] )
{
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication application( argc , argv );
    QIcon        icon( "wallpaper.png" );

    if ( icon.isNull() )
    {
        QMessageBox::critical( nullptr, "Error", "Program icon not found" );
        return 0;
    }

    QApplication::setWindowIcon( icon );

    WallPaper::WallPaperMainWindowStart( icon, &application );

    return QApplication::exec( );
}
