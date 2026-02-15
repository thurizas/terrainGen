#include "imageProps.h"
#include "terrainGen.h"
#include "hexagon.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSpacerItem>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QFrame>
#include <QDebug>


imagePropDlg::imagePropDlg(struct imageProps*  pip, bool doSave, QWidget* p, Qt::WindowFlags f) : QDialog(p, f), m_pParent(nullptr),m_bSave(doSave),m_pImageProps(pip)
{

   setupUI(doSave);

   if (nullptr != pip)
   {
     m_edtHeight->setText(QString("%1").arg(pip->imageHeight));
     m_edtWidth->setText(QString("%1").arg(pip->imageWidth));
     m_edtHexagonSize->setText(QString("%1").arg(pip->hexagonSize));

     if ((pip->hexagonProps & bFilled) == bFilled) m_cbxSolid->setChecked(true);
     if ((pip->hexagonProps & bColor) == bColor) m_cbxColor->setChecked(true);
     if ((pip->hexagonProps & bDispCenter) == bDispCenter) m_cbxCenter->setChecked(true);
     if ((pip->hexagonProps & bDispIndex) == bDispIndex) m_cbxIndex->setChecked(true);

     if (pip->hexagonOrient == CHexagon::VERTICAL)
     {
       m_vertical->setChecked(true);
     }
     else
     {
       m_horizontal->setChecked(true);
     }

     m_spnPlates->setValue(pip->cntPlates);
     for (int ndx = 0; ndx < nbrHarmonics; ndx++)
     {
       m_edtAmplitudes[ndx]->setText(QString("%1").arg(pip->amplitude[ndx]));
       m_edtFrequency[ndx]->setText(QString("%1").arg(pip->frequency[ndx]));
     }
   }
}




imagePropDlg::~imagePropDlg()
{


}



