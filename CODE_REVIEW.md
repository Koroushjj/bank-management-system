# Code review

Reviewed on 2026-10-13 against `src/BankSystem.cpp`. This report describes the preserved implementation; the issues below have not been fixed in this packaging pass.

## Assessment

The project provides a useful introductory demonstration of C++ inheritance, overriding, runtime type inspection, and object relationships. The account hierarchy and the split between customer and bank views are recognizable. Standard-account deposits and withdrawals reject nonpositive amounts, and transfers check account existence, locks, and available balance.

Correctness rules are uneven across entry points, however. The current implementation should be presented as coursework rather than as a reliable financial system.

## Findings

| Priority | Location / evidence | Effect and proposed improvement |
| --- | --- | --- |
| High | `Girokonto::auszahlen` | No positive-amount check. With balance 100, withdrawing -50 raises the balance to 150. Reject nonpositive/nonfinite values before mutation. |
| High | `Bank::ueberweisen` | No positive-amount check. Transferring -50 from a 100-balance account to another 100-balance account produces balances 150 and 50. Validate the amount centrally; reject self-transfers explicitly. |
| Medium | `Sparkonto::ueberweisungErlaubt`, user-menu case 6, administration case 4 | Only the user menu checks savings-transfer permission. Administration can transfer from a savings account while its transfer permission is disabled. Specify whether this override is intended; enforce the chosen policy in the domain operation. |
| Medium | All three account-creation branches / `Konto` constructor | Opening balances accept negative values without an account-specific rule. Validate opening balances in one shared creation path. |
| Medium | `leseInt`, `leseDouble` | End-of-file is repeatedly cleared and retried, potentially causing an endless output loop. Numeric-prefix input such as `12abc` is partially accepted. Read and validate whole lines and terminate cleanly on EOF. |
| Medium | `new Benutzer`, `new Konto`, `new Girokonto`, `new Sparkonto`; pointer vectors | Allocations have no matching cleanup and ownership is ambiguous. Introduce one owner using `std::unique_ptr`, with non-owning references elsewhere. Add a virtual destructor to the polymorphic base before deleting through `Konto*`. |
| Medium | `double` balances and transaction amounts | Binary floating-point does not provide exact decimal currency arithmetic. Introduce an integer minor-unit representation with explicit parsing, range checks, and formatting. |
| Low | `Konto::kontostandAendern` is public | Callers can bypass validation and transaction logging. Make balance mutation an internal operation with consistent invariants. |
| Low | `main()` and all classes share one source file | Repeated creation logic and mixed console/domain responsibilities complicate tests. Extract domain classes and input handling after behavior is covered. |

## Functional boundaries

- No database, files, networking, authentication, or durable audit log.
- Administration is a demonstration menu, not an access-control boundary.
- Only one interactive customer is maintained; admin-created accounts are not attached to that customer's account list.
- Savings-account unlocking also releases its transfer-permission flag. Locking does not reset that flag, though the general lock still blocks transfers.
- Current-account overdraft is supported for withdrawals, but transfers require sufficient positive balance.
- Names containing spaces are not read in full.
- The unreferenced `#include iostream.txt` draft contains an interest-calculation concept absent from the compiled source; interest calculation is therefore not advertised as a feature.

## Dependencies and packaging

The compiled source includes only C++ standard-library headers: `iostream`, `vector`, `string`, `chrono`, `ctime`, `sstream`, and `iomanip`. The project targets MSVC v143 and Windows SDK `10.0`, with no custom external package references in the supplied project file. `localtime_s` uses Microsoft's signature.

The canonical application is the `.cpp` referenced by the Visual Studio project, not the separate draft text file. The repository retains that source without edits and relocates the IDE project files under `ide/visual-studio`. No existing executables, debug symbols, `.vs` database, machine-specific `.user` file, or build logs are included.

## Suggested repair order

1. Add regression coverage for invalid amounts, blocked accounts, savings-transfer policy, and overdraft boundaries.
2. Centralize monetary validation and creation rules; choose an explicit administration override policy.
3. Introduce clear ownership and a virtual base destructor.
4. Separate console input from business operations; handle EOF and complete-line parsing.
5. Adopt exact minor-unit arithmetic before adding persistence or more account features.
