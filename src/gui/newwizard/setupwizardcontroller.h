/*
 * Copyright (C) Fabian Müller <fmueller@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#pragma once

#include "account.h"
#include "pages/abstractsetupwizardpage.h"
#include "setupwizardaccountbuilder.h"
#include "setupwizardwindow.h"
#include "syncmode.h"

#include <QDialog>
#include <optional>

namespace OCC::Wizard {
/**
 * This class is the backbone of the new setup wizard. It instantiates the required UI elements and fills them with the correct data. It also provides the public API for the settings UI.
 *
 * The new setup wizard uses dependency injection where applicable. The account object is created using the builder pattern.
 */
class SetupWizardController : public QObject
{
    Q_OBJECT

public:
    explicit SetupWizardController(QWidget *parent);
    ~SetupWizardController() noexcept override;

    /**
     * Provides access to the controller's setup wizard window.
     * @return pointer to window
     */
    SetupWizardWindow *window();

Q_SIGNALS:
    /**
     * Emitted when the wizard has finished. It passes the built account object.
     */
    void finished(AccountPtr newAccount, SyncMode syncMode);

private:
    void nextStep(std::optional<PageIndex> currentPage, std::optional<PageIndex> desiredPage);

    SetupWizardWindow *_wizardWindow;

    // keeping a pointer on the current page allows us to check whether the controller has been initialized yet
    // the pointer is also used to clean up the page
    QPointer<AbstractSetupWizardPage> _currentPage;

    SetupWizardAccountBuilder _accountBuilder;

    AccessManager *_accessManager;
};
}
