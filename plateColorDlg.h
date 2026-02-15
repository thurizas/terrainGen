#ifndef _plateColorDlg_h_
#define _plateColorDlg_h_

#include <QDialog>

class QLabel;
class QPushButton;
class QHBoxLayout;

class plateColorDlg : public QDialog
{
  Q_OBJECT
public:
  plateColorDlg(QWidget* p = nullptr);

private slots:
  void onOK();
  void onCancel();

private:
  void setupUI();


  int32_t       cntDefaultColor = 0;
  QLabel**      m_labels = nullptr;
  QLabel**      m_clrBox = nullptr;
  QPushButton** m_btnChange = nullptr;
  QHBoxLayout** m_rowLayout = nullptr;

};

#endif

