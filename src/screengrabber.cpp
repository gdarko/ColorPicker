// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors, 2022 Darko Gjorgjijoski
// Adopted from Flameshoot to support Qt6

#include "screengrabber.h"
#include "qguiappcurrentscreen.h"
#include <QApplication>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>

#if defined(Q_OS_LINUX)
#include "request.h"
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QUrl>
#include <QUuid>
#endif

ScreenGrabber::ScreenGrabber(QObject* parent)
  : QObject(parent)
{}

void ScreenGrabber::freeDesktopPortal(bool& ok, QPixmap& res)
{

#if defined(Q_OS_LINUX)
    QDBusInterface screenshotInterface(
      QStringLiteral("org.freedesktop.portal.Desktop"),
      QStringLiteral("/org/freedesktop/portal/desktop"),
      QStringLiteral("org.freedesktop.portal.Screenshot"));

    // unique token
    QString token =
      QUuid::createUuid().toString().remove('-').remove('{').remove('}');

    // premake interface
    auto* request = new OrgFreedesktopPortalRequestInterface(
      QStringLiteral("org.freedesktop.portal.Desktop"),
      "/org/freedesktop/portal/desktop/request/" +
        QDBusConnection::sessionBus().baseService().remove(':').replace('.',
                                                                        '_') +
        "/" + token,
      QDBusConnection::sessionBus(),
      nullptr);

    QEventLoop loop;
    const auto gotSignal = [&res, &loop](uint status, const QVariantMap& map) {
        if (status == 0) {
            // Parse this as URI to handle unicode properly
            QUrl uri = map.value("uri").toString();
            QString uriString = uri.toLocalFile();
            res = QPixmap(uriString);
            res.setDevicePixelRatio(qApp->devicePixelRatio());
            QFile imgFile(uriString);
            imgFile.remove();
        }
        loop.quit();
    };

    // prevent racy situations and listen before calling screenshot
    QMetaObject::Connection conn = QObject::connect(
      request, &org::freedesktop::portal::Request::Response, gotSignal);

    screenshotInterface.call(
      QStringLiteral("Screenshot"),
      "",
      QMap<QString, QVariant>({ { "handle_token", QVariant(token) },
                                { "interactive", QVariant(false) } }));

    loop.exec();
    QObject::disconnect(conn);
    request->Close().waitForFinished();
    delete request;

    if (res.isNull()) {
        ok = false;
    }
#endif
}
QPixmap ScreenGrabber::grabEntireDesktop(bool& ok)
{
    ok = true;
#if defined(Q_OS_MACOS)
    QScreen* currentScreen = QGuiAppCurrentScreen().currentScreen();
    QPixmap screenPixmap(
      currentScreen->grabWindow(0,
                                currentScreen->geometry().x(),
                                currentScreen->geometry().y(),
                                currentScreen->geometry().width(),
                                currentScreen->geometry().height()));
    screenPixmap.setDevicePixelRatio(currentScreen->devicePixelRatio());
    return screenPixmap;
#elif defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    if (m_info.waylandDetected()) {
        QPixmap res;
        // On KDE, try KWin's native screenshot API (silent, no dialog)
        if (m_info.windowManager() == DesktopInfo::KDE) {
            if (kwinCapture(ok, res)) {
                return res;
            }
            qDebug() << "KWin capture not available, trying fallbacks";
        }
        // Try fallback screenshot tools (silent, no dialog)
        if (fallbackCapture(ok, res)) {
            return res;
        }
        // Fall back to D-Bus portal (may show permission dialog)
        ok = true;
        freeDesktopPortal(ok, res);
        if (!ok) {
           qInfo() << "Unable to capture screen";
        }
        return res;
    }
#endif
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    QRect geometry = desktopGeometry();
    QPixmap p(QGuiApplication::primaryScreen()->grabWindow(
      0,
      geometry.x(),
      geometry.y(),
      geometry.width(),
      geometry.height()));
    /*
    auto screenNumber = QApplication::desktop()->screenNumber();
    QScreen* screen = QApplication::screens()[screenNumber];*/

    QScreen* screen = QGuiAppCurrentScreen().currentScreen();

    p.setDevicePixelRatio(screen->devicePixelRatio());
    return p;
#endif
}

