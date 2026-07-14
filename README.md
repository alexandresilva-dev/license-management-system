# License Management System (C)

A command-line software license management system built in C, developed as a solo project for the Programming I course (Instituto Politecnico de Viana do Castelo).

The system lets an organization track software licenses, assign them to machines, manage renewals, and generate usage/cost reports, with two access levels (Administrator and Software Manager) and persistent binary-file storage.

## Features

**Authentication and roles**
- Admin and Software Manager accounts, with forced password change on first login
- Self-registration for Software Managers, subject to Admin validation

**License management**
- Add, edit, remove, list and sort licenses
- License types: Perpetual, Annual, OEM, Educational (perpetual licenses have no expiry date)
- Automatic status update: active licenses past their expiry date are marked Expired on startup
- Search licenses and filter by status

**Installations and assignments**
- Register installations per license, with seat-limit and validity-window checks
- Register uninstallations, with date consistency checks
- Software Managers can only manage their own assignments; Admins can manage all

**Renewals**
- Software Managers can request a license renewal
- Admins can approve (with a new expiry date) or reject requests
- Prevents duplicate pending requests for the same license

**Reporting and audit**
- Cost and usage reports
- Export licenses to CSV and TXT
- Expiry alerts and usage statistics
- Full audit trail of sensitive operations (relatorios/auditoria.txt)
- Per-license history

**Persistence**
- All data is stored in binary files (dados/) and reloaded on startup
- Data structures are implemented as singly linked lists (users, licenses, assignments, renewal requests)

## Tech stack

- Language: C (C11)
- Build tool: Make (GCC)
- Data structures: Singly linked lists
- Storage: Binary files (no external database)

## Project structure

```
license-management-system/
|-- src/                  # Source files
|   |-- main.c            # Entry point, menus and session flow
|   |-- utilizadores.c    # Users, authentication, registration
|   |-- licencas.c        # License CRUD, sorting, search, expiry
|   |-- atribuicoes.c     # Installations / uninstallations
|   |-- renovacoes.c      # Renewal request workflow
|   |-- relatorios.c      # Reports, exports, statistics, audit log
|   |-- utilitarios.c     # Shared helpers (input parsing, dates, folders)
|-- include/              # Header files (one per module)
|-- docs/
|   |-- user-guide.md     # Full user guide (menus, workflows, rules)
|-- Makefile
|-- abrir_sistema.command     # macOS launcher
|-- abrir_sistema_windows.bat # Windows launcher
|-- .gitignore
```

At runtime the program creates a dados/ folder (binary data files) and a relatorios/ folder (audit log and report exports) next to the executable. Both are intentionally excluded from version control, see docs/user-guide.md for details.

## Getting started

```bash
make        # compiles the project (gcc, C11)
make run    # builds and runs the program
```

On macOS you can also double-click abrir_sistema.command; on Windows, abrir_sistema_windows.bat.

Default login: username admin, password admin (you will be asked to change it on first login).

See docs/user-guide.md for the full menu reference and business rules (renewal workflow, license removal constraints, report formats, etc.).

## Author

Alexandre Silva, Computer Engineering student, Instituto Politecnico de Viana do Castelo

- LinkedIn: https://www.linkedin.com/in/alexandresilva-dev
- GitHub: https://github.com/alexandresilva-dev

## Academic context

Developed individually for the Programming I course, first year of the Computer Engineering degree. The source code (comments and interface) is kept in Portuguese, as originally written and submitted; this README and the user guide are provided in English for portfolio purposes.
