#ifndef SOLVER_H
#define SOLVER_H

#include <QThread>
#include <QTableWidget>
#include <QProgressBar>
#include <QSpinBox>
#include <QTextBrowser>

class Solver : public QThread
{
    Q_OBJECT
public:
    void run();
public:
    QTableWidget * table;
    QSpinBox * spin;
private:
    void combinate(unsigned int N, unsigned int K);
    void combiStep(int pos);
signals:
    void sendText(QString);
    void progress(int);
};

#endif // SOLVER_H
