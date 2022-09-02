// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2022 Darko Gjorgjijoski <dg@darkog.com>

#include "request.h"
OrgFreedesktopPortalRequestInterface::OrgFreedesktopPortalRequestInterface(
  const QString& service,
  const QString& path,
  const QDBusConnection& connection,
  QObject* parent)
  : QDBusAbstractInterface(service,
                           path,
                           staticInterfaceName(),
                           connection,
                           parent)
{}

OrgFreedesktopPortalRequestInterface::~OrgFreedesktopPortalRequestInterface() =
  default;
