#include "TaxonomyDataWidget.h"

#include <CasTaxonomyData/Taxonomy.h>

#include <QGridLayout>

TaxonomyDataWidget::TaxonomyDataWidget()
{
    _titleLabel = new QLabel("Title:", this);
    _title = new QLabel("Actual title", this);

    _rationaleLabel = new QLabel("Rationale:", this);
    _rationale = new QLabel("Rationale", this);

    _annotationGroupBox = new QGroupBox();

    QGridLayout* gridLayout = new QGridLayout();

    gridLayout->addWidget(_titleLabel, 0, 0);
    gridLayout->addWidget(_title, 0, 1);

    gridLayout->addWidget(_rationaleLabel, 1, 0);
    gridLayout->addWidget(_rationale, 1, 1);

    gridLayout->addWidget(_annotationGroupBox, 2, 0, 2, 1);

    setLayout(gridLayout);
}

void TaxonomyDataWidget::setAnnotation(Annotation& annotation)
{
    _title->setText(annotation.cell_label);
    _rationale->setText(annotation.rationale);

    if (_annotationGroupBox->layout() == nullptr)
    {
        QGridLayout* gridLayout = new QGridLayout();
        _annotationGroupBox->setLayout(gridLayout);
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
        QLabel* keyLabel = new QLabel(it->first);
        QLabel* valueLabel = new QLabel(it->second);
        qDebug() << ">>>>>>>>>>>>>>>>>>>>> FOUND ANNOTATION" << it->first << it->second;
        gridLayout->addWidget(keyLabel, row, 0);
        gridLayout->addWidget(valueLabel, row, 1);
        row++;
    }

    //_annotationGroupBox->setLayout(gridLayout);
}
