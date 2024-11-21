#pragma once

#include <QWidget>
#include <QLabel>
#include <QGroupBox>

class Annotation;

class TaxonomyDataWidget : public QWidget
{
public:
    TaxonomyDataWidget();

    void setAnnotation(Annotation& annotation);

private:
    QGroupBox*  _annotationGroupBox;

    QLabel*     _titleLabel;
    QLabel*     _title;

    QLabel*     _rationaleLabel;
    QLabel*     _rationale;

    QVector<QLabel*> _authorAnnotationFields;
};