void imagePropDlg::setupUI(bool doSave)
{
    // declare font to be used in dialog...
    QFont font;
    font.setFamily("Courier New");
    font.setPointSize(10);

    // set up size policy...
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    

    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("Dialog"));
    this->setWindowTitle("Image properties");
    this->resize(569, 469);
    this->setSizePolicy(sizePolicy);

    // build layout widget
    QWidget* layoutWidget = new QWidget(this);
    layoutWidget->setObjectName("layoutWidget");
    layoutWidget->setGeometry(QRect(210, 430, 351, 33));

    // build button box
    QHBoxLayout* hboxLayout = new QHBoxLayout(layoutWidget);
    hboxLayout->setSpacing(6);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    
    QSpacerItem* spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    //QPushButton* btnSave = new QPushButton(layoutWidget);
    //btnSave->setObjectName("saveButton");
    //btnSave->setText("Save");
    //btnSave->setFont(font);
    //if (!doSave) btnSave->setEnabled(false);

    QPushButton* btnOK = new QPushButton(layoutWidget);
    btnOK->setObjectName("okButton");
    btnOK->setText("OK");
    btnOK->setFont(font);
    if (!doSave) btnOK->setEnabled(false);

    QPushButton* btnCancel = new QPushButton(layoutWidget);
    btnCancel->setObjectName("m_btnCancel");
    btnCancel->setText("Cancel");
    btnCancel->setFont(font); 

    hboxLayout->addItem(spacerItem);
    //hboxLayout->addWidget(btnSave);
    hboxLayout->addWidget(btnOK);    
    hboxLayout->addWidget(btnCancel);
    
    // build text entry fields
    QLabel* label1 = new QLabel(this);
    label1->setObjectName(QString::fromUtf8("label1"));
    label1->setGeometry(QRect(10, 10, 128, 16));
    label1->setText("image width (px)");
    label1->setFont(font);
    
    m_edtWidth = new QLineEdit(this);
    m_edtWidth->setObjectName(QString::fromUtf8("m_edtWidth"));
    m_edtWidth->setGeometry(QRect(160, 10, 73, 22));
    m_edtWidth->setFont(font);

    QLabel* label14 = new QLabel(this);
    label14->setObjectName("label14");
    label14->setGeometry(QRect(270, 10, 141, 16));
    label14->setText("Number of plates: ");
    label14->setFont(font);

    //m_edtPlates = new QLineEdit(this);
    //m_edtPlates->setObjectName("m_edtPlates");
    //m_edtPlates->setGeometry(QRect(420, 10, 51, 20));
    //m_edtPlates->setFont(font);

    m_spnPlates = new QSpinBox(this);
    m_spnPlates->setObjectName("m_spnPlates");
    m_spnPlates->setGeometry(QRect(420, 8, 42, 20));
    m_spnPlates->setMinimum(2);
    m_spnPlates->setMaximum(15);

    QLabel* label2 = new QLabel(this);
    label2->setObjectName(QString::fromUtf8("label2"));
    label2->setGeometry(QRect(10, 40, 136, 16));
    label2->setText("image height (px)");
    label2->setFont(font);

    m_edtHeight = new QLineEdit(this);
    m_edtHeight->setObjectName(QString::fromUtf8("m_edtHeight"));
    m_edtHeight->setGeometry(QRect(160, 40, 73, 22));
    m_edtHeight->setFont(font);

    QLabel* label3 = new QLabel(this);
    label3->setObjectName(QString::fromUtf8("label3"));
    label3->setGeometry(QRect(10, 70, 144, 16));
    label3->setText("grid size (px)");
    label3->setFont(font);

    m_edtHexagonSize = new QLineEdit(this);
    m_edtHexagonSize->setObjectName(QString::fromUtf8("m_edtHexagonSize"));
    m_edtHexagonSize->setGeometry(QRect(160, 70, 73, 22));
    m_edtHexagonSize->setFont(font);

    // set up orientation group box....
    QGroupBox* groupBox = new QGroupBox(this);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(10, 100, 231, 41));
    groupBox->setTitle("Orientation");

    m_vertical = new QRadioButton(groupBox);
    m_vertical->setObjectName(QString::fromUtf8("m_vertical"));
    m_vertical->setGeometry(QRect(20, 20, 82, 17));
    m_vertical->setText("vertical");
    m_vertical->setFont(font);

    m_horizontal = new QRadioButton(groupBox);
    m_horizontal->setObjectName(QString::fromUtf8("m_horizontal"));
    m_horizontal->setGeometry(QRect(130, 20, 101, 17));
    m_horizontal->setText("horizontal");
    m_horizontal->setFont(font);

    QGroupBox* gridProps = new QGroupBox(this);
    gridProps->setObjectName(QString::fromUtf8("GridProps"));
    gridProps->setGeometry(QRect(260, 50, 261, 161));
    gridProps->setTitle("Grid Properties");

    m_cbxSolid = new QCheckBox(gridProps);
    m_cbxSolid->setObjectName(QString::fromUtf8("cbxSolid"));
    m_cbxSolid->setGeometry(QRect(10, 20, 131, 17));
    m_cbxSolid->setText("Solid cells");
    m_cbxSolid->setFont(font);

    m_cbxColor = new QCheckBox(gridProps);
    m_cbxColor->setObjectName(QString::fromUtf8("cbxColor"));
    m_cbxColor->setGeometry(QRect(10, 40, 101, 17));
    m_cbxColor->setText("Color display");
    m_cbxColor->setFont(font);

    m_cbxCenter = new QCheckBox(gridProps);
    m_cbxCenter->setObjectName(QString::fromUtf8("cbxCenter1"));
    m_cbxCenter->setGeometry(QRect(10, 60, 151, 17));
    m_cbxCenter->setText("Display center");
    m_cbxCenter->setFont(font);

    m_cbxIndex = new QCheckBox(gridProps);
    m_cbxIndex->setObjectName(QString::fromUtf8("cbxIndex"));
    m_cbxIndex->setGeometry(QRect(10, 80, 121, 17));
    m_cbxIndex->setText("Display Index");
    m_cbxIndex->setFont(font);

    m_spare2 = new QCheckBox(gridProps);
    m_spare2->setObjectName(QString::fromUtf8("checkBox_4"));
    m_spare2->setGeometry(QRect(10, 100, 131, 17));
    m_spare2->setText("spare 2");
    m_spare2->setEnabled(false);
    m_spare2->setVisible(false);
    m_spare2->setFont(font);

    m_spare3 = new QCheckBox(gridProps);
    m_spare3->setObjectName(QString::fromUtf8("checkBox_5"));
    m_spare3->setGeometry(QRect(10, 120, 131, 17));
    m_spare3->setText("spare 3");
    m_spare3->setEnabled(false);
    m_spare3->setVisible(false);
    m_spare3->setFont(font);

    m_spare4 = new QCheckBox(gridProps);
    m_spare4->setObjectName(QString::fromUtf8("checkBox_6"));
    m_spare4->setGeometry(QRect(10, 140, 121, 17));
    m_spare4->setText("spare 4");
    m_spare4->setEnabled(false);
    m_spare4->setVisible(false);
    m_spare4->setFont(font);

    // set up dividing line
    QFrame* line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(0, 210 , 671, 16));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    // set up noise generation controls .....
    // principal harmonic display ....
    QLabel* label10 = new QLabel(this);
    label10->setObjectName(QString::fromUtf8("label10"));
    label10->setGeometry(QRect(40, 230, 81, 16));
    label10->setText("Principal");
    label10->setFont(font);
   
    QLabel* m_principalHarmonic = new QLabel(this);
    m_principalHarmonic->setObjectName(QString::fromUtf8("m_principalHarmonic"));
    m_principalHarmonic->setGeometry(QRect(10, 250, 131, 141));
    m_principalHarmonic->setFrameShape(QFrame::Box);

    QLabel* label4 = new QLabel(this);                        // this will be a pix map later in life....
    label4->setObjectName(QString::fromUtf8("label4"));
    label4->setGeometry(QRect(10, 390, 80, 16));             
    label4->setText("amplitude");
    label4->setFont(font);

    m_edtAmplitudes[0] = new QLineEdit(this);
    m_edtAmplitudes[0]->setObjectName(QString::fromUtf8("m_principleAmplitude"));
    m_edtAmplitudes[0]->setGeometry(QRect(90, 390, 50, 20));   
    m_edtAmplitudes[0]->setFont(font);

    QLabel* label5 = new QLabel(this);
    label5->setObjectName(QString::fromUtf8("label5"));
    label5->setGeometry(QRect(10, 410, 80, 16));
    label5->setText("frequency");
    label5->setFont(font);

    m_edtFrequency[0] = new QLineEdit(this);
    m_edtFrequency[0]->setObjectName(QString::fromUtf8("m_principalFreq"));
    m_edtFrequency[0]->setGeometry(QRect(90, 410, 50, 20));
    m_edtFrequency[0]->setFont(font);

    // secondary harmonic displays
    QLabel* label11 = new QLabel(this);
    label11->setObjectName(QString::fromUtf8("label11"));
    label11->setGeometry(QRect(150, 230, 111, 16));
    label11->setText("1st Harmonic");
    label11->setFont(font);

    QLabel* m_2Harmonic = new QLabel(this);
    m_2Harmonic->setObjectName(QString::fromUtf8("m_2Harmonic"));
    m_2Harmonic->setGeometry(QRect(150, 250, 131, 141));
    m_2Harmonic->setFrameShape(QFrame::Box);

    QLabel* label6 = new QLabel(this);
    label6->setObjectName(QString::fromUtf8("label6"));
    label6->setGeometry(QRect(150, 390, 80, 16));
    label6->setText("amplitude");
    label6->setFont(font);

    m_edtAmplitudes[1] = new QLineEdit(this);
    m_edtAmplitudes[1]->setObjectName(QString::fromUtf8("m_2Amplitude"));
    m_edtAmplitudes[1]->setGeometry(QRect(230, 390, 50, 20));
    m_edtAmplitudes[1]->setFont(font);

    QLabel* label7 = new QLabel(this);
    label7->setObjectName("label7");
    label7->setGeometry(QRect(150, 410, 80, 16));
    label7->setText("frequency");
    label7->setFont(font);

    m_edtFrequency[1] = new QLineEdit(this);
    m_edtFrequency[1]->setObjectName("m_2lFreq");
    m_edtFrequency[1]->setGeometry(QRect(230, 410, 50, 20));
    m_edtFrequency[1]->setFont(font);

    // Tertiary harmonic displays....
    QLabel* label12 = new QLabel(this);
    label12->setObjectName(QString::fromUtf8("label12"));
    label12->setGeometry(QRect(300, 230, 111, 16));
    label12->setText("2nd Harmonic");
    label12->setFont(font);

    QLabel* m_3Harmonic = new QLabel(this);
    m_3Harmonic->setObjectName("m_3Harmonic");
    m_3Harmonic->setGeometry(QRect(290, 250, 131, 141));
    m_3Harmonic->setFrameShape(QFrame::Box);

    QLabel* label8 = new QLabel(this);
    label8->setObjectName("label8");
    label8->setGeometry(QRect(290, 390, 80, 16));
    label8->setText("amplitude");
    label8->setFont(font);

    m_edtAmplitudes[2] = new QLineEdit(this);
    m_edtAmplitudes[2]->setObjectName(QString::fromUtf8("m_3Amplitude"));
    m_edtAmplitudes[2]->setGeometry(QRect(370, 390, 50, 20));
    m_edtAmplitudes[2]->setFont(font);

    QLabel* label9 = new QLabel(this);
    label9->setObjectName("label9");
    label9->setGeometry(QRect(290, 410, 80, 16));
    label9->setText("frequency");
    label9->setFont(font);

    m_edtFrequency[2] = new QLineEdit(this);
    m_edtFrequency[2]->setObjectName("m_3Freq");
    m_edtFrequency[2]->setGeometry(QRect(370, 410, 50, 20));
    m_edtFrequency[2]->setFont(font);
    
    // total noise display
    QLabel* label13 = new QLabel(this);
    label13->setObjectName(QString::fromUtf8("label13"));
    label13->setGeometry(QRect(470, 230, 47, 13));
    label13->setText("Total");
    label13->setFont(font);

    QLabel* m_TotalNoise = new QLabel(this);
    m_TotalNoise->setObjectName(QString::fromUtf8("m_TotalNoise"));
    m_TotalNoise->setGeometry(QRect(430, 250, 131, 141));
    m_TotalNoise->setFrameShape(QFrame::Box);

    //connect(btnSave, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(btnOK, SIGNAL(clicked()), this, SLOT(onAccept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}



