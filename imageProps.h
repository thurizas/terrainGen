#ifndef _imapeProps_h_
#define _imageProps_h_

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>


#include "constants.h"



class QRadioButton;
class QCheckBox;
class terrainGen;

// TODO : setup spin box to change value in edit control....

class imagePropDlg : public QDialog
{
	Q_OBJECT

public:
	explicit imagePropDlg(struct imageProps*, bool doSave, QWidget* p = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~imagePropDlg();

	uint64_t width() { return m_edtWidth->text().toInt(); }
	uint64_t height() { return m_edtHeight->text().toInt(); }
	uint64_t hexagon() { return m_edtHexagonSize->text().toInt(); }
	uint8_t orientation();
	uint8_t hexProps() { return m_hexProps; }
	uint32_t plates() { return m_spnPlates->value(); }
	float*   amplitudes(int* cnt) { *cnt = nbrHarmonics; return m_amplitude; }
	float*   frequencies(int* cnt) { *cnt = nbrHarmonics;  return m_frequency; }

private slots:
	void       onAccept();

private:
	terrainGen*        m_pParent;     // pointer to our main window
	bool               m_bSave;       // flag if we want to save the properties
	struct imageProps* m_pImageProps; // pointer totheimage properites to save values into
	// UI controls ....
	QLineEdit*   m_edtWidth;
	QLineEdit*   m_edtHeight;
	QLineEdit*   m_edtHexagonSize;

	QRadioButton* m_vertical;
	QRadioButton* m_horizontal;
	QLineEdit*   m_edtPlates;
	QSpinBox*    m_spnPlates;
	QLineEdit*   m_edtAmplitudes[nbrHarmonics];
	QLineEdit*   m_edtFrequency[nbrHarmonics];

	uint64_t    m_imageWidth;
	uint64_t    m_imageHeight;
	uint64_t    m_hexagonSize;
	uint8_t     m_hexagonOrient;
	uint8_t     m_hexProps;
	uint32_t    m_cntPlates;

	QCheckBox*  m_cbxSolid;
	QCheckBox*  m_cbxColor;
	QCheckBox*  m_cbxCenter;
	QCheckBox*  m_cbxIndex;
	QCheckBox*  m_spare2;
	QCheckBox*  m_spare3;
	QCheckBox*  m_spare4;

	uint8_t*    m_pPrimary;
	uint8_t*    m_pSecondary;
	uint8_t*    m_pTertiary;
	uint8_t*    m_pTotal;

	float       m_amplitude[nbrHarmonics];
	float       m_frequency[nbrHarmonics];

	void setupUI(bool);
	void saveValues();
};


#endif

