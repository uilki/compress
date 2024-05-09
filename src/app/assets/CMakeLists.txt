qt_add_library(assets STATIC)
qt_add_qml_module(assets
    URI "assets"
    VERSION 1.0
    QML_FILES
        TextLabel.qml
        FileList.qml
    RESOURCE_PREFIX /
)

