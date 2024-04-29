# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]

## [1.0.0] - 2024-04-29

### Added

- example how to change I2C address for SLF3x sensors

### Changed

renamings in sf06_lf_i2c: 
- init_driver() changed to sf06_lf_init()
- naming of i2c address defines changed
- added prefix to enum types in sf06_lf_i2c header (cmd_id_t -> sf06_lf_cmd_id_t, inv_flow_scale_factors -> sf06_lf_inv_flow_scale_factors)

## [0.1.0] - 2022-03-30

Initial release