QRect ScreenGrabber::screenGeometry(QScreen* screen)
{
    QPixmap p;
    QRect geometry;
    if (m_info.waylandDetected()) {
        QPoint topLeft(0, 0);
#ifdef Q_OS_WIN
        for (QScreen* const screen : QGuiApplication::screens()) {
            QPoint topLeftScreen = screen->geometry().topLeft();
            if (topLeft.x() > topLeftScreen.x() ||
                topLeft.y() > topLeftScreen.y()) {
                topLeft = topLeftScreen;
            }
        }
#endif
        geometry = screen->geometry();
        geometry.moveTo(geometry.topLeft() - topLeft);
    } else {
        QScreen* currentScreen = QGuiAppCurrentScreen().currentScreen();
        geometry = currentScreen->geometry();
    }
    return geometry;
}

QPixmap ScreenGrabber::grabScreen(QScreen* screen, bool& ok)
{
    if (m_info.waylandDetected()) {
        QPixmap p;

        QRect geometry = screenGeometry(screen);
        p = grabEntireDesktop(ok);
        if (ok) {
            return p.copy(geometry);
        }
        return p;
    } else {
        ok = true;
        return screen->grabWindow(0);
    }
}

QRect ScreenGrabber::desktopGeometry()
{
    QRect geometry;

    for (QScreen* const screen : QGuiApplication::screens()) {
        QRect scrRect = screen->geometry();
        scrRect.moveTo(scrRect.x() / screen->devicePixelRatio(),
                       scrRect.y() / screen->devicePixelRatio());
        geometry = geometry.united(scrRect);
    }
    return geometry;
}

bool ScreenGrabber::isWayland() {
#if defined(Q_OS_LINUX) || (defined(Q_OS_UNIX) && !defined(Q_OS_MAC))
    return (bool) m_info.waylandDetected();
#else
    return false;
#endif
}

QString ScreenGrabber::findFallbackTool()
{
#if defined(Q_OS_LINUX)
    static const QStringList tools = {
        QStringLiteral("grim"),
        QStringLiteral("spectacle"),
        QStringLiteral("gnome-screenshot"),
        QStringLiteral("flameshot")
    };
    for (const auto& tool : tools) {
        QString path = QStandardPaths::findExecutable(tool);
        if (!path.isEmpty()) {
            return tool;
        }
    }
#endif
    return QString();
}

bool ScreenGrabber::hasFallbackTools()
{
    if (!m_fallbackToolScanned) {
        m_cachedFallbackTool = findFallbackTool();
        m_fallbackToolScanned = true;
    }
    return !m_cachedFallbackTool.isEmpty();
}

QString ScreenGrabber::detectedFallbackTool()
{
    if (!m_fallbackToolScanned) {
        m_cachedFallbackTool = findFallbackTool();
        m_fallbackToolScanned = true;
    }
    return m_cachedFallbackTool;
}

bool ScreenGrabber::executeScreenshotTool(const QString& tool, const QString& outputPath, bool& ok, QPixmap& res)
{
#if defined(Q_OS_LINUX)
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);

    if (tool == QStringLiteral("grim")) {
        process.start(QStringLiteral("grim"), { outputPath });
    } else if (tool == QStringLiteral("spectacle")) {
        process.start(QStringLiteral("spectacle"), { QStringLiteral("-bn"), QStringLiteral("-o"), outputPath });
    } else if (tool == QStringLiteral("gnome-screenshot")) {
        process.start(QStringLiteral("gnome-screenshot"), { QStringLiteral("-f"), outputPath });
    } else if (tool == QStringLiteral("flameshot")) {
        // flameshot outputs raw PNG to stdout
        process.start(QStringLiteral("flameshot"), { QStringLiteral("full"), QStringLiteral("--raw") });
    } else {
        ok = false;
        return false;
    }

    if (!process.waitForFinished(5000)) {
        qInfo() << "Screenshot tool" << tool << "timed out";
        ok = false;
        return false;
    }

    if (process.exitCode() != 0) {
        qInfo() << "Screenshot tool" << tool << "failed with exit code" << process.exitCode();
        ok = false;
        return false;
    }

    if (tool == QStringLiteral("flameshot")) {
        QByteArray data = process.readAllStandardOutput();
        if (!res.loadFromData(data, "PNG")) {
            qInfo() << "Failed to load screenshot from flameshot stdout";
            ok = false;
            return false;
        }
    } else {
        res = QPixmap(outputPath);
        if (res.isNull()) {
            qInfo() << "Failed to load screenshot from" << outputPath;
            ok = false;
            return false;
        }
    }

    res.setDevicePixelRatio(qApp->devicePixelRatio());
    ok = true;
    return true;
