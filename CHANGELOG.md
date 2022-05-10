# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]
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

[Unreleased]: https://gitlab.com/lkkmpn/schoenflies/-/compare/1.0.0...main
[1.0.0]: https://gitlab.com/lkkmpn/schoenflies/-/releases/1.0.0
