set(PYTHON_SOURCES
    __init__.py
    design_matrix_reader.py
    gen_kw_collector.py
    misfit_collector.py
    summary_collector.py
    summary_observation_collector.py
)

add_python_package("python.ert.enkf.export"  ${PYTHON_INSTALL_PREFIX}/ert/enkf/export "${PYTHON_SOURCES}" True)

