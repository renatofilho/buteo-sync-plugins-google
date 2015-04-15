set(GOOGLE_CONTACTS_CLIENT googlecontacts-client)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${ACCOUNTS_INCLUDE_DIRS}
    ${BUTEOSYNCFW_INCLUDE_DIRS}
    ${LIBSIGNON_INCLUDE_DIRS}
)

# Buteo oop plugin
add_definitions(-DCLASSNAME=GContactClient)
add_definitions(-DCLASSNAME_H=\"GContactClient.h\")
add_definitions(-DCLIENT_PLUGIN)
set(GOOGLE_CONTACT_BUTEO_SRCS
    ${BUTEOSYNCFW_INCLUDEDIR}/ButeoPluginIfaceAdaptor.h
    ${BUTEOSYNCFW_INCLUDEDIR}/PluginCbImpl.h
    ${BUTEOSYNCFW_INCLUDEDIR}/PluginServiceObj.h
    ${BUTEOSYNCFW_INCLUDEDIR}/ButeoPluginIfaceAdaptor.cpp
    ${BUTEOSYNCFW_INCLUDEDIR}/PluginCbImpl.cpp
    ${BUTEOSYNCFW_INCLUDEDIR}/PluginServiceObj.cpp
    ${BUTEOSYNCFW_INCLUDEDIR}/plugin_main.cpp
)

set(GOOGLE_CONTACTS_CLIENT_SRCS
    atom_global.h
    buteo-gcontact-plugin_global.h
    buteosyncfw_p.h
    GAtom.cpp
    GAtom.h
    GAuth.cpp
    GAuth.h
    GConfig.h
    GContactClient.cpp
    GContactClient.h
    GContactCustomDetail.cpp
    GContactCustomDetail.h
    GContactDetail.h
    GContactEntry.cpp
    GContactEntry.h
    GContactsBackend.cpp
    GContactsBackend.h
    GParseStream.cpp
    GParseStream.h
    GTransport.cpp
    GTransport.h
    GWriteStream.cpp
    GWriteStream.h
    ${GOOGLE_CONTACT_BUTEO_SRCS}
)


add_executable(${GOOGLE_CONTACTS_CLIENT}
    ${GOOGLE_CONTACTS_CLIENT_SRCS}
)

target_link_libraries(${GOOGLE_CONTACTS_CLIENT}
    ${ACCOUNTS_LIBRARIES}
    ${BUTEOSYNCFW_LIBRARIES}
    ${LIBSIGNON_LIBRARIES}
)

qt5_use_modules(${GOOGLE_CONTACTS_CLIENT} Core DBus Network Contacts)

install(TARGETS ${GOOGLE_CONTACTS_CLIENT}
    RUNTIME DESTINATION "/usr/lib/buteo-plugins-qt5/oopp"
)

add_subdirectory(xmls)


