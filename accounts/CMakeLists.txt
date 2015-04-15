set(SERVICE_FILES
     google-contacts.service
)

install(FILES ${SERVICE_FILES}
    DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/accounts/services
)
