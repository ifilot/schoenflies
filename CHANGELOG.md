# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]

### Added
- Added an Apple Silicon macOS workflow that builds, tests, audits, and packages a self-contained disk image
- Added configurable per-atom 1s, 2p, 3d, and 4f basis-function visualization
- Added multi-orbital atom assignments, live modal previews, phase-color swatches, and a custom color picker
- Added translucent ghost orbitals during symmetry-operation animations

### Fixed
- Removed dark triangular banding artifacts from translucent orbital rendering

## [1.2.1] - 2026-09-22

### Added
- Added a welcoming startup screen with four shortcuts to the application's main workflows
- Added helpful interaction and guided point-group tips to the startup screen
- Added a Help menu command for reopening the welcome screen

### Changed
- Standardized Windows and macOS release asset names so the latest-download links remain stable across versions
- Updated the Windows package to include the application version in the installer filename
- Fully migrated the application, build system, documentation, and CI workflows to Qt 6
- Changed the library selection highlight to blue for better contrast and readability
- Improved Windows deployment to bundle and verify all required runtime DLLs

### Fixed
- Restored the Yes/No controls in the point-group decision tree under Qt 6

## [1.2.0] - 2026-09-22

### Added
- Added anti-aliasing to the molecular renderer
- Added translucent starting-position guides during symmetry-operation animations
- Added guided point-group determination directly from the molecule library
- Added a Practice menu command to start guided point-group determination for the currently loaded library molecule
- Added a synchronized, resizable full-tree overview alongside the focused route lens
- Added a light SVG decision-tree map in a dedicated three-column practice layout; it highlights the active route and unavailable branches

### Changed
- Redesigned point-group questions as clearer interactive cards with visual feedback
- Refactored the build and compilation workflow across platforms
- Migrated the codebase to require C++20
- Improved cross-platform consistency for Linux and Windows (MinGW)
- Modernized CMake configuration and dependency handling

### Fixed
- Resolved multiple build and packaging issues on Windows (MinGW)
- Improved robustness of the deployment and release process

## [1.1.1] - 2022-06-06
### Fixed
- The _σ_<sub>v</sub>&#x2032; symmetry operation is now correctly visible for
  the _D_<sub>2h</sub> point group (see issue #2)
- The program no longer crashes when practice is started without any modules
  selected (see issue #3)
- The flowchart practice module now shows the correct answer to the question
  whether a molecule has two or more _C<sub>n</sub>_ (_n_ > 2) for point groups
  with many higher-order rotations (see issue #4)

## [1.1.0] - 2022-05-10
### Added
- Built-in library of 36 example structures (File → Open from library)
- Character table viewer (View → Character tables)
- Support for labeling atoms on the structure (View → Show atom labels)
- Three types of randomly generated practice exercises using the library
  - Determining the point group of a structure
  - Determining the irreducible representations to which a basis set belongs
  - Applying the projection operator to an orbital of a single atom

### Fixed
- Explicitly add a return code when running Schoenflies from the command line
  (see issue #1)

## [1.0.0] - 2022-02-04
### Added
- Initial release

[Unreleased]: https://gitlab.com/lkkmpn/schoenflies/-/compare/1.2.1...main
[1.2.1]: https://gitlab.com/lkkmpn/schoenflies/-/compare/1.2.0...1.2.1
[1.2.0]: https://gitlab.com/lkkmpn/schoenflies/-/compare/1.1.1...1.2.0
[1.1.1]: https://gitlab.com/lkkmpn/schoenflies/-/compare/1.1.0...1.1.1
[1.1.0]: https://gitlab.com/lkkmpn/schoenflies/-/compare/1.0.0...1.1.0
[1.0.0]: https://gitlab.com/lkkmpn/schoenflies/-/releases/1.0.0
