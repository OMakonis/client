/*
 * Copyright (C) by Klaas Freitag <freitag@owncloud.com>
 * Copyright (C) by Krzesimir Nowak <krzesimir@endocode.com>
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

#include <QDir>
#include <QFileDialog>
#include <QUrl>
#include <QTimer>
#include <QPushButton>
#include <QMessageBox>
#include <QMenu>
#include <QSsl>
#include <QSslCertificate>
#include <QNetworkAccessManager>
#include <QBuffer>

#include "QProgressIndicator.h"

#include "guiutility.h"
#include "wizard/owncloudwizardcommon.h"
#include "wizard/owncloudsetuppage.h"
#include "theme.h"
#include "account.h"

#include "ui_owncloudsetuppage.h"

namespace OCC {

OwncloudSetupPage::OwncloudSetupPage(QWidget *parent)
    : QWizardPage()
    , _ui(new Ui_OwncloudSetupPage)
    , _oCUrl()
    , _ocUser()
    , _authTypeKnown(false)
    , _checking(false)
    , _progressIndi(new QProgressIndicator(this))
{
    _ui->setupUi(this);
    _ocWizard = qobject_cast<OwncloudWizard *>(parent);

    Theme *theme = Theme::instance();
    setTitle(WizardCommon::titleTemplate().arg(tr("Connect to %1").arg(theme->appNameGUI())));
    setSubTitle(WizardCommon::subTitleTemplate().arg(tr("Setup %1 server").arg(theme->appNameGUI())));

    if (theme->overrideServerUrlV2().isEmpty()) {
        _ui->leUrl->setPostfix(theme->wizardUrlPostfix());
        _ui->leUrl->setPlaceholderText(theme->wizardUrlHint());
    } else {
        _ui->leUrl->setEnabled(false);
    }


    registerField(QLatin1String("OCUrl*"), _ui->leUrl);

    _ui->resultLayout->addWidget(_progressIndi);
    stopSpinner();

    slotUrlChanged(QString()); // don't jitter UI
    connect(_ui->leUrl, &QLineEdit::textChanged, this, &OwncloudSetupPage::slotUrlChanged);
    connect(_ui->leUrl, &QLineEdit::editingFinished, this, &OwncloudSetupPage::slotUrlEditFinished);
}

void OwncloudSetupPage::setServerUrl(const QString &newUrl)
{
    _oCUrl = newUrl;
    if (_oCUrl.isEmpty()) {
        _ui->leUrl->clear();
        return;
    }

    _ui->leUrl->setText(_oCUrl);
}

// slot hit from textChanged of the url entry field.
void OwncloudSetupPage::slotUrlChanged(const QString &url)
{
    _authTypeKnown = false;

    QString newUrl = url;
    if (url.endsWith("index.php")) {
        newUrl.chop(9);
    }
    if (_ocWizard && _ocWizard->account()) {
        QString webDavPath = _ocWizard->account()->davPath();
        if (url.endsWith(webDavPath)) {
            newUrl.chop(webDavPath.length());
        }
        if (webDavPath.endsWith(QLatin1Char('/'))) {
            webDavPath.chop(1); // cut off the slash
            if (url.endsWith(webDavPath)) {
                newUrl.chop(webDavPath.length());
            }
        }
    }
    if (newUrl != url) {
        _ui->leUrl->setText(newUrl);
    }

    if (!url.startsWith(QLatin1String("https://"))) {
        _ui->urlLabel->setPixmap(Utility::getCoreIcon(QStringLiteral("lock-http")).pixmap(_ui->urlLabel->size()));
        _ui->urlLabel->setToolTip(tr("This url is NOT secure as it is not encrypted.\n"
                                     "It is not advisable to use it."));
    } else {
        _ui->urlLabel->setPixmap(Utility::getCoreIcon(QStringLiteral("lock-https")).pixmap(_ui->urlLabel->size()));
        _ui->urlLabel->setToolTip(tr("This url is secure. You can use it."));
    }
}

void OwncloudSetupPage::slotUrlEditFinished()
{
    QString url = _ui->leUrl->fullText();
    if (QUrl(url).isRelative() && !url.isEmpty()) {
        // no scheme defined, set one
        url.prepend("https://");
        _ui->leUrl->setFullText(url);
    }
}

bool OwncloudSetupPage::isComplete() const
{
    return !_ui->leUrl->text().isEmpty() && !_checking;
}

void OwncloudSetupPage::initializePage()
{
    WizardCommon::initErrorLabel(_ui->errorLabel);

    _authTypeKnown = false;
    _checking = false;

    QAbstractButton *nextButton = wizard()->button(QWizard::NextButton);
    QPushButton *pushButton = qobject_cast<QPushButton *>(nextButton);
    if (pushButton)
        pushButton->setDefault(true);

    // If url is overriden by theme, it's already set and
    // we just check the server type and switch to second page
    // immediately.
    if (Theme::instance()->overrideServerUrlV2().isEmpty()) {
        _ui->leUrl->setFocus();
    } else {
        setCommitPage(true);
        // Hack: setCommitPage() changes caption, but after an error this page could still be visible
        setButtonText(QWizard::CommitButton, tr("&Next >"));
        validatePage();
    }
}

int OwncloudSetupPage::nextId() const
{
    switch (_ocWizard->authType()) {
    case DetermineAuthTypeJob::AuthType::Basic:
        return WizardCommon::Page_HttpCreds;
    case DetermineAuthTypeJob::AuthType::OAuth:
        return WizardCommon::Page_OAuthCreds;
    case DetermineAuthTypeJob::AuthType::Unknown:
        Q_UNREACHABLE();
    }
    return WizardCommon::Page_HttpCreds;
}

QString OwncloudSetupPage::url() const
{
    QString url = _ui->leUrl->fullText().simplified();
    return url;
}

bool OwncloudSetupPage::validatePage()
{
    if (!_authTypeKnown) {
        slotUrlEditFinished();
        QString u = url();
        QUrl qurl(u);
        if (!qurl.isValid() || qurl.host().isEmpty()) {
            setErrorString(tr("Invalid URL"));
            return false;
        }

        setErrorString(QString());
        _checking = true;
        startSpinner();
        emit completeChanged();

        emit determineAuthType(u);
        return false;
    } else {
        // connecting is running
        stopSpinner();
        _checking = false;
        emit completeChanged();
        return true;
    }
}

void OwncloudSetupPage::setAuthType()
{
    _authTypeKnown = true;
    stopSpinner();
}

void OwncloudSetupPage::setErrorString(const QString &err)
{
    if (err.isEmpty()) {
        _ui->errorLabel->setVisible(false);
    } else {
        _ui->errorLabel->setVisible(true);
        _ui->errorLabel->setText(err);
    }
    _checking = false;
    emit completeChanged();
    stopSpinner();
}

void OwncloudSetupPage::startSpinner()
{
    _ui->resultLayout->setEnabled(true);
    _progressIndi->setVisible(true);
    _progressIndi->startAnimation();
}

void OwncloudSetupPage::stopSpinner()
{
    _ui->resultLayout->setEnabled(false);
    _progressIndi->setVisible(false);
    _progressIndi->stopAnimation();
}

OwncloudSetupPage::~OwncloudSetupPage()
{
}

} // namespace OCC
