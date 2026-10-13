# Validation record

Date: 2026-10-13. Validation was local only; no GitHub repository was created or updated.

## Source preservation

The copied `src/BankSystem.cpp` and the supplied compiled-project source have the same SHA-256:

```text
68FDB8822BECAF623C4710EDA874AC2DC458FEF487EAC54DDF1B6DA4D6E2AF86
```

Only solution/project source paths were updated. Original files were not edited. The separate draft text file is not part of the supplied build and was omitted.

## Build checks

- Installed toolchain: Visual Studio 2022 Community, MSVC 14.44.35207, x64.
- Relocated solution: Release/x64 source compilation succeeded, but linking failed with `LNK1101: incorrect MSPDB140.DLL version; recheck installation of this product`.
- Retrying with the installed toolchain directory first on PATH produced the same linker failure.
- Direct compilation of the packaged source with `cl /nologo /EHsc /W4`, followed by linking with `/DEBUG:NONE`, succeeded with exit code 0. No compiler warnings were printed.
- Executed tests used this newly built executable, not the executable supplied in the coursework folder.

The normal solution build is therefore not verified end-to-end on this machine. The README includes the successful direct-build alternative. No Visual Studio installation changes were made.

## Executed scenarios

Each scenario ran in a fresh process with scripted standard input, a five-second timeout, output checks, and a successful exit code of 0.

| Scenario | Input / operation | Observed outcome |
| --- | --- | --- |
| Basic account and deposit | Create a standard account with 100; deposit 25; show balance; return and exit | Balance 125; expected behavior confirmed |
| Negative current-account withdrawal | Create a current account with 100; withdraw -50; show balance | Balance 150; defect reproduced |
| Negative transfer | Create two standard accounts with 100 each; transfer -50 from 1001 to 1002; show balances | Destination balance 50; invalid negative transfer accepted, defect reproduced |

These are focused smoke checks and defect reproductions, not a complete regression suite. Locking, savings-release behavior, EOF handling, nonfinite amounts, allocation failures, and all menu combinations were not runtime-tested. No cross-platform build was attempted.

## Package inspection

- Excluded generated executables, symbols, object files, IDE databases, logs, and user-specific settings.
- Checked packaged source/project files for common credential keywords and absolute user paths; no matches were found beyond XML namespace URLs. This was a basic text scan, not a dedicated secret-scanner audit.
- No third-party runtime dependency or license file was found in the supplied build inputs.
- No open-source license was added and no publication was performed.
