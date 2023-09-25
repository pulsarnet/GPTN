//IDI_ICON1               ICON    DISCARDABLE     "todo"


1 VERSIONINFO
    FILEVERSION    ${CMAKE_PROJECT_VERSION_MAJOR},${CMAKE_PROJECT_VERSION_MINOR},${CMAKE_PROJECT_VERSION_PATCH}
    PRODUCTVERSION ${CMAKE_PROJECT_VERSION_MAJOR},${CMAKE_PROJECT_VERSION_MINOR},${CMAKE_PROJECT_VERSION_PATCH}
{
    BLOCK "StringFileInfo"
    {
        BLOCK "000004B0"
        {
            VALUE "CompanyName",        "${PROJECT_NAME} Team\0"
            VALUE "FileDescription",    "${PROJECT_NAME} executable\0"
            VALUE "FileVersion",        "${CMAKE_PROJECT_VERSION}"
            VALUE "OriginalFilename",   "${PROJECT_NAME}.exe\0"
            VALUE "ProductName",        "${PROJECT_NAME}"
            VALUE "ProductVersion",     "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}"
        }
    }
    BLOCK "VarFileInfo"
    {
        VALUE "Translation", 0x409, 1200
    }
}