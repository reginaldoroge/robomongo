#include "robomongo/gui/widgets/workarea/WelcomeTab.h"

#ifndef __linux__  // --------------------- Windows, macOS impl --------------------------// 

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

namespace Robomongo {
    namespace {
        QLabel* createLabel(QString const& text, char const* objectName)
        {
            auto label = new QLabel(text);
            label->setObjectName(objectName);
            label->setWordWrap(true);
            label->setTextInteractionFlags(Qt::NoTextInteraction);
            return label;
        }

        QFrame* createPanel(QString const& title, QString const& bodyHtml)
        {
            auto panel = new QFrame;
            panel->setObjectName("revivalPanel");

            auto layout = new QVBoxLayout(panel);
            layout->setContentsMargins(20, 18, 20, 18);
            layout->setSpacing(9);

            layout->addWidget(createLabel(title, "revivalPanelTitle"));

            auto body = createLabel(bodyHtml, "revivalPanelBody");
            body->setTextFormat(Qt::RichText);
            body->setOpenExternalLinks(false);
            layout->addWidget(body);
            layout->addStretch();

            return panel;
        }

        QWidget* createWelcomeContent()
        {
            auto content = new QWidget;
            content->setObjectName("revivalPage");
            content->setMinimumWidth(760);

            auto pageLayout = new QVBoxLayout(content);
            pageLayout->setContentsMargins(58, 54, 58, 70);
            pageLayout->setSpacing(0);

            auto brandLayout = new QHBoxLayout;
            brandLayout->setContentsMargins(0, 0, 0, 0);
            brandLayout->setSpacing(18);

            auto mark = createLabel("D", "revivalMark");
            mark->setAlignment(Qt::AlignCenter);
            mark->setFixedSize(72, 72);
            mark->setWordWrap(false);
            brandLayout->addWidget(mark, 0, Qt::AlignTop);

            auto titleLayout = new QVBoxLayout;
            titleLayout->setContentsMargins(0, 0, 0, 0);
            titleLayout->setSpacing(12);
            titleLayout->addWidget(createLabel("dino-robomongo 2.0.0", "revivalTitle"));
            titleLayout->addWidget(createLabel(
                "A lightweight MongoDB desktop client revived for current machines, "
                "modern builds, and explicit driver compatibility.",
                "revivalSubtitle"));
            brandLayout->addLayout(titleLayout, 1);

            pageLayout->addLayout(brandLayout);
            pageLayout->addSpacing(34);

            auto status = createLabel(
                "Legacy MongoDB 4.2 path + modern mongocxx 4.3.1 path",
                "revivalStatus");
            status->setWordWrap(false);
            pageLayout->addWidget(status, 0, Qt::AlignLeft);
            pageLayout->addSpacing(30);

            auto grid = new QGridLayout;
            grid->setContentsMargins(0, 0, 0, 0);
            grid->setHorizontalSpacing(16);
            grid->setVerticalSpacing(16);

            grid->addWidget(createPanel(
                "What changed",
                "<ul>"
                "<li>Native macOS ARM64 build is working.</li>"
                "<li>The old promotional welcome screen is gone.</li>"
                "<li>Connection testing shows which driver family is active.</li>"
                "<li>MongoDB 8 direct connections can use the modern driver path.</li>"
                "</ul>"), 0, 0);

            grid->addWidget(createPanel(
                "Driver plan",
                "<p>The app carries both the legacy MongoDB 4.2 path and a modern "
                "mongocxx path. Direct non-SSH/non-SSL connections can move through "
                "the modern backend while legacy fallback remains available.</p>"), 0, 1);

            grid->addWidget(createPanel(
                "Why revive it",
                "<p>dino-robomongo keeps the direct, native, shell-centric workflow "
                "and removes the stale promotional surface.</p>"), 1, 0);

            grid->addWidget(createPanel(
                "Compatibility target",
                "<ul>"
                "<li>Legacy backend: MongoDB 3.6 and older-compatible flows.</li>"
                "<li>Modern backend: MongoDB 6, 7, and 8 direct-connection work.</li>"
                "<li>Cross-platform builds: macOS first, then Linux x64 and Windows x64.</li>"
                "</ul>"), 1, 1);

            pageLayout->addLayout(grid);
            pageLayout->addSpacing(30);

            pageLayout->addWidget(createLabel(
                "This is a revival build. The modern driver is already present; "
                "the remaining work is expanding coverage across shell/query and "
                "advanced connection modes.",
                "revivalFooter"));
            pageLayout->addStretch();

            content->setStyleSheet(QStringLiteral(
                "#revivalPage {"
                "  background: #f6f7f8;"
                "  color: #202124;"
                "}"
                "#revivalMark {"
                "  background: #23b66f;"
                "  border-radius: 12px;"
                "  color: white;"
                "  font-size: 36px;"
                "  font-weight: 800;"
                "}"
                "#revivalTitle {"
                "  color: #202124;"
                "  font-size: 42px;"
                "  font-weight: 750;"
                "}"
                "#revivalSubtitle {"
                "  color: #4d5156;"
                "  font-size: 20px;"
                "  line-height: 145%;"
                "}"
                "#revivalStatus {"
                "  background: #fff8dc;"
                "  border: 1px solid #e4c65a;"
                "  border-radius: 6px;"
                "  color: #684b00;"
                "  font-size: 14px;"
                "  font-weight: 650;"
                "  padding: 8px 11px;"
                "}"
                "#revivalPanel {"
                "  background: white;"
                "  border: 1px solid #dde1e6;"
                "  border-radius: 8px;"
                "}"
                "#revivalPanelTitle {"
                "  color: #202124;"
                "  font-size: 17px;"
                "  font-weight: 700;"
                "}"
                "#revivalPanelBody {"
                "  color: #4d5156;"
                "  font-size: 14px;"
                "  line-height: 150%;"
                "}"
                "#revivalFooter {"
                "  color: #5f6368;"
                "  font-size: 14px;"
                "  line-height: 155%;"
                "}"
            ));

            return content;
        }
    }