#else
    Q_UNUSED(tool)
    Q_UNUSED(outputPath)
    ok = false;
    return false;
#endif
}

bool ScreenGrabber::fallbackCapture(bool& ok, QPixmap& res)
{
#if defined(Q_OS_LINUX)
    if (!hasFallbackTools()) {
        return false;
    }

    static const QStringList tools = {
        QStringLiteral("grim"),
        QStringLiteral("spectacle"),
        QStringLiteral("gnome-screenshot"),
        QStringLiteral("flameshot")
    };

    for (const auto& tool : tools) {
        if (QStandardPaths::findExecutable(tool).isEmpty())
            continue;

        QString tempPath;

        // flameshot reads from stdout, no temp file needed
        if (tool != QStringLiteral("flameshot")) {
            QTemporaryFile tmpFile(QDir::tempPath() + QStringLiteral("/colorpicker_XXXXXX.png"));
            tmpFile.setAutoRemove(false);
            if (!tmpFile.open())
                continue;
            tempPath = tmpFile.fileName();
            tmpFile.close();
        }

        if (executeScreenshotTool(tool, tempPath, ok, res)) {
            if (!tempPath.isEmpty())
                QFile::remove(tempPath);
            return true;
        }

        // Clean up temp file on failure before trying next tool
        if (!tempPath.isEmpty())
            QFile::remove(tempPath);
    }

    return false;
#else
    Q_UNUSED(ok)
    Q_UNUSED(res)
    return false;
#endif
}

bool ScreenGrabber::kwinCapture(bool& ok, QPixmap& res)
{
#if defined(Q_OS_LINUX)
    // Create a temp file for KWin to write the screenshot into
    QTemporaryFile tmpFile;
    if (!tmpFile.open()) {
        ok = false;
        return false;
    }

    // KWin ScreenShot2 expects the CALLER to provide a writable fd
    QDBusUnixFileDescriptor dbusWriteFd(tmpFile.handle());

    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.kde.KWin"),
        QStringLiteral("/org/kde/KWin/ScreenShot2"),
        QStringLiteral("org.kde.KWin.ScreenShot2"),
        QStringLiteral("CaptureWorkspace"));

    QVariantMap options;
    options[QStringLiteral("include-cursor")] = false;
    options[QStringLiteral("native-resolution")] = true;

    msg << QVariant::fromValue(options) << QVariant::fromValue(dbusWriteFd);

    QDBusMessage reply = QDBusConnection::sessionBus().call(msg, QDBus::Block, 5000);
    if (reply.type() != QDBusMessage::ReplyMessage) {
        ok = false;
        return false;
    }

    // Reply contains raw image metadata: width, height, stride, format
    QVariantMap resultMap = reply.arguments().first().value<QVariantMap>();
    uint width = resultMap[QStringLiteral("width")].toUInt();
    uint height = resultMap[QStringLiteral("height")].toUInt();
    uint stride = resultMap[QStringLiteral("stride")].toUInt();
    uint format = resultMap[QStringLiteral("format")].toUInt();

    if (width == 0 || height == 0 || stride == 0) {
        ok = false;
        return false;
    }

    // Read raw pixel data written by KWin
    tmpFile.seek(0);
    QByteArray rawData = tmpFile.readAll();
    tmpFile.close();

    if (rawData.isEmpty()) {
        ok = false;
        return false;
    }

    // Construct QImage from raw pixel data using KWin's metadata
    QImage img(reinterpret_cast<const uchar*>(rawData.constData()),
               width, height, stride,
               static_cast<QImage::Format>(format));
    img = img.copy(); // deep copy before rawData goes out of scope

    res = QPixmap::fromImage(img);
    res.setDevicePixelRatio(qApp->devicePixelRatio());
    ok = true;
    return true;
#else
    Q_UNUSED(ok)
    Q_UNUSED(res)
    return false;
#endif
}
