#include "TaxonomyDataWidget.h"

#include "ElidedLabel.h"

#include <CasTaxonomyData/Taxonomy.h>

#include <QGridLayout>

TaxonomyDataWidget::TaxonomyDataWidget()
{
    //setStyleSheet("QLabel { height-color: yellow }");

    _titleLabel = new QLabel("Title:", this);
    _title = new QLabel("", this);

    _rationaleLabel = new QLabel("Rationale:", this);
    _rationale = new QLabel("", this);

    _annotationGroupBox = new QGroupBox();

    QGridLayout* gridLayout = new QGridLayout();

    gridLayout->addWidget(_titleLabel, 0, 0);
    gridLayout->addWidget(_title, 0, 1);

    gridLayout->addWidget(_rationaleLabel, 1, 0);
    gridLayout->addWidget(_rationale, 1, 1);

    gridLayout->addWidget(_annotationGroupBox, 2, 0, 2, 1);
    gridLayout->setRowStretch(gridLayout->rowCount(), 1);
    setLayout(gridLayout);

    setMaximumWidth(600);
}

void TaxonomyDataWidget::setAnnotation(Annotation& annotation)
{
    _title->setText(annotation.cell_label);
    _rationale->setText(annotation.rationale);

    if (_annotationGroupBox->layout() == nullptr)
    {
        QGridLayout* gridLayout = new QGridLayout();
        _annotationGroupBox->setLayout(gridLayout);
        _annotationGroupBox->setMaximumWidth(600);
    }
    else
    {
        QLayoutItem* item;
        while ((item = _annotationGroupBox->layout()->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
    }

    QGridLayout* gridLayout = static_cast<QGridLayout*>(_annotationGroupBox->layout());
    
    int row = 0;
    for (auto it = annotation.author_annotation_fields.keyValueBegin(); it != annotation.author_annotation_fields.keyValueEnd(); ++it)
    {
        ElidedLabel* keyLabel = new ElidedLabel(it->first, this);
        keyLabel->setMinimumWidth(200);
        QLabel* valueLabel = new QLabel(it->second);
        valueLabel->setWordWrap(true);
        qDebug() << ">>>>>>>>>>>>>>>>>>>>> FOUND ANNOTATION" << it->first << it->second;
        gridLayout->addWidget(keyLabel, row, 0);
        gridLayout->addWidget(valueLabel, row, 1);
        row++;
    }
    gridLayout->setRowStretch(gridLayout->rowCount(), 1);
    //_annotationGroupBox->setLayout(gridLayout);
}

// TOO SLOW
//void TaxonomyDataWidget::setAnnotation(Annotation& annotation)
//{
//    _title->setText(annotation.cell_label);
//    _rationale->setText(annotation.rationale);
//
//    _annotationGroupAction->removeAllActions();
//
//    for (auto it = annotation.author_annotation_fields.keyValueBegin(); it != annotation.author_annotation_fields.keyValueEnd(); ++it)
//    {
//        StringAction* stringAction = new StringAction(this, it->first, it->second);
//
//        _annotationGroupAction->addAction(stringAction);
//    }
//}
