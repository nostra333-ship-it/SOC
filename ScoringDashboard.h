# ScoringDashboard.h

#ifndef SCORINGDASHBOARD_H
#define SCORINGDASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

class ScoringDashboard : public QWidget {
    Q_OBJECT

public:
    ScoringDashboard(QWidget *parent = nullptr);
    void updateScore(int score);
    void updateAccuracy(float accuracy);
    void addSessionHistory(const QString &session);

private:
    QLabel *scoreLabel;
    QLabel *accuracyLabel;
    QTableWidget *sessionHistoryTable;
};

#endif // SCORINGDASHBOARD_H