uint8_t imagePropDlg::orientation()
{
    if (m_vertical->isChecked())
        return CHexagon::VERTICAL;
    else
        return CHexagon::HORIZONTAL;
}


/************************************************************************************************************************
 * function  : onAccept
 *
 * abstract  : This function save the current parameters locally (to the image properties structure) and the closes the
 *             dialog.
 *
 * parameters:
 *
 * returns   :
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void imagePropDlg::onAccept()
{
    if(m_bSave) saveValues();
    QDialog::accept();
}



/************************************************************************************************************************
 * function : saveValues
 *
 * abstract  : This function stores the properties locally (to the image property structure).  This function is call
 *             by both onAccept (which closes the dialog box) and onSave (which saves the current properites to the 
 *             configuration data -- typically the registry on Windows). 
 *
 * parameters: void 
 *
 * returns   : void 
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void imagePropDlg::saveValues()
{
  m_pImageProps->hexagonProps = 0;
    // move values from controls to member variables...
    m_pImageProps->imageWidth = m_edtWidth->text().toInt();
    m_pImageProps->imageHeight = m_edtHeight->text().toInt();
    m_pImageProps->hexagonSize = m_edtHexagonSize->text().toInt();
    m_pImageProps->hexagonOrient = (m_vertical->isChecked() ? CHexagon::VERTICAL : CHexagon::HORIZONTAL);
    int cnt = m_spnPlates->value();
    m_pImageProps->cntPlates = m_spnPlates->value();

    if (m_cbxSolid->isChecked()) m_pImageProps->hexagonProps |= bFilled;
    if (m_cbxColor->isChecked()) m_pImageProps->hexagonProps |= bColor;
    if (m_cbxCenter->isChecked()) m_pImageProps->hexagonProps |= bDispCenter;
    if (m_cbxIndex->isChecked()) m_pImageProps->hexagonProps |= bDispIndex;

    for (int ndx = 0; ndx < nbrHarmonics; ndx++)
    {
      m_pImageProps->amplitude[ndx] = m_edtAmplitudes[ndx]->text().toFloat();
      m_pImageProps->frequency[ndx] = m_edtFrequency[ndx]->text().toFloat();
    }
}