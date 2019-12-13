/*
 *
 *    ______
 *   /_  __/___  ____ ___  ___  ____
 *    / / / __ \/ __ `__ \/ _ \/ __ \
 *   / / / /_/ / / / / / /  __/ /_/ /
 *  /_/  \____/_/ /_/ /_/\___/\____/
 *              video for no reason
 *
 * 2811 cw2 November 2019 by twak
 */

#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QImageReader>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QMargins>
#include "the_player.h"
#include "the_button.h"
#include <QtWidgets/QScrollArea>

using namespace std;

// read in videos and thumbnails to this directory
vector<TheButtonInfo> getInfoIn (string loc) {

    vector<TheButtonInfo> out =  vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

        if (!f.contains(".png")) { // if it isn't an image
            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // but a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                    }
                    else
                        cerr << "warning: skipping video because I couldn't process thumbnail " << thumb.toStdString() << endl;
            }
            else
                cerr << "warning: skipping video because I couldn't find thumbnail " << thumb.toStdString() << endl;
        }
    }

    return out;
}









int main(int argc, char *argv[]) {
    // let's just check that Qt is operational first
    cout << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

    //adding a scroll area
    QScrollArea * Scroll = new QScrollArea();
    QWidget window;
    QHBoxLayout *top = new QHBoxLayout();

    // create the main window and layout
    //set the window properties
    window.setLayout(top);
    window.setWindowTitle("Outdoor Application");
    window.setMinimumSize(768, 509);//size of the smallest ipad
    window.setMaximumSize(1060,813); //size for the largest ipad


    // collect all the videos in the folder
    vector<TheButtonInfo> videos;
    if (argc == 1)
        videos = getInfoIn( "/home/csunix/sc18msru/Videos/2811_videos" );
    else
        videos = getInfoIn( string(argv[1]) );
    if (videos.size() == 0) {
        cerr << "no videos found! download from https://vcg.leeds.ac.uk/wp-content/static/2811/the/videos.zip into /tmp/XXX, and update the code on line 77";
        exit(-1);
    }


    // the widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;
    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;
    player->setVolume(0); //change the volume
    player->setVideoOutput(videoWidget);



    // a button widget
    QWidget *buttonWidget = new QWidget();
    // a list of the buttons
    vector<TheButton*> buttons;
    // the buttons are arranged vertically
    QVBoxLayout *layout = new QVBoxLayout();
    //set the buttons into a layout
    buttonWidget->setLayout(layout);

        //adds the button and connects it
        for ( int i = 0; i < 7; i++ ) { //changed to 6 instead of 4
            TheButton *button = new TheButton(buttonWidget);
            button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo* ))); // when clicked, tell the player to play.
            buttons.push_back(button);
            button->setFixedSize(150,100);
//            button->setIconSize(QSize(140,90));
//            button->setStyleSheet("border: 1px solid red;");
//            button->setFlat(true);

            layout->addWidget(button);
            button->init(&videos.at(i));
        }


    Scroll->setWidget(buttonWidget);
    Scroll->setFixedWidth(200);
    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);


    // add the video and the buttons to the top level widget
    top->addWidget(videoWidget);
    top->addWidget(Scroll);

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
