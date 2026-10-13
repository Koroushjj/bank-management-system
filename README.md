# Bank Management System

A C++ console application exploring object-oriented account management, developed as coursework for **Hardwarenahe Objektorientierte Programmierung** at WHZ (summer semester 2026).

The project models customers, three account types, and transaction histories through a German-language command-line interface. It demonstrates inheritance, virtual dispatch, encapsulation, and collection-based object relationships.

**Status:** preserved coursework implementation, packaged for portfolio review. This is an educational simulation with known correctness limitations, not a production banking application. See [Code review](docs/CODE_REVIEW.md).

## Features

- Create standard, current (`Girokonto`), and savings (`Sparkonto`) accounts.
- Deposit and withdraw funds; current-account withdrawals support an overdraft limit of 100 currency units.
- Transfer funds between accounts and inspect account-level and bank-level histories.
- Find, list, lock, and unlock accounts through an administration menu.
- Restrict savings-account transfers in the user menu until released by the administration flow.
- Generate customer/account identifiers and date-stamped transaction records in memory.

The application starts with no accounts. Data is lost when the program exits. The menu provides one active customer; the administration menu is freely accessible and does not authenticate users.

## Build and run

### Requirements

- Windows with Visual Studio 2022 and the **Desktop development with C++** workload.
- MSVC v143 and a Windows 10/11 SDK compatible with the project's `10.0` SDK selection.
- No third-party libraries, database, network service, or package manager is required.

The implementation uses the Microsoft form of `localtime_s`; Linux/macOS support has not been implemented. The project inherits MSVC's language-standard default.

### Visual Studio

1. Open `BankSystem.sln`.
2. Select **Release** and **x64**.
3. Build the solution and run **BankSystem** without debugging (`Ctrl+F5`).

Alternatively, from a Visual Studio Developer PowerShell in the repository root:

```powershell
msbuild BankSystem.sln /p:Configuration=Release /p:Platform=x64
.\x64\Release\BankSystem.exe
```

On the preparation machine, the solution compiled but linking failed with `LNK1101: incorrect MSPDB140.DLL version`. A direct build without debug symbols succeeded. If you encounter the same local toolchain issue, this verified alternative can be run from Developer PowerShell in the repository root:

```powershell
New-Item -ItemType Directory -Force build | Out-Null
cl /nologo /EHsc /W4 /Fe:build\BankSystem.exe /Fo:build\BankSystem.obj src\BankSystem.cpp /link /DEBUG:NONE
.\build\BankSystem.exe
```

## Try a short demo

1. Select `1. Benutzer` and enter a single-word demo name, such as `Demo`.
2. Select `1. Standardkonto` and enter an opening balance of `100`.
3. The first account number is `1001` and the first customer ID is `100`.
4. Select `2. Einzahlen`, enter account `1001`, and deposit `25`.
5. Select `4. Kontostand anzeigen`: the balance should be `125`.
6. Select `5. Transaktionshistorie anzeigen` to inspect the deposit record.
7. Select `7. Zurueck`, then `3. Beenden`.

Use a decimal point for fractional amounts. Names are read as a single token. Account creation does not add an opening-balance transaction to the history.

## Design

| Class | Responsibility |
| --- | --- |
| `Transaktion` | Transaction type, amount, date, description, and console display |
| `Konto` | Balance, account identity, locking, deposits, withdrawals, and history |
| `Girokonto` | Overrides withdrawal behavior with an overdraft allowance |
| `Sparkonto` | Controls the savings-account transfer permission flag |
| `Benutzer` | Customer identity and references to associated accounts |
| `Bank` | Account/customer collections, account lookup, transfers, and bank history |

The source retains its original German identifiers and single-file layout. `main()` contains the user and administration menus. A later refactor can separate domain classes from console interaction after regression coverage is in place.

## Repository layout

```text
bank-management-system/
|-- BankSystem.sln
|-- README.md
|-- .gitignore
|-- .gitattributes
|-- src/
|   `-- BankSystem.cpp
|-- ide/visual-studio/
|   |-- BankSystem.vcxproj
|   `-- BankSystem.vcxproj.filters
`-- docs/
    |-- CODE_REVIEW.md
    `-- VALIDATION.md
```

## Known limitations and next steps

Negative transfer amounts and negative current-account withdrawals are not rejected consistently. Savings-transfer restrictions are checked in the user menu rather than centrally. Input handling does not handle end-of-file cleanly. Money uses `double`, and dynamically allocated accounts/customers have no owning cleanup mechanism.

Priorities are central amount validation, consistent account policies, explicit memory ownership, robust input parsing, and behavioral regression tests. Persistence and authentication are outside the current implementation.

See [Code review](docs/CODE_REVIEW.md) for evidence and [Validation](docs/VALIDATION.md) for the checks actually performed.

## Provenance and licensing

Prepared for the **Koroushjj** portfolio from the supplied WHZ coursework folder. The application source is preserved byte-for-byte; only the repository organization and Visual Studio source paths were changed. Build outputs, IDE caches, user-specific settings, and an unreferenced draft text file were excluded.

No license was included in the supplied project, and no open-source license has been selected for this preparation. Publication and license selection remain with the owner.