    // ------------------ WelcomeTab
    WelcomeTab::WelcomeTab(QScrollArea *parent) :
        QWidget(parent), _parent(parent)
    {
        auto mainLayout = new QVBoxLayout;
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->addWidget(createWelcomeContent());
        setLayout(mainLayout);
    }

}

#else   // -------------------------------- Linux impl. ------------------------------------// 

#include <QObject>
#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDesktopServices>
#include <QScrollArea>
#include <QScrollBar>
#include <QEvent>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QToolButton>
#include <QMenu>

#include "robomongo/core/AppRegistry.h"
#include "robomongo/gui/GuiRegistry.h"
#include "robomongo/gui/MainWindow.h"
#include "robomongo/core/domain/App.h"
#include "robomongo/core/EventBus.h"
#include "robomongo/core/settings/SettingsManager.h"
#include "robomongo/core/settings/ConnectionSettings.h"
#include "robomongo/core/utils/Logger.h"       
#include "robomongo/core/utils/QtUtils.h"
#include "robomongo/utils/common.h"

namespace Robomongo
{
    bool deleteOldCacheFile(QString const& absFilePath);

    bool saveIntoCache(QString const& fileName, QString const& fileData,
                       QString const& lastModifiedKey, QString const& lastModifiedDate);

    bool saveIntoCache(QString const& fileName, QPixmap const& pixMap,
                       QString const& lastModifiedKey, QString const& lastModifiedDate);

    bool saveIntoCache(QString const& fileName, QByteArray* data,
                       QString const& lastModifiedKey, QString const& lastModifiedDate);

    /**
    * @brief Container structure to hold data of a blog link
    */
    struct BlogInfo
    {
        BlogInfo(QString const& title, QString const& link, QString const& publishDate)
            : title(title), link(link), publishDate(publishDate) {}

        QString const title;
        QString const link;
        QString const publishDate;
    };

    /**
    * @brief Custom label for identifying a blog link label
    */
    struct BlogLinkLabel : public QLabel
    {
        BlogLinkLabel(QString const& args) 
            : QLabel(args) {}
    };

    QString const WhatsNew = "<p><h1><font color=\"#2d862d\">%1</h1></p>";
    QString const BlogsHeader = "<p><h1><font color=\"#2d862d\">Blog Posts</h1></p>";
    QString const BlogLinkTemplate = "<a style = 'color: #106CD6; text-decoration: none;'"
                                     "href='%1'>%2</a>";

