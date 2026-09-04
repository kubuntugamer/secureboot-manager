#include "mokaboutdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout> // 🔍 Added for the horizontal alignment row track
#include <QLabel>
#include <QPushButton>

MokAboutDialog::MokAboutDialog(QWidget *parent) : QDialog(parent)
{
    this->setFixedSize(440, 480);
    this->setWindowTitle("About Secure Boot Manager");

    QVBoxLayout *masterLayout = new QVBoxLayout(this);
    masterLayout->setContentsMargins(20, 20, 20, 20);
    masterLayout->setSpacing(12);

    // 🎨 BRANDING: Vector Shield Asset Frame
    QLabel *logoLabel = new QLabel(this);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setPixmap(QPixmap(":/assets/kdesbm_logo.jpeg").scaled(400, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    masterLayout->addWidget(logoLabel);

    // 📝 TEXT CONTENT BLOCK: Version & Metadata
    QLabel *textLabel = new QLabel(this);
    textLabel->setTextFormat(Qt::RichText);
    textLabel->setWordWrap(true);
    textLabel->setText(
        "<p style='font-size:11px; margin-top:5px; color:#bdc3c7;'> "
        "<b>Version:</b> 0.1.0-9<br>"
        "<b>Maintainer:</b> chazz &lt;chasbigham@gmail.com&gt;</p>"
        "<hr style='border:0; border-top:1px solid #3f4142;'>"
        "<p style='font-size:11px; color:#bdc3c7;'>Licensed under GNU GPLv3 liability protection rules.</p>"
    );
    masterLayout->addWidget(textLabel);

    // 🚀 STEP 1: Push everything above this line to the absolute top ceiling
    masterLayout->addStretch(1);

    // 🚀 STEP 2: Create a bottom horizontal row to pin the button to the right
    QHBoxLayout *buttonRowLayout = new QHBoxLayout();

    QPushButton *closeBtn = new QPushButton("Close", this);
    closeBtn->setMinimumHeight(35);
    closeBtn->setMinimumWidth(100); // Give the 5-letter button a solid structural base width
    closeBtn->setMaximumWidth(120);

    buttonRowLayout->addStretch(1);     // Left-side spring pushes button to the right margin
    buttonRowLayout->addWidget(closeBtn); // Attach button on the far right

    // Nest the horizontal row inside our master vertical frame
    masterLayout->addLayout(buttonRowLayout);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}
