cmake_minimum_required(VERSION 3.5)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../unity.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/thirdparty_headers.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

list(APPEND HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/attributes/iattribute.h
    ${CMAKE_CURRENT_LIST_DIR}/attributes/iattributerange.h
    ${CMAKE_CURRENT_LIST_DIR}/attributes/valuetype.h
    ${CMAKE_CURRENT_LIST_DIR}/commands/command.h
    ${CMAKE_CURRENT_LIST_DIR}/commands/compoundcommand.h
    ${CMAKE_CURRENT_LIST_DIR}/commands/icommand.h
    ${CMAKE_CURRENT_LIST_DIR}/commands/icommandmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/elementid_containers.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/elementid_debug.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/elementid.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/elementtype.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/grapharray.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/grapharray_json.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/igrapharrayclient.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/igrapharray.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/igraphcomponent.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/igraph.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/igraphmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/graph/imutablegraph.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/biopaxfileparser.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/gmlfileparser.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/graphmlparser.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/iparser.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/iparserthread.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/iurltypes.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/jsongraphparser.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/pairwisetxtfileparser.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/progressfn.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/progress_iterator.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/tabulardata.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/matfileparser.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/adjacencymatrixfileparser.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/xlsxtabulardataparser.h
    ${CMAKE_CURRENT_LIST_DIR}/loading/urltypes.h
    ${CMAKE_CURRENT_LIST_DIR}/plugins/basegenericplugin.h
    ${CMAKE_CURRENT_LIST_DIR}/plugins/baseplugin.h
    ${CMAKE_CURRENT_LIST_DIR}/plugins/iplugin.h
    ${CMAKE_CURRENT_LIST_DIR}/plugins/nodeattributetablemodel.h
    ${CMAKE_CURRENT_LIST_DIR}/plugins/tableproxymodel.h
    ${CMAKE_CURRENT_LIST_DIR}/plugins/userdata.h
    ${CMAKE_CURRENT_LIST_DIR}/plugins/userdatavector.h
    ${CMAKE_CURRENT_LIST_DIR}/plugins/userelementdata.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/idocument.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/iselectionmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/visualisations/ielementvisual.h
    ${CMAKE_CURRENT_LIST_DIR}/updates/updates.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/cancellable.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/checksum.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/circularbuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/color.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/constants.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/crypto.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/container.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/container_randomsample.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/debugger.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/deferredexecutor.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/doasyncthen.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/enumbitmask.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/enumreflection.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/failurereason.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/fixedsizestack.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/flags.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/function_traits.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/iterator_range.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/is_detected.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/modelcompleter.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/movablepointer.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/pair_iterator.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/passkey.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/performancecounter.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/preferences.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/preferenceswatcher.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/qmlpreferences.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/progressable.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/qmlenum.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/qmlutils.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/random.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/redirects.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/scopetimer.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/scope_exit.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/singleton.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/static_visitor.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/statistics.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/string.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/thread.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/threadpool.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/typeidentity.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/utils.h
)

list(APPEND SHARED_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/graph/elementtype.cpp
    ${CMAKE_CURRENT_LIST_DIR}/loading/biopaxfileparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/loading/matfileparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/loading/gmlfileparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/loading/graphmlparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/loading/jsongraphparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/loading/pairwisetxtfileparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/loading/xlsxtabulardataparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/loading/tabulardata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/loading/urltypes.cpp
    ${CMAKE_CURRENT_LIST_DIR}/plugins/basegenericplugin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/plugins/nodeattributetablemodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/plugins/tableproxymodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/plugins/userdata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/plugins/userdatavector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/updates/updates.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/crypto.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/deferredexecutor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/modelcompleter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/performancecounter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/preferences.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/preferenceswatcher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/qmlpreferences.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/random.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/scopetimer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/string.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/threadpool.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/typeidentity.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/utils.cpp
)

list(APPEND RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/updates/update_keys.qrc
    ${CMAKE_CURRENT_LIST_DIR}/ui/shared.qrc
    ${CMAKE_CURRENT_LIST_DIR}/ui/js.qrc
)

if(UNITY_BUILD)
    GenerateUnity(ORIGINAL_SOURCES SHARED_SOURCES UNITY_PREFIX "shared")
endif()

list(APPEND SOURCES ${SHARED_SOURCES})

add_library(shared STATIC ${SOURCES} ${HEADERS} ${RESOURCES})

find_package(Qt5 COMPONENTS Qml Widgets Xml REQUIRED)
target_link_libraries(shared
    Qt5::Qml
    Qt5::Widgets
    Qt5::Xml
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(shared Threads::Threads)

target_link_libraries(shared thirdparty thirdparty_static)
