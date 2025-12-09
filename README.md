# ALPS Native (C)
ALPS (Application Layer Presentation Selection) enables selection of a presentation from a **Dolby** AC-4 bitstream. The **Dolby** AC-4 decoder will subsequently decode only the selected presentation from that bitstream, ignoring all the other presentations. This repository provides a C implementation and optional CLI for testing purposes.

## Table of Contents
- [Installation](#installation)
- [Quickstart](#quickstart)
- [API](#api)
- [**Dolby** AC-4 presentation signaling](#dolby-ac-4-presentation-signaling)
- [Known limitations](#known-limitations)
- [Support / Contact](#support--contact)
- [License & third-party](#license--third-party)
- [Release notes](#release-notes)

## Installation
This project uses CMake and Conan to manage the build and dependencies.

1. Install CMake.
2. Install Conan package manager.
3. If you do not have a Conan profile yet, run:
   `conan profile detect --name <CONAN_PROFILE>`
4. Change directory to project's root:
   `cd <project_root>`
5. Install dependencies:
   `conan install . --profile=<CONAN_PROFILE> --build=missing`
6. List CMake presets that were created by Conan and choose one:
   `cmake --list-presets`
7. Configure with chosen preset:
   `cmake --preset <PRESET_NAME>`
8. Build:
   `cmake --build --preset <PRESET_NAME>`

Options:
- `BUILD_SHARED_LIBS` — build shared instead of static libraries.
- `BUILD_SHARED_ALPS_LIB` - build dlb_alps_native as a shared library instead of static.
- `BUILD_CLI` — build the CLI tool.
- `BUILD_UNIT_TESTS` — build unit tests.
- `BUILD_DOCUMENTATION` — build documentation.

## Quickstart
### Library
Minimal flow using the C API:
```c
#include "dlb_alps_native.h"

size_t mem_size = 0;
void *mem = NULL;
alps_ctx *alps = NULL;

alps_query_mem(&mem_size);
mem = malloc(mem_size);
alps_init(&alps, mem);

/* choose desired presentation ID */
alps_set_active_presentation_id(alps, 1);

/* load ISOBMFF segment bytes into buffer */
alps_process_isobmff_segment(alps, buffer, buffer_size);

/* read available presentations if needed */
alps_presentation *presentations = NULL;
size_t count = 0;
alps_get_presentations(alps, &presentations, &count);

alps_destroy(alps);
free(mem);
```

**alps object should be used for single period of content.** For multi-period content, multiple
instances of alps_ctx should be used.
 
Alps processes memory buffers that contain ISOBMFF segment data. These buffers are typically the result of network requests performed by the media player. After the player has downloaded the ISOBMFF segment data, data is processed in-place using the ALPS method 'alps_process_isobmff_segment'. The media player then receives the processed data and forwards it to the decoder. Integration with the media player depends on the player and differs from player to player implementation.

### CLI
If built with `BUILD_CLI=ON` (default), a CLI binary is produced. Usage example where the last argument is the desired presentation ID:
```
./alps_cli <segment1.m4s> <segment2.m4s> ... <active_presentation_id>
```

## API
Public headers are in `src/dlb_alps_native/include`. For key entry points see [dlb_alps_native.h](src/dlb_alps_native/include/dlb_alps_native/dlb_alps_native.h).

HTML code documentation can be found in [docs](docs/html)

## **Dolby** AC-4 presentation signaling
ALPS supports two signaling methods:

- ISOBMFF init segment level signaling
- External signaling

### ISOBMFF init segment signaling
ALPS supports presentation signaling on ISOBMFF init segment level and provides parsing support for 
this data. An abbreviated example of the ISOBMFF structure with boxes that are relevant to ALPS can be 
found below. **Dolby** AC-4 bitstreams may be packaged containing this information so that the available 
presentations and their properties can be derived from the ISOBMFF segment. Information derived from
those boxes can be accessed through the `alps_get_presentations` method.

```
[moov]
    [trak]
        [tkhd]
            track_ID
[meta]
    [grpl]
        [prsl]
            entity_id
            preselection_tag
            selection_priority
            [elng]
                extended_language
            [labl]
                label_id
                language
                label
            [ardi]
                audio_rendering_indication
            [kind]
                schemeURI
                value
            [udta]
                [diap]
                    dialog_gain

```

### External signaling
ALPS supports presentation signaling by external means, typically done via MPEG-DASH manifest. 
Parsing this data is out-of-scope for the ALPS library and must be performed on application level. 
In such a case ALPS still allows to set the active presentation using `alps_set_active_presentation_id`. The `alps_get_presentations` method can be used to validate whether the presentations from the external source match those from the ISOBMFF init segment. Note that ALPS will always validate the selected 
presentation ID against the presentation IDs found in the TOC and disable processing if the requested 
ID is not available in the TOC.

## Known limitations
### CMAF compliance
The ALPS library has been tested using **CMAF (Common Media Application Format)** compliant content.
Using the ALPS library with content that uses features beyond the standardized CMAF feature subset 
might cause unexpected behavior.

### **Dolby** AC-4 content requirements
ALPS functionality depends on parsing and modifying **Dolby** AC-4 TOC. DRM protected content may be
compatible with ALPS if the TOC is left unencrypted. In that case, only the audio substreams are
encrypted.

### Buffer management
Devices typically buffer some audio to ensure a good playback experience in cases of varying network
conditions. These buffers need to be flushed when changing the presentation to apply the changed
audio configuration quickly after the user selection has been made. Flushing buffers should ideally
be provided by the player and should be triggered when a different presentation is selected.

## Support / Contact
- Questions, issues, or feature requests: please open an issue in this repository.
- For commercial support inquiries, contact your Dolby representative.

## License & third-party
- License: see [LICENSE](LICENSE).
- Third-party: see `third-party/`.

## Release notes
See [RELEASENOTES.md](RELEASENOTES.md)
