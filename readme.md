Monk's Vault
Monk's Vault is a lightweight Windows desktop application built with the Win32 API in C. It serves as a personal software repository, allowing users to browse and manage their applications with an intuitive interface.

Features
Dual‑column layout – Left panel displays a scalable application icon and a detailed description; right panel lists available software (EDR and Favorites).

Binary resource management – Each application is associated with an embedded binary file (RCDATA) that can be saved to disk or executed.

Save As – Extract the selected application's binary and save it to a user‑chosen location.

Install – Extract the binary to a temporary directory and run it (supports .exe files).

Multi‑language UI – Interface strings are fully localized in English, Simplified Chinese, Traditional Chinese, Japanese, and Korean. Language can be switched on the fly via the menu.

Dynamic layout – Controls automatically resize when the window is resized, with the left side occupying one‑third of the window width.

Clean white background – All static controls and edit boxes use a white background for a modern look.

Screenshot
(You can add a screenshot here)

Requirements
Windows OS (XP/Vista/7/8/10/11)

Visual Studio (or any Windows SDK compatible compiler)

SourceTree (for Git management)

Building
Clone the repository.

Open the solution in Visual Studio.

Ensure all resource files (edr_icon.bmp, fav_icon.bmp, edr.bin, fav.bin, app.ico) are present in the project directory and set to "Resource Compiler" item type.

Build the solution (F7).

Run the executable.

Resource Files
edr_icon.bmp, fav_icon.bmp – Application icons (BMP format, recommended size 256×256 for scaling).

edr.bin, fav.bin – Binary files associated with each application (any content; will be extracted on Save/Install).

app.ico – Main application icon for the window (optional).

Language Support
The application detects the current UI language via resource tables. You can extend it by adding new LANGUAGE blocks in resource.rc and corresponding string IDs.

License
(Specify your license here, e.g., MIT, GPL, or proprietary)

