#include "CasTaxonomyViewer.h"

#include "TaxonomyWidget.h"
#include "TaxonomyDataWidget.h"

#include <event/Event.h>

#include <DatasetsMimeData.h>

#include <QDebug>
#include <QMimeData>

Q_PLUGIN_METADATA(IID "studio.manivault.CasTaxonomyViewer")

using namespace mv;

CasTaxonomyViewer::CasTaxonomyViewer(const PluginFactory* factory) :
    ViewPlugin(factory),
    _taxonomyWidget(new TaxonomyWidget()),
    _annotationPropertyWidget(new TaxonomyDataWidget()),
    _dropWidget(nullptr),
    _dataset(),
    _currentDatasetName(),
    _currentDatasetNameLabel(new QLabel())
{
    // This line is mandatory if drag and drop behavior is required
    _currentDatasetNameLabel->setAcceptDrops(true);

    // Align text in the center
    _currentDatasetNameLabel->setAlignment(Qt::AlignCenter);
}

void CasTaxonomyViewer::init()
{
    // Load webpage
    _taxonomyWidget->setPage(":cas_taxonomy_viewer/cas_taxonomy_viewer/taxonomy_view.html", "qrc:/cas_taxonomy_viewer/cas_taxonomy_viewer/");

    // Create layout
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    connect(&_taxonomyWidget->getCommObject(), &JSCommunicationObject::partitionHovered, this, &CasTaxonomyViewer::onPartitionHovered);

    layout->addWidget(_taxonomyWidget);
    layout->addWidget(_annotationPropertyWidget);

    //layout->addWidget(_currentDatasetNameLabel);

    // Apply the layout
    getWidget().setLayout(layout);

    // Instantiate new drop widget
    _dropWidget = new DropWidget(_taxonomyWidget);

    // Set the drop indicator widget (the widget that indicates that the view is eligible for data dropping)
    _dropWidget->setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(&getWidget(), "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    // Initialize the drop regions
    _dropWidget->initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
        // A drop widget can contain zero or more drop regions
        DropWidget::DropRegions dropRegions;

        const auto datasetsMimeData = dynamic_cast<const DatasetsMimeData*>(mimeData);

        if (datasetsMimeData == nullptr)
            return dropRegions;

        if (datasetsMimeData->getDatasets().count() > 1)
            return dropRegions;

        // Gather information to generate appropriate drop regions
        const auto dataset = datasetsMimeData->getDatasets().first();
        const auto datasetGuiName = dataset->getGuiName();
        const auto datasetId = dataset->getId();
        const auto dataType = dataset->getDataType();
        const auto dataTypes = DataTypes({ CasTaxonomyType });

        // Visually indicate if the dataset is of the wrong data type and thus cannot be dropped
        if (!dataTypes.contains(dataType)) {
            dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", "exclamation-circle", false);
        }
        else {

            // Get points dataset from the core
            auto candidateDataset = mv::data().getDataset<CasTaxonomy>(datasetId);

            // Accept points datasets drag and drop
            if (dataType == CasTaxonomyType) {
                const auto description = QString("Load %1 into example view").arg(datasetGuiName);

                if (_dataset == candidateDataset) {

                    // Dataset cannot be dropped because it is already loaded
                    dropRegions << new DropWidget::DropRegion(this, "Warning", "Data already loaded", "exclamation-circle", false);
                }
                else {

                    // Dataset can be dropped
                    dropRegions << new DropWidget::DropRegion(this, "Points", description, "map-marker-alt", true, [this, candidateDataset]() {
                        _dataset = candidateDataset;
                        _taxonomyWidget->setData(_dataset->getTaxonomy());
                        _dropWidget->setShowDropIndicator(!_dataset.isValid());
                    });
                }
            }
        }

        return dropRegions;
    });

    // Respond when the name of the dataset in the dataset reference changes
    connect(&_dataset, &Dataset<CasTaxonomy>::guiNameChanged, this, [this]() {

        auto newDatasetName = _dataset->getGuiName();

        // Update the current dataset name label
        _currentDatasetNameLabel->setText(QString("Current points dataset: %1").arg(newDatasetName));

        // Only show the drop indicator when nothing is loaded in the dataset reference
        _dropWidget->setShowDropIndicator(newDatasetName.isEmpty());
    });

    // Alternatively, classes which derive from hdsp::EventListener (all plugins do) can also respond to events
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetAdded));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataChanged));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetRemoved));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));
    _eventListener.registerDataEventByType(CasTaxonomyType, std::bind(&CasTaxonomyViewer::onDataEvent, this, std::placeholders::_1));
}

