#include "musiclrcsearchtablewidget.h"
#include "musicmessagebox.h"
#include "musicitemdelegate.h"
#include "musicdownloadqueryfactory.h"

MusicLrcSearchTableWidget::MusicLrcSearchTableWidget(QWidget *parent)
    : MusicQueryItemTableWidget(parent)
{
    setColumnCount(5);
    QHeaderView *headerview = horizontalHeader();
    headerview->resizeSection(0, 30);
    headerview->resizeSection(1, 225);
    headerview->resizeSection(2, 193);
    headerview->resizeSection(3, 55);
    headerview->resizeSection(4, 24);
}

MusicLrcSearchTableWidget::~MusicLrcSearchTableWidget()
{
    clearAllItems();
}

QString MusicLrcSearchTableWidget::getClassName()
{
    return staticMetaObject.className();
}

void MusicLrcSearchTableWidget::startSearchQuery(const QString &text)
{
    if(!M_NETWORK_PTR->isOnline())
    {   //no network connection
        emit showDownLoadInfoFor(MusicObject::DW_DisConnection);
        return;
    }
    MusicQueryItemTableWidget::startSearchQuery(text);
    connect(m_downLoadManager, SIGNAL(downLoadDataChanged(QString)), SIGNAL(resolvedSuccess()));
    m_downLoadManager->startSearchSong(MusicDownLoadQueryThreadAbstract::LrcQuery, text);
}

void MusicLrcSearchTableWidget::musicDownloadLocal(int row)
{
    if( row < 0 || (row >= rowCount() - 1))
    {
        MusicMessageBox message;
        message.setText(tr("Please Select One Item First!"));
        message.exec();
        return;
    }

    MusicObject::MusicSongInfomations musicSongInfos(m_downLoadManager->getMusicSongInfos());
    ///download lrc
    MusicDownLoadThreadAbstract *lrcDownload = M_DOWNLOAD_QUERY_PTR->getDownloadLrc(musicSongInfos[row].m_lrcUrl,
                             MusicUtils::Core::lrcPrefix() + m_downLoadManager->getSearchedText() + LRC_FILE,
                             MusicDownLoadThreadAbstract::Download_Lrc, this);
    connect(lrcDownload, SIGNAL(downLoadDataChanged(QString)),
                         SIGNAL(lrcDownloadStateChanged(QString)));
    lrcDownload->startToDownload();
}

void MusicLrcSearchTableWidget::clearAllItems()
{
    MusicQueryItemTableWidget::clearAllItems();
    setColumnCount(5);
}

void MusicLrcSearchTableWidget::createSearchedItems(const QString &songname,
                         const QString &artistname, const QString &time)
{
    int count = rowCount();
    setRowCount(count + 1);

    QTableWidgetItem *item = new QTableWidgetItem;
    item->setData(MUSIC_CHECK_ROLE, false);
    setItem(count, 0, item);

                      item = new QTableWidgetItem;
    item->setText(MusicUtils::Widget::elidedText(font(), songname, Qt::ElideRight, 170));
    item->setTextColor(QColor(50, 50, 50));
    item->setTextAlignment(Qt::AlignCenter);
    item->setToolTip(songname);
    setItem(count, 1, item);

                      item = new QTableWidgetItem;
    item->setText(MusicUtils::Widget::elidedText(font(), artistname, Qt::ElideRight, 144));
    item->setTextColor(QColor(50, 50, 50));
    item->setTextAlignment(Qt::AlignCenter);
    item->setToolTip(artistname);
    setItem(count, 2, item);

                      item = new QTableWidgetItem(time);
    item->setTextColor(QColor(50, 50, 50));
    item->setTextAlignment(Qt::AlignCenter);
    setItem(count, 3, item);

                      item = new QTableWidgetItem;
    item->setIcon(QIcon(QString::fromUtf8(":/tiny/lb_star")));
    setItem(count, 4, item);
}

void MusicLrcSearchTableWidget::itemDoubleClicked(int row, int column)
{
    if(column <= 0)
    {
        return;
    }
    musicDownloadLocal(row);
}

void MusicLrcSearchTableWidget::contextMenuEvent(QContextMenuEvent *event)
{
    MusicQueryItemTableWidget::contextMenuEvent(event);
    QMenu rightClickMenu(this);
    createContextMenu(rightClickMenu);

    rightClickMenu.exec(QCursor::pos());
}
