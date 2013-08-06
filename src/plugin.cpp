/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
 *
 * This file is part of the signon-apparmor-extension
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "plugin.h"
#include "access-control-manager.h"

#include <QDebug>

using namespace SignOn;

Plugin::Plugin(QObject *parent):
    QObject(parent)
{
    setObjectName(QLatin1String("apparmor-ac"));
}

AbstractAccessControlManager *
Plugin::accessControlManager(QObject *parent) const
{
    qDebug() << Q_FUNC_INFO;
    return new AccessControlManager(parent);
}