    // For info only. Starting from 1.2.1, PROJECT_VERSION is used.
    // URL Folder number for Pic1 and Text1
    enum {
        URL_FOLDER_1_0_0      = 1,
        URL_FOLDER_1_1_0_BETA = 2,
        URL_FOLDER_1_1_1      = 3,
        URL_FOLDER_1_2_0_BETA = 4,        
        // Starting from 1.2.1, PROJECT_VERSION is used.
    };

    QString const IMAGE_PATH = QString(PROJECT_VERSION) + "/image.png";
    QString const CONTENTS_PATH = QString(PROJECT_VERSION) + "/contents.txt";

    QString const RssFileName = "rss.xml";

    QString const Text1_LastModifiedDateKey("wtText1LastModifiedDate");
    QString const Image1_LastModifiedDateKey("wtImage1LastModifiedDate");
    QString const Rss_LastModifiedDateKey("wtRssLastModifiedDate");

    auto const TEXT_TO_TAB_RATIO = 0.6; 
    auto const IMAGE_TO_TAB_RATIO = 0.25;
    auto const BLOG_TO_TAB_RATIO = 0.28;

/* ------------------------------------- Welcome Tab --------------------------------------- */

    WelcomeTab::WelcomeTab(QScrollArea *parent) :
        QWidget(parent), _parent(parent)
    {
        _pic1_URL = "https://files.studio3t.com/rm-feed_3t_io/" + IMAGE_PATH;
        _text1_URL = "https://files.studio3t.com/rm-feed_3t_io/" + CONTENTS_PATH;
        _rss_URL = "https://blog.robomongo.org/rss/";

#ifdef __APPLE__
        constexpr int HEADER_POINT_SIZE = 10;
#else 
        constexpr int HEADER_POINT_SIZE = 7;
#endif                
        //// What's new section
        _whatsNewHeader = new QLabel;
        _whatsNewHeader->setHidden(true);
        QFont headerFont { _whatsNewHeader->font() };
        headerFont.setPointSize(HEADER_POINT_SIZE);
        _whatsNewHeader->setFont(headerFont);

        // _whatsNewHeader->setFont
        _whatsNewText = new QLabel;
        _whatsNewText->setTextInteractionFlags(Qt::TextSelectableByMouse);
        _whatsNewText->setTextFormat(Qt::RichText);
        _whatsNewText->setTextInteractionFlags(Qt::TextBrowserInteraction);
        _whatsNewText->setOpenExternalLinks(true);
        _whatsNewText->setWordWrap(true);
        _whatsNewText->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        _pic1 = new QLabel;
        _pic1->setTextInteractionFlags(Qt::TextSelectableByMouse);
        _pic1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        _pic1->setScaledContents(true);

        _blogsHeader = new QLabel(BlogsHeader);
        _blogsHeader->setHidden(true);
        _blogsHeader->setFont(headerFont);

        //// --- Network Access Managers
        if (!AppRegistry::instance().settingsManager()->disableHttpsFeatures()) {
            auto text1Downloader = new QNetworkAccessManager;
            VERIFY(connect(text1Downloader, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(on_downloadTextReply(QNetworkReply*))));
            text1Downloader->head(QNetworkRequest(_text1_URL));

            auto pic1Downloader = new QNetworkAccessManager;
            VERIFY(connect(pic1Downloader, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(on_downloadPictureReply(QNetworkReply*))));
            pic1Downloader->head(QNetworkRequest(_pic1_URL));

            auto rssDownloader = new QNetworkAccessManager;
            VERIFY(connect(rssDownloader, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(on_downloadRssReply(QNetworkReply*))));
            rssDownloader->get(QNetworkRequest(_rss_URL));
        }

        //// --- Layouts
        _allBlogsButton = new QPushButton("All Blog Posts");
        _allBlogsButton->setHidden(true);
        _allBlogsButton->setStyleSheet("color: #106CD6");
        VERIFY(connect(_allBlogsButton, SIGNAL(clicked()), this, SLOT(on_allBlogsButton_clicked())));
        _allBlogsButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);


        _blogLinksLay = new QVBoxLayout;
        _blogLinksLay->setAlignment(Qt::AlignLeft);

