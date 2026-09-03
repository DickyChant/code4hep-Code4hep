# DELPHI converter integration

Status: design contract for `integrate/delphi-converter`.

## Decision

The DELPHI DST to EDM4hep converter should be exposed as a native Code4hep
input component. The legacy DELPHI reader remains an implementation dependency;
users should run conversion through a Code4hep configuration rather than a
separate conversion workflow.

The first integration must reuse the conversion code from
`DickyChant/delphi-edm4hep`. Rewriting domain conversion while changing the
framework boundary would make physics-equivalence failures impossible to
localise. After the Code4hep boundary is covered by identity tests, SKELANA can
be removed one domain at a time.

## Build the first integration

Check out both repositories next to one another, source the DELPHI environment
before the normal Code4hep/Stitched environment, then configure Code4hep with
the converter source directory. A separately sourced Key4hep stack is not
required; Code4hep's EDM4hep, podio, and ROOT packages are used:

```sh
source /cvmfs/delphi.cern.ch/setup.sh
# Source the normal Code4hep/Stitched environment here.
unset CXXFLAGS CFLAGS LDFLAGS

cmake -S Code4hep -B build \
  -DCODE4HEP_DELPHI_SOURCE_DIR="$PWD/delphi-edm4hep/delphi_edm4hep"
cmake --build build -j
ctest --test-dir build --output-on-failure
```

The resulting Code4hep build includes `delphi_sdst_pass`, `delphi_fdst_pass`,
`delphi_bs_fit`, `delphi_btag_check`, and the converter tests. An empty
`CODE4HEP_DELPHI_SOURCE_DIR` leaves the normal Code4hep build unchanged.

## Framework shape

PHDST and the DELPHI Fortran packages use process-global COMMON blocks and are
not thread-safe. The converter therefore belongs in an `InputSource`, not an
ordinary concurrent producer. The source owns the PHDST loop and publishes the
EDM4hep collections already built by `delphi-edm4hep` into each Code4hep event.
Only one DELPHI input stream may run in a process.

Two-pass conversion remains explicit:

1. short-DST creates the common and short-DST collections;
2. full-DST matches `(run, event)` against the short-DST result and adds the
   full-DST collections.

Until the second pass can consume framework products directly, it may use the
existing podio intermediate. This is a migration boundary, not the intended
final user interface.

## Removing SKELANA

The direct replacements supplied by Dietrich are:

| Concern | Direct DELPHI interface | EDM4hep/Code4hep result |
| --- | --- | --- |
| processing version | `CALL DSTQID(CDTYPE)` | store the four-character value as event metadata |
| magnetic field | `CALL BPILOT(BTESLA, BGEVCM)` | store both values; use `BTESLA` for field-dependent reconstruction |
| beamspot | reproduce `PSBEAM`, including its MC branch; fixed values are set through the VD package from `PSCBSD` defaults | publish the beamspot vertex and pass the same values to BTAG |
| b tagging | call AABTAG with that beamspot and expose its COMMON blocks through a typed C++ header | publish tags, fit status, track values, and the AABTAG vertex |
| secondary interactions (later) | reproduce the `PSBEG` `LDTOP`/`LPV`/`LPA` walk and call `MAKEMOD8` for code 120 | restore module 8 before converting affected particles |

`DSTQID` and `BPILOT` are already called by
`delphi-edm4hep/src/Event/Event.cpp`. AABTAG COMMON wrappers already exist in
`include/delphi_edm4hep/internal/AabtagCommons.h`. Those implementations should
be retained and moved behind stable interfaces rather than duplicated.

### Critical constraint

`PSBEG` currently does substantially more than the items above. Conversion code
reads SKELANA `PSC*` COMMON blocks for event quantities, track/vector selection,
vertices, VD hits, particle identification, calorimetry, reconstructed V0s, and
truth. Removing `PSBEG` before each reader has a direct-bank or direct-package
replacement would leave stale COMMON data and can produce plausible but wrong
events.

The removal sequence is therefore:

1. add a Code4hep source around the unchanged converter and establish byte- or
   digest-level collection equivalence;
2. extract a typed DELPHI event-context layer for `DSTQID`, `BPILOT`, beamspot,
   and AABTAG;
3. replace one `PSC*` consumer domain at a time, requiring collection and
   parameter equivalence on data and MC;
4. remove `PSINI`/`PSBEG` and `libskelanaxx` only when no runtime `PSC*` input
   remains;
5. add the lower-priority `MAKEMOD8` correction and bless its expected physics
   difference separately.

## Acceptance tests

The native integration is complete only when all of these pass:

- the existing `delphi-edm4hep` unit and conversion-identity tests;
- Code4hep reads every produced DELPHI collection through its product registry;
- fixed input samples give the same run/event IDs, collection names, sizes,
  relations, and value digests through the standalone and Code4hep entrypoints;
- data and MC samples verify `DSTQID`, both `BPILOT` values, beamspot values and
  errors, AABTAG status, and track-to-tag association;
- a multi-thread Code4hep configuration cannot schedule more than one DELPHI
  source stream;
- failures crossing Fortran callbacks become controlled framework errors and
  never unwind through Fortran.

## Repository boundary

The converter source currently has no explicit licence, while Code4hep has its
own distribution terms. Source copying or vendoring must wait for an explicit
licensing decision. Until then, Code4hep should discover and link an installed
`delphi-edm4hep` package. A small steering repository may pin known-compatible
commits and environments, but it must not become the implementation home.
