/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/
#ifndef KWINDOWINFO_MAC_P_H
#define KWINDOWINFO_MAC_P_H

#include "kwindowinfo.h"
#include <Carbon/Carbon.h>
#include <QList>
#include <QString>

// bah, why do header files invade my namespace and define such normal words as check...
#ifdef check
#undef check
#endif

struct Q_DECL_HIDDEN KWindowInfo::Private {
    Private();
    ~Private();
    int ref;
    WId win;
    bool isLocal;
    AXUIElementRef axElement() const
    {
        return m_axWin;
    }
    void setAxElement(const AXUIElementRef &axWin);
    ProcessSerialNumber psn() const
    {
        return m_psn;
    }
    pid_t pid() const
    {
        return m_pid;
    }
    void setProcessSerialNumber(const ProcessSerialNumber &psn);
    QString name;
#ifdef Q_OS_MAC32
    FSSpec iconSpec;
#else
    FSRef iconSpec;
#endif
    bool loadedData;
    void updateData();
    AXUIElementRef m_axWin;
    QList<KWindowInfo::Private *> children;
    KWindowInfo::Private *parent;

private:
    Private(const Private &);
    void operator=(const Private &);
    ProcessSerialNumber m_psn;
    pid_t m_pid;
};

#endif
