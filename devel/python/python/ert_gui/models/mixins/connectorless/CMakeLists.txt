set(PYTHON_SOURCES
    __init__.py
    default_boolean_model.py
    default_path_model.py
    function_button_model.py
)

add_python_package("python.ert_gui.models.mixins.connectorless"  ${PYTHON_INSTALL_PREFIX}/ert_gui/models/mixins/connectorless "${PYTHON_SOURCES}" True)

