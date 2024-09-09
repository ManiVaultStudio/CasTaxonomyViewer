#pragma once

#include <ViewPlugin.h>

#include <Dataset.h>
#include <widgets/DropWidget.h>

#include <CasTaxonomyData/CasTaxonomyData.h>

#include <QWidget>

/** All plugin related classes are in the ManiVault plugin namespace */
using namespace mv::plugin;

/** Drop widget used in this plugin is located in the ManiVault gui namespace */
using namespace mv::gui;

/** Dataset reference used in this plugin is located in the ManiVault util namespace */
using namespace mv::util;

class TaxonomyWidget;
class QLabel;

class CasTaxonomyViewer : public ViewPlugin
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param factory Pointer to the plugin factory
     */
    CasTaxonomyViewer(const PluginFactory* factory);

    /** Destructor */
    ~CasTaxonomyViewer() override = default;
    
    /** This function is called by the core after the view plugin has been created */
    void init() override;

    /**
     * Invoked when a data event occurs
     * @param dataEvent Data event which occurred
     */
    void onDataEvent(mv::DatasetEvent* dataEvent);

protected:
    TaxonomyWidget*         _taxonomyWidget;

    DropWidget*             _dropWidget;                /** Widget for drag and drop behavior */
    mv::Dataset<CasTaxonomy>     _dataset;                    /** Points smart pointer */
    QString                 _currentDatasetName;        /** Name of the current dataset */
    QLabel*                 _currentDatasetNameLabel;   /** Label that show the current dataset name */
};

/**
 * Note: Factory does not need to be altered (merely responsible for generating new plugins when requested)
 */
class CasTaxonomyViewerFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.CasTaxonomyViewer"
                      FILE  "CasTaxonomyViewer.json")

public:

    /** Default constructor */
    CasTaxonomyViewerFactory() {}

    /** Destructor */
    ~CasTaxonomyViewerFactory() override {}
    
    /** Creates an instance of the example view plugin */
    ViewPlugin* produce() override;

    /** Returns the data types that are supported by the example view plugin */
    mv::DataTypes supportedDataTypes() const override;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions(const mv::Datasets& datasets) const override;
};