void CasTaxonomyViewer::onDataEvent(mv::DatasetEvent* dataEvent)
{
    // Get smart pointer to dataset that changed
    const auto changedDataSet = dataEvent->getDataset();

    // Get GUI name of the dataset that changed
    const auto datasetGuiName = changedDataSet->getGuiName();

    // The data event has a type so that we know what type of data event occurred (e.g. data added, changed, removed, renamed, selection changes)
    switch (dataEvent->getType()) {

        // A points dataset was added
        case EventType::DatasetAdded:
        {
            // Cast the data event to a data added event
            const auto dataAddedEvent = static_cast<DatasetAddedEvent*>(dataEvent);

            // Get the GUI name of the added points dataset and print to the console
            qDebug() << datasetGuiName << "was added";

            break;
        }

        // Points dataset data has changed
        case EventType::DatasetDataChanged:
        {
            // Cast the data event to a data changed event
            const auto dataChangedEvent = static_cast<DatasetDataChangedEvent*>(dataEvent);

            // Get the name of the points dataset of which the data changed and print to the console
            qDebug() << datasetGuiName << "data changed";

            break;
        }

        // Points dataset data was removed
        case EventType::DatasetRemoved:
        {
            // Cast the data event to a data removed event
            const auto dataRemovedEvent = static_cast<DatasetRemovedEvent*>(dataEvent);

            // Get the name of the removed points dataset and print to the console
            qDebug() << datasetGuiName << "was removed";

            break;
        }

        // Points dataset selection has changed
        case EventType::DatasetDataSelectionChanged:
        {
            // Cast the data event to a data selection changed event
            const auto dataSelectionChangedEvent = static_cast<DatasetDataSelectionChangedEvent*>(dataEvent);

            // Get the selection set that changed
            const auto& selectionSet = changedDataSet->getSelection<CasTaxonomy>();

            // Print to the console
            qDebug() << datasetGuiName << "selection has changed";

            break;
        }

        default:
            break;
    }
}

void CasTaxonomyViewer::onPartitionHovered(QString name)
{
    qDebug() << "CAS You hovered over partition: " << name;

    Taxonomy& taxonomy = _dataset->getTaxonomy();

    qDebug() << taxonomy.author_name;

    for (Annotation& annotation : taxonomy.annotations)
    {
        if (annotation.cell_label == name)
        {
            qDebug() << annotation.cell_label << annotation.author_annotation_fields;
            _annotationPropertyWidget->setAnnotation(annotation);
        }
    }
}

ViewPlugin* CasTaxonomyViewerFactory::produce()
{
    return new CasTaxonomyViewer(this);
}

mv::DataTypes CasTaxonomyViewerFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;

    // This example analysis plugin is compatible with points datasets
    supportedTypes.append(CasTaxonomyType);

    return supportedTypes;
}

mv::gui::PluginTriggerActions CasTaxonomyViewerFactory::getPluginTriggerActions(const mv::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getPluginInstance = [this]() -> CasTaxonomyViewer* {
        return dynamic_cast<CasTaxonomyViewer*>(plugins().requestViewPlugin(getKind()));
    };

    const auto numberOfDatasets = datasets.count();

    if (numberOfDatasets >= 1 && PluginFactory::areAllDatasetsOfTheSameType(datasets, CasTaxonomyType)) {
        auto pluginTriggerAction = new PluginTriggerAction(const_cast<CasTaxonomyViewerFactory*>(this), this, "Example", "View example data", getIcon(), [this, getPluginInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
            for (auto dataset : datasets)
                getPluginInstance();
        });

        pluginTriggerActions << pluginTriggerAction;
    }

    return pluginTriggerActions;
}
