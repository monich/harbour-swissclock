/*
 * Copyright (C) 2014-2023 Slava Monich <slava@monich.com>
 * Copyright (C) 2014-2016 Jolla Ltd.
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3. Neither the names of the copyright holders nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "QuickClock.h"
#include "ClockSettings.h"
#include "ClockDebug.h"

#include "HarbourBattery.h"
#include "HarbourDisplayBlanking.h"

#include <QtGui>
#include <QtQuick>
#include <sailfishapp.h>

#define REGISTER_SINGLETON(class,uri,v1,v2) \
    qmlRegisterSingletonType<class>(uri, v1, v2, #class, class::createSingleton)
#define REGISTER_TYPE(class,uri,v1,v2) \
    qmlRegisterType<class>(uri, v1, v2, #class)

void registerClockTypes(const char* uri, int v1 = 1, int v2 = 0)
{
    qmlRegisterType<QuickClock>(uri, v1, v2, "Clock");
    REGISTER_SINGLETON(ClockSettings, uri, v1, v2);
    REGISTER_SINGLETON(HarbourBattery, uri, v1, v2);
    REGISTER_TYPE(HarbourDisplayBlanking, uri, v1, v2);
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    int result = 0;
    QGuiApplication* app = SailfishApp::application(argc, argv);

    // Load translations
    QLocale locale;
    QTranslator* translator = new QTranslator(app);
#ifdef OPENREPOS
    QString transDir("/usr/share/translations");
#else
    QString transDir = SailfishApp::pathTo("translations").toLocalFile();
#endif
    QString transFile(CLOCK_APP_NAME);
    if (translator->load(locale, transFile, "-", transDir) ||
        translator->load(transFile, transDir)) {
        app->installTranslator(translator);
    } else {
        HDEBUG("Failed to load translator for" << locale);
        delete translator;
    }

    //setenv("QUICK_CLOCK_TIME", "10:25:13.500", true);
    registerClockTypes("harbour.swissclock", 1, 0);

    // Create and set up the view
    QQuickView* view = SailfishApp::createView();
    // Enable multisampling. The maximum number of samples that makes any
    // difference on Jolla1 is 4. But let's try 16 anyway...
    QSurfaceFormat format = view->format();
    format.setSamples(16);
    view->setFormat(format);
    view->setSource(SailfishApp::pathTo(QString("qml/main.qml")));
    view->show();

    result = app->exec();

    delete view;
    delete app;
    return result;
}
