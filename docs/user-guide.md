# User Guide

## 1. Building the program

Open a terminal in the project folder and run:

```
make
```

## 2. Running the program

```
make run
```

On macOS you can also use the abrir_sistema.command file. On Windows, use abrir_sistema_windows.bat.

## 3. First access

The initial user is:

```
Username: admin
Password: admin
```

On first login, the program forces the Administrator to change the password.

## 4. Main menu

The main menu shows:

1. Log in
2. Register as Software Manager
0. Exit

If a user tries to log in and the account doesn't exist, the program asks whether they want to register as a Software Manager.

## 5. Administrator menu

The Administrator can:

- validate Software Managers;
- add licenses;
- list licenses;
- edit licenses;
- remove licenses;
- sort licenses;
- register installations;
- register uninstallations;
- list assignments;
- generate reports;
- export licenses to CSV;
- export licenses to TXT;
- view expiry alerts;
- view statistics;
- manage renewal requests;
- view license history;
- search licenses;
- list licenses by status.

## 6. Software Manager menu

The Software Manager can:

- view their assigned licenses;
- register installations;
- register uninstallations (only for their own assignments);
- list their own assignments;
- view statistics;
- request renewals;
- list their own renewal requests;
- view license history;
- search licenses.

## 7. Adding a license

To add a license, choose the corresponding option in the Administrator menu and fill in:

- product name;
- manufacturer;
- license type;
- activation key;
- number of allowed seats;
- acquisition date;
- expiry date (except for Perpetual licenses, which have no expiry date);
- cost;
- status.

Note: Perpetual licenses have no expiry date, the program does not ask for one, and the license is never automatically switched to Expired.

## 8. Registering an installation

To register an installation, provide:

- license ID;
- machine/seat identifier;
- person responsible;
- installation date.

The system does not allow installing licenses that are not active or that already have all seats occupied.
The installation date must fall between the license's acquisition date and its expiry date.
When the installation is registered by a Software Manager, that manager is automatically set as the person responsible.

## 9. Registering an uninstallation

To register an uninstallation, provide:

- assignment ID;
- uninstallation date.

The uninstallation date cannot be earlier than the installation date. A Software Manager can only uninstall assignments they are responsible for.

## 10. Renewal requests

A Software Manager can request the renewal of a license.

The Administrator can then:

- approve the request and enter a new expiry date;
- reject the request.

There cannot be two pending requests for the same license. When approving, the new date must be later than the current expiry date.

## 11. Reports

The Administrator can generate:

- cost report;
- usage report;
- CSV export;
- TXT export.

Files are created in the folder:

```
relatorios/
```

Expiry alerts automatically use the current system date.
On startup, any license still marked as active whose expiry date has already passed automatically switches to Expired.

## 12. Audit log

Sensitive operations are recorded in:

```
relatorios/auditoria.txt
```

This file allows you to review the change history of a license.

## 13. Binary files

Persistent data is stored in:

```
dados/utilizadores.bin
dados/licencas.bin
dados/atribuicoes.bin
dados/renovacoes.bin
```

## 14. Exiting the program

To exit correctly, use the option:

```
0. Exit
```

This ensures data is saved to the binary files.

## 15. Removing licenses

A license cannot be removed while it has associated assignments or renewal requests. This rule prevents losing the license's history.