        auto rightLayout = new QVBoxLayout;
        rightLayout->setContentsMargins(20, -1, -1, -1);
        rightLayout->addWidget(_blogsHeader, 0, Qt::AlignTop);
        rightLayout->addLayout(_blogLinksLay);
        rightLayout->addSpacing(15);
        rightLayout->addWidget(_allBlogsButton, 0, Qt::AlignLeft);
        rightLayout->addStretch();

        auto leftLayout = new QVBoxLayout;
        leftLayout->addWidget(_whatsNewHeader, 0, Qt::AlignTop);
        leftLayout->addWidget(_pic1, 0, Qt::AlignTop);
        leftLayout->addWidget(_whatsNewText, 0, Qt::AlignTop);
        leftLayout->addStretch();
        leftLayout->setSizeConstraint(QLayout::SetMinimumSize);

        auto mainLayout = new QHBoxLayout;
        mainLayout->setContentsMargins(20, 20, -1, -1);
        mainLayout->addLayout(leftLayout);
        mainLayout->addSpacing(20);
        mainLayout->addLayout(rightLayout);
        mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

        setLayout(mainLayout);
    }

    WelcomeTab::~WelcomeTab()
    {

    }

    void WelcomeTab::on_downloadTextReply(QNetworkReply* reply)
    {
        auto hideOrShowWhatsNewHeader = [this]() {
            if ((!_pic1->pixmap() || _pic1->pixmap()->isNull()) && _whatsNewText->text().isEmpty())
                _whatsNewHeader->setHidden(true);
            else 
                _whatsNewHeader->setVisible(true);
        };

        if (reply->operation() == QNetworkAccessManager::HeadOperation) {
            if (reply->error() == QNetworkReply::NoError) { // No network error
                QString const& createDate = reply->header(QNetworkRequest::LastModifiedHeader).toString();
                if (createDate == AppRegistry::instance().settingsManager()->cacheData(Text1_LastModifiedDateKey)
                    && fileExists(CacheDir + _text1_URL.fileName()))
                {
                    // Load from cache
                    QFile file(CacheDir + _text1_URL.fileName());
                    if (!file.open(QFile::ReadOnly | QFile::Text))
                        return;

                    QTextStream in(&file);
                    QString str(in.readAll());
                    setWhatsNewHeaderAndText(str);
                    hideOrShowWhatsNewHeader();
                    return;
                }
                else {  // Get from internet
                    reply->manager()->get(QNetworkRequest(_text1_URL));
                }
            }
            else {  // There is a network error 
                // Load from cache
                QFile file(CacheDir + _text1_URL.fileName());
                if (!file.open(QFile::ReadOnly | QFile::Text))
                    return;

                QTextStream in(&file);
                QString str(in.readAll());
                setWhatsNewHeaderAndText(str);
                hideOrShowWhatsNewHeader();
                return;
            }
        }
        else if (reply->operation() == QNetworkAccessManager::GetOperation) {
            // todo: handle get operation fails
            QString str(QUrl::fromPercentEncoding(reply->readAll()));
            if (str.isEmpty()) {
                LOG_MSG("WelcomeTab: Failed to download text file from URL. Reason: " + reply->errorString(),
                    mongo::logger::LogSeverity::Warning());
                hideOrShowWhatsNewHeader();
                return;
            }

            setWhatsNewHeaderAndText(str);
            hideOrShowWhatsNewHeader();
            saveIntoCache(_text1_URL.fileName(), str, Text1_LastModifiedDateKey,
                          reply->header(QNetworkRequest::LastModifiedHeader).toString());
        }
    }

    void WelcomeTab::on_downloadPictureReply(QNetworkReply* reply)
    {
        auto const FIFTY_PERCENT_OF_TAB = _parent->width() * IMAGE_TO_TAB_RATIO;

        QPixmap image;
        auto hideOrShowWhatsNewHeader = [this]() {
            if ((!_pic1->pixmap() || _pic1->pixmap()->isNull()) && _whatsNewText->text().isEmpty())
                _whatsNewHeader->setHidden(true);
            else
                _whatsNewHeader->setVisible(true);
        };

        // Network error, load from cache
        if (reply->error() != QNetworkReply::NoError) { 
            image = QPixmap(CacheDir + _pic1_URL.fileName());
            if (image.isNull())
                return;

            _image = image;
            _pic1->setPixmap(_image);

            if (0 == _image.size().width())
                return;

            _pic1->setFixedSize(FIFTY_PERCENT_OF_TAB, (FIFTY_PERCENT_OF_TAB / _image.size().width()) * _image.size().height());
            adjustSize();
            hideOrShowWhatsNewHeader();
            return;
        }

        // No network error
        if (reply->operation() == QNetworkAccessManager::HeadOperation) {
            QString const& createDate = reply->header(QNetworkRequest::LastModifiedHeader).toString();
            // If the file in URL is not newer load from cache, otherwise get from internet
            if (createDate == AppRegistry::instance().settingsManager()->cacheData(Image1_LastModifiedDateKey)
                && fileExists(CacheDir + _pic1_URL.fileName())) 
            {
                image = QPixmap(CacheDir + _pic1_URL.fileName());    // Load from cache
            }
            else {   // Get from internet
                reply->manager()->get(QNetworkRequest(_pic1_URL));
                return;
            }
        }
        else if (reply->operation() == QNetworkAccessManager::GetOperation) {
            image.loadFromData(reply->readAll());

            if (image.isNull()) {
                LOG_MSG("WelcomeTab: Failed to download image file from internet. Reason: " + reply->errorString(),
                         mongo::logger::LogSeverity::Warning());
                image = QPixmap(CacheDir + _pic1_URL.fileName());
            }
            else {
                saveIntoCache(_pic1_URL.fileName(), image, Image1_LastModifiedDateKey,
                              reply->header(QNetworkRequest::LastModifiedHeader).toString());
            }
        }

        // Set the image
        if (image.isNull())
            return;

        _image = image;

        if (0 == _image.size().width()) 
            return

        hideOrShowWhatsNewHeader();
        resize();
    }

    void WelcomeTab::on_downloadRssReply(QNetworkReply* reply)
    {
        auto const THIRTY_PERCENT_OF_TAB = _parent->width() * BLOG_TO_TAB_RATIO;

        QByteArray data = reply->readAll();
        if (data.isEmpty() || reply->error() != QNetworkReply::NoError) {
            // Load from cache
            QFile file(CacheDir + RssFileName);
            if (!file.open(QFile::ReadOnly | QFile::Text))
                return;

            data = file.readAll();
        }
        
        QXmlStreamReader xmlReader(data);

        int count = 0;
        int const MaxBlogCountShown = 10;
        QString title, link, pubDate;
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();
            if (xmlReader.isStartElement()) {
                if (xmlReader.name() == "title") 
                    title = xmlReader.readElementText();
                else if (xmlReader.name() == "link")
                    link = xmlReader.readElementText();
                else if (xmlReader.name() == "pubDate")
                    pubDate = xmlReader.readElementText().left(16);
                
                if (!pubDate.isEmpty()) {
                    auto blogLink = new BlogLinkLabel(BlogLinkTemplate.arg(link, title));
                    blogLink->setMouseTracking(true);
                    blogLink->setAttribute(Qt::WA_Hover);
                    blogLink->installEventFilter(this);
                    blogLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
                    blogLink->setOpenExternalLinks(true);
                    blogLink->setWordWrap(true);
                    blogLink->setMinimumWidth(THIRTY_PERCENT_OF_TAB);
                    _blogLinksLay->addWidget(blogLink);
                    _blogLinksLay->addWidget(new QLabel("<font color='gray'>" + pubDate + "</font>"));
                    _blogLinksLay->addSpacing(_blogLinksLay->spacing());
                    pubDate.clear();
                    ++count;
                    if (MaxBlogCountShown == count)
                        break;
                }
            }
        }

        _blogsHeader->setVisible(true);
        _allBlogsButton->setVisible(true);
        adjustSize();

        // Save into cache
        saveIntoCache(RssFileName, data, Rss_LastModifiedDateKey, "NotImplemented");    // todo
    }

    void WelcomeTab::on_allBlogsButton_clicked()
    {
        QDesktopServices::openUrl(QUrl("https://blog.robomongo.org/"));
    }

    void WelcomeTab::setWhatsNewHeaderAndText(QString const& str)
    {
        if (!str.contains("\n") || str.size() == 0)
            return;

        auto const firstNewLineIndex = str.indexOf("\n");
        auto const leftOfStr = str.left(firstNewLineIndex);
        auto const rightOfStr = str.right(str.size() - firstNewLineIndex - 7);
        _whatsNewHeader->setText(WhatsNew.arg(leftOfStr));
        _whatsNewText->setText(rightOfStr);
        auto const SIXTY_PERCENT_OF_TAB = _parent->width() * TEXT_TO_TAB_RATIO;
        _whatsNewText->setMaximumWidth(SIXTY_PERCENT_OF_TAB);
        adjustSize();
    }
    
    void WelcomeTab::resize()
    {
        auto const tabWidth = _parent->width();
        auto const FIFTY_PERCENT_OF_TAB = _parent->width() * IMAGE_TO_TAB_RATIO;

        _whatsNewText->setFixedWidth(tabWidth * TEXT_TO_TAB_RATIO);
        _pic1->setPixmap(_image);

        if (0 == _image.size().width())
            return;

        _pic1->setFixedSize(FIFTY_PERCENT_OF_TAB, (FIFTY_PERCENT_OF_TAB / _image.size().width())*_image.size().height());

        _blogsHeader->setFixedWidth(tabWidth * BLOG_TO_TAB_RATIO);
        for (int i = 0; i < _blogLinksLay->count(); ++i) {
            if(auto wid = _blogLinksLay->itemAt(i)->widget())
                wid->setFixedWidth(tabWidth * BLOG_TO_TAB_RATIO);
        }

        adjustSize();
    }

    bool WelcomeTab::eventFilter(QObject *target, QEvent *event)
    {
        auto blogLinkLabel = dynamic_cast<BlogLinkLabel*>(target);
        // Make blog link underlined on mouse hover
        if (blogLinkLabel) {
            if (event->type() == QEvent::HoverEnter) {
                blogLinkLabel->setText(blogLinkLabel->text().replace("text-decoration: none;", "text-decoration: ;"));
                setCursor(Qt::PointingHandCursor);
                return true;
            }
            else  if (event->type() == QEvent::HoverLeave) {
                blogLinkLabel->setText(blogLinkLabel->text().replace("text-decoration: ;", "text-decoration: none;"));
                setCursor(Qt::ArrowCursor);
                return true;
            }
        }
        return QWidget::eventFilter(target, event);
    }

    bool deleteOldCacheFile(QString const& absFilePath)
    {
        if (!fileExists(absFilePath))
            return true;

        if (!QFile::remove(absFilePath)) {
            LOG_MSG("WelcomeTab: Failed to delete cached file at: " + absFilePath,
                     mongo::logger::LogSeverity::Warning());
            return false;
        }

        return true;
    }

    bool saveIntoCache(QString const& fileName, QString const& fileData,
                       QString const& lastModifiedKey, QString const& lastModifiedDate)
    {
        if (!QDir(CacheDir).exists())
            QDir().mkdir(CacheDir);
        else 
            deleteOldCacheFile(CacheDir + fileName);

        QFile file(CacheDir + fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);
        out << fileData;

        // Save file's last modified date into settings
        AppRegistry::instance().settingsManager()->addCacheData(lastModifiedKey, lastModifiedDate);
        AppRegistry::instance().settingsManager()->save();
        return true;
    }

    bool saveIntoCache(QString const& fileName, QPixmap const& pixMap,
                       QString const& lastModifiedKey, QString const& lastModifiedDate)
    {
        if (!QDir(CacheDir).exists())
            QDir().mkdir(CacheDir);
        else 
            deleteOldCacheFile(CacheDir + fileName);

        QFile file(CacheDir + fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        pixMap.save(CacheDir + fileName);

        // Save file's last modified date into settings
        AppRegistry::instance().settingsManager()->addCacheData(lastModifiedKey, lastModifiedDate);
        AppRegistry::instance().settingsManager()->save();
        return true;
    }

    bool saveIntoCache(QString const& fileName, QByteArray* data,
                       QString const& lastModifiedKey, QString const& lastModifiedDate)
    {
        if (!QDir(CacheDir).exists())
            QDir().mkdir(CacheDir);
        else 
            deleteOldCacheFile(CacheDir + fileName);

        QFile file(CacheDir + fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        file.write(*data);

        // Save file's last modified date into settings
        AppRegistry::instance().settingsManager()->addCacheData(lastModifiedKey, lastModifiedDate);
        AppRegistry::instance().settingsManager()->save();
        return true;
    }

}

#endif // -------------------------------- end of Linux impl. ------------------------------------// 
