#include "plateColorDlg.h"
#include "constants.h"


#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QColor>
#include <QString>
#include <QSpacerItem>
#include <QHBoxLayout>


plateColorDlg::plateColorDlg(QWidget* p) : QDialog(p)
{
  cntDefaultColor = sizeof(plateColors) / sizeof(plateColors[0]);

  m_labels = new QLabel*[cntDefaultColor];
  m_clrBox = new QLabel*[cntDefaultColor];
  m_btnChange = new QPushButton*[cntDefaultColor];
  m_rowLayout = new QHBoxLayout*[cntDefaultColor];

  setupUI();
}





void plateColorDlg::setupUI()
{
  if (this->objectName().isEmpty())
    this->setObjectName("plateColorDialog");
  this->resize(200, 500);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);

  // build button box...
  QHBoxLayout* btnLayout = new QHBoxLayout;
  QPushButton* btnOK = new QPushButton;
  btnOK->setText("OK");
  QPushButton* btnCancel = new QPushButton;
  btnCancel->setText("Cancel");
  btnLayout->addSpacerItem(new QSpacerItem(300, 16));
  btnLayout->addWidget(btnOK);
  btnLayout->addWidget(btnCancel);


  // build each row
  for (int32_t ndx = 0; ndx < cntDefaultColor; ndx++)
  {
    m_labels[ndx] = new QLabel();
    m_labels[ndx]->setObjectName(QString("plate%1").arg(ndx));
    m_labels[ndx]->setText(QString("plate%1").arg(ndx));
    m_labels[ndx]->resize(75, 16);

    m_clrBox[ndx] = new QLabel();
    m_clrBox[ndx]->setText(QString("plate%1").arg(ndx));
    QString   clrStr = QString("QLabel { background-color: %1; color: black;}").arg(plateColors[ndx].name());
    m_clrBox[ndx]->setStyleSheet(clrStr);
    m_clrBox[ndx]->resize(100, 16);

    m_btnChange[ndx] = new QPushButton();
    m_btnChange[ndx]->setObjectName(QString("button%1").arg(ndx));
    m_btnChange[ndx]->setText("change...");
    m_btnChange[ndx]->resize(75, 16);

    m_rowLayout[ndx] = new QHBoxLayout();
    m_rowLayout[ndx]->addWidget(m_labels[ndx]);
    m_rowLayout[ndx]->addWidget(m_clrBox[ndx]);
    m_rowLayout[ndx]->addWidget(m_btnChange[ndx]);
    m_rowLayout[ndx]->addSpacerItem(new QSpacerItem(200, 16));
  }

  for (int32_t ndx = 0; ndx < cntDefaultColor; ndx++)
  {
    mainLayout->addLayout(m_rowLayout[ndx]);
  }
  mainLayout->addLayout(btnLayout);

  connect(btnOK, &QPushButton::clicked, this, &plateColorDlg::onOK);
  connect(btnCancel, &QPushButton::clicked, this, &plateColorDlg::onCancel);

}

void plateColorDlg::onOK()
{

  QDialog::accept();
}

void plateColorDlg::onCancel()
{

  QDialog::reject();
}
