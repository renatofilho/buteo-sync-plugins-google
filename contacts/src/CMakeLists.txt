set(GOOGLE_CONTACTS_CLIENT googlecontacts-client)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${buteo-contact-client_SOURCE_DIR}
    ${ACCOUNTS_INCLUDE_DIRS}
    ${BUTEOSYNCFW_INCLUDE_DIRS}
    ${LIBSIGNON_INCLUDE_DIRS}
)

# Buteo oop plugin
add_definitions(-DCLASSNAME=GContactsClient)
add_definitions(-DCLASSNAME_H=\"GContactsClient.h\")
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
    GConfig.h
    GConfig.cpp
    GContactsClient.h
    GContactsClient.cpp
    GContactDetail.h
    GContactEntry.cpp
    GContactEntry.h
    GParseStream.cpp
    GParseStream.h
    GRemoteSource.h
    GRemoteSource.cpp
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
    ubuntu-contact-client
)

qt5_use_modules(${GOOGLE_CONTACTS_CLIENT} Core DBus Network Contacts)

install(TARGETS ${GOOGLE_CONTACTS_CLIENT}
    RUNTIME DESTINATION "/usr/lib/buteo-plugins-qt5/oopp"
)

add_subdirectory(xmls)


